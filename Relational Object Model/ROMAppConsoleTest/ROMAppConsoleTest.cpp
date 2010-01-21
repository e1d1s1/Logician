// ROMAppConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include "ROMTree.h"
#include "LinearEngine.h"

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
int main(int argc, char* argv[])
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

		Log("Getting attribute list on the base object");
		map<wstring, map<wstring, wstring> > allAttrs = myBaseObject.GetAllAttributes(myRootNode);
		for (map<wstring, map<wstring, wstring> >::iterator it = allAttrs.begin(); it != allAttrs.end(); it++)
		{
			wstring attrName = it->first;
			map<wstring, wstring> kvp = it->second;
			Log(L"Name: " + attrName);
			for (map<wstring, wstring>::iterator it_kvp = kvp.begin(); it_kvp != kvp.end(); it_kvp++)
			{
				Log(L"Key: " + it_kvp->first + L" Value: " + it_kvp->second);
			}
		}

		Log("loading rules");
		wstring path;
		#ifdef WIN32
		path = L"..\\..\\EDSEngine\\EDSEngineTestApp\\test_project.xml";
		#else
		path = L"../../EDSEngine/EDSEngineTestApp/test_project.xml";
		#endif
		if (myBaseObject.LoadRules(path))
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

			Log("Testing the LinearEngine class");
			LinearEngine engine(&myBaseObject, myRootNode, L"Dictionary");			

			Log("Checking dictionary size");
			map<wstring, ROMDictionaryAttribute>* attrs = engine.GetAllDictionaryAttrs();
			if (attrs->size() == 5)
				Log("size ok");
			else
				Log("FAILURE loading dictionary");

			vector<ROMDictionaryAttribute*>* order = engine.GetEvalList();
			if (order != NULL && order->size() == 5 &&
				order->at(0)->Name == L"cDictAttr1" &&
				order->at(1)->Name == L"dDictAttr2" &&
				order->at(2)->Name == L"aDictAttr3" &&
				order->at(3)->Name == L"bDictAttr4" &&
				order->at(4)->Name == L"eDictAttr5")
				Log("Order OK");
			else
				Log("FAILURE to assess the evaluation order");

			Log("Testing evaluation");
			engine.EvaluateAll();
			ROMDictionaryAttribute* attr1 = engine.GetDictionaryAttr("cDictAttr1");
			ROMDictionaryAttribute* attr2 = engine.GetDictionaryAttr("dDictAttr2");
			if (attr2->AvailableValues.size() == 0 && attr2->PossibleValues.size() == 3 && 
				attr1->AvailableValues.size() == 4)
				Log("Default Eval OK");
			else
				Log("FAILURE to initially evaluate an attribute");

			engine.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[0]);
			string val_pick1 = myBaseObject.GetAttribute(myRootNode, "dDictAttr2");
			string val_bool1 = myBaseObject.GetAttribute(myRootNode, "aDictAttr3");
			string val_multi1 = myBaseObject.GetAttribute(myRootNode, "bDictAttr4");
			string edit1 = myBaseObject.GetAttribute(myRootNode, "eDictAttr5");
			engine.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[1]);
			string val_pick2 = myBaseObject.GetAttribute(myRootNode, "dDictAttr2");
			string val_bool2 = myBaseObject.GetAttribute(myRootNode, "aDictAttr3");
			engine.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[2]);
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
		{
			Log("Could not load rules");
		}

		Log("Testing complete...press any key to exit");


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

