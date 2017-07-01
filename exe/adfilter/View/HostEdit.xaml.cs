using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace adfilter.View
{
    /// <summary>
    /// Interaction logic for HostEdit.xaml
    /// </summary>
    public partial class HostEdit : UserControl
    {
        public HostEdit()
        {
            InitializeComponent();

            // make sure this function will be called when exiting program
            // see:https://stackoverflow.com/questions/14479038/how-to-fire-unload-event-of-usercontrol-in-a-wpf-window
            this.Dispatcher.ShutdownStarted += Dispatcher_ShutdownStarted;
        }

        void Dispatcher_ShutdownStarted(object sender, EventArgs e)
        {
            ((IDisposable)this.DataContext).Dispose();
        }

    }
}
