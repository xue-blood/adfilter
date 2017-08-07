using adfilter.Model;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Input;

using Res = adfilter.Properties.Resources;

namespace adfilter.ViewModel
{
    abstract class HostViewModel : INotifyPropertyChanged, IDisposable
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged([CallerMemberName] string name = null)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(name));
        }

        public List<HostList> Hosts { get; set; }

        protected abstract bool AddHost(string host);

        protected abstract bool DelHost(string host);

        protected abstract string RegKey { get; }

        #region function
        public void Add(string host)
        {
            // check is a valid host name
            if( !CheckHost(host))
            {
                Msg.Instance.Show(Res.msg_host_invalid,true);
                return;
            }

            // is host already in list
            if (Hosts.Find(x => x.Host == host) != null)
            {
                Msg.Instance.Show(Res.msg_host_exist,true);
                return;
            }

            if (AddHost(host))
                Hosts.Add(new HostList(host));

            Msg.Instance.Show(Res.msg_host_add);
        }

        public void Del(string host)
        {
            if (Hosts.Count == 0) return;

            // find host index
            int index = Hosts.FindIndex(x => x.Host == host);
            if (index < 0 || index > Hosts.Count - 1) return;

            if (DelHost(Hosts[index].Host))
                Hosts.RemoveAt(index);

            Msg.Instance.Show(Res.msg_host_del);
        }

        public void Del(int index)
        {
            if (index < 0 || index > Hosts.Count - 1) return;

            if (Hosts.Count == 0) return;


            if (DelHost(Hosts[index].Host))
                Hosts.RemoveAt(index);

            Msg.Instance.Show(Res.msg_host_del);
        }

        bool CheckHost(string host)
        {
            if (host.Split('.').Length == 1) return false;

            return true;
        }

        string GetFilePath()
        {
            using (RegistryKey key = Registry.LocalMachine.OpenSubKey(@"SYSTEM\CurrentControlSet\services\adfilter") )
            {
                return (key.GetValue(RegKey) as string).Substring(4);
            }
        }

        public HostViewModel()
        {
            Hosts = new List<HostList>();

            try
            {
                using (StreamReader sr = new StreamReader(GetFilePath()))
                {
                    string l;
                    while ((l = sr.ReadLine()) != null)
                    {
                        if (l.Length > 0)
                            Hosts.Add(new HostList(l));
                    }
                }
            }
            catch(Exception e)
            {
                //MessageBox.Show(e.Message);
            }
        }

        public void Dispose()
        {
            try
            {
                using (StreamWriter sw = new StreamWriter(GetFilePath()))
                {
                    foreach (HostList h in Hosts)
                    {
                        if(h.Host.Length > 0)
                            sw.WriteLine(h.Host);
                    }

                    sw.Flush();
                }
            }
            catch (Exception e)
            {
                //MessageBox.Show(e.Message);
            }
        }

        #endregion
    }

    class WhiteHostViewModel : HostViewModel
    {
        protected override bool AddHost(string host)
        {
            return Adf.Instance.AddHost(host, true);
        }

        protected override bool DelHost(string host)
        {
            return Adf.Instance.DelHost(host, true);
        }

        protected override string RegKey
        {
            get { return "ExceptFilePath"; }
        }
    }

    class BlackHostViewModel : HostViewModel
    {
        protected override bool AddHost(string host)
        {
            return Adf.Instance.AddHost(host, false);
        }

        protected override bool DelHost(string host)
        {
            return Adf.Instance.DelHost(host, false);
        }

        protected override string RegKey
        {
            get { return "UserFilePath"; }
        }
    }
}
