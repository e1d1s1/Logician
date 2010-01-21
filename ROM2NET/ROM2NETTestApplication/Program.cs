using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ROM2NET;

namespace ROM2NETTestApplication
{
    class Program
    {
        static void Main(string[] args)
        {
            Log("Creating application/doc object MyBaseObject");
            ROMTreeNET myBaseObject = new ROMTreeNET("Test_Application");
            Log("MyBaseObject Created");

            Log("Getting the root node");
            ROMNode myRootNode = myBaseObject.GetRoot();
            Log("Root obtained");		

            Log("Setting some attributes");
            myBaseObject.SetAttribute(myRootNode, "inputAttr1", "some value of test1");
            myBaseObject.SetAttribute(myRootNode, "inputAttr2", "some value of test2");
            myBaseObject.SetAttribute(myRootNode, "inputAttr3", "some value of test3");
            myBaseObject.SetAttribute(myRootNode, "inputAttr3", "test3a", "some sub value of attr3_a");
            myBaseObject.SetAttribute(myRootNode, "inputAttr3", "test3b", "some sub value of attr3_b");
            Log("Attrs set");

            Log("Creating a child object");
            ROMNode childNode = myBaseObject.CreateROMObject("ChildObject");
            myBaseObject.AddChildROMObject(myRootNode, childNode);
            myBaseObject.SetAttribute(childNode, "childAttr", "some value of value");
            //setting a value on the Object Node
            myBaseObject.SetROMObjectValue(childNode, "valueTest", "myValue");

            Log("Dump current xml state");
            string s = myBaseObject.DumpTree();
            Log(s);

            Log("Setting attrs to test eval, inputAttr1 = A, inputAttr2 = 10, outsideAttr1 = 28");
            myBaseObject.SetAttribute(myRootNode, "inputAttr1", "A");
            myBaseObject.SetAttribute(myRootNode, "inputAttr2", "10");
            myBaseObject.SetAttribute(myRootNode, "outsideAttr1", "28");
            Log("loading rules");
            if (myBaseObject.LoadRules("..\\..\\..\\..\\EDSEngine\\EDSEngineTestApp\\test_project.xml"))
            {
                Log("...loaded");
                Log("Evaluating table testtable1");
                string[] res = myBaseObject.EvaluateTable(myRootNode, "testtable1", "outputAttr1", true);
                foreach (string str in res)
                {
                    Log(str);
                }
                Log("Evaluation complete");

                Log("Evaluating table testtable2: out1");
                string[] res2 = myBaseObject.EvaluateTable(myRootNode, "testtable2", "out1", true);
                foreach (string str in res2)
                {
                    Log(str);
                }
                Log("Evaluation complete");

                Log("Testing the LinearEngine class");
                LinearEngineNET engine = new LinearEngineNET(myBaseObject, myRootNode, "Dictionary");

                Log("Checking dictionary size");
                Dictionary<string, ROMDictionaryAttributeNET> attrs = engine.GetAllDictionaryAttrs();
                if (attrs.Count == 5)
                    Log("size ok");
                else
                    Log("FAILURE loading dictionary");

                List<ROMDictionaryAttributeNET> order = engine.GetEvalList();
                if (order != null && order.Count == 5 &&
                    order[0].Name == "cDictAttr1" &&
                    order[1].Name == "dDictAttr2" &&
                    order[2].Name == "aDictAttr3" &&
                    order[3].Name == "bDictAttr4" &&
                    order[4].Name == "eDictAttr5")
                    Log("Order OK");
                else
                    Log("FAILURE to assess the evaluation order");

                Log("Testing evaluation");
                engine.EvaluateAll();
                ROMDictionaryAttributeNET attr1 = engine.GetDictionaryAttr("cDictAttr1");
                ROMDictionaryAttributeNET attr2 = engine.GetDictionaryAttr("dDictAttr2");
                if (attr2.AvailableValues.Length == 0 && attr2.PossibleValues.Length == 3 &&
                    attr1.AvailableValues.Length == 4)
                    Log("Default Eval OK");
                else
                    Log("FAILURE to initially evaluate an attribute");

                engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[0]);
                string val_pick1 = myBaseObject.GetAttribute(myRootNode, "dDictAttr2");
                string val_bool1 = myBaseObject.GetAttribute(myRootNode, "aDictAttr3");
                string val_multi1 = myBaseObject.GetAttribute(myRootNode, "bDictAttr4");
                string edit1 = myBaseObject.GetAttribute(myRootNode, "eDictAttr5");
                engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[1]);
                string val_pick2 = myBaseObject.GetAttribute(myRootNode, "dDictAttr2");
                string val_bool2 = myBaseObject.GetAttribute(myRootNode, "aDictAttr3");
                engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[2]);
                string val_bool3 = myBaseObject.GetAttribute(myRootNode, "aDictAttr3");
                string val_multi3 = myBaseObject.GetAttribute(myRootNode, "bDictAttr4");
                engine.EvaluateForAttribute("eDictAttr5", "999");
                string edit4 = myBaseObject.GetAttribute(myRootNode, "eDictAttr5");
                if (val_pick1 == "ResultByOption1" && val_pick2 == "Result2" &&
                    val_bool1 == "Y" && val_bool2 == "Y" && val_bool3 == "N" &&
                    val_multi1 == "Selection2|Selection3" && val_multi3 == "Selection2" &&
                    edit1 == "10" && edit4 == "50")
                    Log("Evaluation of attributes ok");
                else
                    Log("FAILURE to evaluate an attribute");
            }
            else
                Log("Could not load rules");

            Log("Testing complete...press any key to exit");
            Console.ReadLine();
        }

        static void Log(string s)
        {
            Console.WriteLine(s);
        }
    }
}
