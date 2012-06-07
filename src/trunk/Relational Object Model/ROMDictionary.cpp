/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

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
#include "stdafx.h"
#include "utilities.h"
#include "ROMDictionary.h"
#include <algorithm>

using namespace ROM;
using namespace ROMUTIL;

void ROMDictionary::CreateROMDictionary(ROMNode* context)
{
	m_ROMContext = context;
}

/*A dictionary table should have the following outputs:
Name - ROM attribute name
Description - Visible attribute description in UI
DefaultValue - The initial value to set
AttributeType - default empty value(SINGLESELECT), SINGLESELECT, MULTISELECT, BOOLEAN, EDIT, STATIC
RuleTable - table to evaluate to obtain the value (will override default if exists). Each rule table should have
	an output column name that matches the attribute name
*/
void ROMDictionary::LoadDictionary(wstring dictionaryTable)
{
	m_dict.clear();
	map<wstring, vector<wstring> > res = m_ROMContext->EvaluateTable(dictionaryTable, true);	
	vector<wstring> allNames = res[L"Name"];

	for (size_t i = 0; i < allNames.size(); i++)
	{		
		ROMDictionaryAttribute dictAttr;
		dictAttr.Name = allNames[i];		
		dictAttr.Index = i;
		if (res[L"DefaultValue"].size() > 0 && res[L"DefaultValue"][i] != L"~") dictAttr.DefaultValue = res[L"DefaultValue"][i];
		if (res[L"Description"].size() > 0 && res[L"Description"][i] != L"~") dictAttr.Description = res[L"Description"][i];
		if (res[L"RuleTable"].size() > 0 && res[L"RuleTable"][i] != L"~") dictAttr.RuleTable = res[L"RuleTable"][i];		

		string strAttrType;
		if (res[L"AttributeType"].size() > 0  && res[L"AttributeType"][i] != L"~") strAttrType = ToASCIIString(res[L"AttributeType"][i]);
		transform(strAttrType.begin(), strAttrType.end(), strAttrType.begin(), ::toupper);

		if (strAttrType.length() == 0 || strAttrType == "SINGLESELECT")
		{
			dictAttr.AttributeType = SINGLESELECT;
		}
		else if (strAttrType == "MULTISELECT")
		{
			dictAttr.AttributeType = MULTISELECT;
		}
		else if (strAttrType == "BOOLEAN")
		{
			dictAttr.AttributeType = BOOLEANSELECT;
		}
		else if (strAttrType == "EDIT")
		{
			dictAttr.AttributeType = EDIT;
		}
		else if (strAttrType == "STATIC")
		{
			dictAttr.AttributeType = STATIC;
		}

		//on load, just set default values and possibilities
		//only set a default if there is no rules table and no current value		
		wstring value = m_ROMContext->GetAttribute(dictAttr.Name);
		if (((value.length() == 0 && dictAttr.RuleTable.length() == 0) || dictAttr.AttributeType == STATIC) && dictAttr.DefaultValue.length() > 0 && dictAttr.DefaultValue != L"~")
		{
			if (dictAttr.AttributeType == BOOLEANSELECT)			
				m_ROMContext->SetAttribute(dictAttr.Name, dictAttr.DefaultValue.substr(0, 1));
			else
				m_ROMContext->SetAttribute(dictAttr.Name, dictAttr.DefaultValue);
		}

		if (dictAttr.RuleTable.length() > 0)
			dictAttr.PossibleValues = m_ROMContext->GetPossibleValues(dictAttr.RuleTable, dictAttr.Name);

		m_dict[dictAttr.Name] = dictAttr;
	}
}

ROMDictionaryAttribute* ROMDictionary::GetDictionaryAttr(wstring dictAttrName)
{
	ROMDictionaryAttribute* retval = NULL;

	map<wstring, ROMDictionaryAttribute>::iterator itFind = m_dict.find(dictAttrName);
	if (itFind != m_dict.end())
		retval = &m_dict[dictAttrName];

	return retval;
}

//ASCII Overloads
void ROMDictionary::LoadDictionary(string dictionaryTable)
{
	LoadDictionary(ROMUTIL::MBCStrToWStr(dictionaryTable));
}

ROMDictionaryAttribute* ROMDictionary::GetDictionaryAttr(string dictAttrName)
{
	return GetDictionaryAttr(ROMUTIL::MBCStrToWStr(dictAttrName));
}

