using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using EDSNET;

namespace EDSEngineTestApp
{
    class Program
    {
        static void Main(string[] args)
        {
            string dir = System.IO.Directory.GetCurrentDirectory();
            string filename = "..\\..\\..\\..\\EDSEngine\\EDSEngineTestApp\\test_project.xml";
            EDSEngine knowledge = new EDSEngine(filename);
            if (knowledge.IsOpen() == false)
            {
                write_result("FAILURE: Could not open rules xml file: " + filename);
            }
            else
            {
                write_result("OK: File opened");
            }

            knowledge.DebugDelegate = write_debug;
            knowledge.SetDebugging(true);

            var cnt = knowledge.TableCount();
            if (cnt == 12)
            {
                write_result("OK: 12 Tables loaded");
            }
            else
            {
                write_result("FAILURE: Not all tables loaded: " + cnt.ToString());
            }

            string tableName = "testtable1";
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
            }

            write_result("Loading attr input names for " + tableName);
            var allInputNames = knowledge.GetInputAttrs(tableName);
            for (var i = 0; i < allInputNames.Length; i++)
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
            }

            write_result("Loading dependency names for " + tableName);
            var allDepNames = knowledge.GetInputDependencies(tableName);
            for (var i = 0; i < allDepNames.Length; i++)
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
            }

            write_result("getting table type of testtable1");
            var isGetAll = knowledge.TableIsGetAll(tableName);
            if (isGetAll)
            {
                write_result(tableName + " was of 'GetAll' type as expected");
            }
            else
            {
                write_result("FAILURE: " + tableName + " was not of 'GetAll' type as expected");
            }

            //testing table evaluation
            write_result("testing evaluation of testtable1 with inputAttr1 = 'C', get first only");
            knowledge.SetInputValue("inputAttr1", "C");
            var results2 = knowledge.EvaluateTable(tableName, false);
            if (results2.Keys.Count == 2 && results2["outputAttr1"].Length == 1 &&
                results2["outputAttr1"][0] == "2")
            {
                write_result("OK: " + results2["outputAttr1"][0]);
            }
            else
            {
                write_result("FAILURE: Did not get proper eval result");
            }

            write_result("testing evaluation of testtable1 with inputAttr1 = 'C', get all");
            knowledge.SetInputValue("inputAttr1", "C");
            var results = knowledge.EvaluateTable(tableName, true);
            if (results.Keys.Count == 2 && results["outputAttr1"].Length == 3 &&
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
            }

            write_result("testing evaluation of testtable1 with inputAttr1 = 'C', inputAttr2 = 10, get all");
            knowledge.SetInputValue("inputAttr1", "C");
            knowledge.SetInputValue("inputAttr2", "10");
            var results3 = knowledge.EvaluateTable(tableName, true);
            if (results3.Keys.Count == 2 && results3["outputAttr1"].Length == 4 &&
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
            }

            write_result("testing evaluation of testtable1 with inputAttr1 = 'C', inputAttr2 = 58,  outsideAttr1 = 28, get all");
            knowledge.SetInputValue("inputAttr1", "C");
            knowledge.SetInputValue("inputAttr2", "58");
            knowledge.SetInputValue("outsideAttr1", "28");
            var results4 = knowledge.EvaluateTable(tableName, true);
            if (results4.Keys.Count == 2 && results4["outputAttr1"].Length == 5 &&
                results4["outputAttr1"][0] == "2" &&
                results4["outputAttr1"][1] == "28 with concat" && //empty becuase we never fed it outsideAttr1
                results4["outputAttr1"][2] == "4" &&
                results4["outputAttr1"][3] == "5" &&
#if PYTHON
                results4["outputAttr1"][4] == "30")
#else
                results4["outputAttr1"][4] == "py(28 + 2)")
#endif
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
            }

            write_result("testing table chaining");
            tableName = "testtable2";
            var result5 = knowledge.EvaluateTable(tableName, "out1", true);
            if (result5.Length == results4["outputAttr1"].Length &&
                result5[0] == "2" &&
                result5[1] == "28 with concat" && //empty becuase we never fed it outsideAttr1
                result5[2] == "4" &&
                result5[3] == "5" &&
#if PYTHON
                result5[4] == "30")
#else
                result5[4] == "py(28 + 2)")
#endif
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
            }

            write_result("testing input get()");
            tableName = "testtable3";
            knowledge.SetInputValue("someAttr", "3");
            var result6 = knowledge.EvaluateTable(tableName, "outputAttr1", true);
            if (result6.Length == 1 && result6[0] == "inputAttr2: 58 is greater than someAttr: 3")
            {
                write_result("OK: " + result6[0]);
            }
            else
            {
                write_result("FAILURE: Did not get proper eval result on get() test");
            }

            write_result("testing NULL conditions");
            knowledge.SetInputValue("inputAttr1", "");
            knowledge.SetInputValue("inputAttr2", "");
            var result7 = knowledge.EvaluateTable("testtable4", "outputAttr1", true);
            if (result7.Length == 4 && result7[2] == "both attrs are NULL")
	        {
                write_result(result7[0]);
                write_result(result7[1]);
                write_result(result7[2]);
                write_result(result7[3]);
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
                write_result(result7[0]);
                write_result(result7[1]);
            }
            else
            {
                write_result("FAILURE: Did not get proper eval result on NULL test #2");
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
            knowledge.SetInputValue("OutColor", "green");
            var result8 = knowledge.ReverseEvaluateTable("ReverseTest", true);
            if (result8.Count == 2 && result8["Color1"][0] == "blue" &&
		        result8["Color2"][0] == "yellow")
	        {
                write_result("OK");
	        }
	        else
	        {
                write_result("FAILURE: reverse evaluation failed");
	        }

#if DEBUG
            Quit();
#endif
        }

        static void write_result(string s)
        {
            Console.WriteLine(s);
        }

        static void write_debug(string s)
        {
            Console.WriteLine("DEBUGGING: " + s);
        }

        static void Quit()
        {
            write_result("Press any key to quit");
            Console.ReadKey(true);
            return;
        }
    }
}
