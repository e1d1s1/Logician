// EDSEngineTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <limits>
#include <thread>
#include <memory>
#include "../KnowledgeBase.h"


using namespace std;
using namespace EDS;

void Log(std::string strLogLine, int thread_id)
{
    std::cout<<"Thread: "<<thread_id<<" ";
	std::cout<<strLogLine<<endl;
}

class TestResult
{
public:
	bool bPassed;
	string strMessage;

	TestResult()
	{
		Reset();
	}

	void SetResult(bool res, string msg, int thread_id)
	{
		bPassed = res;
		strMessage = msg;

		if (bPassed)
		{
			string finalMsg = "OK";
			if (msg.length() > 0)
				finalMsg += ", " + msg;
			Log(finalMsg, thread_id);
		}
		else
		{
			Log("FAILURE: " + msg, thread_id);
		}
	}

	void Reset()
	{
		bPassed = false;
		strMessage = "";
	}
};

void pause()
{
    std::puts("Press any key to continue...");
#ifndef __GNUC__
 	std::getchar();
#else
    std::cin.get();
 #endif
}

vector<TestResult> testResults;

void DebugMessage(const string& msg)
{
	Log("DEBUGGER: " + msg, -1);
}

int runTest(int thread_id)
//int main(int argc, char* argv[])
{
	map<string, string> state;
	//Loading
	TestResult res;
	Log("Loading Test project", thread_id);
	//load up some sample table
	#ifndef __GNUC__
	std::unique_ptr<IKnowledgeBase> knowledge = std::make_unique<CKnowledgeBase>("..\\EDSEngineTestApp\\test_project.gz");
	if (!knowledge->IsOpen())
		knowledge->CreateKnowledgeBase("test_project.gz");
	#else
	EDS::CKnowledgeBase knowledge("../../test_project.gz");
	#endif

	if (!knowledge->IsOpen())
	{
		Log("Could not open rules file", thread_id);
		pause();
		return 0;
	}

	//debugging
	knowledge->SetDebugHandler(DebugMessage);

	//multithreaded solver
	knowledge->SetMaxThreads(2);

	//decisionlogic connection
	knowledge->EnableRemoteDebugger(false);
	knowledge->SetInputValueGetter([&](const string& attrName, void* ctx)
	{
		string retval;
		auto it = state.find(attrName);
		if (it != end(state))
		{
			retval = it->second;
		}
		return retval;
	});

	string s = to_string(knowledge->TableCount());
	Log("# of Tables loaded: " + s, thread_id);
	if (s == "14")
	{
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not load all the tables", thread_id);
	}

	//getting a table's attrs
	string tableName = "testtable1";
	res.Reset();
	Log("Loading attr output names for testtable1", thread_id);
	vector<string> allOutputNames = knowledge->GetOutputAttrs(tableName);
	for (size_t i = 0; i < allOutputNames.size(); i++)
	{
		Log(allOutputNames[i], thread_id);
	}
	if (allOutputNames.size() == 2 && allOutputNames[0] == "anotherOutput" && allOutputNames[1] == "outputAttr1")
	{
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not load all the output names", thread_id);
	}

	res.Reset();
	Log("Loading attr input names for testtable1", thread_id);
	vector<string> allInputNames = knowledge->GetInputAttrs(tableName);
	for (size_t i = 0; i < allInputNames.size(); i++)
	{
		Log(allInputNames[i], thread_id);
	}
	if (allInputNames.size() == 2 && allInputNames[0] == "inputAttr1" && allInputNames[1] == "inputAttr2" )
	{
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not load all the input names", thread_id);
	}

	res.Reset();
	Log("Loading dependency names for testtable1", thread_id);
	vector<string> allDepNames = knowledge->GetInputDependencies(tableName);
	for (size_t i = 0; i < allDepNames.size(); i++)
	{
		Log(allDepNames[i], thread_id);
	}
	if (allDepNames.size() == 3 && allDepNames[0] == "inputAttr1" &&
		allDepNames[1] == "inputAttr2" &&
		allDepNames[2] == "outsideAttr1" )
	{
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not load all the dependency names", thread_id);
	}

	res.Reset();
	Log("Getting all possible output values for outputAttr1", thread_id);
	vector<string> allOutputs = knowledge->GetAllPossibleOutputs(tableName, "outputAttr1");
	for (size_t i = 0; i < allOutputs.size(); i++)
	{
		Log(allOutputs[i], thread_id);
	}
	if (allOutputs.size() == 10 &&
		allOutputs[0] == "1" &&
		allOutputs[1] == "2" &&
		allOutputs[2] == "get(outsideAttr1) with concat" &&
		allOutputs[3] == "4" &&
		allOutputs[4] == "5" &&
		allOutputs[5] == "py(get(outsideAttr1) + 2)" &&
		allOutputs[6] == "js(get(outsideAttr1) + 2)" &&
		allOutputs[7] == "js(alterparameter())" &&
		allOutputs[8] == "py(alterparameter())" &&
		allOutputs[9] == "Threading")
	{
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not load all the possible output values", thread_id);
	}

	res.Reset();
	Log("Checking the table type...", thread_id);
	bool bIsGetAll = knowledge->TableIsGetAll(tableName);
	if (bIsGetAll)
		res.SetResult(true, "testtable1 is of type: GetAll", thread_id);
	else
		res.SetResult(false, "testtable1 is not of type \"GetAll\" as expected", thread_id);

	//testing table evaluation
	res.Reset();
	Log("testing evaluation of testtable1 with inputAttr1 = 'C', get first only", thread_id);
	state["inputAttr1"] = "C";
	map<string, vector<string> > results2 = knowledge->EvaluateTable(tableName, false);
	if (results2.size() == 2 && results2["outputAttr1"].size() == 1 &&
		results2["outputAttr1"].at(0) == "2")
	{
		Log(results2["outputAttr1"].at(0), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result", thread_id);
	}


	res.Reset();
	Log("testing evaluation of testtable1 with inputAttr1 = 'C', get all", thread_id);
	state["inputAttr1"] = "C";
	map<string, vector<string> > results = knowledge->EvaluateTable(tableName, bIsGetAll);
	if (results.size() == 2 && results["outputAttr1"].size() == 3 &&
		results["outputAttr1"].at(0) == "2" &&
		results["outputAttr1"].at(1) == "4" &&
		results["outputAttr1"].at(2) == "5")
	{
		Log(results["outputAttr1"].at(0), thread_id);
		Log(results["outputAttr1"].at(1), thread_id);
		Log(results["outputAttr1"].at(2), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result", thread_id);
	}

	res.Reset();
	Log("testing evaluation of testtable1 with inputAttr1 = 'C', inputAttr2 = 10, get all", thread_id);
	state["inputAttr1"] = "C";
	state["inputAttr2"] = "10";
	map<string, vector<string> > results3 = knowledge->EvaluateTable(tableName, bIsGetAll);
	if (results3.size() == 2 && results3["outputAttr1"].size() == 4 &&
		results3["outputAttr1"].at(0) == "2" &&
		results3["outputAttr1"].at(1) == " with concat" && //empty becuase we never fed it outsideAttr1
		results3["outputAttr1"].at(2) == "4" &&
		results3["outputAttr1"].at(3) == "5")
	{
		Log(results3["outputAttr1"].at(0), thread_id);
		Log(results3["outputAttr1"].at(1), thread_id);
		Log(results3["outputAttr1"].at(2), thread_id);
		Log(results3["outputAttr1"].at(3), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result", thread_id);
	}

	res.Reset();
	Log("testing evaluation (Javascript) of testtable1 with inputAttr1 = 'C', inputAttr2 = 78, get all, outsideAttr1 = 28", thread_id);
	state["inputAttr1"] = "C";
	state["inputAttr2"] = "78";
	state["outsideAttr1"] = "28";
	map<string, vector<string> > results4 = knowledge->EvaluateTable(tableName, bIsGetAll);
	if (results4.size() == 2 && results4["outputAttr1"].size() == 5 &&
		results4["outputAttr1"].at(0) == "2" &&
		results4["outputAttr1"].at(1) == "28 with concat" &&
		results4["outputAttr1"].at(2) == "4" &&
		results4["outputAttr1"].at(3) == "5" &&
		results4["outputAttr1"].at(4) == "30") //28 + 2
	{
		Log(results4["outputAttr1"].at(0), thread_id);
		Log(results4["outputAttr1"].at(1), thread_id);
		Log(results4["outputAttr1"].at(2), thread_id);
		Log(results4["outputAttr1"].at(3), thread_id);
		Log(results4["outputAttr1"].at(4), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result", thread_id);
	}

	res.Reset();
	Log("testing evaluation (Javascript) with state parameter on testtable1 with inputAttr1 = 'TestParameterJS' and inputAttr2 = 'TestParameterJS'", thread_id);
	state["inputAttr1"] = "TestParameterJS";
	state["inputAttr2"] = "TestParameterJS";
	string paramValue = "PassedValue";
	string origValue = paramValue;
	vector<string> results9 = knowledge->EvaluateTableWithParam(tableName, (string)"outputAttr1", paramValue);
	if (results9.size() == 4 && results9.at(3) == "eval ok" &&
		paramValue == "PassedValue modified")
	{
		Log("Javascript state parameter working", thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Problem with Javascript state parameter", thread_id);
	}
#ifndef NOPYTHON
	res.Reset();
	Log("testing evaluation (Python) with state parameter on testtable1 with inputAttr1 = 'TestParameterPY' and inputAttr2 = 'TestParameterPY'", thread_id);
	state["inputAttr1"] = "TestParameterPY";
	state["inputAttr2"] = "TestParameterPY";
	string paramValue = "PassedValue";
	string origValue = paramValue;
	vector<string> results10 = knowledge->EvaluateTableWithParam(tableName, (string)"outputAttr1", paramValue);
	if (results10.size() == 4 && results10.at(3) == "eval ok" &&
		paramValue == "PassedValue modified")
	{
		Log("Python state parameter working");
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Problem with Python state parameter");
	}


	res.Reset();
	Log("testing evaluation (Python) of testtable1 with inputAttr1 = 'C', inputAttr2 = 58, get all, outsideAttr1 = 28", thread_id);
	state["inputAttr1"] = "C";
	state["inputAttr2"] = "58";
	state["outsideAttr1"] = "28";
	results4 = knowledge->EvaluateTable(tableName, true);
	if (results4.size() == 2 && results4["outputAttr1"].size() == 5 &&
		results4["outputAttr1"].at(0) == "2" &&
		results4["outputAttr1"].at(1) == "28 with concat" &&
		results4["outputAttr1"].at(2) == "4" &&
		results4["outputAttr1"].at(3) == "5" &&
		results4["outputAttr1"].at(4) == "30") //28 + 2
	{
		Log(results4["outputAttr1"].at(0), thread_id);
		Log(results4["outputAttr1"].at(1), thread_id);
		Log(results4["outputAttr1"].at(2), thread_id);
		Log(results4["outputAttr1"].at(3), thread_id);
		Log(results4["outputAttr1"].at(4), thread_id);
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result");
	}
#endif

	res.Reset();
	state["inputAttr1"] = "C";
	state["inputAttr2"] = "58";
	state["outsideAttr1"] = "28";
	Log("testing table chaining", thread_id);
	vector<string> result5 = knowledge->EvaluateTable("testtable2", "out1", true);
	if (result5.size() == 5 &&
		result5.at(0) == "2" &&
		result5.at(1) == "28 with concat" &&
		result5.at(2) == "4" &&
		result5.at(3) == "5" &&
#ifndef NOPYTHON
		result5.at(4) == "30") //28 + 2
#else
		result5.at(4) == "py(28 + 2)")
#endif
	{
		Log(result5.at(0), thread_id);
		Log(result5.at(1), thread_id);
		Log(result5.at(2), thread_id);
		Log(result5.at(3), thread_id);
		Log(result5.at(4), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on chain", thread_id);
	}

	res.Reset();
	Log("testing input get()", thread_id);
	state["someAttr"] = "3";
	vector<string> result6 = knowledge->EvaluateTable("testtable3", "outputAttr1", true);
	if (result6.size() == 1 && result6.at(0) == "inputAttr2: 58 is greater than someAttr: 3")
	{
		Log(result6.at(0), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on get() test", thread_id);
	}

	res.Reset();
	Log("testing NULL conditions", thread_id);
	state["inputAttr1"] = "";
	state["inputAttr2"] = "";
	vector<string> result7 = knowledge->EvaluateTable("testtable4", "outputAttr1", true);
	if (result7.size() == 4 && result7.at(2) == "both attrs are NULL")
	{
		Log(result7.at(0), thread_id);
		Log(result7.at(1), thread_id);
		Log(result7.at(2), thread_id);
		Log(result7.at(3), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on NULL test", thread_id);
	}

	state["inputAttr1"] = "blah";
	state["c"] = "";
	result7 = knowledge->EvaluateTable("testtable4", "outputAttr1", true);
	if (result7.size() == 2 && result7.at(0) == "inputAttr2 is NULL")
	{
		Log(result7.at(0), thread_id);
		Log(result7.at(1), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on NULL test", thread_id);
	}

	res.Reset();
	Log("testing exclusing evaluation", thread_id);
	state["inputAttr1"] = "A";
	vector<string> result8 = knowledge->EvaluateTable("exclusion", "outputAttr1", true);
	if (result8.size() == 4 && result8.at(0) == "not X or Y" &&
		result8.at(1) == "not X" &&
		result8.at(2) == "not Y" &&
		result8.at(3) == "fallout")
	{
		Log(result8.at(0), thread_id);
		Log(result8.at(1), thread_id);
		Log(result8.at(2), thread_id);
		Log(result8.at(3), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on exclusion test", thread_id);
	}

	Log("allowing a match: Y", thread_id);
	state["inputAttr1"] = "Y";
	result8 = knowledge->EvaluateTable("exclusion", "outputAttr1", true);
	if (result8.size() == 4 && result8.at(0) == "is X or Y" &&
		result8.at(1) == "not X" &&
		result8.at(2) == "is Y" &&
		result8.at(3) == "fallout")
	{
		Log(result8.at(0), thread_id);
		Log(result8.at(1), thread_id);
		Log(result8.at(2), thread_id);
		Log(result8.at(3), thread_id);
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on exclusion test 2", thread_id);
	}

	Log("testing translation of: A", thread_id);
	string localeValue = knowledge->Localize("A", "en-US");
	string reverse = knowledge->DeLocalize(localeValue);
	Log(localeValue + ":" + reverse, thread_id);
	if (localeValue == "A trans" && reverse == "A")
		res.SetResult(true, "", thread_id);
	else
		res.SetResult(false, "translation failed", thread_id);

	Log("testing reverse evaluation of ReverseTest table", thread_id);
	state["OutColor"] = "green";
	map<string, vector<string> > result9 = knowledge->ReverseEvaluateTable("ReverseTest", true);
	if (result9.size() == 2 && result9["Color1"].at(0) == "blue" &&
		result9["Color2"].at(0) == "yellow")
	{
		res.SetResult(true, "", thread_id);
	}
	else
	{
		res.SetResult(false, "reverse evaluation failed", thread_id);
	}

	return 0;
}
const int NUM_THREADS = 1;
int main()
{
    try
    {
        thread ts[NUM_THREADS];

        //test multi-threading
        for (int i = 0; i < NUM_THREADS; i++)
            ts[i] = thread(runTest, i);

        for (int i = 0; i < NUM_THREADS; i++)
            ts[i].join();
    }
    catch(exception& ex)
    {
        cout<<ex.what();
    }
    catch(...)
    {
        cout<<"Unknown error";
    }

    pause();
    return 0;
}


