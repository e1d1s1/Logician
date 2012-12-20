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

namespace LogicianSilverlightTestApp
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
        }

        private void btnTest_Click(object sender, RoutedEventArgs e)
        {
            clear_results();        
            var filename = "test_project.xml";
            var knowledge = new KnowledgeBase(filename);
            if (knowledge.IsOpen() == false)
            {
                write_result("FAILURE: Could not open rules xml file: " + filename);
                return;
            }
            else
            {
                write_result("OK: File opened");
            }

            knowledge.SetDebugging(true);
            knowledge.DebugDelegate = debugHandler;
                        
            var cnt = knowledge.TableCount();
            if (cnt == 12)
            {
                write_result("OK: 12 Tables loaded");
            }
            else
            {
                write_result("FAILURE: Not all tables loaded: " + cnt.ToString());
                return;
            }
            
            var tableName = "testtable1";
            write_result("Loading attr output names for " + tableName);
            var allOutputNames = knowledge.GetOutputAttrs(tableName);
            for (var i = 0; i < allOutputNames.Length; i++)
	        {
		        write_result(allOutputNames[i]);
	        }
	        if (allOutputNames.Length == 2 && allOutputNames[0] == "anotherOutput" && allOutputNames[1] == "outputAttr1")
	        {
		        write_result("OK");
	        }
	        else
	        {
		        write_result("FAILURE: Did not load all the output names");
		        return;
	        }
	        
	        write_result("Loading attr input names for " + tableName);
	        var allInputNames = knowledge.GetInputAttrs(tableName);
	        for (int i = 0; i < allInputNames.Length; i++)
	        {
		        write_result(allInputNames[i]);
	        }
	        if (allInputNames.Length == 2 && allInputNames[0] == "inputAttr1" && allInputNames[1] == "inputAttr2")
	        {
		        write_result("OK");
	        }
	        else
	        {
		        write_result("FAILURE: Did not load all the input names");
		        return;
	        }
	        
	        write_result("Loading dependency names for " + tableName);
	        var allDepNames = knowledge.GetInputDependencies(tableName);
	        for (int i = 0; i < allDepNames.Length; i++)
	        {
		        write_result(allDepNames[i]);
	        }
	        if (allDepNames.Length == 3 && allDepNames[0] == "inputAttr1" && 
	            allDepNames[1] == "inputAttr2" &&
	            allDepNames[2] == "outsideAttr1")
	        {
		        write_result("OK");
	        }
	        else
	        {
		        write_result("FAILURE: Did not load all the dependency names");
		        return;
	        }
	        
	        var outputAttr = "outputAttr1";
	        write_result("Getting all possible output values for " + outputAttr);
	        var allOutputs = knowledge.GetAllPossibleOutputs(tableName, outputAttr);
	        for (var i = 0; i < allOutputs.Length; i++)
	        {
		        write_result(allOutputs[i]);
	        }
	        if (allOutputs.Length == 9 && 
	            allOutputs[0] == "1" && 
	            allOutputs[1] == "2" &&
	            allOutputs[2] == "get(outsideAttr1) with concat" &&
	            allOutputs[3] == "4" &&
	            allOutputs[4] == "5" &&
	            allOutputs[5] == "py(get(outsideAttr1) + 2)" &&
	            allOutputs[6] == "js(get(outsideAttr1) + 2)" &&
	            allOutputs[7] == "js(alterparameter())" &&
	            allOutputs[8] == "py(alterparameter())")
	        {
		        write_result("OK");
	        }
	        else
	        {
		        write_result("FAILURE: Did not load all the possible output values");
		        return;
	        }
	        
	        write_result("checking the table type for testtable1...");
	        var isGetAll = knowledge.TableIsGetAll(tableName);
	        if (isGetAll)
	            write_result(tableName + " is GetAll as expected");
	        else
	        {
	            write_result("FAILURE: table was not GetAll as expected");
	            return;
	        }
	        
	        //testing table evaluation
	        write_result("testing evaluation of testtable1 with inputAttr1 = 'C', get first only");
	        knowledge.SetInputValue("inputAttr1", "C");
	        var results2 = knowledge.EvaluateTable(tableName, false);
	        if (results2.Count == 2 && results2["outputAttr1"][0] == "2")
	        {
	            write_result("OK: " + results2["outputAttr1"][0]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result");
	            MessageBox.Show(results2["outputAttr1"].Length.ToString());
                MessageBox.Show(results2.Count.ToString());
                MessageBox.Show("'" + results2["outputAttr1"][0] + "'");
		        return;
	        }
	        
	        write_result("testing evaluation of testtable1 with inputAttr1 = 'C', get all");
	        knowledge.SetInputValue("inputAttr1", "C");
	        var results = knowledge.EvaluateTable(tableName, true);
	        if (results.Count == 2 && results["outputAttr1"].Length == 3 &&
	            results["outputAttr1"][0] == "2" &&
	            results["outputAttr1"][1] == "4" &&
	            results["outputAttr1"][2] == "5")
	        {
	            write_result("OK: " + results["outputAttr1"][0] + "\n" + 
	            results["outputAttr1"][1] + "\n" +
	            results["outputAttr1"][2]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result");
		        return;
	        }
	        
	        write_result("testing evaluation of testtable1 with inputAttr1 = 'C', inputAttr2 = 10, get all");
	        knowledge.SetInputValue("inputAttr1", "C");
	        knowledge.SetInputValue("inputAttr2", "10");
	        var results3 = knowledge.EvaluateTable(tableName, true);
	        if (results3.Count == 2 && results3["outputAttr1"].Length == 4 &&
	            results3["outputAttr1"][0] == "2" &&
	            results3["outputAttr1"][1] == " with concat" && //empty becuase we never fed it outsideAttr1
	            results3["outputAttr1"][2] == "4" &&
	            results3["outputAttr1"][3] == "5")
	        {
	            write_result("OK: " + results3["outputAttr1"][0] + "\n" + 
	            results3["outputAttr1"][1] + "\n" +
	            results3["outputAttr1"][2] + "\n" + 
	            results3["outputAttr1"][3]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result");
		        return;
	        }
	        
	        write_result("testing evaluation (Javascript) of testtable1 with inputAttr1 = 'C', inputAttr2 = 78,  outsideAttr1 = 28, get all");
	        knowledge.SetInputValue("inputAttr1", "C");
	        knowledge.SetInputValue("inputAttr2", "78");
	        knowledge.SetInputValue("outsideAttr1", "28");
	        var results4 = knowledge.EvaluateTable(tableName, true);
	        if (results4.Count == 2 && results4["outputAttr1"].Length == 5 &&
	            results4["outputAttr1"][0] == "2" &&
	            results4["outputAttr1"][1] == "28 with concat" && //empty becuase we never fed it outsideAttr1
	            results4["outputAttr1"][2] == "4" &&
	            results4["outputAttr1"][3] == "5"&&
	            results4["outputAttr1"][4] == "30")
	        {
	            write_result("OK: " + results4["outputAttr1"][0] + "\n" + 
	            results4["outputAttr1"][1] + "\n" +
	            results4["outputAttr1"][2] + "\n" + 
	            results4["outputAttr1"][3] + "\n" + 
	            results4["outputAttr1"][4]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result");
		        return;
	        }
	        
	        write_result("testing evaluation (Javascript) with state parameter on testtable1 with inputAttr1 = 'TestParameterJS' and inputAttr2 = 'TestParameterJS'");
	        knowledge.SetInputValue("inputAttr1", "TestParameterJS");
	        knowledge.SetInputValue("inputAttr2", "TestParameterJS");
	        var result9 = knowledge.EvaluateTableWithParam(tableName, "outputAttr1", "PassedValue");
	        var retParam = knowledge.GetEvalParameter();
	        write_result(result9.Length.ToString());
	        if (result9.Length == 4 && result9[3] == "eval ok" &&
	            retParam == "PassedValue modified")
	        {
	            write_result("Javascript state parameter working");
	        }
	        else
	        {
	            write_result("FAILURE: Problem with Javascript state parameter: " + result9[3] + ":" + retParam);
	            return;
	        }      
	        
	        
	        write_result("testing table chaining");
	        knowledge.SetInputValue("inputAttr1", "C");
	        knowledge.SetInputValue("inputAttr2", "78");
	        knowledge.SetInputValue("outsideAttr1", "28");
	        tableName = "testtable2";
	        var result5 = knowledge.EvaluateTable(tableName, "out1", true);
	        if (result5.Length == results4["outputAttr1"].Length &&
	            result5[0] == "2" &&
	            result5[1] == "28 with concat" && //empty becuase we never fed it outsideAttr1
	            result5[2] == "4" &&
	            result5[3] == "5"&&
	            result5[4] == "30")
	        {
	            write_result("OK: " + result5[0] + "\n" + 
	            result5[1] + "\n" +
	            result5[2] + "\n" + 
	            result5[3] + "\n" + 
	            result5[4]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result on chain");
		        return;
	        }
	        
	        write_result("testing input get(), and debug");
	        tableName = "testtable3";
	        knowledge.SetInputValue("someAttr", "3");
	        
	        var result6 = knowledge.EvaluateTable(tableName, "outputAttr1", true);
	        if (result6.Length == 1 && result6[0] == "inputAttr2: 78 is greater than someAttr: 3")           
	        {
	            write_result("OK: " + result6[0]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result on get() test");
		        return;
		    }
	        
	        write_result("testing NULL conditions");
	        knowledge.SetInputValue("inputAttr1", "");
	        knowledge.SetInputValue("inputAttr2", "");
	        var result7 = knowledge.EvaluateTable("testtable4", "outputAttr1", true);
	        if (result7.Length == 4 && result7[2]== "both attrs are NULL")
	        {
		        write_result("OK: " + result7[0] + "\n" + 
		        result7[1] + "\n" + 
		        result7[2] + "\n" + 
		        result7[3]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result on NULL test #1");
	        }
	        
	        knowledge.SetInputValue("inputAttr1", "blah");
	        knowledge.SetInputValue("c", "");
            result7 = knowledge.EvaluateTable("testtable4", "outputAttr1", true);
            if (result7.Length == 2 && result7[0] == "inputAttr2 is NULL")
            {
                write_result("OK: " + result7[0] + "\n" +
                result7[1]);
            }
            else
            {
                write_result("FAILURE: Did not get proper eval result on NULL test #2");
            }

	        knowledge.SetInputValue("inputAttr1", "blah");
	        knowledge.SetInputValue("c", "");
	        result7 = knowledge.EvaluateTable("testtable4", "outputAttr1", true);
	        if (result7.Length == 2 && result7[0] == "inputAttr2 is NULL")
	        {
		        write_result("OK: " + result7[0] + "\n" +
		        result7[1]);
	        }
	        else
	        {
	            write_result("FAILURE: Did not get proper eval result on NULL test");
	        }	
	        
	        write_result("testing translation of: A");
	        var localeValue = knowledge.Localize("A", "en-US");
	        var reverse = knowledge.DeLocalize(localeValue);
	        write_result(localeValue + ":" + reverse);
	        if (localeValue == "A trans" && reverse == "A")
	            write_result("OK");
	        else
	            write_result("FAILURE: translation failed");  
	            
	        write_result("testing reverse evaluation of ReverseTest table");
	        knowledge.SetInputValue("OutColor","green");
	        var result8 = knowledge.ReverseEvaluateTable("ReverseTest", true);
	        if (result8.Count == 2 && result8["Color1"][0] == "blue" &&
	            result8["Color2"][0] == "yellow")
	        {
		        write_result("OK: " + result8["Color1"][0] + ", " + result8["Color2"][0]);
	        }
	        else
	        {
		        write_result("FAILURE: reverse evaluation failed");
	        }          
            	       
            return;
        }     

        private void btnTest2_Click(object sender, RoutedEventArgs e)
        {
            var start = 0; var elapsed = 0; var testROMTreeStart = 0; var testROMTreeEnd = 0;
            string msg = "";
            var iMax = 1000;
            try
            {
                clear_results();

                write_result("Test ROMNode Objects");
                write_result("Creating root node");
                var rootNode = new ROMNode("TestApplication");
                write_result("Root ROMNode created");

                write_result("Setting some attributes");
                rootNode.SetAttribute("inputAttr1", "some value of test1");
                rootNode.SetAttribute("inputAttr2", "some value of test2");
                rootNode.SetAttribute("inputAttr3", "some value of test3");
                rootNode.SetAttribute("inputAttr3", "test3a", "some sub value of attr3_a");
                rootNode.SetAttribute("inputAttr3", "test3b", "some sub value of attr3_b");
                write_result("Attrs Set");
    		    
                write_result("Testing what we have set");
                write_result("inputAttr1 = " + rootNode.GetAttribute("inputAttr1", true));

	            write_result("Creating a child object");
	            var childNode = new ROMNode("ChildObject");
	            rootNode.AddChildROMObject(childNode);
	            childNode.SetAttribute("childAttr", "some value of value");
	            //setting a value on the Object Node
	            childNode.SetROMObjectValue("valueTest", "myValue");
	            var childOfChild = new ROMNode("ChildObject2");
	            var resAdd = childNode.AddChildROMObject(childOfChild);
	            var findTest = rootNode.FindAllObjectsByID("ChildObject", true);
                var findTestXPATH = rootNode.FindObjects("//Object[@id='ChildObject']");
                var findTestXPATH2 = rootNode.FindObjects("//Object[@id='ChildObject2']");
                if (findTest.Length == 1 && findTestXPATH.Length == 1 && findTestXPATH2.Length == 1 &&
                    findTest[0].GetROMGUID() == findTestXPATH[0].GetROMGUID() &&
                    findTestXPATH2[0].GetROMObjectID() == "ChildObject2")
                    write_result("OK");
                else
                    write_result("FAILURE creating/obtaining child object"); 

                write_result("Dump current xml state");
                var result = rootNode.SaveXML(true);
                write_result(result);

                write_result("Setting attrs to test eval, inputAttr1 = A, inputAttr2 = 1");
                rootNode.SetAttribute("inputAttr1", "A");
                rootNode.SetAttribute("inputAttr2", "1");
    	
                write_result("New xml state");
                result = rootNode.SaveXML(true);
                write_result(result);
    	        
                write_result("Getting attribute list on the base object");
                Dictionary<string, Dictionary<string, string>> allAttrs = rootNode.GetAllAttributes();
                foreach (KeyValuePair<string, Dictionary<string, string>> kvp in allAttrs)
                {
                    var attrName = kvp.Key;
                    var attrDict = allAttrs[attrName];
                    write_result("Name: " + attrName);
                    foreach(KeyValuePair<string, string> kvpAttr in attrDict)
                    {
                        write_result("Key: " + kvpAttr.Key + " Value: " + kvpAttr.Value);
                    }
                }

                write_result("loading rules");
                if (rootNode.LoadRules("test_project.xml"))
                {
                    write_result("...loaded");
                    //deubg
                    rootNode.SetDebugging(true);
                    rootNode.DebugDelegate = debugHandler;
                    write_result("Evaluating table testtable1");
                    var res = rootNode.EvaluateTable("testtable1", "outputAttr1", true);
                    for (var i = 0; i < res.Length; i++)
                    {
                        write_result(res[i]);
                    }
                    write_result("Evaluation complete");

                    write_result("Evaluating table testtable2: out1");
                    var res2 = rootNode.EvaluateTable("testtable2", "out1", true);
                    for (var i = 0; i < res2.Length; i++)
                    {
                        write_result(res2[i]);
                    }
                    write_result("Evaluation complete");
    	        
    	            
                    write_result("Testing the LinearEngine object");
                    var engine = new LinearEngine(rootNode, "Dictionary");

                    write_result("Checking dictionary size");
                    var attrs = engine.GetAllDictionaryAttrs();
                    if (attrs.Count == 5)
                        write_result("size ok");
                    else
                        write_result("FAILURE loading dictionary:" + attrs.Count);

                    var order = engine.GetEvalList();
                    if (order != null && order.Length == 5 &&
                        order[0].Name == "cDictAttr1" &&
                        order[1].Name == "dDictAttr2" &&
                        order[2].Name == "aDictAttr3" &&
                        order[3].Name == "bDictAttr4" &&
                        order[4].Name == "eDictAttr5")
                        write_result("Order OK");
                    else
                        write_result("FAILURE to assess the evaluation order:" + order.Length + order[0].Name);

                    var triggers = engine.GetTriggers();
                    if (triggers.Count == 3 &&
                        triggers["aDictAttr3"].Length == 2 &&
                        triggers["aDictAttr3"][0] == "bDictAttr4" &&
                        triggers["aDictAttr3"][1] == "eDictAttr5")
                        write_result("Triggers OK");
                    else
                        write_result("FAILURE to assess the triggers");
				    
                    write_result("Testing evaluation");
                    engine.EvaluateAll();
                    var attr1 = engine.GetDictionaryAttr("cDictAttr1");
                    var attr2 = engine.GetDictionaryAttr("dDictAttr2");
                    if (attr2.AvailableValues.Length == 0 && attr2.PossibleValues.Length == 3 &&
                        attr1.AvailableValues.Length == 4)
                        write_result("Default Eval OK");
                    else
                        write_result("FAILURE to initially evaluate an attribute");

                    engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[0], true);
                    var val_pick1 = rootNode.GetAttribute("dDictAttr2", true);
                    var val_bool1 = rootNode.GetAttribute("aDictAttr3", true);
                    var val_multi1 = rootNode.GetAttribute("bDictAttr4", true);
                    var edit1 = rootNode.GetAttribute("eDictAttr5", true);
                    engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[1], true);
                    var val_pick2 = rootNode.GetAttribute("dDictAttr2", true);
                    var val_bool2 = rootNode.GetAttribute("aDictAttr3", true);
                    engine.EvaluateForAttribute("cDictAttr1", attr1.AvailableValues[2], true);            
                    var val_bool3 = rootNode.GetAttribute( "aDictAttr3", true);
                    var val_multi3 = rootNode.GetAttribute( "bDictAttr4", true);
                    engine.EvaluateForAttribute("eDictAttr5", "999", true);
                    var edit4 = rootNode.GetAttribute("eDictAttr5", true);
                    if (val_pick1 == "ResultByOption1" && val_pick2 == "Result2" &&
                        val_bool1 == "Y" && val_bool2 == "Y" && val_bool3 == "N" &&
                        val_multi1 == "Selection2|Selection3" && val_multi3 == "Selection2" &&
                        edit1 == "10" && edit4 == "50")
                    {
                        write_result("Evaluation of attributes ok");
                    }
                    else
                    {
                        write_result("FAILURE to evaluate an attribute");
                        write_result("val_pick1:" + val_pick1 + " should be ResultByOption1");
                        write_result("val_bool1:" + val_bool1 + " should be Y");
                        write_result("val_multi1:" + val_multi1 + " should be Selection2|Selection3");
                        write_result("edit1:" + edit1 + " should be 10");
                        write_result("val_pick2:" + val_pick2 + " should be Result2");
                        write_result("val_bool2:" + val_bool2 + " should be Y");
                        write_result("val_bool3:" + val_bool3 + " should be N");
                        write_result("val_multi3:" + val_multi3 + " should be Selection2");
                        write_result("edit4:" + edit4 + " should be 50");
                    }
                }
                else 
                {
                    write_result("Could not load rules");
                }
                
                start = 0;
                start = getTickCount();
                testROMTreeStart = start;
                write_result("Stress Testing...");
                write_result("Setting a bunch of values...");
                for (var i = 0; i < iMax; i++)
                {
                    var attrToSet = "attr";
                    attrToSet+=i.ToString();
                    rootNode.SetAttribute(attrToSet, i.ToString());
                }
                write_result("Retrieving a bunch of values...");
                for (var i = 0; i < iMax; i++)
                {
                    var attrToGet = "attr";
                    attrToGet += i.ToString();
                    rootNode.GetAttribute(attrToGet, true);
                }
                elapsed = getTickCount() - start;
                msg = "attribute test complete in ";
                msg += elapsed.ToString();
                msg+="ms";
                write_result(msg);

                start = getTickCount();
                write_result("Create a large object hierarchy for query testing");
                for (int i = 0; i < iMax; i++)
                {
                    var objectToCreate = "SubLevel1Object";
                    objectToCreate += i.ToString();
                    var newNode = new ROMNode(objectToCreate);
                    rootNode.AddChildROMObject(newNode);

                    objectToCreate = "SubLevel2Object";
                    objectToCreate+=i.ToString();
                    var newNode2 = new ROMNode(objectToCreate);
                    newNode.AddChildROMObject(newNode2);
                    newNode2.SetAttribute("sumtester", "1");
                    newNode2.SetAttribute("sumtester2", "2");
                    newNode2.SetAttribute("sumtester3", "3");
                    newNode2.SetAttribute("testvalue", "value found");
                }
                elapsed = getTickCount() - start;
                msg = "objects created in ";
                msg += elapsed.ToString();
                msg+="ms";
                write_result(msg);

                write_result("Performing queries");
                start = getTickCount();
                var queryTime = start;
                var xpath = "sum(//Attribute[@id='sumtester']/@value)";
                var xpathRes = rootNode.EvaluateXPATH(xpath);
                elapsed = getTickCount() - start;
                write_result(xpath + " result: " + xpathRes);
                msg = "object query test complete in ";
                msg += elapsed.ToString();
                msg+="ms";
                write_result(msg);

                start = getTickCount();
                var xpath2 = "sum(//Attribute[@id='sumtester2']/@value)";
                var xpathRes2 = rootNode.EvaluateXPATH(xpath2);
                elapsed = getTickCount() - start;
                write_result(xpath2 + " result: " + xpathRes2);
                msg = "object query test complete in ";
                msg += elapsed.ToString();
                msg+="ms";
                write_result(msg);

                write_result("altering the object state");
                rootNode.SetAttribute("Change", "Y");

                start = getTickCount();
                var xpath3 = "sum(//Attribute[@id='sumtester3']/@value)";
                var xpathRes3 = rootNode.EvaluateXPATH(xpath3);
                elapsed = getTickCount() - start;
                write_result(xpath3 + " result: " + xpathRes3);
                msg = "object query test complete in ";
                msg += elapsed.ToString();
                msg+="ms";
                write_result(msg);

                start = getTickCount();
                var xpath4 = "//Object[@id='SubLevel2Object10']/Attribute[@id='testvalue']/@value";
                var xpathRes4 = rootNode.EvaluateXPATH(xpath4);
                var finished = getTickCount();
                testROMTreeEnd = finished;
                elapsed = finished - start;
                write_result(xpath4 + " result: " + xpathRes4);
                msg = "object query test complete in ";
                msg += elapsed.ToString();
                msg+="ms";
                write_result(msg);

                elapsed = finished - queryTime;
                msg = "All object query tests completed in ";
                msg += elapsed.ToString();
                msg+="ms";
                write_result(msg);
            }
            catch(Exception err)
            {
                MessageBox.Show(err.ToString()); 
            }
        }

        private void debugHandler(string str)
        {
            MessageBox.Show("DEBUGGING:\n" + str);
        }

        private void write_result(string res)
        {
            txtResults.Text += res + "\n";
        }

        private void clear_results() 
        {
            txtResults.Text = "";
        }
        
        private int getTickCount()
        {            
            return Environment.TickCount;
        }
    }
}
