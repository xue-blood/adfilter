using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace adfilter
{
    public class Adf : IDisposable
    {

        #region c function
        [DllImport("adfcon.dll")]
        static extern IntPtr adf_open();

        [DllImport("adfcon.dll")]
        static extern void adf_close(IntPtr handle);
        #endregion

        static Adf instance = null;
        public static Adf Instance
        {
            get { return instance ?? (new Adf()); }
        }

        IntPtr handle;

        public Adf()
        {
            handle = adf_open();
        }

        public bool Invalid
        {
            get { return (handle.ToInt32() == -1); }
        }

        public void Close()
        {
            adf_close(handle);
        }


        public void Dispose()
        {
            adf_close(handle);
        }
    }
}
