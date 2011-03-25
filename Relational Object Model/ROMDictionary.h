/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2011 Eric D. Schmidt

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
		wstring	GetTableDebugMessages() {if (m_ROMContext != NULL) return m_ROMContext->GetTableDebugMessages();}
		void GenerateTableDebugMessages(bool bGenerate) {if (m_ROMContext != NULL) m_ROMContext->GenerateTableDebugMessages(bGenerate);}

		void LoadDictionary(wstring dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(wstring dictAttrName);
		map<wstring, ROMDictionaryAttribute>* GetAllDictionaryAttrs() {return &m_dict;}

		//ASCII overloadas
		void LoadDictionary(string dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(string dictAttrName);

	private:
		ROMNode *m_ROMContext;

		map<wstring, ROMDictionaryAttribute> m_dict;
	};
}
