using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using LogicianSilverlight;

namespace LogicianDebugCtrl
{
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
