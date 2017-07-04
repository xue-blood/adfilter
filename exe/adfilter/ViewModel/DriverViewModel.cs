using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

using adfilter.Model;
using System.Windows.Input;

namespace adfilter.ViewModel
{
    class DriverViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        void RaisePropertyChanged([CallerMemberName] string name=null)
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(name));
        }

        private ICommand startCommand;
        public ICommand StartCommand
        {
            get
            {
                return startCommand ?? (startCommand = new BaseCommand
                {
                    ExecuteDelegate = x => 
                        {
                            Adf.Instance.Pause = false;
                            Msg.Instance.Show("Driver started");
                        }
                });
            }
        }

        private ICommand stopCommand;
        public ICommand StopCommand
        {
            get
            {
                return stopCommand ?? (stopCommand = new BaseCommand
                    {
                        ExecuteDelegate = x =>
                        {
                            Adf.Instance.Pause = true;
                            Msg.Instance.Show("Driver stopped");
                        }
                    });
            }
        }
    }
}
