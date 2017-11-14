using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Hydrophones;

// 1285 lines of code
namespace UnitTestProject
{
    [TestClass]
    public class BoneTest
    {
        const int BUFFER_SIZE   = 256;
        const int R1MSps        = 1000000;
        const int F25kHz        = 25000;
        const int FXkHz         = 10000;
        const int PAGE_COUNT    = 3;
        const int CHANNEL_COUNT = 3;
        const int PHASE         = 0;
        const int START_TIME    = 0;

        private ushort[][] GenerateBasicTests()
        {
            return new ushort[][] {
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, FXkHz*0, PHASE, R1MSps, START_TIME),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, FXkHz*1, PHASE, R1MSps, START_TIME),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, FXkHz*2, PHASE, R1MSps, START_TIME),
            };
        }

        private ushort[] GenerateBasicStream()
        {
            int size = BUFFER_SIZE * CHANNEL_COUNT * PAGE_COUNT;
            int pageSize = BUFFER_SIZE * CHANNEL_COUNT;
            ushort[] result = new ushort[size];

            ushort[][] tests = GenerateBasicTests();
            for(int c = 0; c < CHANNEL_COUNT; c++)
            {
                for(int p = 0; p < PAGE_COUNT; p++)
                {
                    int offset = (p * pageSize) + (c * BUFFER_SIZE);
                    for(int k = 0; k < BUFFER_SIZE; k++)
                        result[offset + k] = tests[c][k];
                }
            }

            return result;
        }

        [TestMethod]
        public void CreateBone()
        {
            Bone bone = new Bone();
        }

        [TestMethod]
        public void DataToFrame()
        {
            ushort[] data = GenerateBasicStream();
            ushort[][] tests = GenerateBasicTests();
            Bone.Frame frame = Bone.DataToFrame(data, BUFFER_SIZE);
            
            int chSize = CHANNEL_COUNT * BUFFER_SIZE;
            Assert.AreEqual(CHANNEL_COUNT, frame.Channels.Length);
            for(int i = 0; i < CHANNEL_COUNT; i++)
                Assert.AreEqual(chSize, frame.Channels[i].Length);
            
            for(int c = 0; c < CHANNEL_COUNT; c++)
                for(int k = 0; k < BUFFER_SIZE; k++)
                    Assert.AreEqual(tests[c][k], frame.Channels[c][k]);
        }

        [TestMethod]
        public void NormalizedTime()
        {
            double[] time = new double[] {
                3.5, 1, 2.25
            };

            Bone.NormalizeTime(time);

            Assert.AreEqual(2.50, time[0]);
            Assert.AreEqual(0.00, time[1]);
            Assert.AreEqual(1.25, time[2]);
        }

        [TestMethod]
        public void CalcTimeDifferences()
        {
            const double THRESHOLD = 0.5;
            const int SPEED_OF_SOUND = 1450;

            const double TRIANGLE_SIDE = 0.51;
            const double X = 6, Y = 6;
            double radius = Math.Sqrt(3) * TRIANGLE_SIDE / 3;
            ADCEmulator.Vector2 machine = new ADCEmulator.Vector2(radius, 0);
            ADCEmulator.Vector2 pinger = new ADCEmulator.Vector2(X, Y);
            const int SENSOR_COUNT = 3;
            const double OFFSET = 0.000127;

            Bone.Frame frame = new Bone.Frame();
            frame.Channels = new ushort[CHANNEL_COUNT][];
            int frameBufSize = PAGE_COUNT * BUFFER_SIZE;
            double[] time = ADCEmulator.GenerateTimeDifferences(machine, pinger, SENSOR_COUNT, SPEED_OF_SOUND);
            Bone.NormalizeTime(time);
            for(int i = 0; i < CHANNEL_COUNT; i++)
                frame.Channels[i] = ADCEmulator.GenerateWaveSample(
                    frameBufSize, F25kHz, PHASE, R1MSps, time[i] + OFFSET);

            double tolerance = Bone.GetSampleSize(F25kHz) / (double)R1MSps;
            double[] ptime = Bone.CalcTimeDifferences(frame, F25kHz, THRESHOLD);
            
            for(int i = 0; i < SENSOR_COUNT; i++)
                if ((time[i] + tolerance) < ptime[i] || (time[i] - tolerance) > ptime[i])
                    Assert.Fail();
        }
    }
}
