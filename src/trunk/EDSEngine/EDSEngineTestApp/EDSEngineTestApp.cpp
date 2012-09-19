// EDSEngineTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <limits>
#include "../KnowledgeBase.h"


using namespace std;
using namespace EDS;

void Log(std::string strLogLine)
{
	std::cout<<strLogLine<<endl;
}

void Log(std::wstring strLogLine)
{
	std::wcout<<strLogLine<<endl;
}

std::string stringifyDouble(double x)
{
	std::ostringstream o;
	if (!(o << x))
	 throw std::runtime_error("stringify(double)");
	return o.str();
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

	void SetResult(bool res, string msg)
	{
		bPassed = res;
		strMessage = msg;

		if (bPassed)
		{
			string finalMsg = "OK";
			if (msg.length() > 0)
				finalMsg += ", " + msg;
			Log(finalMsg);
		}
		else
		{
			Log("FAILURE: " + msg);
		}
	}

	void Reset()
	{
		bPassed = false;
		strMessage = "";
	}
};

#ifndef __GNUC__
void pause()
{
	std::puts("Press any key to continue...");
 	std::getchar();
}
#endif

vector<TestResult> testResults;

void DebugMessage(wstring msg)
{
	Log(L"DEBUGGER: " + msg);
}

int main()
//int main(int argc, char* argv[])
{
	//Loading
	TestResult res;
	Log("Loading Test project");
	//load up some sample table
	#ifndef __GNUC__
	EDS::CKnowledgeBase knowledge(L"..\\EDSEngineTestApp\\test_project.gz");
	#else
	EDS::CKnowledgeBase knowledge(L"test_project.gz");
	#endif
	if (!knowledge.IsOpen())
	{
		Log("Could not open rules file");
		pause();
		return 0;
	}

	//push-style callback debugging
	knowledge.SetDebugHandler(DebugMessage);
	//pull-style debugging
	//knowledge.GenerateDebugMessages(true);

	string s = stringifyDouble(knowledge.TableCount());
	Log("# of Tables loaded: " + s);
	if (s == "14")
	{
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not load all the tables");
	}

	//getting a table's attrs
	wstring tableName = L"testtable1";
	res.Reset();
	Log("Loading attr output names for testtable1");
	vector<wstring> allOutputNames = knowledge.GetOutputAttrs(tableName);
	for (size_t i = 0; i < allOutputNames.size(); i++)
	{
		Log(allOutputNames[i]);
	}
	if (allOutputNames.size() == 2 && allOutputNames[0] == L"anotherOutput" && allOutputNames[1] == L"outputAttr1")
	{
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not load all the output names");
	}

	res.Reset();
	Log("Loading attr input names for testtable1");
	vector<wstring> allInputNames = knowledge.GetInputAttrs(tableName);
	for (size_t i = 0; i < allInputNames.size(); i++)
	{
		Log(allInputNames[i]);
	}
	if (allInputNames.size() == 2 && allInputNames[0] == L"inputAttr1" && allInputNames[1] == L"inputAttr2" )
	{
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not load all the input names");
	}

	res.Reset();
	Log("Loading dependency names for testtable1");
	vector<wstring> allDepNames = knowledge.GetInputDependencies(tableName);
	for (size_t i = 0; i < allDepNames.size(); i++)
	{
		Log(allDepNames[i]);
	}
	if (allDepNames.size() == 3 && allDepNames[0] == L"inputAttr1" &&
		allDepNames[1] == L"inputAttr2" &&
		allDepNames[2] == L"outsideAttr1" )
	{
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not load all the dependency names");
	}

	res.Reset();
	Log("Getting all possible output values for outputAttr1");
	vector<wstring> allOutputs = knowledge.GetAllPossibleOutputs(tableName, L"outputAttr1");
	for (size_t i = 0; i < allOutputs.size(); i++)
	{
		Log(allOutputs[i]);
	}
	if (allOutputs.size() == 9 &&
		allOutputs[0] == L"1" &&
		allOutputs[1] == L"2" &&
		allOutputs[2] == L"get(outsideAttr1) with concat" &&
		allOutputs[3] == L"4" &&
		allOutputs[4] == L"5" &&
		allOutputs[5] == L"py(get(outsideAttr1) + 2)" &&
		allOutputs[6] == L"js(get(outsideAttr1) + 2)" &&
		allOutputs[7] == L"js(alterparameter())" &&
		allOutputs[8] == L"py(alterparameter())")
	{
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not load all the possible output values");
	}

	res.Reset();
	Log("Checking the table type...");
	bool bIsGetAll = knowledge.TableIsGetAll(tableName);
	if (bIsGetAll)
		res.SetResult(true, "testtable1 is of type: GetAll");
	else
		res.SetResult(false, "testtable1 is not of type \"GetAll\" as expected");

	//testing table evaluation
	res.Reset();
	Log("testing evaluation of testtable1 with inputAttr1 = 'C', get first only");
	knowledge.SetInputValue(L"inputAttr1", L"C");
	map<wstring, vector<wstring> > results2 = knowledge.EvaluateTable(tableName, false);
	if (results2.size() == 2 && results2[L"outputAttr1"].size() == 1 &&
		results2[L"outputAttr1"].at(0) == L"2")
	{
		Log(results2[L"outputAttr1"].at(0));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result");
	}


	res.Reset();
	Log("testing evaluation of testtable1 with inputAttr1 = 'C', get all");
	knowledge.SetInputValue(L"inputAttr1", L"C");
	map<wstring, vector<wstring> > results = knowledge.EvaluateTable(tableName, bIsGetAll);
	if (results.size() == 2 && results[L"outputAttr1"].size() == 3 &&
		results[L"outputAttr1"].at(0) == L"2" &&
		results[L"outputAttr1"].at(1) == L"4" &&
		results[L"outputAttr1"].at(2) == L"5")
	{
		Log(results[L"outputAttr1"].at(0));
		Log(results[L"outputAttr1"].at(1));
		Log(results[L"outputAttr1"].at(2));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result");
	}

	res.Reset();
	Log("testing evaluation of testtable1 with inputAttr1 = 'C', inputAttr2 = 10, get all");
	knowledge.SetInputValue(L"inputAttr1", L"C");
	knowledge.SetInputValue(L"inputAttr2", L"10");
	map<wstring, vector<wstring> > results3 = knowledge.EvaluateTable(tableName, bIsGetAll);
	if (results3.size() == 2 && results3[L"outputAttr1"].size() == 4 &&
		results3[L"outputAttr1"].at(0) == L"2" &&
		results3[L"outputAttr1"].at(1) == L" with concat" && //empty becuase we never fed it outsideAttr1
		results3[L"outputAttr1"].at(2) == L"4" &&
		results3[L"outputAttr1"].at(3) == L"5")
	{
		Log(results3[L"outputAttr1"].at(0));
		Log(results3[L"outputAttr1"].at(1));
		Log(results3[L"outputAttr1"].at(2));
		Log(results3[L"outputAttr1"].at(3));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result");
	}

	res.Reset();
	Log("testing evaluation (Javascript) of testtable1 with inputAttr1 = 'C', inputAttr2 = 78, get all, outsideAttr1 = 28");
	knowledge.SetInputValue(L"inputAttr1", L"C");
	knowledge.SetInputValue(L"inputAttr2", L"78");
	knowledge.SetInputValue(L"outsideAttr1", L"28");
	map<wstring, vector<wstring> > results4 = knowledge.EvaluateTable(tableName, bIsGetAll);
	if (results4.size() == 2 && results4[L"outputAttr1"].size() == 5 &&
		results4[L"outputAttr1"].at(0) == L"2" &&
		results4[L"outputAttr1"].at(1) == L"28 with concat" &&
		results4[L"outputAttr1"].at(2) == L"4" &&
		results4[L"outputAttr1"].at(3) == L"5" &&
		results4[L"outputAttr1"].at(4) == L"30") //28 + 2
	{
		Log(results4[L"outputAttr1"].at(0));
		Log(results4[L"outputAttr1"].at(1));
		Log(results4[L"outputAttr1"].at(2));
		Log(results4[L"outputAttr1"].at(3));
		Log(results4[L"outputAttr1"].at(4));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result");
	}

	res.Reset();
	Log("testing evaluation (Javascript) with state parameter on testtable1 with inputAttr1 = 'TestParameterJS' and inputAttr2 = 'TestParameterJS'");
	knowledge.SetInputValue(L"inputAttr1", L"TestParameterJS");
	knowledge.SetInputValue(L"inputAttr2", L"TestParameterJS");
	vector<wstring> results9 = knowledge.EvaluateTableWithParam(tableName, (wstring)L"outputAttr1", (wstring)L"PassedValue");
	wstring retParam = knowledge.GetEvalParameter();
	if (results9.size() == 4 && results9.at(3) == L"eval ok" &&
		retParam == L"PassedValue modified")
	{
		Log("Javascript state parameter working");
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Problem with Javascript state parameter");
	}
#ifndef NOPYTHON
	res.Reset();
	Log("testing evaluation (Python) with state parameter on testtable1 with inputAttr1 = 'TestParameterPY' and inputAttr2 = 'TestParameterPY'");
	knowledge.SetInputValue(L"inputAttr1", L"TestParameterPY");
	knowledge.SetInputValue(L"inputAttr2", L"TestParameterPY");
	vector<wstring> results10 = knowledge.EvaluateTableWithParam(tableName, (wstring)L"outputAttr1", (wstring)L"PassedValue");
	retParam = knowledge.GetEvalParameter();
	if (results10.size() == 4 && results10.at(3) == L"eval ok" &&
		retParam == L"PassedValue modified")
	{
		Log("Python state parameter working");
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Problem with Python state parameter");
	}


	res.Reset();
	Log("testing evaluation (Python) of testtable1 with inputAttr1 = 'C', inputAttr2 = 58, get all, outsideAttr1 = 28");
	knowledge.SetInputValue(L"inputAttr1", L"C");
	knowledge.SetInputValue(L"inputAttr2", L"58");
	knowledge.SetInputValue(L"outsideAttr1", L"28");
	results4 = knowledge.EvaluateTable(tableName, true);
	if (results4.size() == 2 && results4[L"outputAttr1"].size() == 5 &&
		results4[L"outputAttr1"].at(0) == L"2" &&
		results4[L"outputAttr1"].at(1) == L"28 with concat" &&
		results4[L"outputAttr1"].at(2) == L"4" &&
		results4[L"outputAttr1"].at(3) == L"5" &&
		results4[L"outputAttr1"].at(4) == L"30") //28 + 2
	{
		Log(results4[L"outputAttr1"].at(0));
		Log(results4[L"outputAttr1"].at(1));
		Log(results4[L"outputAttr1"].at(2));
		Log(results4[L"outputAttr1"].at(3));
		Log(results4[L"outputAttr1"].at(4));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result");
	}
#endif

	res.Reset();
	knowledge.SetInputValue(L"inputAttr1", L"C");
	knowledge.SetInputValue(L"inputAttr2", L"58");
	knowledge.SetInputValue(L"outsideAttr1", L"28");
	Log("testing table chaining");
	vector<wstring> result5 = knowledge.EvaluateTable(L"testtable2", L"out1", true);
	if (result5.size() == results4[L"outputAttr1"].size() &&
		result5.at(0) == L"2" &&
		result5.at(1) == L"28 with concat" &&
		result5.at(2) == L"4" &&
		result5.at(3) == L"5" &&
#ifndef NOPYTHON
		result5.at(4) == L"30") //28 + 2
#else
		result5.at(4) == L"py(28 + 2)")
#endif
	{
		Log(result5.at(0));
		Log(result5.at(1));
		Log(result5.at(2));
		Log(result5.at(3));
		Log(result5.at(4));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on chain");
	}

	res.Reset();
	Log("testing input get()");
	knowledge.SetInputValue(L"someAttr", L"3");
	vector<wstring> result6 = knowledge.EvaluateTable(L"testtable3", L"outputAttr1", true);
	if (result6.size() == 1 && result6.at(0) == L"inputAttr2: 58 is greater than someAttr: 3")
	{
		Log(result6.at(0));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on get() test");
	}

	res.Reset();
	Log("testing NULL conditions");
	knowledge.SetInputValue(L"inputAttr1", L"");
	knowledge.SetInputValue(L"inputAttr2", L"");
	vector<wstring> result7 = knowledge.EvaluateTable(L"testtable4", L"outputAttr1", true);
	if (result7.size() == 4 && result7.at(2) == L"both attrs are NULL")
	{
		Log(result7.at(0));
		Log(result7.at(1));
		Log(result7.at(2));
		Log(result7.at(3));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on NULL test");
	}

	knowledge.SetInputValue(L"inputAttr1", L"blah");
	knowledge.SetInputValue(L"c", L"");
	result7 = knowledge.EvaluateTable(L"testtable4", L"outputAttr1", true);
	if (result7.size() == 2 && result7.at(0) == L"inputAttr2 is NULL")
	{
		Log(result7.at(0));
		Log(result7.at(1));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on NULL test");
	}

	res.Reset();
	Log("testing exclusing evaluation");
	knowledge.SetInputValue(L"inputAttr1", L"A");
	vector<wstring> result8 = knowledge.EvaluateTable(L"exclusion", L"outputAttr1", true);
	if (result8.size() == 4 && result8.at(0) == L"not X or Y" &&
		result8.at(1) == L"not X" &&
		result8.at(2) == L"not Y" &&
		result8.at(3) == L"fallout")
	{
		Log(result8.at(0));
		Log(result8.at(1));
		Log(result8.at(2));
		Log(result8.at(3));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on exclusion test");
	}

	Log("allowing a match: Y");
	knowledge.SetInputValue(L"inputAttr1", L"Y");
	result8 = knowledge.EvaluateTable(L"exclusion", L"outputAttr1", true);
	if (result8.size() == 4 && result8.at(0) == L"is X or Y" &&
		result8.at(1) == L"not X" &&
		result8.at(2) == L"is Y" &&
		result8.at(3) == L"fallout")
	{
		Log(result8.at(0));
		Log(result8.at(1));
		Log(result8.at(2));
		Log(result8.at(3));
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "Did not get proper eval result on exclusion test 2");
	}

	Log("testing translation of: A");
	wstring localeValue = knowledge.Localize(L"A", L"en-US");
	wstring reverse = knowledge.DeLocalize(localeValue);
	Log(localeValue + L":" + reverse);
	if (localeValue == L"A trans" && reverse == L"A")
		res.SetResult(true, "");
	else
		res.SetResult(false, "translation failed");

	Log("testing reverse evaluation of ReverseTest table");
	knowledge.SetInputValue(L"OutColor", L"green");
	map<wstring, vector<wstring> > result9 = knowledge.ReverseEvaluateTable(L"ReverseTest", true);
	if (result9.size() == 2 && result9[L"Color1"].at(0) == L"blue" &&
		result9[L"Color2"].at(0) == L"yellow")
	{
		res.SetResult(true, "");
	}
	else
	{
		res.SetResult(false, "reverse evaluation failed");
	}

	//Log(L"Debugging output:\n" + knowledge.GetDebugMessages());
	pause();

	return 0;
}


