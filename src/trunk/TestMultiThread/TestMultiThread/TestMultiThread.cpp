// TestMultiThread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KnowledgeBase.h"


#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <deque>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <thread>

using namespace std;
using namespace EDS;

class TraceMessage
{
public:
	//ConsoleColor Color;
	TraceMessage(const wstring& msg, int color){ Message = msg; Color = color; }
	wstring Message;
	int Color;
};

int TESTCNT = 100;
vector<wstring> colors1 = { L"red", L"blue" };
vector<wstring> colors2 = { L"yellow", L"blue" };
vector<vector<pair<wstring, wstring>>> testValues;
recursive_mutex _messageMutex;

void pause()
{
	std::puts("Press any key to continue...");
#ifndef __GNUC__
	std::getchar();
#else
	std::cin.get();
#endif
}

void write_result(wstring& msg)
{
	std::wcout << msg;
}

void write_result(const wchar_t* msg)
{
	std::wcout << msg;
}

wstring get_value(const wstring& name, void* context)
{
	map<wstring, wstring>* state = (map<wstring, wstring>*)context;
	if (state->find(name) != state->end())
		return (*state)[name];
	else
		return L"";
	
}

void TestCycle(CKnowledgeBase* rules, deque<TraceMessage>* messages, deque<wstring>* results, int threadID = 0)
{
	vector<void*> pointersToDelete;
	for (int cnt = 0; cnt < TESTCNT; cnt++)
	{
		int color = rand() % 15 + 1;
		wstring msg1 = std::to_wstring(threadID) + L" - begun " + std::to_wstring(cnt) + L": " + testValues[cnt][0].second + L"," + testValues[cnt][1].second;
		
		{
			lock_guard<recursive_mutex> lock(_messageMutex);
			messages->push_back(TraceMessage(msg1, color));
		}
		
		wstring retval = L"";
		map<wstring, wstring>* thisState = new map<wstring, wstring>();
		thisState->insert(testValues[cnt][0]);
		thisState->insert(testValues[cnt][1]);
		pointersToDelete.push_back(thisState);

		retval = rules->GetFirstTableResult(L"ReverseTest", L"OutColor", thisState);

		wstring msg2 = std::to_wstring(threadID) + L" - complete " + std::to_wstring(cnt) + L": " + retval;
		{
			lock_guard<recursive_mutex> lock(_messageMutex);
			messages->push_back(TraceMessage(msg2, color));
		}
		
		results->push_back(retval);
		//write_result(msg); 
	}

	for (auto ptr : pointersToDelete)
		delete ptr;
}

int _tmain(int argc, _TCHAR* argv[])
{
	deque<TraceMessage> trace;
	deque<wstring> resultsSingleThread, resultsThread1, resultsThread2, resultsThread3, resultsThread4, resultsThread5;

	wstring filename = L"test_project.gz";
	wstring path = L"..\\..\\EDSEngine\\EDSEngineTestApp\\" + filename;
	CKnowledgeBase* knowledge = new CKnowledgeBase(path);
	if (knowledge->IsOpen() == false)
	{
		delete knowledge;
		knowledge = new CKnowledgeBase(filename);
		if (knowledge->IsOpen() == false)
		{
			write_result(L"FAILURE: Could not open rules xml file: " + path);
			delete knowledge;
			return 1;
		}
	}

	if (knowledge->IsOpen() == true)
		write_result(L"OK: File opened");

	knowledge->InputValueGetterPtr = get_value;

	srand(time(NULL));
	for (int cnt = 0; cnt < TESTCNT; cnt++)
	{
		wstring color1 = colors1[rand() % 2];
		wstring color2 = colors2[rand() % 2];
		
		testValues.push_back(vector<pair<wstring, wstring>> { pair<wstring, wstring>(L"Color1", color1), pair<wstring, wstring>(L"Color2", color2) });
	}

	//single threaded result           
	TestCycle(knowledge, &trace, &resultsSingleThread, 0);

	const int NUM_THREADS = 5;
	thread ts[NUM_THREADS];

	//test multi-threading
	for (int i = 0; i < NUM_THREADS; i++)
		switch (i)
	{
		case 0:
			ts[i] = thread(TestCycle, knowledge, &trace, &resultsThread1, 1);
			break;
		case 1:
			ts[i] = thread(TestCycle, knowledge, &trace, &resultsThread2, 2);
			break;
		case 2:
			ts[i] = thread(TestCycle, knowledge, &trace, &resultsThread3, 3);
			break;
		case 3:
			ts[i] = thread(TestCycle, knowledge, &trace, &resultsThread4, 4);
			break;
		case 4:
			ts[i] = thread(TestCycle, knowledge, &trace, &resultsThread5, 5);
			break;
	}		

	for (int i = 0; i < NUM_THREADS; i++)
		ts[i].join();

#ifdef WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	for (auto& traceItem : trace)
	{
		wstring msg = traceItem.Message + L"\n";
#ifdef WIN32
		SetConsoleTextAttribute(hConsole, traceItem.Color);
#endif
		write_result(msg);
	}


	delete knowledge;

	pause();
	return 0;
}

