/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

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
#include <functional>

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
		
		void LoadDictionary(const wstring& dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(const wstring& dictAttrName);
		map<wstring, ROMDictionaryAttribute>* GetAllDictionaryAttrs() {return &m_dict;}

		//debugging
		void SetTableDebugHandler(std::function<void(const wstring&)> debugger) { if (m_ROMContext) m_ROMContext->SetTableDebugHandler(debugger); }
		void EnableRemoteDebugger(bool enable) { if (m_ROMContext) m_ROMContext->EnableRemoteDebugger(enable); }

		//ASCII overloadas
		void LoadDictionary(const string& dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(const string& dictAttrName);

	private:
		ROMNode *m_ROMContext;

		map<wstring, ROMDictionaryAttribute> m_dict;
	};
}
