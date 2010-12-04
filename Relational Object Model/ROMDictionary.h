/*
This file is part of the Relational Object Model 2 Library.
Copyright (C) 2009 Eric D. Schmidt

    Relational Object Model 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Relational Object Model 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Relational Object Model 2.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ROMNode.h"
#include "ROMDictionaryAttribute.h"

#pragma once

using namespace std;

namespace ROM
{
	class ROMDictionary
	{
	friend class LinearEngine;
	public:
		ROMDictionary(ROMNode* context) {CreateROMDictionary(context);}
		ROMDictionary() {}
		void CreateROMDictionary(ROMNode* context);
		virtual ~ROMDictionary(void){}

		void LoadDictionary(wstring dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(wstring dictAttrName);
		map<wstring, ROMDictionaryAttribute>* GetAllDictionaryAttrs() {return &m_dict;}

		//ASCII overloadas
		void LoadDictionary(string dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(string dictAttrName);

	private:
		ROMNode *m_ROMContext;
		//map<wstring, vector<wstring> > _evaluateTable(wstring evalTable, bool bGetAll = true);
		//vector<wstring> m_ROMContext->EvaluateTableForAttr(wstring evalTable, wstring outputName, bool bGetAll = true);
		//wstring m_ROMContext->GetAttribute(wstring name, bool bImmediate = false);
		//void m_ROMContext->SetAttribute(wstring name, wstring value);
		//vector<wstring> m_ROMContext->GetPossibleValues(wstring evalTable, wstring outputName);	
		//EDS::CKnowledgeBase* _getKnowledgeBase();

		map<wstring, ROMDictionaryAttribute> m_dict;
	};
}
