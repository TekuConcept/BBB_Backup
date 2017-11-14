using System;

namespace Hydrophones
{
    public class Bone
    {
        const int SPEED_OF_SOUND = 1450;
        const int R1MSps = 1000000;

        public struct Frame {
            public ushort[][] Channels;
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
                result.Real = (left.Real * right.Real - left.Imaginary * right.Imaginary);
                result.Imaginary = (left.Real * right.Imaginary + left.Imaginary * right.Real);
                return result;
            }
            public static Complex Add(Complex left, Complex right)
            {
                Complex result = new Complex();
                result.Real = left.Real + right.Real;
                result.Imaginary = left.Imaginary + right.Imaginary;
                return result;
            }
            public static Complex Subtract(Complex left, Complex right)
            {
                Complex result = new Complex();
                result.Real = left.Real - right.Real;
                result.Imaginary = left.Imaginary - right.Imaginary;
                return result;
            }
            public static Complex FromPolar(int magnitude, double phase)
            {
                Complex result = new Complex();
                result.Real = magnitude * Math.Cos(phase);
                result.Imaginary = magnitude * Math.Sin(phase);
                return result;
            }
            public static double Magnitude(Complex value)
            {
                return Math.Sqrt(Math.Pow(value.Real, 2) + Math.Pow(value.Imaginary, 2));
            }
        }

        public struct Vector2 {
            public double X;
            public double Y;
        }

        public static Frame DataToFrame(ushort[] data, int bufferSize)
        {
            if (data.Length % 9 != 0 || data.Length % bufferSize != 0)
                throw new ArgumentException(
                    "Data must have 3 pages, 3 channels of data, and must be a multiple of bufferSize.",
                    "data");

            const int PAGE_COUNT = 3;
            const int CHANNEL_COUNT = 3;
            int pageSize = bufferSize * CHANNEL_COUNT;
            int channelSize = bufferSize * PAGE_COUNT;

            Frame result = new Frame();
            result.Channels = new ushort[][] {
                new ushort[channelSize],
                new ushort[channelSize],
                new ushort[channelSize]
            };
            
            for(int c = 0; c < CHANNEL_COUNT; c++)
                for(int p = 0; p < PAGE_COUNT; p++)
                {
                    int doffset = (p * pageSize) + (c * bufferSize);
                    int coffset = (p * bufferSize);
                    for(int k = 0; k < bufferSize; k++)
                    {
                        result.Channels[c][coffset + k] = data[doffset + k];
                    }
                }

            return result;
        }

        public static double[] CalcTimeDifferences(Frame frame, int targetFrequency, double threshold)
        {
            if (ValidateFrame(frame) < 0)
                throw new ArgumentException(
                    "Frame does not contain equal channels or contains 0 channels",
                    "frame");

            int
                channelCount = frame.Channels.Length,
                bufferSize = frame.Channels[0].Length;
            double[] predictions = new double[channelCount];

            for(int i = 0; i < channelCount; i++)
                predictions[i] = GetStartTime(frame.Channels[i], targetFrequency, threshold);

            NormalizeTime(predictions);
            return predictions;
        }

        private static int ValidateFrame(Frame frame)
        {
            int channelCount = frame.Channels.Length;
            if (channelCount == 0)
                return -1;

            int check = frame.Channels[0].Length;
            for(int i = 1; i < channelCount; i++)
                if (frame.Channels[i].Length != check)
                    return -1;

            return 0;
        }

        private static double GetStartTime(ushort[] buffer, int targetFrequency, double threshold)
        {
            Complex[] data = DataToComplex(buffer);

            int partition = 0;
            int sampleSize = GetSampleSize(targetFrequency);

            do {
                if (HasFrequency(data, partition, sampleSize, threshold))
                    break;
                partition += sampleSize;
            } while ((partition + sampleSize) < data.Length);
            partition += (sampleSize >> 1);

            return partition / (double)R1MSps;
        }

        private static Complex[] DataToComplex(ushort[] data)
        {
            int size = data.Length;
            Complex[] result = new Complex[size];
            const int HALF = 0x0FFF >> 1;

            for(int i = 0; i < size; i++)
            {
                double r = (data[i] - HALF)/((double)HALF);
                if (r > 1) r = 1;
                else if (r < -1) r = -1;
                result[i] = new Complex(r, 0);
            }

            return result;
        }

        public static int GetSampleSize(int targetFrequency)
        {
            return ((R1MSps>>1) / targetFrequency);
        }

        private static double DFT(Complex[] input, int start, int length)
        {
            int N = length;
            Complex[] output = new Complex[N];
            double arg = -2.0 * Math.PI / N;
            for (int n = 0; n < N; n++)
            {
                output[n] = new Complex();
                for (int k = 0; k < N; k++)
                    output[n] = Complex.Add(
                        Complex.Multiply(
                            input[k + start],
                            Complex.FromPolar(1, arg * n * k)
                        ),
                        output[n]
                    );
            }

            double max = 0;
            for(int i = 0; i < N; i++)
            {
                double mag = Complex.Magnitude(output[i]);
                if (mag > max) max = mag;
            }
            if (max == 0) max = 1;

            return Complex.Magnitude(output[0]) / max;
        }

        private static bool HasFrequency(Complex[] buffer, int start, int length, double threshold)
        {
            double result = DFT(buffer, start, length);
            return result >= threshold;
        }

        public static void NormalizeTime(double[] time)
        {
            int size = time.Length;
            if (size == 0)
                throw new ArgumentException("Need at least array of size 1.", "time");
            double min = time[0];
            for (int i = 1; i < size; i++)
                if (min > time[i])
                    min = time[i];
            for (int i = 0; i < size; i++)
                time[i] -= min;
        }

        public static Vector2 ApolloniusLocation(Vector2[] sensors, double[] times)
        {
            if (sensors.Length != 3 || times.Length != 3)
                throw new ArgumentException(
                    "Apollonius requires 3 unique locations and respective times.");

            Vector2 result = new Vector2();
            Vector2
                p1 = sensors[0],
                p2 = sensors[1],
                p3 = sensors[2];

            double
                r1 = times[0] * SPEED_OF_SOUND,
                r2 = times[1] * SPEED_OF_SOUND,
                r3 = times[2] * SPEED_OF_SOUND;

            bool flag = false;
            double
                s1 = flag ? 1 : -1,
                s2 = flag ? 1 : -1,
                s3 = flag ? 1 : -1;

            const double min_r = 0.00001;
            if (r1 == 0) r1 = min_r;
            if (r2 == 0) r2 = min_r;
            if (r3 == 0) r3 = min_r;
            
            double v11 = 2 * p2.X - 2 * p1.X;
            double v12 = 2 * p2.Y - 2 * p1.Y;
            double v13 = p1.X * p1.X - p2.X * p2.X + p1.Y * p1.Y - p2.Y * p2.Y - r1 * r1 + r2 * r2;
            double v14 = 2 * s2 * r2 - 2 * s1 * r1;

            double v21 = 2 * p3.X - 2 * p2.X + min_r;
            double v22 = 2 * p3.Y - 2 * p2.Y;
            double v23 = p2.X * p2.X - p3.X * p3.X + p2.Y * p2.Y - p3.Y * p3.Y - r2 * r2 + r3 * r3;
            double v24 = 2 * s3 * r3 - 2 * s2 * r2;

            double w12 = v12 / v11;
            double w13 = v13 / v11;
            double w14 = v14 / v11;

            double w22 = v22 / v21 - w12;
            double w23 = v23 / v21 - w13;
            double w24 = v24 / v21 - w14;

            double P = -w23 / w22;
            double Q = w24 / w22;
            double M = -w12 * P - w13;
            double N = w14 - w12 * Q;

            double a = N * N + Q * Q - 1;
            double b = 2 * M * N - 2 * N * p1.X + 2 * P * Q - 2 * Q * p1.Y + 2 * s1 * r1;
            double c = p1.X * p1.X + M * M - 2 * M * p1.X + P * P + p1.Y * p1.Y - 2 * P * p1.Y - r1 * r1;

            double D = b * b - 4 * a * c;

            double radius = (-b - Math.Sqrt(D)) / (2 * a);
            result.X = M + N * radius;
            result.Y = P + Q * radius;
            return result;
        }
    }
}
