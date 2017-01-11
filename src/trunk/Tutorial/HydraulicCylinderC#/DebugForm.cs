using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HydraulicCylinder
{
    public partial class DebugForm : Form
    {
        public DebugForm()
        {
            InitializeComponent();
        }

        public void Initialize(ROMNET.ROMNode node)
        {
            ((LogicianDebuggerWPF.LogicianDebugCtrl)Debugger.Child).RootNode = node;
        }

        public void Update()
        {
            ((LogicianDebuggerWPF.LogicianDebugCtrl)Debugger.Child).Update();
        }
    }
}
