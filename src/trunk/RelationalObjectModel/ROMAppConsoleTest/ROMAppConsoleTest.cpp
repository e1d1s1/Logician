// ROMAppConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <memory>
#include "ROMNode.h"
#include "LinearEngine.h"

using namespace std;
using namespace ROM;

#ifndef WIN32
#include <sys/time.h>
#endif

class BaseObject : public ROMNode
{
public:
	BaseObject(wstring id, ObjectFactory factory) : ROMNode(id, factory) { }
	virtual ~BaseObject(void) {}

	using ROMNode::GetAttribute;
	virtual wstring GetAttribute(const wstring& id, const wstring& name, bool immediate = false) override
	{
		if (id == L"CLASS")
			return L"BaseObject";
		return ROMNode::GetAttribute(id, name, immediate);
	}
};

class ApplicationObject : public BaseObject
{
public:
	ApplicationObject(wstring id, ObjectFactory factory) : BaseObject(id, factory) { }
	virtual ~ApplicationObject(void) {}

	using BaseObject::GetAttribute;
	virtual wstring GetAttribute(const wstring& id, const wstring& name, bool immediate = false) override
	{
		if (id == L"CLASS")
			return L"ApplicationObject";
		return BaseObject::GetAttribute(id, name, immediate);;
	}
};

class DerivedObject : public BaseObject
{
public:
	DerivedObject(wstring id, ObjectFactory factory) : BaseObject(id, factory) {}
	virtual ~DerivedObject(void) {}

	using BaseObject::GetAttribute;
	virtual wstring GetAttribute(const wstring& id, const wstring& name, bool immediate = false) override
	{
		if (id == L"CLASS")
			return L"DerivedObject";
		return BaseObject::GetAttribute(id, name, immediate);
	}
};

class DerivedObject2 : public BaseObject
{
public:
	DerivedObject2(wstring id, ObjectFactory factory) : BaseObject(id, factory) { }
	virtual ~DerivedObject2(void) {}

	using BaseObject::GetAttribute;
	virtual wstring GetAttribute(const wstring& id, const wstring& name, bool immediate = false) override
	{
		if (id == L"CLASS")
			return L"DerivedObject2";
		return BaseObject::GetAttribute(id, name, immediate);
	}
};

ROMNode* ROMObjectFactory(wstring id)
{
	if (id == L"TestApplication")
		return new ApplicationObject(id, ROMObjectFactory);
	else if (id == L"DerivedObject")
		return new DerivedObject(id, ROMObjectFactory);
	else if (id == L"DerivedObject2")
		return new DerivedObject2(id, ROMObjectFactory);

	return nullptr;
}

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
    struct timeval t;
    gettimeofday ( &t, NULL );
    t.tv_sec %= 8640; // one day ticks 24*60*60
    return ( t.tv_sec*1000 + t.tv_usec/1000 );
    #endif
}

void cvtInt(int num, char *str)
{
    sprintf( str, "%d", num );
}

void DebugMessage(wstring msg)
{
	Log(L"DEBUGGER: " + msg);
}

void CreateChildNodes(ROMNode *rootNode)
{
	Log("Creating a child object");
	ROMNode *childNode = ROMObjectFactory(L"DerivedObject");
	rootNode->AddChildROMObject(childNode);
	childNode->SetAttribute(L"childAttr", L"some value of value");
	//setting a value on the Object Node
	childNode->SetROMObjectValue(L"valueTest", L"myValue");
	ROMNode *childOfChild = ROMObjectFactory(L"DerivedObject2");
	childNode->AddChildROMObject(childOfChild);
}

int runTest(int thread_id)
{
	long start = 0, elapsed = 0, testROMNodeStart = 0, testROMNodeEnd = 0;
	string msg;
	const int iMax = 1000;
	char time[32] = "";
	char temp[32] = "";
	wstring path, filename = L"test_project.gz";
	#ifdef WIN32
	path = L"..\\..\\EDSEngine\\EDSEngineTestApp\\test_project.gz";
	#else
	path = L"../../EDSEngine/EDSEngineTestApp/test_project.gz";
	#endif

    Log("Testing ROMNode Objects");
    Log("Creating root node");
	ApplicationObject rootNode(L"TestApplication", ROMObjectFactory);
    Log("Root ROMNode created");

    Log("Setting some attributes");
    rootNode.SetAttribute(L"inputAttr1", L"some value of test1");
    rootNode.SetAttribute(L"inputAttr2", L"some value of test2");
    rootNode.SetAttribute(L"inputAttr3", L"some value of test3");
    rootNode.SetAttribute(L"inputAttr3", L"test3a", L"some sub value of attr3_a");
    rootNode.SetAttribute(L"inputAttr3", L"test3b", L"some sub value of attr3_b");
    Log("Attrs set");

    Log("Testing what we have set");
    Log(L"inputAttr1 = " + rootNode.GetAttribute(L"inputAttr1"));

    CreateChildNodes(&rootNode);
    vector<ROMNode*> findTest = rootNode.FindAllObjectsByID("DerivedObject", true);
    vector<ROMNode*> findTestXPATH = rootNode.FindObjects("//Object[@id='DerivedObject']");
    vector<ROMNode*> findTestXPATH2 = rootNode.FindObjects("//Object[@id='DerivedObject2']");
    if (findTest.size() == 1 && findTestXPATH.size() == 1 && findTestXPATH2.size() == 1 &&
        findTestXPATH[0]->GetROMGUID() == findTest[0]->GetROMGUID() &&
        findTestXPATH2[0]->GetROMObjectID() == L"DerivedObject2")
        Log("OK");
    else
        Log("FAILURE creating/obtaining child object");


    Log("Dump current xml state");
    wstring result = rootNode.SaveXML(true);
    string s(result.begin(), result.end());
    Log(s);

    Log("Testing Cloning");
    auto clone = unique_ptr<ROMNode>(rootNode.Clone());

	Log("Dump clone xml state");
	result = clone->SaveXML(true);
	string sClone(result.begin(), result.end());
	Log(sClone);

    findTest = clone->FindAllObjectsByID("DerivedObject", true);
    findTestXPATH = clone->FindObjects("//Object[@id='DerivedObject']");
    findTestXPATH2 = clone->FindObjects("//Object[@id='DerivedObject2']");
    if (findTest.size() == 1 && findTestXPATH.size() == 1 && findTestXPATH2.size() == 1 &&
        findTestXPATH[0]->GetROMGUID() == findTest[0]->GetROMGUID() &&
        findTestXPATH2[0]->GetROMObjectID() == L"DerivedObject2" &&
		result.length() == s.length())
        Log("Cloned object OK");
    else
        Log("FAILURE cloning");

	Log("Test the object factory and inheritence pattern on the clone");
	auto rootTest = (ApplicationObject*)clone->FindAllObjectsByID("TestApplication", true)[0];
	auto childTest = (DerivedObject2*)clone->FindObjects("//Object[@id='DerivedObject2']")[0];
	if (rootTest->GetAttribute(L"CLASS", true) == L"ApplicationObject" &&
		childTest->GetAttribute(L"CLASS", true) == L"DerivedObject2")
		Log("inheritence OK");
	else
		Log("FAILURE in inheritence pattern");

	Log("Test loading from xml");
	auto loadedObj = unique_ptr<ROMNode>(ROMNode::LoadXMLFromString(result, ROMObjectFactory));
	wstring testLoaded = loadedObj->SaveXML(true);
	if (result.length() == testLoaded.length() &&
		rootNode.GetAllChildren(true).size() == loadedObj->GetAllChildren(true).size())
		Log("XML loading OK");
	else
		Log("FAILURE loading from xml");

	Log("Test the object factory and inheritence pattern on loaded XML");
	rootTest = (ApplicationObject*)loadedObj->FindAllObjectsByID("TestApplication", true)[0];
	childTest = (DerivedObject2*)loadedObj->FindObjects("//Object[@id='DerivedObject2']")[0];
	if (rootTest->GetAttribute(L"CLASS", true) == L"ApplicationObject" &&
		childTest->GetAttribute(L"CLASS", true) == L"DerivedObject2")
		Log("inheritence OK");
	else
		Log("FAILURE in inheritence pattern");

	Log("Test for equlity");
	auto childTest2 = (DerivedObject2*)loadedObj->FindObjects("//Object[@id='DerivedObject2']")[0];
	if ((ROMNode*)childTest == (ROMNode*)childTest2 && (ROMNode*)childTest != (ROMNode*)rootTest)
		Log("equality test OK");
	else
		Log("FAILURE equality test");


    Log("Setting attrs to test eval, inputAttr1 = A, inputAttr2 = 1");
    rootNode.SetAttribute(L"inputAttr1", L"A");
    rootNode.SetAttribute(L"inputAttr2", L"1");

    Log("New xml state");
    result = rootNode.SaveXML(true);
    string s2(result.begin(), result.end());
    Log(s2);

    Log("Getting attribute list on the base object");
    auto allAttrs2 = rootNode.GetAllAttributes();
    for (auto it = allAttrs2.begin(); it != allAttrs2.end(); it++)
    {
        wstring attrName = it->first;
        auto kvp = it->second;
        Log(L"Name: " + attrName);
        for (auto it_kvp = kvp.begin(); it_kvp != kvp.end(); it_kvp++)
        {
            Log(L"Key: " + it_kvp->first + L" Value: " + it_kvp->second);
        }
    }

    Log("loading rules");
	EDS::CKnowledgeBase* rules = new EDS::CKnowledgeBase();
	bool bLoaded = rules->CreateKnowledgeBase(path);
	if (!bLoaded)
		bLoaded = rules->CreateKnowledgeBase(filename);
	if (bLoaded && rules->IsOpen())
    {		
		rules->DebugHandlerPtr = DebugMessage;
		rules->InputValueGetterPtr = [](const wstring& attrName, void* obj)
		{
			return ((ROMNode*)obj)->GetAttribute(attrName, false);
		};
		rules->SetMaxThreads(4);

		rootNode.SetKnowledgeBase(rules);

        Log("...loaded");
        Log("Evaluating table testtable1");
        vector<wstring> res = rootNode.EvaluateTable(L"testtable1", L"outputAttr1", true);
        for (vector<wstring>::iterator it = res.begin(); it != res.end(); it++)
        {
            Log(*it);
        }
        Log("Evaluation complete");

        Log("Evaluating table testtable2: out1");
        vector<wstring> res2 = rootNode.EvaluateTable(L"testtable2", L"out1", true);
        for (vector<wstring>::iterator it = res2.begin(); it != res2.end(); it++)
        {
            Log(*it);
        }
        Log("Evaluation complete");

        Log("Testing the LinearEngine class");
        LinearEngine engine2(&rootNode, L"Dictionary");
		engine2.InitializeEngine();

        Log("Checking dictionary size");
        map<wstring, ROMDictionaryAttribute>* attrs = engine2.GetAllDictionaryAttrs();
        if (attrs->size() == 6)
            Log("size ok");
        else
            Log("FAILURE loading dictionary");

        vector<ROMDictionaryAttribute*> order = engine2.GetEvalList();
        if (order.size() == 6 &&
            order.at(0)->Name == L"cDictAttr1" &&
            order.at(1)->Name == L"dDictAttr2" &&
            order.at(2)->Name == L"aDictAttr3" &&
            order.at(3)->Name == L"bDictAttr4" &&
            order.at(4)->Name == L"eDictAttr5" &&
            order.at(5)->Name == L"eDictAttr6")
            Log("Order OK");
        else
            Log("FAILURE to assess the evaluation order");

		Log("Checking a subclass dictionary");
		ROMNode* childNode = ROMObjectFactory(L"DerivedObject");
		rootNode.AddChildROMObject(childNode);
		LinearEngine engineSubclass1(childNode, L"ClassDictionary");
		engineSubclass1.InitializeEngine();

		map<wstring, ROMDictionaryAttribute>* attrs2 = engineSubclass1.GetAllDictionaryAttrs();
		if (attrs2->size() == 2)
			Log("subclass dictionary ok");
		else
			Log("FAILURE loading subclass dictionary");

		ROMNode* childNode2 = ROMObjectFactory(L"DerivedObject2");
		rootNode.AddChildROMObject(childNode2);
		LinearEngine engineSubclass2(childNode2, L"ClassDictionary");
		engineSubclass2.InitializeEngine();

		map<wstring, ROMDictionaryAttribute>* attrs3 = engineSubclass2.GetAllDictionaryAttrs();
		if (attrs3->size() == 1)
			Log("subclass dictionary ok");
		else
			Log("FAILURE loading subClass2 dictionary");

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
        string val_pick1 = rootNode.GetAttribute("dDictAttr2");
        string val_bool1 = rootNode.GetAttribute("aDictAttr3");
        string val_multi1 = rootNode.GetAttribute("bDictAttr4");
        string edit1 = rootNode.GetAttribute("eDictAttr5");
        engine2.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[1]);
        string val_pick2 = rootNode.GetAttribute("dDictAttr2");
        string val_bool2 = rootNode.GetAttribute("aDictAttr3");
        engine2.EvaluateForAttribute(L"cDictAttr1", attr1->AvailableValues[2]);
        string val_bool3 = rootNode.GetAttribute("aDictAttr3");
        string val_multi3 = rootNode.GetAttribute("bDictAttr4");
        engine2.EvaluateForAttribute(L"eDictAttr5", L"999");
        string edit4 = rootNode.GetAttribute("eDictAttr5");
        if (val_pick1 == "ResultByOption1" && val_pick2 == "Result2" &&
            val_bool1 == "Y" && val_bool2 == "Y" && val_bool3 == "N" &&
            val_multi1 == "Selection2|Selection3" && val_multi3 == "Selection2" &&
            edit1 == "10" && edit4 == "50")
            Log("Evaluation of attributes ok");
        else
            Log("FAILURE to evaluate an attribute");

        engine2.EvaluateForAttribute(L"cDictAttr1", L"Option1");
        string dictAttr1 = rootNode.GetAttribute("cDictAttr1");
        engine2.EvaluateForAttribute(L"dDictAttr2", L"ResultByOption1");
        string dictAttr2 = rootNode.GetAttribute("dDictAttr2");
        string boolDict3 = rootNode.GetAttribute("aDictAttr3");
        string boolDict6 = rootNode.GetAttribute("eDictAttr6");
        engine2.EvaluateForAttribute(L"eDictAttr6", L"Y");
        boolDict6 = rootNode.GetAttribute("eDictAttr6");
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
        rootNode.SetAttribute(attrToSet, temp);
    }
    Log("Retrieving a bunch of values...");
    for (int i = 0; i < iMax; i++)
    {
        cvtInt(i, temp);
        string attrToGet = "attr";
        attrToGet+=temp;
        rootNode.GetAttribute(attrToGet);
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
        rootNode.AddChildROMObject(newNode);

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
    string xpathRes = rootNode.EvaluateXPATH(xpath);
    elapsed = getTickCount() - start;
    Log(xpath + " result: " + xpathRes);
    cvtInt(elapsed, time);
    msg = "object query test complete in ";
    msg+=time;
    msg+="ms";
    Log(msg);

    start = getTickCount();
    string xpath2 = "sum(//Attribute[@id='sumtester2']/@value)";
    string xpathRes2 = rootNode.EvaluateXPATH(xpath2);
    elapsed = getTickCount() - start;
    Log(xpath2 + " result: " + xpathRes2);
    cvtInt(elapsed, time);
    msg = "object query test complete in ";
    msg+=time;
    msg+="ms";
    Log(msg);

    Log("altering the object state");
    rootNode.SetAttribute("Change", "Y");

    start = getTickCount();
    string xpath3 = "sum(//Attribute[@id='sumtester3']/@value)";
    string xpathRes3 = rootNode.EvaluateXPATH(xpath3);
    elapsed = getTickCount() - start;
    Log(xpath3 + " result: " + xpathRes3);
    cvtInt(elapsed, time);
    msg = "object query test complete in ";
    msg+=time;
    msg+="ms";
    Log(msg);

    start = getTickCount();
    string xpath4 = "//Object[@id='SubLevel2Object10']/Attribute[@id='testvalue']/@value";
    string xpathRes4 = rootNode.EvaluateXPATH(xpath4);
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

    msg = "stress test total time: ";
    elapsed = testROMNodeEnd - testROMNodeStart;
    cvtInt(elapsed, time);
    msg += time;
    msg+="ms";
    Log(msg);
    Log("Testing complete...press any key to exit");

	return 0;
}

const int NUM_THREADS = 1;
int main(int argc, char* argv[])
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
    #ifdef USE_MSXML
	catch(_com_error &e)
	{
		wstring err = (_bstr_t)e.ErrorMessage();
		string strErr(err.begin(), err.end());
		cout<<strErr;
	}
    #endif
	catch(...)
	{
		cout<<"Unknown error";
	}

    Log("Press any key to continue...");
	getchar();
	return 0;
}
