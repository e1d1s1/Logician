/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2015 Eric D. Schmidt, DigiRule Solutions LLC

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
#include <vector>
#include <string>
#include <map>
#include "ROMDictionaryAttribute.h"

using namespace std;

namespace ROM
{
	class IDictionaryInterface
	{
	public:
		virtual void LoadDictionary(const string& dictionaryTable) = 0;
		virtual ROMDictionaryAttribute* GetDictionaryAttr(const string& dictAttrName) = 0;
		virtual map<string, ROMDictionaryAttribute>* GetAllDictionaryAttrs() = 0;

#ifdef CLR //these internal methods are called by .NET to assist with passing of managed objects
		virtual void _loadDictionary(const string& dictionaryTable, void* context) = 0;
#endif
	};
}