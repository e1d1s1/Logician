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
using System.Windows.Browser;
using System.Diagnostics;
using System.Text;
using System.IO;

namespace HydraulicCylinderSilverlight
{
    public partial class MainPage : UserControl
    {
        private bool bLoadingItems = false;
        private ROMNode m_rootNode;
        private LinearEngine m_engine = null;

        public MainPage()
        {
            InitializeComponent();
            SetupApplication();
        }

        private void SetupApplication()
        {
            string rulesPath = "HydraulicCylinderRules.xml";
            m_rootNode = new ROMNode("HydraulicCylinder");
            if (!m_rootNode.LoadRules(rulesPath))
            {
                MessageBox.Show("Error loading rules file");
                HtmlPage.Window.Eval("window.close()");
            }

            m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
            m_engine.EvaluateAll();

            UpdateControls();
        }

        private void UpdateControls()
        {
            bLoadingItems = true;
            Dictionary<string, ROMDictionaryAttribute> allAttrs = m_engine.GetAllDictionaryAttrs();
            foreach (KeyValuePair<string, ROMDictionaryAttribute> kvp in allAttrs)
            {
                SetControlUI(kvp.Value);
            }
            bLoadingItems = false;
            UpdateCatalog();
        }

        private void UpdateCatalog()
        {
            //catalog number is the concat of all the chars returned from the CatalogNumber table evaluation
            string[] allChars = m_rootNode.EvaluateTable("CatalogNumber", "Code", true);
            string Catnum = "";
            foreach (string subStr in allChars)
                Catnum += subStr;

            if (Catalog != null)
                Catalog.Text = Catnum;
        }

        private void SetControlUI(ROMDictionaryAttribute attr)
        {
            try
            {
                Control ctrl = (Control)FindName(attr.Name);
                TextBlock label = (TextBlock)FindName("lb" + attr.Name);

                if (ctrl != null)
                {
                    ctrl.IsEnabled = attr.Enabled;
                    if (ctrl is ComboBox)
                    {
                        ComboBox combo = (ComboBox)ctrl;
                        combo.Items.Clear();
                        foreach (string val in attr.AvailableValues)
                            combo.Items.Add(val);
                        combo.SelectedValue = m_rootNode.GetAttribute(attr.Name);
                    }
                    else if (ctrl is TextBox)
                    {
                        TextBox txtBox = (TextBox)ctrl;
                        txtBox.Text = m_rootNode.GetAttribute(attr.Name);
                    }

                    if (label != null)
                    {
                        label.Text = attr.Description;
                        if (attr.ChangedByUser == true)
                            label.FontWeight = FontWeights.Bold;
                        else
                            label.FontWeight = FontWeights.Normal;

                        if (attr.Valid == true)
                        {
                            if (attr.Enabled)
                                label.Foreground = new SolidColorBrush(Colors.Black);
                            else
                                label.Foreground = new SolidColorBrush(Colors.Gray);
                        }
                        else
                            label.Foreground = new SolidColorBrush(Colors.Red);
                    }

#if DEBUG
                    Debug.WriteLine(attr.Name + " " + attr.Enabled.ToString());
#endif
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(attr.Name + ":" + ex.Message);
            }
        }

        private void btnOpen_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Filter = "XML files (*.xml)|*.xml";

            if (dialog.ShowDialog() == false)
                return;

            string contents = null;
            StreamReader reader = new StreamReader(dialog.File.OpenRead());
            while (!reader.EndOfStream)
            {
                contents = reader.ReadToEnd();
            }

            //reload
            if (!string.IsNullOrEmpty(contents) && m_rootNode.LoadXML(contents))
            {
                m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
                m_engine.EvaluateAll();

                UpdateControls();
            }
        }

        private void btnSave_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.Filter = "XML files (*.xml)|*.xml";

            if (dialog.ShowDialog() == false)
                return;

            string outStr = m_rootNode.SaveXML(true);
            using (Stream stream = dialog.OpenFile())
            {
                StreamWriter sw = new StreamWriter(stream, System.Text.Encoding.UTF8);
                sw.Write(outStr);
                sw.Close();
                stream.Close();
            }
        }

        private void btnExit_Click(object sender, RoutedEventArgs e)
        {
            HtmlPage.Window.Eval("window.close()");
        }

        private void comboEvaluate(object sender, SelectionChangedEventArgs e)
        {
            if (bLoadingItems)
                return;
            m_engine.EvaluateForAttribute(((ComboBox)sender).Name, ((ComboBox)sender).SelectedValue.ToString());
            UpdateControls();
        }

        private string txtValue;
        private void text_BeginEdit(object sender, RoutedEventArgs e)
        {
            txtValue = ((TextBox)sender).Text;
        }

        private void text_TextChanged(object sender, RoutedEventArgs e)
        {
            if (bLoadingItems)
                return;
            if (txtValue != ((TextBox)sender).Text)
            {
                m_engine.EvaluateForAttribute(((TextBox)sender).Name, ((TextBox)sender).Text);
                UpdateControls();
            }
        }
    }
}
