using Microsoft.VisualStudio.TestTools.UnitTesting;
using Hydrophones;

namespace UnitTestProject
{
    [TestClass]
    public class ArduinoTest
    {
        const int BUFFER_SIZE = 256;
        const int R1MSps = 1000000;
        const int F25kHz = 25000;
        const int FXkHz = 10000;
        const double THRESHOLD = 28;
        const int PHASE = 0;
        const int START_TIME = 0;

        private Arduino.Page GetBasicPage()
        {
            ushort[] dma = ADCEmulator.GenerateDMABuffer(BUFFER_SIZE, new ushort[][] {
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE,F25kHz,0,R1MSps,0),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE,F25kHz,0,R1MSps,0),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE,F25kHz,0,R1MSps,0)
            });
            Arduino.Page page = Arduino.SortDMA(dma);
            return page;
        }

        private ushort[][][] GenerateTestData()
        {
            ushort[][][] result = new ushort[4][][];
            double[]
                phase = new double[] { 0, 0, 0 },
                startTime = new double[] { 0, 0, 0 };

            // this dma update shall be overwritten
            result[0] = new ushort[][] {
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, 0, phase[0], R1MSps, startTime[0]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, 0, phase[1], R1MSps, startTime[1]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, 0, phase[2], R1MSps, startTime[2])
            };

            // this dma update shall produce the first starting frequency,
            // but not yet be detected by the frequency tester
            startTime[1] = 0.000127;
            result[1] = new ushort[][] {
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, 0, phase[0], R1MSps, startTime[0]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, phase[1], R1MSps, startTime[1]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, 0, phase[2], R1MSps, startTime[2])
            };

            // this dma update shall be detected by the frequency tester
            phase[1] = ADCEmulator.GetNextPhaseAngle(BUFFER_SIZE, F25kHz, phase[1], R1MSps, startTime[1]);
            startTime[0] = 0.000127;
            startTime[1] = 0;
            startTime[2] = 0.000127;
            result[2] = new ushort[][] {
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, phase[0], R1MSps, startTime[0]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, phase[1], R1MSps, startTime[1]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, phase[2], R1MSps, startTime[2])
            };

            // this dma update shall be the collateral
            phase[0] = ADCEmulator.GetNextPhaseAngle(BUFFER_SIZE, F25kHz, phase[0], R1MSps, startTime[0]);
            phase[1] = ADCEmulator.GetNextPhaseAngle(BUFFER_SIZE, F25kHz, phase[1], R1MSps, startTime[1]);
            phase[2] = ADCEmulator.GetNextPhaseAngle(BUFFER_SIZE, F25kHz, phase[2], R1MSps, startTime[2]);
            startTime[0] = 0;
            startTime[1] = 0;
            startTime[2] = 0;
            result[3] = new ushort[][] {
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, phase[0], R1MSps, startTime[0]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, phase[1], R1MSps, startTime[1]),
                    ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, phase[2], R1MSps, startTime[2])
            };

            return result;
        }

        [TestMethod]
        public void PageChannelLength()
        {
            Arduino.Page page = GetBasicPage();

            Assert.AreEqual(3, page.Buffer.Length);
            Assert.AreEqual(BUFFER_SIZE, page.Buffer[0].Length);
            Assert.AreEqual(BUFFER_SIZE, page.Buffer[1].Length);
            Assert.AreEqual(BUFFER_SIZE, page.Buffer[2].Length);
        }

        [TestMethod]
        public void StripsTag()
        {
            Arduino.Page page = GetBasicPage();

            for (int i = 0; i < BUFFER_SIZE; i++)
                for(int j = 0; j < 3; j++)
                    Assert.AreEqual(0, page.Buffer[j][i] & 0xF000);
        }

        [TestMethod]
        public void SortsBuffers()
        {
            ushort[][] buffer = new ushort[][] {
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, 0, R1MSps, 0),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, 0, R1MSps, 0),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, F25kHz, 0, R1MSps, 0)
            };
            ushort[] dma = ADCEmulator.GenerateDMABuffer(BUFFER_SIZE, buffer);
            Arduino arduino = new Arduino();
            Arduino.Page page = Arduino.SortDMA(dma);

            for(int i = 0; i < BUFFER_SIZE; i++)
                for(int j = 0; j < 3; j++)
                    Assert.AreEqual(buffer[j][i], page.Buffer[j][i]);
        }

        [TestMethod]
        public void SetsTargetFrequency()
        {
            Arduino arduino = new Arduino();
            arduino.SetTargetFrequency(F25kHz);
            Assert.AreEqual(F25kHz, arduino.GetTargetFrequency());
        }

        [TestMethod]
        public void SetsTargetThreshold()
        {
            Arduino arduino = new Arduino();
            arduino.SetThreshold(THRESHOLD);

            double result = arduino.GetThreshold();

            Assert.AreEqual(THRESHOLD, result);
        }

        [TestMethod]
        public void HasTargetFrequency()
        {
            Arduino.Page page = GetBasicPage();
            Arduino arduino = new Arduino();

            if (!Arduino.HasTargetFrequency(page, F25kHz, R1MSps, THRESHOLD))
                Assert.Fail();
        }

        [TestMethod]
        public void StoresData()
        {
            Arduino arduino = new Arduino();
            arduino.SetTargetFrequency(F25kHz);
            arduino.SetThreshold(THRESHOLD);

            ushort[][][] tests = GenerateTestData();
            for (int i = 0; i < 4; i++ )
                arduino.Update(ADCEmulator.GenerateDMABuffer(BUFFER_SIZE, tests[i]));

            Arduino.Page[] cache = arduino.GetCache();
            Assert.AreEqual(3, cache.Length);

            for (int dma = 1; dma <= 3; dma++)
                for (int buf = 0; buf < 3; buf++)
                    for (int i = 0; i < BUFFER_SIZE; i++)
                        Assert.AreEqual(tests[dma][buf][i], cache[dma - 1].Buffer[buf][i]);
        }

        [TestMethod]
        public void IsValidData()
        {
            Arduino arduino = new Arduino();
            Assert.IsFalse(arduino.DataIsValid());
            
            arduino.SetTargetFrequency(F25kHz);
            arduino.SetThreshold(THRESHOLD);
            ushort[][][] tests = GenerateTestData();
            for (int i = 0; i < 4; i++)
                arduino.Update(ADCEmulator.GenerateDMABuffer(BUFFER_SIZE, tests[i]));

            Assert.IsTrue(arduino.DataIsValid());
        }

        [TestMethod]
        public void PagesToStream()
        {
            const int PAGE_COUNT = 3;
            const int CHANNEL_COUNT = 3;
            Arduino.Page[] pages = new Arduino.Page[PAGE_COUNT];

            ushort[][] data = new ushort[][]
            {
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, FXkHz*0, PHASE, R1MSps, START_TIME),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, FXkHz*1, PHASE, R1MSps, START_TIME),
                ADCEmulator.GenerateWaveSample(BUFFER_SIZE, FXkHz*2, PHASE, R1MSps, START_TIME)
            };
            pages[0] = Arduino.SortDMA(ADCEmulator.GenerateDMABuffer(BUFFER_SIZE, data));
            pages[2] = pages[1] = pages[0];
            ushort[] stream = Arduino.StreamFromPages(pages);
            
            int pageSize = BUFFER_SIZE * CHANNEL_COUNT;
            for (int c = 0; c < CHANNEL_COUNT; c++)
            {
                for (int p = 0; p < PAGE_COUNT; p++)
                {
                    int offset = (p * pageSize) + (c * BUFFER_SIZE);
                    for (int k = 0; k < BUFFER_SIZE; k++)
                        Assert.AreEqual(data[c][k], stream[offset + k]);
                }
            }
        }
    }
}
