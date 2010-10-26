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
#include "ROMTree.h"
#include "ROMDictionaryAttribute.h";

#pragma once

using namespace std;

namespace ROM
{
	class ROMDictionary
	{
	friend class LinearEngine;
	public:
		ROMDictionary(ROMTree* tree, Node context);
		virtual ~ROMDictionary(void){}

		void LoadDictionary(wstring dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(wstring dictAttrName);
		map<wstring, ROMDictionaryAttribute>* GetAllDictionaryAttrs() {return &m_dict;}
		
		//ASCII overloadas
		void LoadDictionary(string dictionaryTable);
		ROMDictionaryAttribute* GetDictionaryAttr(string dictAttrName);

	private:
		ROMTree *m_tree;
		Node m_context;

		map<wstring, ROMDictionaryAttribute> m_dict;
	};
}