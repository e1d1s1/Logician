// ColorMixConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "KnowledgeBase.h"

using namespace std;

#ifndef __GNUC__
void pause()
{
    system ("pause");
}
#endif

EDS::CKnowledgeBase m_TableEvaluator;
map<wstring, wstring> mAppData;

string GetSingleSolution(string tableToEvaluate, string nameOfOutput) //could reuse this function for all similar aplication events
{
  vector<string> results = m_TableEvaluator.EvaluateTable(tableToEvaluate, nameOfOutput);
  //EDSEngine supports returning multiple true results on a sigle line, but in this case we just want a single result (the first one it finds)
  
  if (results.size() > 0)
    return results[0];
  else
    return ""; //shouldn't happen since we have a "fallout" rule at the end of the table for unspecified combos
}

string GetResultingColor()
{  
  return GetSingleSolution("ColorMixingTable", "ResultColor");  
}

int main(int argc, char* argv[])
{
	cout<<"Loading the rules file ColorRules.xml...";
	m_TableEvaluator.CreateKnowledgeBase("ColorRules.xml");
	m_TableEvaluator.InputValueGetterPtr = [](const wstring& attrName, void* context)
	{
		wstring wval;
		if (mAppData.find(attrName) != end(mAppData))
		{
			wval = mAppData[attrName];
		}
		return wval;
	};
	cout<<"done\n";

	cout<<"Enter red or blue for first paint color:";
	wcin>>mAppData[L"PaintColor1"];
	cout<<"Enter yellow or blue for second paint color:";
	wcin>>mAppData[L"PaintColor2"];

	cout<<"The result is: " + GetResultingColor() + "\n";

	pause();
}

