// ROMAppConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include "ROMTree.h"

using namespace std;
using namespace ROM;

void Log(std::string strLogLine)
{
	std::cout<<strLogLine<<endl;
}
void Log(std::wstring strLogLine)
{
	string s(strLogLine.begin(), strLogLine.end());
	std::cout<<s<<endl;
}
int _tmain(int argc, _TCHAR* argv[])
{	
	try
	{
		Log("Creating application/doc object myBaseObject");
		ROMTree myBaseObject(L"Test Application");   
		Log("yyBaseObject Created");

		Log("Getting the root node");
		Node myRootNode = myBaseObject.GetRoot();
		Log("Root obtained");		

		Log("Setting some attributes");
		myBaseObject.SetAttribute(myRootNode, L"inputAttr1", L"some value of test1");
		myBaseObject.SetAttribute(myRootNode, L"inputAttr2", L"some value of test2");
		myBaseObject.SetAttribute(myRootNode, L"inputAttr3", L"some value of test3");
		myBaseObject.SetAttribute(myRootNode, L"inputAttr3", L"test3a", L"some sub value of attr3_a");
		myBaseObject.SetAttribute(myRootNode, L"inputAttr3", L"test3b", L"some sub value of attr3_b");
		Log("Attrs set");

		Log("Testing what we have set");
		Log(L"inputAttr1 = " + myBaseObject.GetAttribute(myRootNode, L"inputAttr1"));

		Log("Creating a child object");
		Node childNode = myBaseObject.CreateROMObject(L"ChildObject");
		myBaseObject.AddChildROMObject(myRootNode, childNode);
		myBaseObject.SetAttribute(childNode, L"childAttr", L"some value of value");
		//setting a value on the Object Node
		myBaseObject.SetROMObjectValue(childNode, L"valueTest", L"myValue");

		Log("Dump current xml state");
		wstring result = myBaseObject.DumpTree(1);
		string s(result.begin(), result.end());
		Log(s);

		Log("Setting attrs to test eval, inputAttr1 = A, inputAttr2 = 1");
		myBaseObject.SetAttribute(myRootNode, L"inputAttr1", L"A");
		myBaseObject.SetAttribute(myRootNode, L"inputAttr2", L"1");

		Log("New xml state");
		result = myBaseObject.DumpTree(1);
		string s2(result.begin(), result.end());
		Log(s2);

		Log("loading rules");
		if (myBaseObject.LoadRules(L"..\\EDSEngineTestApp\\test_project.xml"))
		{
			Log("...loaded");
			Log("Evaluating table testtable1");
			vector<wstring> res = myBaseObject.EvaluateTable(myRootNode, L"testtable1", L"outputAttr1", true);
			for (vector<wstring>::iterator it = res.begin(); it != res.end(); it++)
			{
				Log(*it);
			}
			Log("Evaluation complete");

			Log("Evaluating table testtable2: out1");
			vector<wstring> res2 = myBaseObject.EvaluateTable(myRootNode, L"testtable2", L"out1", true);
			for (vector<wstring>::iterator it = res2.begin(); it != res2.end(); it++)
			{
				Log(*it);
			}
			Log("Evaluation complete");
		}
		else
		{
			Log("Could not load rules");
		}


	}
#ifdef USE_MSXML
	catch(_com_error &e)
	{
		wstring err = (_bstr_t)e.ErrorMessage();
		string strErr(err.begin(), err.end());
		Log(strErr);
	}
#endif
	catch(...)
	{
		Log("Error");
	}
	
	std::puts("Press any key to continue...");
	std::getchar();
	return 0;
}

