/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2014 Eric D. Schmidt, DigiRule Solutions LLC

    Relational Object Model is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Relational Object Model is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Relational Object Model.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "ROMDictionary.h"

#define MAX_RECURSION 1000

using namespace std;

namespace ROM
{
	enum INVALIDATEMODE_E
	{
		NORMALINVALIDATE,
		FLAGINVALIDATE
	};

	class LinearEngine : public ROMDictionary
	{
	public:
		LinearEngine(){InvalidateMode = NORMALINVALIDATE;}
		LinearEngine(ROMNode* context, const wstring& dictionaryTable) :ROMDictionary(context) { CreateLinearEngine(context, dictionaryTable); }
		void CreateLinearEngine(ROMNode* context, const wstring& dictionaryTable);
		virtual ~LinearEngine(){}
		void EvaluateForAttribute(const wstring& dictAttrName, vector<wstring>& newValues, bool bEvalDependents = true);
		void EvaluateForAttribute(const wstring& dictAttrName, const wstring& newValue, bool bEvalDependents = true);
		void EvaluateAll();
		vector<ROMDictionaryAttribute*> GetEvalList() {return m_vEvalList;}
		map<wstring, vector<wstring> > GetTriggers() {return m_mapTriggers;}
		bool DictionaryIsValid();

		//behavioral properties
		int InvalidateMode;

		//ASCII overloads
		LinearEngine(ROMNode* context, const string& dictionaryTable);
		void EvaluateForAttribute(const string& dictAttrName, const vector<string>& newValues, bool bEvalDependents = true);
		void EvaluateForAttribute(const string& dictAttrName, const string& newValue, bool bEvalDependents = true);


	private:
		void InitializeEngine(const wstring& dictionaryTable);
		void OrderDictionary();
		void EvalSingleSelect(const wstring& dictAttrName, const wstring& newValue);
		void EvalMultiSelect(const wstring& dictAttrName, const vector<wstring>& newValues);
		void EvalBoolean(const wstring& dictAttrName, const wstring& newValue);
		void EvalEdit(const wstring& dictAttrName, const wstring& newValue);
		void EvaluateDependencies(const wstring& dictAttrName);
		void FlagAttrInvalid(const wstring& dictAttrName);
		bool IsTouchedByUser(const wstring& dictAttrName);
		void SetTouchedByUser(const wstring& dictAttrName);
		void RemoveTouchedByUser(const wstring& dictAttrName);
		void LoadTrackingAttrs();
		vector<wstring> ParseOutPrefixes(int AttributeType, const vector<wstring>& values, vector<wstring>& valuesWithoutPrefixes); //remove the special character flags from the values
		vector<wstring> GetSelectedValues(ROMDictionaryAttribute* attr);
		void ResetValueChanged();		

		vector<ROMDictionaryAttribute*> m_vEvalList;
		map<wstring, vector<wstring> > m_mapTriggers;
		int m_CurrentRecursion;
		vector<ROMDictionaryAttribute*> m_vEvalListRecursChecker;
		bool m_EvalInternal;

		wstring INVISPREFIX;
		wstring DEFAULTPREFIX;
		wstring DISABLEPREFIX;
		wstring TBUATTR;
	};	
}
