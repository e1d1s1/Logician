using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ROMNETTestApplication
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            
        }

        public void Log(string s)
        {
            richTextBox1.AppendText(s);
        }

        private void richTextBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            Close();
        }
    }
}
