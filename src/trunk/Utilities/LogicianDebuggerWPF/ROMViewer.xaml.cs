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
using Microsoft.Windows.Controls;
using System.Collections.ObjectModel;
using System.Xml;
using System.IO;
using System.Windows.Markup;
using ROMNET;

namespace LogicianDebuggerWPF
{
    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
    public partial class ROMViewer : UserControl
    {
        private ROMNode m_rootNode = null;
        private bool m_bLoading = false;

        public ROMViewer()
        {
            InitializeComponent();
        }

        public ROMNode RootNode
        {
            get { return m_rootNode; }
            set { m_rootNode = value; Update(); }
        }

        public void Update()
        {
            if (m_rootNode == null)
                return;

            m_bLoading = true;
            ROMNode node = null;
            TreeViewItem item = (TreeViewItem)treeCtrl.SelectedItem;
            if (item != null)
            {
                node = (ROMNode)item.Tag;
            }

            BuildTree();
            item = null;
            if (node != null)
            {
                item = (TreeViewItem)treeCtrl.FindName("_" + node.GetROMGUID().Replace('-', '_'));
            }

            if (item != null)
            {
                item.IsSelected = true;
                DisplayAttributeGrid(item);
            }
            else
            {
                if (treeCtrl.Items.Count > 0)
                {
                    ((TreeViewItem)treeCtrl.Items[0]).IsSelected = true;
                    DisplayAttributeGrid((TreeViewItem)treeCtrl.Items[0]);
                }
            }
            m_bLoading = false;
        }

        public void BuildTree()
        {
            treeCtrl.Items.Clear();
            string caption = m_rootNode.GetROMObjectID();
            TreeViewItem current = new TreeViewItem();
            current.Header = caption;
            current.Tag = m_rootNode;
            current.Name = "_" + m_rootNode.GetROMGUID().Replace('-', '_');
            try
            {
                treeCtrl.UnregisterName(current.Name);
            }
            catch { }
            treeCtrl.RegisterName(current.Name, current);
            current.IsSelected = true;
            treeCtrl.Items.Add(current);

            ROMNode[] children = m_rootNode.GetAllChildren(false);
            if (children != null) foreach (ROMNode child in children)
                {
                    AddBranchAndChildren(current, child);
                }
        }

        public void AddBranchAndChildren(TreeViewItem parent, ROMNode node)
        {
            string caption = node.GetROMObjectID();
            TreeViewItem current = null;
            if (parent != null)
            {
                current = new TreeViewItem();
                current.Header = caption;
                current.Tag = node;
                current.Name = "_" + node.GetROMGUID().Replace('-', '_');
                try
                {
                    treeCtrl.UnregisterName(current.Name);
                }
                catch { }
                treeCtrl.RegisterName(current.Name, current);
                parent.Items.Add(current);
            }

            ROMNode[] children = node.GetAllChildren(false);
            if (children != null) foreach (ROMNode child in children)
                {
                    AddBranchAndChildren(current, child);
                }
        }

        private void DisplayAttributeGrid(TreeViewItem item)
        {
            grid.ItemsSource = null;
            if (item != null)
            {
                ROMNode node = (ROMNode)item.Tag;
                if (node != null)
                {
                    List<ROMAttrData> table = new List<ROMAttrData>();
                    Dictionary<string, Dictionary<string, string>> allAttrs = node.GetAllAttributes();

                    foreach (string attr in allAttrs.Keys)
                    {
                        ROMAttrData row = new ROMAttrData();
                        Dictionary<string, string> attrValuePair = allAttrs[attr];
                        row.Attribute = attr;
                        row.Value = attrValuePair["value"];
                        foreach (KeyValuePair<string, string> kvp in attrValuePair)
                        {
                            if (kvp.Key != "value")
                            {
                                if (row.SubValues == null)
                                    row.SubValues = new ObservableCollection<string>();
                                row.SubValues.Add(kvp.Key + ": " + kvp.Value);
                            }
                        }
                        table.Add(row);
                    }

                    FillGrid(table);
                }
            }
        }

        public class ROMAttrData
        {
            public string Attribute { get; set; }
            public string Value { get; set; }
            public ObservableCollection<string> SubValues { get; set; }
        }

        private void FillGrid(List<ROMAttrData> table)
        {
            grid.ItemsSource = null;
            grid.Columns.Clear();

            Microsoft.Windows.Controls.DataGridTextColumn attrCol = CreateTextColumn("attr", "attr");
            attrCol.Binding = new System.Windows.Data.Binding("Attribute");
            grid.Columns.Add(attrCol);
            Microsoft.Windows.Controls.DataGridTextColumn valueCol = CreateTextColumn("value", "value");
            valueCol.Binding = new System.Windows.Data.Binding("Value");
            grid.Columns.Add(valueCol);

            int AdditionalCols = 0;
            foreach (ROMAttrData row in table)
            {
                int cnt = 0;
                if (row.SubValues != null) foreach (string s in row.SubValues)
                    {
                        cnt++;
                        if (cnt > AdditionalCols)
                            AdditionalCols = cnt;
                    }
            }

            for (int i = 0; i < AdditionalCols; i++)
            {
                Microsoft.Windows.Controls.DataGridTemplateColumn col = CreateTemplateColumn(i);
                if (col != null)
                    grid.Columns.Add(col);
            }

            grid.ItemsSource = table;
        }

        private Microsoft.Windows.Controls.DataGridTextColumn CreateTextColumn(string fieldName, string title)
        {
            Microsoft.Windows.Controls.DataGridTextColumn column = new Microsoft.Windows.Controls.DataGridTextColumn();
            column.Header = title;
            column.Binding = new System.Windows.Data.Binding(fieldName);
            return column;
        }

        private StreamReader CreateColumnTemplate(int index)
        {
            StringBuilder CellTemp = new StringBuilder();
            CellTemp.Append("<DataTemplate ");
            CellTemp.Append("xmlns='http://schemas.microsoft.com/winfx/");
            CellTemp.Append("2006/xaml/presentation' ");
            CellTemp.Append("xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>");
            CellTemp.Append(String.Format("<TextBlock Text='{{Binding SubValues[{0}]}}' VerticalAlignment='Center' HorizontalAlignment='Center'/>", index));
            CellTemp.Append("</DataTemplate>");
            MemoryStream stream = new MemoryStream(System.Text.Encoding.UTF8.GetBytes(CellTemp.ToString()));
            return new StreamReader(stream);
        }

        private Microsoft.Windows.Controls.DataGridTemplateColumn CreateTemplateColumn(int i)
        {
            try
            {
                Microsoft.Windows.Controls.DataGridTemplateColumn column = new Microsoft.Windows.Controls.DataGridTemplateColumn();
                column.Header = "sub-value";

                object o = XamlReader.Load(new XmlTextReader(CreateColumnTemplate(i))); //display template
                column.CellTemplate = (DataTemplate)o;
                return column;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.Write(ex.Message);
                return null;
            }
        }

        private void DisplayXML(TreeViewItem item)
        {
            text.Text = "";
            if (item != null)
            {
                ROMNode node = (ROMNode)item.Tag;
                string xml = node.SaveXML(true);
                text.Text = xml;
            }
        }

        private void TreeItemSelected(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (m_bLoading)
                return;
            TreeViewItem item = (TreeViewItem)treeCtrl.SelectedItem;
            if (item != null)
            {
                if (rbAttr.IsChecked == true)
                {
                    DisplayAttributeGrid(item);
                }
                else
                {
                    DisplayXML(item);
                }
            }
        }

        private void btnGet_Click(object sender, RoutedEventArgs e)
        {
            string attrName = txtName.Text;
            if (attrName.Length > 0)
            {
                TreeViewItem item = (TreeViewItem)treeCtrl.SelectedItem;
                if (item != null)
                {
                    ROMNode node = (ROMNode)item.Tag;
                    if (node != null)
                    {
                        string subAttrName = "value";
                        int index = attrName.IndexOf(':');
                        if (index >= 0)
                        {
                            subAttrName = attrName.Substring(index + 1);
                            attrName = attrName.Substring(0, index);
                        }
                        bool immediate = (bool)checkImmediate.IsChecked;
                        string value = node.GetAttribute(attrName, subAttrName, immediate);
                        txtValue.Text = value;
                    }
                }
            }
        }

        private void btnSet_Click(object sender, RoutedEventArgs e)
        {
            string attrName = txtName.Text;
            string attrValue = txtValue.Text;
            if (attrName.Length > 0)
            {
                TreeViewItem item = (TreeViewItem)treeCtrl.SelectedItem;
                if (item != null)
                {
                    ROMNode node = (ROMNode)item.Tag;
                    if (node != null)
                    {
                        string subAttrName = "value";
                        int index = attrName.IndexOf(':');
                        if (index >= 0)
                        {
                            subAttrName = attrName.Substring(index + 1);
                            attrName = attrName.Substring(0, index);
                        }
                        node.SetAttribute(attrName, subAttrName, attrValue);
                        if (rbAttr.IsChecked == true)
                        {
                            DisplayAttributeGrid(item);
                        }
                        else
                        {
                            DisplayXML(item);
                        }
                    }
                }
            }
        }

        private void buttonQuery_Click(object sender, RoutedEventArgs e)
        {
            string query = txtQuery.Text;
            if (query.Length > 0)
            {
                TreeViewItem item = (TreeViewItem)treeCtrl.SelectedItem;
                if (item != null)
                {
                    ROMNode node = (ROMNode)item.Tag;
                    if (node != null)
                    {
                        try
                        {
                            if (checkQueryValue.IsChecked == true) //rule table input type attr query
                            {
                                ROMNode[] results = node.FindObjects(query);
                                string msg = results.Length.ToString() + " Objects Found";
                                MessageBox.Show(msg, "XPATH Result", MessageBoxButton.OK);

                                //highlight the objects in the tree
                                foreach (ROMNode nodeRes in results)
                                {
                                    item = (TreeViewItem)treeCtrl.FindName("_" + nodeRes.GetROMGUID().Replace('-', '_'));
                                    if (item != null)
                                    {
                                        item.IsSelected = true;
                                        item.Foreground = new SolidColorBrush(Colors.Red);
                                        object newParent = item.Parent;
                                        TreeViewItem parent = null;
                                        if (newParent != null && newParent is TreeViewItem)
                                            parent = (TreeViewItem)newParent;
                                        while (parent != null)
                                        {
                                            parent.IsExpanded = true;
                                            newParent = parent.Parent;
                                            if (newParent != null && newParent is TreeViewItem)
                                                parent = (TreeViewItem)newParent;
                                            else
                                                parent = null;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                string msg = "value: ";
                                msg += node.EvaluateXPATH(query);
                                MessageBox.Show(msg, "XPATH Result", MessageBoxButton.OK);
                            }
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show(ex.Message, "Query error", MessageBoxButton.OK);
                        }
                    }
                }
            }
        }

        private void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            Update();
        }

        private void rbAttr_Checked(object sender, RoutedEventArgs e)
        {
            if (m_rootNode == null)
                return;

            if (rbAttr != null && (bool)rbAttr.IsChecked == true)
            {
                textScroller.Visibility = System.Windows.Visibility.Collapsed;
                grid.Visibility = System.Windows.Visibility.Visible;
                TreeViewItem item = (TreeViewItem)treeCtrl.SelectedItem;
                if (item != null)
                {
                    DisplayAttributeGrid(item);
                }
            }
        }

        private void rbXML_Checked(object sender, RoutedEventArgs e)
        {
            if (m_rootNode == null)
                return;

            if (rbXML != null && (bool)rbXML.IsChecked == true)
            {
                textScroller.Visibility = System.Windows.Visibility.Visible;
                grid.Visibility = System.Windows.Visibility.Collapsed;
                TreeViewItem item = (TreeViewItem)treeCtrl.SelectedItem;
                if (item != null)
                {
                    DisplayXML(item);
                }
            }
        }
    }
}
