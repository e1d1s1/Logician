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
#pragma once

namespace ROM
{
	class ROMDictionaryAttribute
	{
	public:
		ROMDictionaryAttribute() {CreateROMDictionaryAttribute();}
		~ROMDictionaryAttribute() {}
		void CreateROMDictionaryAttribute() {Visible = true; Valid = false; ValueChanged = false; ChangedByUser = false; Enabled = true;}
		wstring Name;
		wstring Description;
		wstring DefaultValue;
		wstring RuleTable;
		int AttributeType;

		bool ValueChanged;
		bool ChangedByUser;
		bool Valid;
		bool Visible;
		bool Enabled;

		vector<wstring> PossibleValues;
		vector<wstring> AvailableValues;
	};

	enum
	{
		SINGLESELECT = 0,
		MULTISELECT = 1,
		BOOLEANSELECT = 2,
		EDIT = 3,
		STATIC = 4
	};
}