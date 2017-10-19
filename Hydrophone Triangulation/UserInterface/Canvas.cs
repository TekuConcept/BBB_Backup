using System;
using System.Windows.Forms;

namespace UserInterface
{
    public class Canvas : Panel
    {
        public Canvas()
        {
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            SetStyle(ControlStyles.ResizeRedraw, true);
            SetStyle(ControlStyles.UserPaint, true);
        }
    }
}
