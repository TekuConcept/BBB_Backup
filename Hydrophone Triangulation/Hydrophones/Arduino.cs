using System;

namespace Hydrophones
{
    public class Arduino
    {
        const int BUFFER_SIZE = 256;
        const int R1MSps = 1000000;
        int _targetFrequency;
        double _threshold;
        Page _cache1;
        Page[] _cache2;
        bool _isLocked, _isValid;
        int _pageIndex;

        public Arduino()
        {
            _targetFrequency = 0;
            _threshold = 0;
            _cache1 = new Page();
            _cache1.Buffer = new ushort[][] {
                new ushort[BUFFER_SIZE],
                new ushort[BUFFER_SIZE],
                new ushort[BUFFER_SIZE]
            };
            _cache2 = new Page[3];
            for (int i = 0; i < 3; i++)
                _cache2[i].Buffer = new ushort[][] {
                    new ushort[BUFFER_SIZE],
                    new ushort[BUFFER_SIZE],
                    new ushort[BUFFER_SIZE]
                };
            _isLocked = false;
            _isValid = false;
            _pageIndex = 0;
        }

        public void SetTargetFrequency(int target)
        {
            _targetFrequency = target;
        }

        public int GetTargetFrequency()
        {
            return _targetFrequency;
        }

        public void SetThreshold(double trigger)
        {
            _threshold = trigger;
        }

        public double GetThreshold()
        {
            return _threshold;
        }

        public void Update(ushort[] dma)
        {
             Page page = SortDMA(dma);
             if(HasTargetFrequency(page, _targetFrequency, R1MSps, _threshold)) {
                if(_isLocked)
                    return;
                _isLocked = _pageIndex != 0;
                _pageIndex++;
                _cache2[_pageIndex] = page;
                _isValid = false;
                if (_pageIndex == 2)
                {
                    _cache2[0] = _cache1;
                    _isValid = true;
                }
             }
             else {
                _isLocked = false;
                _pageIndex = 0;
                _cache1 = page;
             }
        }

        public Page[] GetCache()
        {
            return _cache2;
        }

        public bool DataIsValid()
        {
            return _isValid;
        }

        public struct Page
        {
            public ushort[][] Buffer;
        }

        public struct Complex
        {
            public double Real;
            public double Imaginary;

            public Complex(double r, double i)
            {
                Real = r;
                Imaginary = i;
            }

            public static Complex Multiply(Complex left, Complex right)
            {
                Complex result = new Complex();
                result.Real      = (left.Real*right.Real      - left.Imaginary*right.Imaginary);
                result.Imaginary = (left.Real*right.Imaginary + left.Imaginary*right.Real     );
                return result;
            }
            public static Complex Add(Complex left, Complex right)
            {
                Complex result = new Complex();
                result.Real      = left.Real      + right.Real;
                result.Imaginary = left.Imaginary + right.Imaginary;
                return result;
            }
            public static Complex Subtract(Complex left, Complex right)
            {
                Complex result = new Complex();
                result.Real      = left.Real      - right.Real;
                result.Imaginary = left.Imaginary - right.Imaginary;
                return result;
            }
        }

        public static Page SortDMA(ushort[] dma)
        {
            const int channelCount = 3;
            if (dma.Length < channelCount)
                throw new ArgumentException("DMA needs to have a length equal to 3n.", "dma");

            Page result = new Page();
            int size = dma.Length;
            int bufferSize = dma.Length / 3;
            int
                A0Idx = 0,
                A1Idx = 0,
                A2Idx = 0;

            result.Buffer = new ushort[][] {
                new ushort[bufferSize],
                new ushort[bufferSize],
                new ushort[bufferSize]
            };
            
            for(int i = 0; i < size; i++)
            {
                int tag = dma[i] & 0xF000;
                ushort sample = (ushort)(dma[i] & 0x0FFF);
                switch (tag) {
                    case 0x7000:
                        result.Buffer[0][A0Idx] = sample;
                        A0Idx++;
                        break;
                    case 0x6000:
                        result.Buffer[1][A1Idx] = sample;
                        A1Idx++;
                        break;
                    case 0x5000:
                        result.Buffer[2][A2Idx] = sample;
                        A2Idx++;
                        break;
                }
            }

            return result;
        }

        public static bool HasTargetFrequency(Page page, int frequency, int sampleRate, double threshold)
        {
            if (page.Buffer.Length < 1)
                throw new ArgumentException("Buffer must have at least one array.", "page");
            if (page.Buffer[0].Length % 2 != 0)
                throw new ArgumentException("Buffer array must have 2^n elements for the FFT.",
                    "page");

            bool result = false;
            Complex[] test = SamplesToComplex(page.Buffer[0]);

            FFT(test);
            int fidx = GetFrequencyIndex(frequency, sampleRate, page.Buffer[0].Length);
            double radius = Math.Sqrt(Math.Pow(test[fidx].Real, 2) + Math.Pow(test[fidx].Imaginary, 2));
            result = radius >= threshold;

            return result;
        }

        private static int GetFrequencyIndex(int frequency, int sampleRate, int p)
        {
            return (int)Math.Round((frequency * p) / (float)sampleRate);
        }

        public static Complex[] SamplesToComplex(ushort[] data)
        {
            int size = data.Length;
            Complex[] result = new Complex[size];

            const int HALF = 0x0FFF >> 1;

            for(int i = 0; i < size; i++)
            {
                double sample = (data[i]-HALF)/(double)HALF;
                if (sample > 1) sample = 1;
                else if (sample < -1) sample = -1;
                result[i].Real = sample;
                result[i].Imaginary = 0;
            }

            return result;
        }

        public static void FFT(Complex[] buffer)
        {
            int bits = (int)Math.Log(buffer.Length, 2);
            for (int j = 1; j < buffer.Length / 2; j++)
            {
                int swapPos = BitReverse(j, bits);
                var temp = buffer[j];
                buffer[j] = buffer[swapPos];
                buffer[swapPos] = temp;
            }

            for (int N = 2; N <= buffer.Length; N <<= 1)
            {
                for (int i = 0; i < buffer.Length; i += N)
                {
                    for (int k = 0; k < N / 2; k++)
                    {
                        int evenIndex = i + k;
                        int oddIndex  = i + k + (N / 2);
                        Complex even  = buffer[evenIndex];
                        Complex odd   = buffer[oddIndex];

                        double term = -2 * Math.PI * k / N;
                        Complex exp = Complex.Multiply(new Complex(Math.Cos(term), Math.Sin(term)), odd);

                        buffer[evenIndex] = Complex.Add(even, exp);
                        buffer[oddIndex] = Complex.Subtract(even, exp);
                    }
                }
            }
        }

        private static int BitReverse(int n, int bits)
        {
            int reversedN = n;
            int count = bits - 1;

            n >>= 1;
            while (n > 0)
            {
                reversedN = (reversedN << 1) | (n & 1);
                count--;
                n >>= 1;
            }

            return ((reversedN << count) & ((1 << bits) - 1));
        }

        public static ushort[] StreamFromPages(Page[] pages)
        {
            if (ValidatePages(pages) < 0)
                throw new ArgumentException(
                    "Pages are not equal sizes or doesn't contain valid data.", "pages");

            int
                pageCount = pages.Length,
                channelCount = pages[0].Buffer.Length,
                bufferSize = pages[0].Buffer[0].Length,
                pageSize = bufferSize * channelCount;
            ushort[] result = new ushort[pageCount*channelCount*bufferSize];

            for (int c = 0; c < channelCount; c++)
            {
                for (int p = 0; p < pageCount; p++)
                {
                    int offset = (p * pageSize) + (c * BUFFER_SIZE);
                    for (int k = 0; k < BUFFER_SIZE; k++)
                        result[offset + k] = pages[p].Buffer[c][k];
                }
            }

            return result;
        }

        private static int ValidatePages(Page[] pages)
        {
            int pageCount = pages.Length;
            if (pageCount == 0)
                return -1;

            int check = pages[0].Buffer.Length;
            int channelCount = check;
            if (channelCount == 0)
                return -1;
            for(int p = 1; p < pageCount; p++)
                if (pages[p].Buffer.Length != check)
                    return -1;

            check = pages[0].Buffer[0].Length;
            if (check == 0)
                return -1;
            for(int p = 0; p < pageCount; p++)
                for(int c = 0; c < channelCount; c++)
                    if (pages[p].Buffer[c].Length != check)
                        return -1;

            return 0;
        }
    }
}
