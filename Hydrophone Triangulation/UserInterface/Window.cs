using Hydrophones;
using System;
using System.Drawing;
using System.Windows.Forms;

namespace UserInterface
{
    public partial class Window : Form
    {
        const int BUFFER_SIZE = 256;
        const int R1MSps = 1000000;
        const int F25kHz = 27500;//25000;
        bool shouldDraw = false;
        Arduino.Complex[] test;
        Brush blue = Brushes.BlueViolet;
        Brush gray = Brushes.Gray;
        Brush green = Brushes.Green;
        Brush red = Brushes.Orange;
        Pen pgrey = new Pen(Color.Chocolate, 1);

        public Window()
        {
            InitializeComponent();

            const double percent = 0.5;
            ushort[] dma = ADCEmulator.GenerateDMABuffer(BUFFER_SIZE, new ushort[][] {
                ADCEmulator.AddOceanNoise(ADCEmulator.GenerateWaveSample(BUFFER_SIZE,F25kHz,Math.PI,R1MSps,0), percent, R1MSps),
                ADCEmulator.AddOceanNoise(ADCEmulator.GenerateWaveSample(BUFFER_SIZE,F25kHz,Math.PI,R1MSps,0), percent, R1MSps),
                ADCEmulator.AddOceanNoise(ADCEmulator.GenerateWaveSample(BUFFER_SIZE,F25kHz,Math.PI,R1MSps,0), percent, R1MSps)
            });
            Arduino.Page page = Arduino.SortDMA(dma);
            test = Arduino.SamplesToComplex(page.Buffer[0]);
            Arduino.FFT(test);
            //Normalize(test);

            shouldDraw = true;
        }

        ~Window()
        {
            blue.Dispose();
            gray.Dispose();
            green.Dispose();
            red.Dispose();
            pgrey.Dispose();
        }

        private void Normalize(Arduino.Complex[] data)
        {
            double max = 0;
            for(int i = 0; i < data.Length; i++)
                if (data[i].Real > max)
                    max = data[i].Real;
            if(max != 0)
                for(int i = 0; i < data.Length; i++)
                    data[i].Real /= max;

            max = 0;
            for (int i = 0; i < data.Length; i++)
                if (data[i].Imaginary > max)
                    max = data[i].Imaginary;
            if (max != 0)
                for (int i = 0; i < data.Length; i++)
                    data[i].Imaginary /= max;
        }

        private double RadiusFromComplex(Arduino.Complex val)
        {
            return Math.Sqrt(Math.Pow(val.Real, 2) + Math.Pow(val.Imaginary, 2));
        }

        private void canvas_Paint(object sender, PaintEventArgs e)
        {
            if(shouldDraw)
            {
                int size = BUFFER_SIZE / 2;
                //const int SCALER = 50, SCALEI = 50;
                Graphics gfx = e.Graphics;
                int width = (int)(canvas.Width / (size*3.0));
                int height = canvas.Height / 100;
                int center = canvas.Height / 2;

                int thresh = (int)(center - 25);
                gfx.DrawLine(pgrey, 0, thresh, canvas.Width, thresh);

                for(int i = 0; i < size; i++)
                {
                    int x1 = i * 3 * width;
                    int x2 = x1 + width;
                    int x3 = x2 + width;

                    gfx.FillRectangle(red, x1, center, width, 10);

                    int h0 = (int)(RadiusFromComplex(test[i]));
                    int y0 = center - h0;
                    gfx.FillRectangle(gray, x1, y0, width, h0);

                    if (test[i].Real >= 0)
                    {
                        int h = (int)(height * test[i].Real);
                        int y = center - h;
                        gfx.FillRectangle(blue, x2, y, width, h);
                    }
                    else
                    {
                        int h = -(int)(height * test[i].Real);
                        gfx.FillRectangle(blue, x2, center, width, h);
                    }

                    if (test[i].Imaginary >= 0)
                    {
                        int h = (int)(height * test[i].Imaginary);
                        int y = center - h;
                        gfx.FillRectangle(green, x3, y, width, h);
                    }
                    else
                    {
                        int h = -(int)(height * test[i].Imaginary);
                        gfx.FillRectangle(green, x3, center, width, h);
                    }
                }
            }
        }
    }
}
