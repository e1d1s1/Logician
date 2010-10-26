/*
This file is part of ROM2NET.
Copyright (C) 2009 Eric D. Schmidt

    ROM2NET is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROM2NET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with ROM2NET.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Stdafx.h"
#include "utilities.h"
#include "ROMDictionaryNET.h"
#include "ROM2NET.h"
#include <algorithm>

using namespace ROM2NET;

ROMDictionaryNET::ROMDictionaryNET(ROMTreeNET^ tree, ROMNode^ context)
{
	m_tree = tree;
	m_context = context;
	m_dict = gcnew Dictionary<String^, ROMDictionaryAttributeNET^>();
}

/*A dictionary table should have the following outputs:
Name - ROM attribute name
Description - Visible attribute description in UI
DefaultValue - The initial value to set
AttributeType - default empty value(SINGLESELECT), SINGLESELECT, MULTISELECT, BOOLEAN, EDIT
RuleTable - table to evaluate to obtain the value (will override default if exists). Each rule table should have
	an output column name that matches the attribute name
*/
void ROMDictionaryNET::LoadDictionary(String^ dictionaryTable)
{
	m_dict->Clear();
	Dictionary<String^, array<String^>^>^ res = m_tree->EvaluateTable(m_context, dictionaryTable, true);	
	array<String^>^ allNames = res["Name"];

	for(size_t i = 0; i < allNames->Length; i++)
	{
		ROMDictionaryAttributeNET^ dictAttr = gcnew ROMDictionaryAttributeNET();
		dictAttr->Name = allNames[i];		
		if (res["DefaultValue"]->Length > 0 && res["DefaultValue"][i] != "~") dictAttr->DefaultValue = res["DefaultValue"][i];
		if (res["Description"]->Length > 0 && res["Description"][i] != "~") dictAttr->Description = res["Description"][i];
		if (res["RuleTable"]->Length > 0 && res["RuleTable"][i] != "~") dictAttr->RuleTable = res["RuleTable"][i];		

		String^ strAttrType = gcnew String("");
		if (res["AttributeType"]->Length > 0 && res["AttributeType"][i] != "~") strAttrType = res["AttributeType"][i];
		strAttrType = strAttrType->ToUpper();

		if (strAttrType->Length == 0 || strAttrType == "SINGLESELECT")
		{
			dictAttr->AttributeType = SINGLESELECT;
		}
		else if (strAttrType == "MULTISELECT")
		{
			dictAttr->AttributeType = MULTISELECT;
		}
		else if (strAttrType == "BOOLEAN")
		{
			dictAttr->AttributeType = BOOLEANSELECT;
		}
		else if (strAttrType == "EDIT")
		{
			dictAttr->AttributeType = EDIT;
		}

		//on load, just set default values and possibilities
		//only set a default if there is no rules table and no current value
		String^ value = m_tree->GetAttribute(m_context, dictAttr->Name);
		if (value->Length == 0 && dictAttr->RuleTable->Length == 0 && dictAttr->DefaultValue->Length > 0)
		{
			m_tree->SetAttribute(m_context, dictAttr->Name, dictAttr->DefaultValue);
		}

		if (dictAttr->RuleTable->Length > 0)
			dictAttr->PossibleValues = m_tree->GetPossibleValues(m_context, dictAttr->RuleTable, dictAttr->Name);

		m_dict[dictAttr->Name] = dictAttr;
	}
}

ROMDictionaryAttributeNET^ ROMDictionaryNET::GetDictionaryAttr(String^ dictAttrName)
{
	ROMDictionaryAttributeNET^ retval = nullptr;

	if (m_dict->ContainsKey(dictAttrName))
	{
		retval = m_dict[dictAttrName];
	}

	return retval;
}
