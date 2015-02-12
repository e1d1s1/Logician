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
#include <functional>

#include "ROMNode.h"
#include "ROMDictionaryAttribute.h"

#pragma once

using namespace std;

namespace ROM
{
	class ROMDictionary : public IDictionaryInterface
	{
	friend class LinearEngine;
	public:
		ROMDictionary(ROMNode* context) {CreateROMDictionary(context);}
		ROMDictionary() {}
		void CreateROMDictionary(ROMNode* context);
		virtual ~ROMDictionary(void){}		
		
		virtual void LoadDictionary(const wstring& dictionaryTable) override { _loadDictionary(dictionaryTable, m_ROMContext); }
		virtual ROMDictionaryAttribute* GetDictionaryAttr(const wstring& dictAttrName) override;
		virtual map<wstring, ROMDictionaryAttribute>* GetAllDictionaryAttrs() override { return &m_dict; }

		//ASCII overloadas
		void LoadDictionary(const string& dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(const string& dictAttrName);

#ifndef CLR //these internal methods are called by .NET to assist with passing of managed objects
	private:
		virtual void _loadDictionary(const wstring& dictionaryTable, void* context);
#else
	public:
		virtual void _loadDictionary(const wstring& dictionaryTable, void* context) override;
#endif
		

	private:
		ROMNode *m_ROMContext;

		map<wstring, ROMDictionaryAttribute> m_dict;
		wstring m_tableName;
	};
}
