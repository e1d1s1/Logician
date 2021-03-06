﻿using System;
using System.Collections.Generic;
using System.Text;
using ROMNET;
using System.Windows.Forms;
using ROMNETTestApplication;

namespace ROM2NETTestApplication
{
    class Program
    {
        static MainForm frm = null;

        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            frm = new MainForm();
            frm.Shown += delegate(object sender, EventArgs e)
            {
                long start = 0, elapsed = 0, testROMNodeStart = 0, testROMNodeEnd = 0;
                string msg;
                const int iMax = 1000;
                string filename = "test_project.gz";
                string rulesPath = "..\\..\\..\\..\\..\\EDSEngine\\EDSEngineTestApp\\" + filename;
                if (args.Length > 0 && !string.IsNullOrEmpty(args[0]))
                    rulesPath = args[0];

                Log("Creating the root node");
                ROMNode rootNode = new ApplicationObject("TestApplication", ObjectFactory);
                Log("Root created");

                Log("Setting some attributes");
                rootNode.SetAttribute("inputAttr1", "some value of test1");
                rootNode.SetAttribute("inputAttr2", "some value of test2");
                rootNode.SetAttribute("inputAttr3", "some value of test3");
                rootNode.SetAttribute("inputAttr3", "test3a", "some sub value of attr3_a");
                rootNode.SetAttribute("inputAttr3", "test3b", "some sub value of attr3_b");
                Log("Attrs set");

                CreateChildNodes(rootNode);
                ROMNode[] findTest = rootNode.FindAllObjectsByID("DerivedObject", true);
                ROMNode[] findTestXPATH = rootNode.FindObjects("//Object[@id='DerivedObject']");
                ROMNode[] findTestXPATH2 = rootNode.FindObjects("//Object[@id='DerivedObject2']");
                if (findTest.Length == 1 && findTestXPATH.Length == 1 && findTestXPATH2.Length == 1 &&
                    findTest[0].GetROMGUID() == findTestXPATH[0].GetROMGUID() &&
                    findTestXPATH2[0].GetROMObjectID() == "DerivedObject2")
                    Log("OK");
                else
                    Log("FAILURE creating/obtaining child object");

                Log("Dump current xml state");
                string s = rootNode.SaveXML(true);
                Log(s);

                Log("Cloning the root object");
                var clonedObj = rootNode.Clone();
                string testClone = clonedObj.SaveXML(true);
                var rootChildCnt = rootNode.GetAllChildren(true).Length;
                var cloneChildCnt = clonedObj.GetAllChildren(true).Length;
                if (s.Length == testClone.Length &&
                    rootChildCnt == cloneChildCnt)
                    Log("Clone OK");
                else
                    Log("FAILURE Cloning root");

                Log("Test the object factory and inheritence pattern on the clone");
                ApplicationObject rootTest = (ApplicationObject)clonedObj.FindAllObjectsByID("TestApplication", true)[0];
                DerivedObject2 childTest = (DerivedObject2)clonedObj.FindObjects("//Object[@id='DerivedObject2']")[0];
                if (rootTest.GetAttribute("CLASS", true) == "ApplicationObject" &&
                    childTest.GetAttribute("CLASS", true) == "DerivedObject2")
                    Log("inheritence OK");
                else
                    Log("FAILURE in inheritence pattern of cloned object");

                Log("Test loading from xml");
                ROMNode loadedObj = ROMNode.LoadXMLFromString(s, ObjectFactory);
                string testLoaded = loadedObj.SaveXML(true);
                if (s.Length == testLoaded.Length &&
                    rootNode.GetAllChildren(true).Length == loadedObj.GetAllChildren(true).Length)
                    Log("XML loading OK");
                else
                    Log("FAILURE loading from xml");

                Log("Test the object factory and inheritence pattern of loaded xml");
                rootTest = (ApplicationObject)loadedObj.FindAllObjectsByID("TestApplication", true)[0];
                childTest = (DerivedObject2)loadedObj.FindObjects("//Object[@id='DerivedObject2']")[0];
                if (rootTest.GetAttribute("CLASS", true) == "ApplicationObject" &&
                    childTest.GetAttribute("CLASS", true) == "DerivedObject2")
                    Log("inheritence OK");
                else
                    Log("FAILURE in inheritence pattern of loaded xml");

                Log("Test for equlity");
                DerivedObject2 childTest2 = (DerivedObject2)loadedObj.FindObjects("//Object[@id='DerivedObject2']")[0];
                if (childTest == childTest2 && childTest != rootTest)
                    Log("equality test OK");
                else
                    Log("FAILURE equality test");

                Log("Setting attrs to test eval, inputAttr1 = A, inputAttr2 = 10, outsideAttr1 = 28");
                rootNode.SetAttribute("inputAttr1", "A");
                rootNode.SetAttribute("inputAttr2", "10");
                rootNode.SetAttribute("outsideAttr1", "28");
                Log("loading rules: " + rulesPath);

                EDSNET.EDSEngine rules = new EDSNET.EDSEngine(rulesPath);
                if (!rules.IsOpen())
                    rules = new EDSNET.EDSEngine(filename);
                if (rules.IsOpen())
                {
                    rules.DebugDelegate = DebugMessage;
                    rules.EnableRemoteDebugger(false);
                    EDSNET.InputValueGetterDelegate getValueDelegate = (string attrName, object context) =>
                    {
                        ROMNode objContext = (ROMNET.ROMNode)context;
                        return objContext.GetAttribute(attrName, false);
                    };
                    rules.InputGetterDelegate = getValueDelegate;

                    rootNode.Rules = rules;

                    Log("...loaded");
                    Log("Evaluating table testtable1");
                    string[] res = rootNode.EvaluateTable("testtable1", "outputAttr1", true);
                    foreach (string str in res)
                    {
                        Log(str);
                    }
                    Log("Evaluation complete");

                    Log("Evaluating table testtable2: out1");
                    string[] res2 = rootNode.EvaluateTable("testtable2", "out1", true);
                    foreach (string str in res2)
                    {
                        Log(str);
                    }
                    Log("Evaluation complete");

                    Log("Testing the LinearEngine class");
                    LinearEngine engine = new LinearEngine(rootNode, "Dictionary");
                    engine.InitializeEngine();

                    Log("Checking dictionary size");
                    Dictionary<string, ROMDictionaryAttribute> attrs = engine.GetAllDictionaryAttrs();
                    if (attrs.Count == 6)
                        Log("size ok");
                    else
                        Log("FAILURE loading dictionary");

                    ROMDictionaryAttribute[] order = engine.GetEvalList();
                    if (order != null && order.Length == 6 &&
                        order[0].Name == "cDictAttr1" &&
                        order[1].Name == "dDictAttr2" &&
                        order[2].Name == "aDictAttr3" &&
                        order[3].Name == "bDictAttr4" &&
                        order[4].Name == "eDictAttr5" &&
                        order[5].Name == "eDictAttr6")
                        Log("Order OK");
                    else
                        Log("FAILURE to assess the evaluation order");

                    Dictionary<string, string[]> triggers = engine.GetTriggers();
                    if (triggers.Count == 3 &&
                    triggers["aDictAttr3"].Length == 2 &&
                    triggers["aDictAttr3"][0] == "bDictAttr4" &&
                    triggers["aDictAttr3"][1] == "eDictAttr5")
                        Log("Triggers OK");
                    else
                        Log("FAILURE to assess the triggers");

                    Log("Testing evaluation");
                    engine.EvaluateAll();
                    ROMDictionaryAttribute attr1 = engine.GetDictionaryAttr("cDictAttr1");
                    ROMDictionaryAttribute attr2 = engine.GetDictionaryAttr("dDictAttr2");
                    if (attr2.AvailableValues.Length == 0 && attr2.PossibleValues.Length == 3 &&
                        attr1.AvailableValues.Length == 4)
                        Log("Default Eval OK");
                    else
                        Log("FAILURE to initially evaluate an attribute");

                    engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[0]);
                    string val_pick1 = rootNode.GetAttribute("dDictAttr2");
                    string val_bool1 = rootNode.GetAttribute("aDictAttr3");
                    string val_multi1 = rootNode.GetAttribute("bDictAttr4");
                    string edit1 = rootNode.GetAttribute("eDictAttr5");
                    engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[1]);
                    string val_pick2 = rootNode.GetAttribute("dDictAttr2");
                    string val_bool2 = rootNode.GetAttribute("aDictAttr3");
                    engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[2]);
                    string val_bool3 = rootNode.GetAttribute("aDictAttr3");
                    string val_multi3 = rootNode.GetAttribute("bDictAttr4");
                    engine.EvaluateForAttribute("eDictAttr5", "999");
                    string edit4 = rootNode.GetAttribute("eDictAttr5");
                    if (val_pick1 == "ResultByOption1" && val_pick2 == "Result2" &&
                        val_bool1 == "Y" && val_bool2 == "Y" && val_bool3 == "N" &&
                        val_multi1 == "Selection2|Selection3" && val_multi3 == "Selection2" &&
                        edit1 == "10" && edit4 == "50")
                        Log("Evaluation of attributes ok");
                    else
                        Log("FAILURE to evaluate an attribute");

                    start = Environment.TickCount;
                    testROMNodeStart = start;
                    Log("Stress Testing...");
                    Log("Setting a bunch of values...");
                    for (int i = 0; i < iMax; i++)
                    {
                        string attrToSet = "attr";
                        attrToSet += i.ToString();
                        rootNode.SetAttribute(attrToSet, i.ToString());
                    }
                    Log("Retrieving a bunch of values...");
                    for (int i = 0; i < iMax; i++)
                    {
                        string attrToGet = "attr";
                        attrToGet += i.ToString(); ;
                        rootNode.GetAttribute(attrToGet);
                    }
                    elapsed = Environment.TickCount - start;
                    msg = "attribute test complete in ";
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);

                    start = Environment.TickCount;
                    Log("Create a large object heierarchy for querey testing");
                    for (int i = 0; i < iMax; i++)
                    {
                        string objectToCreate = "SubLevel1Object";
                        objectToCreate += i.ToString();
                        ROMNode newNode = new ROMNode(objectToCreate);
                        rootNode.AddChildROMObject(newNode);

                        objectToCreate = "SubLevel2Object";
                        objectToCreate += i.ToString();
                        ROMNode newNode2 = new ROMNode(objectToCreate);
                        newNode.AddChildROMObject(newNode2);
                        newNode2.SetAttribute("sumtester", "1");
                        newNode2.SetAttribute("sumtester2", "2");
                        newNode2.SetAttribute("sumtester3", "3");
                        newNode2.SetAttribute("testvalue", "value found");
                    }
                    elapsed = Environment.TickCount - start;
                    msg = "objects created in ";
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);

                    Log("Performing queries");
                    start = Environment.TickCount;
                    long queryTime = start;
                    string xpath = "sum(//Attribute[@id='sumtester']/@value)";
                    string xpathRes = rootNode.EvaluateXPATH(xpath);
                    elapsed = Environment.TickCount - start;
                    Log(xpath + " result: " + xpathRes);
                    msg = "object query test complete in ";
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);

                    start = Environment.TickCount;
                    string xpath2 = "sum(//Attribute[@id='sumtester2']/@value)";
                    string xpathRes2 = rootNode.EvaluateXPATH(xpath2);
                    elapsed = Environment.TickCount - start;
                    Log(xpath2 + " result: " + xpathRes2);
                    msg = "object query test complete in ";
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);

                    Log("altering the object state");
                    rootNode.SetAttribute("Change", "Y");

                    start = Environment.TickCount;
                    string xpath3 = "sum(//Attribute[@id='sumtester3']/@value)";
                    string xpathRes3 = rootNode.EvaluateXPATH(xpath3);
                    elapsed = Environment.TickCount - start;
                    Log(xpath3 + " result: " + xpathRes3);
                    msg = "object query test complete in ";
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);

                    start = Environment.TickCount;
                    string xpath4 = "//Object[@id='SubLevel2Object10']/Attribute[@id='testvalue']/@value";
                    string xpathRes4 = rootNode.EvaluateXPATH(xpath4);
                    long finished = Environment.TickCount;
                    testROMNodeEnd = finished;
                    elapsed = finished - start;
                    Log(xpath4 + " result: " + xpathRes4);
                    msg = "object query test complete in ";
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);
                    elapsed = finished - queryTime;
                    msg = "All object query tests completed in ";
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);

                    msg = "stress test total time: ";
                    elapsed = testROMNodeEnd - testROMNodeStart;
                    msg += elapsed.ToString();
                    msg += "ms";
                    Log(msg);
                }
                else
                    Log("Could not load rules");

                Log("Testing complete...press any key to exit");
                Console.ReadLine();
            };

            Application.Run(frm);
        }

        

        private static void CreateChildNodes(ROMNode rootNode)
        {
            Log("Creating a child object");
            ROMNode childNode = ObjectFactory("DerivedObject");
            rootNode.AddChildROMObject(childNode);
            childNode.SetAttribute("childAttr", "some value of value");
            //setting a value on the Object Node
            childNode.SetROMObjectValue("valueTest", "myValue");
            ROMNode childOfChild = ObjectFactory("DerivedObject2");
            childNode.AddChildROMObject(childOfChild);
        }

        static void Log(string s)
        {
            string line = s + Environment.NewLine;
            Console.Write(line);
            if (frm != null)
                frm.Log(line);
        }

        static void DebugMessage(string msg)
        {
	        Log("DEBUGGER: " + msg);
        }

        public static ROMNode ObjectFactory(string id)
        {
            switch (id)
            {
                case "TestApplication":
                    return new ApplicationObject(id, ObjectFactory);
                case "DerivedObject":
                    return new DerivedObject(id, ObjectFactory);
                case "DerivedObject2":
                    return new DerivedObject2(id, ObjectFactory);
            }
            return null;
        }
    }
    

    abstract class BaseObject : ROMNode
    {
        public BaseObject(string id, ROMObjectFactoryDelegate factory) : base(id, factory) { }

        public override string GetAttribute(string id, string name, bool immediate)
        {
            if (id == "CLASS")
                return "BaseObject";
            return base.GetAttribute(id, name, immediate);
        }
    }

    class ApplicationObject : BaseObject
    {
        public ApplicationObject(string id, ROMObjectFactoryDelegate factory) : base(id, factory) { }

        public override string GetAttribute(string id, string name, bool immediate)
        {
            if (id == "CLASS")
                return "ApplicationObject";
            return base.GetAttribute(id, name, immediate);
        }
    }

    class DerivedObject : BaseObject
    {
        public DerivedObject(string id, ROMObjectFactoryDelegate factory) : base(id, factory) { }

        public override string GetAttribute(string id, string name, bool immediate)
        {
            if (id == "CLASS")
                return "DerivedObject";
            return base.GetAttribute(id, name, immediate);
        }
    }

    class DerivedObject2 : BaseObject
    {
        public DerivedObject2(string id, ROMObjectFactoryDelegate factory) : base(id, factory) { }

        public override string GetAttribute(string id, string name, bool immediate)
        {
            if (id == "CLASS")
                return "DerivedObject2";
            return base.GetAttribute(id, name, immediate);
        }
    }
}
