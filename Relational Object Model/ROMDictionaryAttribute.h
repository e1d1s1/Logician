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
#pragma once

namespace ROM
{
	class ROMDictionaryAttribute
	{
	public:
		ROMDictionaryAttribute() {Visible = true; Valid = false; ValueChanged = false; ChangedByUser = false; Enabled = true;}
		~ROMDictionaryAttribute() {}
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
		EDIT = 3
	};
}

//const short SINGLESELECT = 0;
//const short MULTISELECT = 1;
//const short BOOLEANSELECT = 2;
//const short EDIT = 3;