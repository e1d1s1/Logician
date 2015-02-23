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

#include <string>
#include <vector>

using namespace std;

namespace ROM
{
	class ROMDictionaryAttribute
	{
	public:
		ROMDictionaryAttribute() {CreateROMDictionaryAttribute();}
		~ROMDictionaryAttribute() {}
		void CreateROMDictionaryAttribute() {Visible = true; Valid = false; ValueChanged = false; ChangedByUser = false; Enabled = true; Index = 0;}
		string Name;
		string Description;
		string DefaultValue;
		string RuleTable;
		int AttributeType;
		size_t Index;
		bool ValueChanged;
		bool ChangedByUser;
		bool Valid;
		bool Visible;
		bool Enabled;

		vector<string> PossibleValues;
		vector<string> AvailableValues;
		string Value;
	};

	enum ATTRTYPE_E
	{
		SINGLESELECT = 0,
		MULTISELECT = 1,
		BOOLEANSELECT = 2,
		EDIT = 3,
		STATIC = 4
	};
}