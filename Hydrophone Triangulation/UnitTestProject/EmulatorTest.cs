using Microsoft.VisualStudio.TestTools.UnitTesting;
using Hydrophones;
using System;

namespace UnitTestProject
{
    [TestClass]
    public class EmulatorTest
    {
        const int SPEED_OF_SOUND = 1450;
        const int BUFFER_SIZE    = 256;
        const int R1MSps         = 1000000;
        const int F25kHz         = 25000;

        [TestMethod]
        public void WaveSampleSize()
        {
            const double START_TIME = 0;
            const double PHASE = 0;
            ushort[] buffer = ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, PHASE, R1MSps, START_TIME);

            Assert.AreEqual(BUFFER_SIZE, buffer.Length);
        }

        [TestMethod]
        public void WaveSampleValues()
        {
            const double START_TIME = 0;
            const double PHASE = 0;
            ushort[] data = new ushort[] {
                2047,2367,2680,2977,3250,
                3495,3703,3871,3994,4069,
                4095,4069,3994,3871,3703,
                3495,3250,2977,2680,2367
            };

            ushort[] buffer = ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, PHASE, R1MSps, START_TIME);

            for(int s = 0; s < data.Length; s++)
            {
                Assert.AreEqual(data[s], buffer[s]);
            }
        }

        [TestMethod]
        public void WaveSampleOffset()
        {
            const double START_TIME = 5E-6;
            const double PHASE = 0;
            ushort[] data = new ushort[] {
                2047,2047,2047,2047,2047,
                2047,2367,2680,2977,3250,
                3495,3703,3871,3994,4069,
                4095,4069,3994,3871,3703,
                3495,3250,2977,2680,2367
            };

            ushort[] buffer = ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, PHASE, R1MSps, START_TIME);
            
            for (int s = 0; s < data.Length; s++)
            {
                Assert.AreEqual(data[s], buffer[s]);
            }
        }

        [TestMethod]
        public void GenerateNoise()
        {
            const double PHASE = 0;
            const double START_TIME = 0;
            ushort[] buffer = ADCEmulator.GenerateWaveSample(BUFFER_SIZE, 0, PHASE, R1MSps, START_TIME);
            buffer = ADCEmulator.AddOceanNoise(buffer, 1, R1MSps);

            int tally = 0;
            for (int i = 0; i < BUFFER_SIZE; i++)
                if (buffer[i] == (0x0FFF >> 1))
                    tally++;
            if (tally > (BUFFER_SIZE >> 1))
                Assert.Fail();
        }

        [TestMethod]
        public void AddNoise()
        {
            const double START_TIME = 0;
            const double PHASE = 0;
            const double PERCENT = 0.50;

            ushort[] buffer = ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, PHASE, R1MSps, START_TIME);
            buffer = ADCEmulator.AddOceanNoise(buffer, PERCENT, R1MSps);

            for (int s = 0; s < buffer.Length; s++)
            {
                if (buffer[s] > 0x0FFF)
                    Assert.Fail();
            }
        }

        [TestMethod]
        public void GetNextPhase()
        {
            const double START_TIME = 0;
            const double PHASE = 0;
            double phase = ADCEmulator.GetNextPhaseAngle(BUFFER_SIZE, F25kHz, PHASE, R1MSps, START_TIME);
            
            // Within 8.5% Error
            if (phase < 2.3 || phase > 2.7)
            {
                Assert.Fail();
            }
        }

        [TestMethod]
        public void GetDMABuffer()
        {
            double[] startTimes = new double[] { 0, 0, 0 };
            const double PHASE = 0;

            int channelCount = 3;
            ushort[][] channels = new ushort[channelCount][];
            for (int i = 0; i < channelCount; i++ )
                channels[i] = ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, PHASE, R1MSps, startTimes[i]);
            ushort[] dma = ADCEmulator.GenerateDMABuffer(BUFFER_SIZE, channels);

            int expectedSize = BUFFER_SIZE * startTimes.Length;
            Assert.AreEqual(expectedSize, dma.Length);

            int[] cflags = new int[] { 7, 6, 5 };
            for(int i = 0; i < dma.Length; i++)
            {
                Assert.AreEqual(cflags[i%cflags.Length], (dma[i] & 0xF000) >> 12);
            }
        }

        [TestMethod]
        public void GenerateTimeDiff()
        {
            const double TRIANGLE_SIDE = 0.51;
            const double X = 6, Y = 6;
            double radius = Math.Sqrt(3) * TRIANGLE_SIDE / 3;
            ADCEmulator.Vector2 machine = new ADCEmulator.Vector2(radius, 0);
            ADCEmulator.Vector2 pinger  = new ADCEmulator.Vector2(X, Y);
            const int SENSOR_COUNT = 3;
            const double TOLERANCE = 0.000001;
            double[] test = new double[] { 0.005710, 0.005802, 0.006048 };

            double[] time = ADCEmulator.GenerateTimeDifferences(machine, pinger, SENSOR_COUNT, SPEED_OF_SOUND);
            Assert.AreEqual(SENSOR_COUNT, time.Length);

            for(int i = 0; i < SENSOR_COUNT; i++)
                if ((test[i] + TOLERANCE) < time[i] || (test[i] - TOLERANCE) > time[i])
                    Assert.Fail();
        }
    }
}
