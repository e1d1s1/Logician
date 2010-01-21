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

int main(int argc, char* argv[])
{
	//Loading
	TestResult res;
	Log("Loading Test project");
	//load up some sample table
	EDS::CKnowledgeBase knowledge(L"../EDSEngineTestApp/test_project.xml");
	if (!knowledge.IsOpen())
	{
		Log("Could not open rules file");
		pause();
		return 0;
	}

	string s = stringifyDouble(knowledge.TableCount());
	Log("# of Tables loaded: " + s);
	if (s == "9")
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
	if (allOutputs.size() == 7 &&
		allOutputs[0] == L"1" &&
		allOutputs[1] == L"2" &&
		allOutputs[2] == L"get(outsideAttr1) with concat" &&
		allOutputs[3] == L"4" &&
		allOutputs[4] == L"5" &&
		allOutputs[5] == L"py(get(outsideAttr1) + 2)" &&
		allOutputs[6] == L"js(get(outsideAttr1) + 2)")
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


	res.Reset();
	Log("testing table chaining");
	vector<wstring> result5 = knowledge.EvaluateTable(L"testtable2", L"out1", true);
	if (result5.size() == results4[L"outputAttr1"].size() &&
		result5.at(0) == L"2" &&
		result5.at(1) == L"28 with concat" &&
		result5.at(2) == L"4" &&
		result5.at(3) == L"5" &&
		result5.at(4) == L"30") //28 + 2
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

	Log("testing translation of: A");
	wstring localeValue = knowledge.Localize(L"A", L"en-US");
	wstring reverse = knowledge.DeLocalize(localeValue);
	Log(localeValue + L":" + reverse);
	if (localeValue == L"A trans" && reverse == L"A")
		res.SetResult(true, "");
	else
		res.SetResult(false, "translation failed");

	pause();

	return 0;
}


