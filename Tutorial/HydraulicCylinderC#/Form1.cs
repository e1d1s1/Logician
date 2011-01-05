﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace HydraulicCylinder
{
    public partial class Form1 : Form
    {
        private ROMNET.ROMNodeNET m_rootNode;
        private ROMNET.LinearEngineNET m_engine = null;
        private bool bLoadingItems = false;

        public Form1()
        {
            InitializeComponent();
            SetupApplication();
        }

        private void SetupApplication()
        {
            string rulesPath = "HydraulicCylinderRules.xml";
            m_rootNode = new ROMNET.ROMNodeNET("HydraulicCylinder");
            if (!m_rootNode.LoadRules(rulesPath))
            {
                MessageBox.Show("Error loading rules file");
                Close();
            }

            m_engine = new ROMNET.LinearEngineNET(m_rootNode, "HydraulicCylinderDictionary");
            m_engine.EvaluateAll();

            UpdateControls();
        }

        private void UpdateControls()
        {
            bLoadingItems = true;
            Dictionary<string, ROMNET.ROMDictionaryAttributeNET> allAttrs = m_engine.GetAllDictionaryAttrs();
            foreach (KeyValuePair<string, ROMNET.ROMDictionaryAttributeNET> kvp in allAttrs)
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

            Label catLabel = (Label)this.Controls.Find("Catalog", true)[0];
            if (catLabel != null)            
                catLabel.Text = Catnum;            
        }

        private void SetControlUI(ROMNET.ROMDictionaryAttributeNET attr)
        {
            Control[] ctrls = this.Controls.Find(attr.Name, true);
            if (ctrls != null && ctrls.Length == 1)
            {
                Control ctrl = ctrls[0];
                ctrls = this.Controls.Find("lb" + attr.Name, true);
                Control label = null;
                if (ctrls != null && ctrls.Length == 1)
                    label = ctrls[0];

                ctrl.Enabled = attr.Enabled;
                if (ctrl is ComboBox)
                {
                    ComboBox combo = (ComboBox)ctrl;
                    combo.Items.Clear();
                    combo.Items.AddRange(attr.AvailableValues);
                    combo.Text = m_rootNode.GetAttribute(attr.Name);
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
                        label.Font = new Font(label.Font, FontStyle.Bold);                    
                    else                    
                        label.Font = new Font(label.Font, FontStyle.Regular);

                    if (attr.Valid == true)
                    {
                        if (attr.Enabled)
                            label.ForeColor = Color.Black;
                        else
                            label.ForeColor = Color.Gray;
                    }
                    else
                        label.ForeColor = Color.Red;
                }

#if DEBUG
                Debug.WriteLine(attr.Name + " " + attr.Enabled.ToString());
#endif
            }
        }

        private void comboEvaluate(object sender, EventArgs e)
        {
            if (bLoadingItems)
                return;
            m_engine.EvaluateForAttribute(((ComboBox)sender).Name, ((ComboBox)sender).Text);
            UpdateControls();
        }

        private string txtValue;
        private void text_TextChanged(object sender, EventArgs e)
        {
            if (bLoadingItems)
                return;
            if (txtValue != ((TextBox)sender).Text)
            {
                m_engine.EvaluateForAttribute(((TextBox)sender).Name, ((TextBox)sender).Text);
                UpdateControls();
            }
        }

        private void text_BeginEdit(object sender, EventArgs e)
        {
            txtValue = ((TextBox)sender).Text;
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Filter = "XML files (*.xml)|*.xml";
            dialog.Title = "Open XML file";

            if (dialog.ShowDialog() == DialogResult.Cancel)
                return;

            System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
            doc.Load(dialog.FileName);

            //reload
            if (m_rootNode.LoadXML(doc.InnerXml))
            {
                m_engine = new ROMNET.LinearEngineNET(m_rootNode, "HydraulicCylinderDictionary");
                m_engine.EvaluateAll();

                UpdateControls();
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.Filter = "XML files (*.xml)|*.xml";
            dialog.Title = "Save XML file";

            if (dialog.ShowDialog() == DialogResult.Cancel)
                return;

            string outStr = m_rootNode.SaveXML(true);
            UTF8Encoding encoding = new UTF8Encoding(false);
            System.IO.File.WriteAllText(dialog.FileName, outStr, encoding);     
        }
    }
}