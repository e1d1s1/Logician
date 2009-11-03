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
            myBaseObject.LoadRules("C:\\Documents and Settings\\Eric\\My Documents\\Development\\EDSEngineTestApp\\test_project.zip");
            Log("...loaded");
            Log("Evaluating table testtable1");
            List<string> res = myBaseObject.EvaluateTable(myRootNode, "testtable1", "outputAttr1", true);
            foreach (string str in res)
            {
                Log(str);
            }
            Log("Evaluation complete");

            Log("Evaluating table testtable2: out1");
            List<string> res2 = myBaseObject.EvaluateTable(myRootNode, "testtable2", "out1", true);
            foreach (string str in res2)
            {
                Log(str);
            }
            Log("Evaluation complete");
            Console.ReadLine();
        }

        static void Log(string s)
        {
            Console.WriteLine(s);
        }
    }
}
