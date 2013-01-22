using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using ROMWinRT;
using Windows.UI.Popups;
using System.Diagnostics;
using Windows.UI.Text;
using Windows.UI;
using Windows.Storage.Pickers;
using Windows.Storage;
using System.Text;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace HydraulicCylinderWinRT
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private ROMNode m_rootNode;
        private LinearEngine m_engine = null;
        private bool bLoadingItems = false;        

        public MainPage()
        {
            this.InitializeComponent();
            SetupApplication();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {

        }

        private void SetupApplication()
        {
            string rulesPath = "HydraulicCylinderRules.xml";
            m_rootNode = new ROMNode("HydraulicCylinder");
            if (!m_rootNode.LoadRules(rulesPath))
            {
                ShowMessageBox("Error loading rules file");
            }

            m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
            m_engine.EvaluateAll();

            UpdateControls();
        }

        async void ShowMessageBox(string msg)
        {
            MessageDialog dialog = new MessageDialog(msg);
            await dialog.ShowAsync();
        }

        private void UpdateControls()
        {
            bLoadingItems = true;
            IDictionary<string, ROMDictionaryAttribute> allAttrs = m_engine.GetAllDictionaryAttrs();
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
            IList<string> allChars = m_rootNode.EvaluateTable("CatalogNumber", "Code", true);
            string Catnum = "";
            foreach (string subStr in allChars)
                Catnum += subStr;

            if (Catalog != null)
                Catalog.Text = Catnum;
        }

        private void SetControlUI(ROMDictionaryAttribute attr)
        {
            Control ctrl = (Control)FindVisualChildByName(LayoutRoot, attr.Name);
            if (ctrl != null)
            {
                TextBlock label = (TextBlock)FindVisualChildByName(LayoutRoot, "lb" + attr.Name);
                ctrl.IsEnabled = attr.Enabled;
                if (ctrl is ComboBox)
                {
                    ComboBox combo = (ComboBox)ctrl;
                    combo.Items.Clear();
                    foreach (string item in attr.AvailableValues)
                        combo.Items.Add(item);
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
                            label.Foreground = new SolidColorBrush(Colors.DarkCyan);
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

        private FrameworkElement FindVisualChildByName(FrameworkElement parent, string name)
        {
            if (parent.Name == name)
                return parent;

            int childCnt = VisualTreeHelper.GetChildrenCount(parent);
            for (int i = 0; i < childCnt; i++)
            {
                FrameworkElement found = (FrameworkElement)VisualTreeHelper.GetChild(parent, i);
                if (found != null)
                {
                    if (found.Name == name)
                        return found;
                    else
                    {
                        found = FindVisualChildByName(found, name);
                        if (found != null)
                            return found;
                    }
                }
            }
            return null;
        }

        private void comboEvaluate(object sender, RoutedEventArgs e)
        {
            if (bLoadingItems)
                return;
            m_engine.EvaluateForAttribute(((ComboBox)sender).Name, ((ComboBox)sender).SelectedValue.ToString());
            UpdateControls();
        }

        private string txtValue;
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

        private void text_BeginEdit(object sender, RoutedEventArgs e)
        {
            txtValue = ((TextBox)sender).Text;
        }

        private async void btnOpen_Click(object sender, RoutedEventArgs e)
        {
            FileOpenPicker dialog = new FileOpenPicker();
            dialog.FileTypeFilter.Add(".xml");
            dialog.CommitButtonText = "Open XML file";

            StorageFile file = await dialog.PickSingleFileAsync();

            if (file == null)
                return;

            string contents = null;
            using (StreamReader reader = new StreamReader(await file.OpenStreamForReadAsync()))
            {
                while (!reader.EndOfStream)
                {
                    contents = reader.ReadToEnd();
                }
            }

            //reload
            if (!string.IsNullOrEmpty(contents) && m_rootNode.LoadXML(contents))
            {
                m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
                m_engine.EvaluateAll();

                UpdateControls();
            }
        }

        private async void btnSave_Click(object sender, RoutedEventArgs e)
        {
            FileSavePicker dialog = new FileSavePicker();
            dialog.FileTypeChoices.Add("XML",new string[]{".xml"});
            dialog.CommitButtonText = "Save XML file";
            dialog.SuggestedFileName = "NewConfig";

            StorageFile file = await dialog.PickSaveFileAsync();
            if (file != null)
            {
                string outStr = m_rootNode.SaveXML(true);
                await FileIO.WriteTextAsync(file, outStr);
            }
        }
    }
}
