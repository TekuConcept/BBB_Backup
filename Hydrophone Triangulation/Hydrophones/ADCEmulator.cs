using System;

namespace Hydrophones
{
    public class ADCEmulator
    {
        const double PI = 3.14159265358979323846;
        const double PI2 = 6.28318530717958647692;
        static Random rand = new Random();

        public struct Vector2 {
            public double X;
            public double Y;
            public Vector2(double x, double y)
            {
                X = x;
                Y = y;
            }
        }
        
        public static ushort[] GenerateWaveSample(int bufferSize, int frequency, double phase, int sampleRate, double startTime)
        {
            ushort[] result = new ushort[bufferSize];
            double[] normalized = GenerateNormalizedWaveSample(bufferSize, frequency, phase, sampleRate, startTime);

            for(int i = 0; i < bufferSize; i++)
            {
                result[i] = (ushort)(0x0FFF * normalized[i]);
            }

            return result;
        }

        private static double[] GenerateNormalizedWaveSample(int bufferSize, int frequency, double phase, int sampleRate, double startTime)
        {
            double[] result = new double[bufferSize];

            int time = (int)(startTime * sampleRate);
            for (int s = 0; s < bufferSize; s++)
            {
                if (s < time)
                    result[s] = 0.5;
                else {
                    double angle = (PI2 * frequency * ((s - time) / (float)sampleRate)) + phase;
                    result[s] = (Math.Sin(angle) + 1) / 2.0;
                }
            }

            return result;
        }

        public static ushort[] AddOceanNoise(ushort[] buffer, double percent, int sampleRate)
        {
            if (percent > 1 || percent < 0)
                throw new ArgumentOutOfRangeException("Percent", "Needs to be a value between 0 and 1.");

            int size = buffer.Length;
            const int HALF = 0x0FFF>>1;
            double percentOriginal = (1 - percent);
            ushort[] result = new ushort[size];
            double[] noise = GenerateOceanNoise(size, sampleRate);
            
            for(int s = 0; s < size; s++)
            {
                int sample = (int)((buffer[s] - HALF)*percentOriginal);
                int snoise = (int)(noise[s]*HALF*percent);
                result[s] = (ushort)(sample + snoise + HALF);
            }

            return result;
        }

        private static double[] GenerateOceanNoise(int bufferSize, int sampleRate)
        {
            const int F10Hz = 10, F40kHz = 40000;

            double[] noise = new double[bufferSize];
            int numOfWaves = rand.Next(1, 11);
            for (int i = 0; i < numOfWaves; i++)
            {
                noise =  AppendWithoutAveraging(noise, GenerateNormalizedWaveSample(
                    bufferSize, rand.Next(F10Hz, F40kHz), rand.NextDouble() * PI2, sampleRate, 0));
            }
            for (int i = 0; i < bufferSize; i++)
            {
                noise[i] = noise[i] / numOfWaves;
            }

            return noise;
        }

        public static double GetNextPhaseAngle(int bufferSize, int frequency, double phase, int sampleRate, double startTime)
        {
            double result = 0;

            int sampleIndex = (int)(bufferSize - (startTime * sampleRate));
            double time = sampleIndex/(float)sampleRate;
            result = ((PI2*frequency*time) + phase)%PI2;
            
            return result;
        }

        private static double[] AppendWithoutAveraging(double[] original, double[] value)
        {
            if (value.Length < original.Length)
                throw new ArgumentException("value's length is not >= original's lenght.", "value");

            int size = original.Length;
            double[] result = new double[size];

            for(int i = 0; i < size; i++)
            {
                result[i] = original[i] + value[i];
            }

            return result;
        }

        public static ushort[] GenerateDMABuffer(int bufferSize, ushort[][] data)
        {
            if (data.Length > 8)
                throw new ArgumentException("Too many start times! Only pass in 8 max.", "startTimes");

            for (int i = 0; i < data.Length; i++)
                if (data[i].Length != bufferSize)
                    throw new ArgumentException("One or more arrays are not the expected buffer size.", "data");

            int channelCount = data.Length;
            int size = bufferSize * channelCount;
            ushort[] result = new ushort[size];
            ushort[][] channels = new ushort[channelCount][];

            for (int i = 0; i < channelCount; i++)
                channels[i] = TagChannel(data[i], (byte)(7 - (i % 8)));
            
            
            // merge channels sequentially into the output
            for(int sample = 0; sample < bufferSize; sample++) {
                int idx = sample*channelCount;
                for(int channel = 0; channel < channelCount; channel++) {
                    result[idx+channel] = channels[channel][sample];
                }
            }

            return result;
        }

        private static ushort[] TagChannel(ushort[] data, byte tag)
        {
            int size = data.Length;
            ushort[] result = new ushort[size];

            for(int i = 0; i < size; i++)
                result[i] = (ushort)(data[i] | (tag<<12));

            return result;
        }

        public static double[] GenerateTimeDifferences(Vector2 machine, Vector2 pinger, int sensorCount, int speedOfSound)
        {
            double[] result = new double[sensorCount];

            double radius = Math.Sqrt(Math.Pow(machine.X, 2) + Math.Pow(machine.Y, 2));
            double angle = PI2 / sensorCount;
            double orientation = GetAngle(machine);
            
            for (int i = 0; i < sensorCount; i++)
            {
                Vector2 sensor = new Vector2(
                    radius * Math.Cos(i * angle + orientation),
                    radius * Math.Sin(i * angle + orientation)
                );
                result[i] = GetDistance(sensor, pinger) / speedOfSound;
            }

            return result;
        }

        private static double GetAngle(Vector2 cartesian)
        {
            if (cartesian.X == 0 && cartesian.Y == 0)
                throw new ArgumentException("Certesian coordinates must not be <0,0>.", "certesian");

            if (cartesian.X == 0)
                return cartesian.Y > 0 ? PI / 2 : -PI / 2;

            double prototype = Math.Atan2(cartesian.Y, cartesian.X);
            if (cartesian.X > 0)
                return prototype;
            else if (cartesian.Y > 0)
                return 180 - prototype;
            else
                return -180 - prototype;
        }

        private static double GetDistance(Vector2 a, Vector2 b)
        {
            return Math.Sqrt(Math.Pow((b.X - a.X), 2) + Math.Pow(b.Y - a.Y, 2));
        }
    }
}
