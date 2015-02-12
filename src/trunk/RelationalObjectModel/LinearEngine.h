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
		LinearEngine(ROMNode* context, const wstring& dictionaryTable) : ROMDictionary(context) { CreateLinearEngine(dictionaryTable); }
		void CreateLinearEngine(const wstring& dictionaryTable);
		void InitializeEngine() { _initializeEngine(m_ROMContext); }
		void ResetEngine() { _resetEngine(m_ROMContext); }
		virtual ~LinearEngine(){}
		void EvaluateForAttribute(const wstring& dictAttrName, vector<wstring>& newValues, bool bEvalDependents = true) { _evaluateForAttribute(dictAttrName, newValues, bEvalDependents, m_ROMContext); }
		void EvaluateForAttribute(const wstring& dictAttrName, const wstring& newValue, bool bEvalDependents = true) { _evaluateForAttribute(dictAttrName, newValue, bEvalDependents, m_ROMContext); }
		void EvaluateAll() { _evaluateAll(m_ROMContext); }
		vector<ROMDictionaryAttribute*> GetEvalList() {return m_vEvalList;}
		map<wstring, vector<wstring> > GetTriggers() {return m_mapTriggers;}
		bool DictionaryIsValid();

		//behavioral properties
		int InvalidateMode;

#ifndef CLR //these internal methods are called by .NET to assist with passing of managed objects
	private:
#else
	public:
#endif
		void _evaluateForAttribute(const wstring& dictAttrName, vector<wstring>& newValues, bool bEvalDependents, void* context);
		void _evaluateForAttribute(const wstring& dictAttrName, const wstring& newValue, bool bEvalDependents, void* context);
		void _evaluateAll(void* context);
		void _initializeEngine(void* context);

	private:		
		void _orderDictionary();
		void _evalSingleSelect(const wstring& dictAttrName, const wstring& newValue, void* context);
		void _evalMultiSelect(const wstring& dictAttrName, const vector<wstring>& newValues, void* context);
		void _evalBoolean(const wstring& dictAttrName, const wstring& newValue, void* context);
		void _evalEdit(const wstring& dictAttrName, const wstring& newValue, void* context);
		void _evaluateDependencies(const wstring& dictAttrName, void* context);
		void _flagAttrInvalid(const wstring& dictAttrName);
		bool _isTouchedByUser(const wstring& dictAttrName);
		void _setTouchedByUser(const wstring& dictAttrName);
		void _removeTouchedByUser(const wstring& dictAttrName);
		void _loadTrackingAttrs();
		vector<wstring> _parseOutPrefixes(int AttributeType, const vector<wstring>& values, vector<wstring>& valuesWithoutPrefixes); //remove the special character flags from the values
		vector<wstring> _getSelectedValues(ROMDictionaryAttribute* attr);
		void _resetValueChanged();		
		void _resetEngine(void* context);

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
