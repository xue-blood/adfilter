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
        [DllImport("adf.dll")]
        static extern IntPtr adf_open();

        [DllImport("adf.dll")]
        static extern void adf_close(IntPtr handle);

        [DllImport("adf.dll")]
        static extern bool adf_set_pause(IntPtr handle, bool pause);

        [DllImport("adf.dll")]
        static extern bool adf_get_pause(IntPtr handle);


        [DllImport("adf.dll")]
        static extern bool adf_host(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string host, int host_len, bool add, bool except);


        #endregion

        static Adf instance = null;
        public static Adf Instance
        {
            get { return instance ?? ( instance = new Adf()); }
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

        public bool Pause
        {
            get { return adf_get_pause(handle); }
            set { adf_set_pause(handle, value); }
        }


        public bool AddHost(string host,bool allow)
        {
            if (host.Length <= 1) return false;
            return adf_host(handle, host, host.Length, true, allow);
        }

        public bool DelHost(string host,bool allow)
        {
            if (host.Length <= 1) return false;
            return adf_host(handle, host, host.Length, false, allow);
        }

        public void Dispose()
        {
            adf_close(handle);
        }
    }
}
