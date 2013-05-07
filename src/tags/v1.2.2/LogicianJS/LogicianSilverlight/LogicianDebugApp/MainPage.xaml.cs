﻿using System;
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
using System.Xml;
using System.Xml.Linq;
using LogicianSilverlight;

namespace LogicianDebugApp
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
            SetupApplication();
        }

        private void SetupApplication()
        {
            XDocument doc = XDocument.Load("config.xml");
            ROMNode node = new ROMNode();
            if (!node.LoadXML(doc.ToString()))
                return;

            ROMNode childNode = new ROMNode("Child1");
            childNode.SetAttribute("attr1", "A child attr");
            node.AddChildROMObject(childNode);
            ROMNode childNode2 = new ROMNode("Child2");
            childNode2.SetAttribute("attr2", "A child attr");
            node.AddChildROMObject(childNode2);
            ROMNode childNode3 = new ROMNode("Child3");
            childNode3.SetAttribute("attr3", "A child attr");
            childNode3.SetAttribute("attr3", "attrSub1", "A sub child attr");
            childNode3.SetAttribute("attr3", "attrSub2", "A 2nd sub child attr");
            childNode2.AddChildROMObject(childNode3);

            Debugger.RootNode = node;
            Debugger.Update();
        }
    }
}