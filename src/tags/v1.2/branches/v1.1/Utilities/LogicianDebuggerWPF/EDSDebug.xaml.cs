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
using EDSNET;
using ROMNET;

namespace LogicianDebuggerWPF
{
    public partial class EDSDebug : UserControl
    {
        EDSEngine _Knowledgebase = null;
        ROMNode _rom = null;
        public EDSEngine Knowledge
        {
            get { return _Knowledgebase; }
            set { _Knowledgebase = value; _Knowledgebase.DebugDelegate = Update; }
        }

        public ROMNode ROM
        {
            get { return _rom; }
            set { _rom = value; _rom.DebugDelegate = Update; }
        }

        public EDSDebug()
        {
            InitializeComponent();
        }

        public void Update(string msg)
        {
            if (rbAll.IsChecked == true)
            {
                txtLog.Text += msg + "\n";
            }
            else
            {
                string[] msgs = msg.Split(new string[] { "\n" }, StringSplitOptions.RemoveEmptyEntries);
                foreach (string s in msgs)
                {
                    int start = 17; //sizeof("<TableEval name=\"");
                    int end = msg.IndexOf('"', start);
                    string tableName = s.Substring(start, end - start);
                    string filter = txtTable.Text;
                    if (rbAll.IsChecked == true || (string.IsNullOrEmpty(filter) || filter.Contains(tableName)))
                    {
                        txtLog.Text += s + "\n";
                    }
                }
            }
        }

        private void btnClear_Click(object sender, RoutedEventArgs e)
        {
            txtLog.Text = "";
        }
    }
}
