#pragma once

#include "ROMDictionary.h"

#define MAX_RECURSION 1000

using namespace std;

namespace ROM
{
	class LinearEngine : public ROMDictionary
	{
	public:
		LinearEngine(ROMTree* tree, Node context, wstring dictionaryTable);
		virtual ~LinearEngine(){}
		void EvaluateForAttribute(wstring dictAttrName, vector<wstring> newValues, bool bEvalDependents = true);
		void EvaluateForAttribute(wstring dictAttrName, wstring newValue, bool bEvalDependents = true);
		void EvaluateAll();
		vector<ROMDictionaryAttribute*>* GetEvalList() {return &m_vEvalList;}

		//ASCII overloads
		LinearEngine(ROMTree* tree, Node context, string dictionaryTable);
		void EvaluateForAttribute(string dictAttrName, vector<string> newValues, bool bEvalDependents = true);
		void EvaluateForAttribute(string dictAttrName, string newValue, bool bEvalDependents = true);
		

	private:
		void InitializeEngine(ROMTree* tree, Node context, wstring dictionaryTable);
		void OrderDictionary();
		void EvalSingleSelect(wstring dictAttrName, wstring newValue);
		void EvalMultiSelect(wstring dictAttrName, vector<wstring> newValues);
		void EvalBoolean(wstring dictAttrName, wstring newValue);
		void EvalEdit(wstring dictAttrName, wstring newValue);
		void EvaluateDependencies(wstring dictAttrName);
		vector<wstring> ParseOutPrefixes(vector<wstring> values, vector<wstring> &valuesWithoutPrefixes); //remove the special character flags from the values
		vector<wstring> GetSelectedValues(ROMDictionaryAttribute* attr);

		vector<ROMDictionaryAttribute*> m_vEvalList;
		map<wstring, vector<wstring>> m_mapTriggers;
		int m_CurrentRecursion;

		wstring INVISPREFIX;
		wstring DEFAULTPREFIX;
		wstring DISABLEPREFIX;
	};
}