// ROMAppConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "ROMNode.h"
#include "LinearEngine.h"

using namespace std;
using namespace ROM;

#ifdef WIN32
#include <iostream>
#include <windows.h>

std::wostream& operator<< (std::wostream& o, const wchar_t * const s) throw(...)
{
	const UINT cp = CP_UTF8;
	bool error = false;
	if (s)
	{
		int bufferSize = WideCharToMultiByte(cp, 0, s, -1, NULL,          0, NULL, NULL);
		char* m = new char[bufferSize];
		//Remember the old console codepage.
		UINT oldcp = GetConsoleOutputCP();
		//Change it to what we want.
		SetConsoleOutputCP (cp);
		/*            */ WideCharToMultiByte(cp, 0, s, -1,    m, bufferSize, NULL, NULL);
		if (o == std::wcout) fwprintf(stdout, L"%S", m);
		else if (o == std::wcerr) fwprintf(stderr, L"%S", m);
		else error = true;
		//It would be nice to have this instead, but unfortunately it does not work.
		//o << m;
		//Revert to the old codepage.
		SetConsoleOutputCP (oldcp);
		delete[] m;
	}
	else
	{
		//If s == NULL we must not try to convert with WideCharToMultiByte or we will get junk.
		//We go straight to fwprintf instead that prints "(null)" - We do not need to convert "(null)"
		//because there are no Unicode characters in it ;)
		if (o == std::wcout) fwprintf(stdout, L"%s", s);
		else if (o == std::wcerr) fwprintf(stderr, L"%s", s);
		else error = true;
	}
	if (error) throw "Not wcout or wcerr";
	return o;
}
#endif

void Log(std::string strLogLine)
{
	std::cout<<strLogLine<<endl;
}

void Log(std::wstring strLogLine)
{
	std::wcout<<strLogLine.c_str()<<endl;
}

long getTickCount()
{
    #ifdef WIN32
    return GetTickCount();
    #else

    #endif
}

void cvtInt(long num, char *str)
{
    sprintf( str, "%d", num );
}

void DebugMessage(wstring msg)
{
	Log(L"DEBUGGER: " + msg);
}

int main(int argc, char* argv[])
{
	long start = 0, elapsed = 0, testROMTreeStart = 0, testROMTreeEnd = 0, testROMNodeStart = 0, testROMNodeEnd = 0;
	string msg;
	const int iMax = 1000;
	char time[32] = "";
	char temp[32] = "";
	wstring path;
	#ifdef WIN32
	path = L"..\\..\\EDSEngine\\EDSEngineTestApp\\test_project.xml";
	#else
	path = L"../../EDSEngine/EDSEngineTestApp/test_project.xml";
	#endif

	try
	{

		Log("Testing ROMNode Objects");
		Log("Creating root node");
		ROMNode* rootNode = new ROMNode(L"TestApplication");
		Log("Root ROMNode created");		

		Log("Setting some attributes");
		rootNode->SetAttribute(L"inputAttr1", L"some value of test1");
		rootNode->SetAttribute(L"inputAttr2", L"some value of test2");
		rootNode->SetAttribute(L"inputAttr3", L"some value of test3");
		rootNode->SetAttribute(L"inputAttr3", L"test3a", L"some sub value of attr3_a");
		rootNode->SetAttribute(L"inputAttr3", L"test3b", L"some sub value of attr3_b");
		Log("Attrs set");

		Log("Testing what we have set");
		Log(L"inputAttr1 = " + rootNode->GetAttribute(L"inputAttr1"));

		Log("Creating a child object");
		ROMNode *childNode = new ROMNode(L"ChildObject");
		rootNode->AddChildROMObject(childNode);
		childNode->SetAttribute(L"childAttr", L"some value of value");
		//setting a value on the Object Node
		childNode->SetROMObjectValue(L"valueTest", L"myValue");
		ROMNode *childOfChild = new ROMNode(L"ChildObject2");
		childNode->AddChildROMObject(childOfChild);
		vector<ROMNode*> findTest = rootNode->FindAllObjectsByID("ChildObject", true);
		vector<ROMNode*> findTestXPATH = rootNode->FindObjects("//Object[@id='ChildObject']");
		vector<ROMNode*> findTestXPATH2 = rootNode->FindObjects("//Object[@id='ChildObject2']");
		if (findTest.size() == 1 && findTestXPATH.size() == 1 && findTestXPATH2.size() == 1 &&
			findTestXPATH[0]->GetROMGUID() == findTest[0]->GetROMGUID() &&
			findTestXPATH2[0]->GetROMObjectID() == L"ChildObject2")
			Log("OK");
		else
			Log("FAILURE creating/obtaining child object");


		Log("Dump current xml state");
		wstring result = rootNode->SaveXML(true);
		string s(result.begin(), result.end());
		Log(s);

		Log("Setting attrs to test eval, inputAttr1 = A, inputAttr2 = 1");
		rootNode->SetAttribute(L"inputAttr1", L"A");
		rootNode->SetAttribute(L"inputAttr2", L"1");

		Log("New xml state");
		result = rootNode->SaveXML(true);
		string s2(result.begin(), result.end());
		Log(s2);

		Log("Getting attribute list on the base object");
		FASTMAP_MAPS allAttrs2 = rootNode->GetAllAttributes();
		for (FASTMAP_MAPS::iterator it = allAttrs2.begin(); it != allAttrs2.end(); it++)
		{
			wstring attrName = it->first;
			FASTMAP kvp = it->second;
			Log(L"Name: " + attrName);
			for (FASTMAP::iterator it_kvp = kvp.begin(); it_kvp != kvp.end(); it_kvp++)
			{
				Log(L"Key: " + it_kvp->first + L" Value: " + it_kvp->second);
			}
		}

		Log("loading rules");
		if (rootNode->LoadRules(path))
		{
			rootNode->SetTableDebugHandler(DebugMessage);
			Log("...loaded");
			Log("Evaluating table testtable1");
			vector<wstring> res = rootNode->EvaluateTable(L"testtable1", L"outputAttr1", true);
			for (vector<wstring>::iterator it = res.begin(); it != res.end(); it++)
			{
				Log(*it);
			}
			Log("Evaluation complete");

			Log("Evaluating table testtable2: out1");
			vector<wstring> res2 = rootNode->EvaluateTable(L"testtable2", L"out1", true);
			for (vector<wstring>::iterator it = res2.begin(); it != res2.end(); it++)
			{
				Log(*it);
			}
			Log("Evaluation complete");

			Log("Testing the LinearEngine class");
			LinearEngine engine2(rootNode, L"ClassDictionary");

			Log("Checking dictionary size");
			map<wstring, ROMDictionaryAttribute>* attrs = engine2.GetAllDictionaryAttrs();
			if (attrs->size() == 5)
				Log("size ok");
			else
				Log("FAILURE loading dictionary");

			vector<ROMDictionaryAttribute*> order = engine2.GetEvalList();
			if (order.size() == 5 &&
				order.at(0)->Name == L"cDictAttr1" &&
				order.at(1)->Name == L"dDictAttr2" &&
				order.at(2)->Name == L"aDictAttr3" &&
				order.at(3)->Name == L"bDictAttr4" &&
				order.at(4)->Name == L"eDictAttr5")
				Log("Order OK");
			else
				Log("FAILURE to assess the evaluation order");

			map<wstring, vector<wstring> > triggers = engine2.GetTriggers();
			if (triggers.size() == 3 &&
				triggers[L"aDictAttr3"].size() == 2 &&
				triggers[L"aDictAttr3"].at(0) == L"bDictAttr4" &&
				triggers[L"aDictAttr3"].at(1) == L"eDictAttr5")
				Log("Triggers OK");
			else
				Log("FAILURE to assess the triggers");

			Log("Testing evaluation");
			engine2.EvaluateAll();
			ROMDictionaryAttribute* attr1 = engine2.GetDictionaryAttr("cDictAttr1");
			ROMDictionaryAttribute* attr2 = engine2.GetDictionaryAttr("dDictAttr2");
			if (attr2->AvailableValues.size() == 0 && attr2->PossibleValues.size() == 3 &&
				attr1->AvailableValues.size() == 4)
				Log("Default Eval OK");
			else
				Log("FAILURE to initially evaluate an attribute");

			engine2.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[0]);
			string val_pick1 = rootNode->GetAttribute("dDictAttr2");
			string val_bool1 = rootNode->GetAttribute("aDictAttr3");
			string val_multi1 = rootNode->GetAttribute("bDictAttr4");
			string edit1 = rootNode->GetAttribute("eDictAttr5");
			engine2.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[1]);
			string val_pick2 = rootNode->GetAttribute("dDictAttr2");
			string val_bool2 = rootNode->GetAttribute("aDictAttr3");
			engine2.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[2]);
			string val_bool3 = rootNode->GetAttribute("aDictAttr3");
			string val_multi3 = rootNode->GetAttribute("bDictAttr4");
			engine2.EvaluateForAttribute("eDictAttr5", "999");
			string edit4 = rootNode->GetAttribute("eDictAttr5");
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

		start = getTickCount();
		testROMNodeStart = start;
		Log("Stress Testing...");
		Log("Setting a bunch of values...");
		for (int i = 0; i < iMax; i++)
		{
			cvtInt(i, temp);
			string attrToSet = "attr";
			attrToSet+=temp;
			rootNode->SetAttribute(attrToSet, temp);
		}
		Log("Retrieving a bunch of values...");
		for (int i = 0; i < iMax; i++)
		{
			cvtInt(i, temp);
			string attrToGet = "attr";
			attrToGet+=temp;
			rootNode->GetAttribute(attrToGet);
		}
		elapsed = getTickCount() - start;
		cvtInt(elapsed, time);
		msg = "attribute test complete in ";
		msg+=time;
		msg+="ms";
		Log(msg);

		start = getTickCount();
		Log("Create a large object heierarchy for query testing");
		for (int i = 0; i < iMax; i++)
		{
			cvtInt(i, temp);
			string objectToCreate = "SubLevel1Object";
			objectToCreate+=temp;
			ROMNode *newNode = new ROMNode(objectToCreate);
			rootNode->AddChildROMObject(newNode);

			objectToCreate = "SubLevel2Object";
			objectToCreate+=temp;
			ROMNode *newNode2 = new ROMNode(objectToCreate);
			newNode->AddChildROMObject(newNode2);
			newNode2->SetAttribute("sumtester", "1");
			newNode2->SetAttribute("sumtester2", "2");
			newNode2->SetAttribute("sumtester3", "3");
			newNode2->SetAttribute("testvalue", "value found");
		}
		elapsed = getTickCount() - start;
		cvtInt(elapsed, time);
		msg = "objects created in ";
		msg+= time;
		msg+="ms";
		Log(msg);

		Log("Performing queries");
		start = getTickCount();
		long queryTime = start;
		string xpath = "sum(//Attribute[@id='sumtester']/@value)";
		string xpathRes = rootNode->EvaluateXPATH(xpath);
		elapsed = getTickCount() - start;
		Log(xpath + " result: " + xpathRes);
		cvtInt(elapsed, time);
		msg = "object query test complete in ";
		msg+=time;
		msg+="ms";
		Log(msg);

		start = getTickCount();
		string xpath2 = "sum(//Attribute[@id='sumtester2']/@value)";
		string xpathRes2 = rootNode->EvaluateXPATH(xpath2);
		elapsed = getTickCount() - start;
		Log(xpath2 + " result: " + xpathRes2);
		cvtInt(elapsed, time);
		msg = "object query test complete in ";
		msg+=time;
		msg+="ms";
		Log(msg);

		Log("altering the object state");
		rootNode->SetAttribute("Change", "Y");

		start = getTickCount();
		string xpath3 = "sum(//Attribute[@id='sumtester3']/@value)";
		string xpathRes3 = rootNode->EvaluateXPATH(xpath3);
		elapsed = getTickCount() - start;
		Log(xpath3 + " result: " + xpathRes3);
		cvtInt(elapsed, time);
		msg = "object query test complete in ";
		msg+=time;
		msg+="ms";
		Log(msg);

		start = getTickCount();
		string xpath4 = "//Object[@id='SubLevel2Object10']/Attribute[@id='testvalue']/@value";
		string xpathRes4 = rootNode->EvaluateXPATH(xpath4);
		long finished = getTickCount();
		testROMNodeEnd = finished;
		elapsed = finished - start;
		Log(xpath4 + " result: " + xpathRes4);
		cvtInt(elapsed, time);
		msg = "object query test complete in ";
		msg+=time;
		msg+="ms";
		Log(msg);
		elapsed = finished - queryTime;
		cvtInt(elapsed, time);
		msg = "All object query tests completed in ";
		msg+=time;
		msg+="ms";
		Log(msg);

		delete rootNode;


		msg = "stress test total time: ";
		elapsed = testROMNodeEnd - testROMNodeStart;
		cvtInt(elapsed, time);
		msg += time;
		msg+="ms";
		Log(msg);
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

	Log("Press any key to continue...");
	getchar();
	return 0;
}

/* legacy test code for ROMTree
Log("Test ROMTree Objects");
Log("Creating application/doc object myBaseObject");
ROMTree myBaseObject("Test Application");
Log("BaseObject Created");

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


start = GetTickCount();
testROMTreeStart = start;
Log("Stress Testing...");
Log("Setting a bunch of values...");
for (int i = 0; i < iMax; i++)
{
	itoa(i, temp, 10);
	string attrToSet = "attr";
	attrToSet+=temp;
	myBaseObject.SetAttribute(myRootNode, attrToSet, temp);
}
Log("Retrieving a bunch of values...");
for (int i = 0; i < iMax; i++)
{
	itoa(i, temp, 10);
	string attrToGet = "attr";
	attrToGet+=temp;
	myBaseObject.GetAttribute(myRootNode, attrToGet);
}
elapsed = GetTickCount() - start;
ltoa(elapsed, time, 10);
msg = "attribute test complete in ";
msg+=time;
msg+="ms";
Log(msg);

start = GetTickCount();
Log("Create a large object heierarchy for querey testing");
for (int i = 0; i < iMax; i++)
{
	itoa(i, temp, 10);
	string objectToCreate = "SubLevel1Object";
	objectToCreate+=temp;
	Node newNode = myBaseObject.CreateROMObject(objectToCreate);
	myBaseObject.AddChildROMObject(myRootNode, newNode);

	objectToCreate = "SubLevel2Object";
	objectToCreate+=temp;
	Node newNode2 = myBaseObject.CreateROMObject(objectToCreate);
	myBaseObject.AddChildROMObject(newNode, newNode2);
	myBaseObject.SetAttribute(newNode2, "sumtester", "1");
	myBaseObject.SetAttribute(newNode2, "sumtester2", "2");
	myBaseObject.SetAttribute(newNode2, "sumtester3", "3");
	myBaseObject.SetAttribute(newNode2, "testvalue", "value found");
}
elapsed = GetTickCount() - start;
ltoa(elapsed, time, 10);
msg = "objects created in ";
msg+= time;
msg+="ms";
Log(msg);

Log("Performing queries");
start = GetTickCount();
long queryTime = start;
string xpath = "sum(//Attribute[@id='sumtester']/@value)";
string xpathRes = myBaseObject.EvaluateXPATH(myRootNode, xpath);
elapsed = GetTickCount() - start;
Log(xpath + " result: " + xpathRes);
ltoa(elapsed, time, 10);
msg = "object query test complete in ";
msg+=time;
msg+="ms";
Log(msg);

start = GetTickCount();
string xpath2 = "sum(//Attribute[@id='sumtester2']/@value)";
string xpathRes2 = myBaseObject.EvaluateXPATH(myRootNode, xpath2);
elapsed = GetTickCount() - start;
Log(xpath2 + " result: " + xpathRes2);
ltoa(elapsed, time, 10);
msg = "object query test complete in ";
msg+=time;
msg+="ms";
Log(msg);

Log("altering the object state");
myBaseObject.SetAttribute(myRootNode, "Change", "Y");

start = GetTickCount();
string xpath3 = "sum(//Attribute[@id='sumtester3']/@value)";
string xpathRes3 = myBaseObject.EvaluateXPATH(myRootNode, xpath3);
elapsed = GetTickCount() - start;
Log(xpath3 + " result: " + xpathRes3);
ltoa(elapsed, time, 10);
msg = "object query test complete in ";
msg+=time;
msg+="ms";
Log(msg);

start = GetTickCount();
string xpath4 = "//Object[@id='SubLevel2Object10']/Attribute[@id='testvalue']/@value";
string xpathRes4 = myBaseObject.EvaluateXPATH(myRootNode, xpath4);
long finished = GetTickCount();
testROMTreeEnd = finished;
elapsed = finished - start;
Log(xpath4 + " result: " + xpathRes4);
ltoa(elapsed, time, 10);
msg = "object query test complete in ";
msg+=time;
msg+="ms";
Log(msg);
ltoa(elapsed, time, 10);

elapsed = finished - queryTime;
ltoa(elapsed, time, 10);
msg = "All object query tests completed in ";
msg+=time;
msg+="ms";
Log(msg);
*/
