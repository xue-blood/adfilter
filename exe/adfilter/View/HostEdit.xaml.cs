using adfilter.Model;
using adfilter.ViewModel;
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


        private void Add_Click(object sender, RoutedEventArgs e)
        {
            HostViewModel vm = this.DataContext as HostViewModel;
            vm.Hosts.Add(new HostList(""));

            this.grid.SelectedIndex = vm.Hosts.Count -1;
            this.grid.Items.Refresh();

        }


        private void Del_Click(object sender, RoutedEventArgs e)
        {
            if (grid.SelectedItem == null) return;

            HostViewModel vm = this.DataContext as HostViewModel;
            vm.Hosts.RemoveAt(this.grid.SelectedIndex);
            string host = (grid.SelectedItem as HostList).Host;

            vm.DelCommand.Execute(host);

            // refresh view after delete
            // see:https://stackoverflow.com/questions/930350/how-to-refresh-a-wpf-datagrid
            grid.Items.Refresh();
        }

        string org_host;

        void s_LostFocus(object sender, RoutedEventArgs e)
        {
            string new_host = (sender as TextBox).Text;
            if (new_host == org_host) return;

            HostViewModel vm = this.DataContext as HostViewModel;
            // delete org host
            vm.DelCommand.Execute(org_host);
            // add new host
            vm.AddCommand.Execute(new_host);
        }

        private void grid_BeginningEdit(object sender, DataGridBeginningEditEventArgs e)
        {
            org_host = (grid.SelectedItem as HostList).Host;
        }
    }
}
