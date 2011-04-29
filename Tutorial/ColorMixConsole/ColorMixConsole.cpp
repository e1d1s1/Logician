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
map<string, string> mAppData;

string GetSingleSolution(string tableToEvaluate, string nameOfOutput) //could reuse this function for all similar aplication events
{
  vector<string> inputsNeeded = m_TableEvaluator.GetInputDependencies(tableToEvaluate);
  //from our application data, obtain the values
  for (int i = 0; i < inputsNeeded.size(); i++)
    m_TableEvaluator.SetInputValue(inputsNeeded[i], mAppData[inputsNeeded[i]]);
  
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
	cout<<"done\n";

	cout<<"Enter red or blue for first paint color:";
	cin>>mAppData["PaintColor1"];
	cout<<"Enter yellow or blue for second paint color:";
	cin>>mAppData["PaintColor2"];

	cout<<"The result is: " + GetResultingColor() + "\n";

	pause();
}

