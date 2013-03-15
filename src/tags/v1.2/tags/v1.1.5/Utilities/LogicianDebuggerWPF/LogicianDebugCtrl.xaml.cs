using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ROMNET;

namespace LogicianDebuggerWPF
{
    /// <summary>
    /// Interaction logic for LogicianDebugCtrl.xaml
    /// </summary>
    public partial class LogicianDebugCtrl : UserControl
    {
        public LogicianDebugCtrl()
        {
            InitializeComponent();
        }

        public ROMNode RootNode
        {
            get { return Viewer.RootNode; }
            set { Viewer.RootNode = value; EDSDebugger.ROM = value; }
        }

        public void Update()
        {
            Viewer.Update();
        }
    }
}
