using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media.Animation;

namespace adfilter.Model
{
    class Msg
    {

        private static Msg instance;
        public static Msg Instance 
        {
            get { return instance ?? (instance = new Msg()); }
        }

        public TextBlock MsgHost { get; set; }
        public void Show(string msg)
        {
            MsgHost.Text = msg;
            Storyboard sb = MsgHost.Resources["Msg_Amimate"] as Storyboard;
            if (sb != null)
                MsgHost.BeginStoryboard(sb);
        }
    }
}
