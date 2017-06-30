using adfilter.Model;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace adfilter.ViewModel
{
    abstract class HostViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged([CallerMemberName] string name = null)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(name));
        }


        protected abstract bool AddHost(string host);

        protected abstract bool DelHost(string host);

        private ICommand addCommand;
        public ICommand AddCommand
        {
            get
            {
                return addCommand ?? (addCommand = new BaseCommand
                    {
                        ExecuteDelegate = x => AddHost(x.ToString())
                    });
            }
        }

        private ICommand delCommand;
        public ICommand DelCommand
        {
            get
            {
                return delCommand ?? (delCommand = new BaseCommand
                    {
                        ExecuteDelegate = x => DelHost(x.ToString())
                    });
            }
        }
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

    }
}
