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
#include "stdafx.h"
#include "utilities.h"
#include "ROMDictionary.h"
#include <algorithm>

using namespace ROM;
using namespace ROMUTIL;

void ROMDictionary::CreateROMDictionary(ROMNode* context)
{
	m_ROMContext = context;
	m_tableName = "";
}

/*A dictionary table should have the following outputs:
Name - ROM attribute name
Description - Visible attribute description in UI
DefaultValue - The initial value to set
AttributeType - default empty value(SINGLESELECT), SINGLESELECT, MULTISELECT, BOOLEAN, EDIT, STATIC
RuleTable - table to evaluate to obtain the value (will override default if exists). Each rule table should have
	an output column name that matches the attribute name
*/
void ROMDictionary::_loadDictionary(const string& dictionaryTable, void* context)
{
	m_tableName = dictionaryTable;
	m_dict.clear();
	map<string, vector<string> > res = m_ROMContext->_evaluateTable(m_tableName, true, context);
	vector<string> allNames = res["Name"];

	for (size_t i = 0; i < allNames.size(); i++)
	{
		ROMDictionaryAttribute dictAttr;
		dictAttr.Name = allNames[i];
		dictAttr.Index = i;
		if (res["DefaultValue"].size() > 0 && res["DefaultValue"][i] != "~") dictAttr.DefaultValue = res["DefaultValue"][i];
		if (res["Description"].size() > 0 && res["Description"][i] != "~") dictAttr.Description = res["Description"][i];
		if (res["RuleTable"].size() > 0 && res["RuleTable"][i] != "~") dictAttr.RuleTable = res["RuleTable"][i];

		string strAttrType;
		if (res["AttributeType"].size() > 0  && res["AttributeType"][i] != "~") strAttrType = res["AttributeType"][i];
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
		string value = m_ROMContext->GetAttribute(dictAttr.Name);
		if (((value.length() == 0 && dictAttr.RuleTable.length() == 0) || dictAttr.AttributeType == STATIC) && dictAttr.DefaultValue.length() > 0 && dictAttr.DefaultValue != "~")
		{
			if (dictAttr.AttributeType == BOOLEANSELECT)
				m_ROMContext->SetAttribute(dictAttr.Name, dictAttr.DefaultValue.substr(0, 1));
			else
				m_ROMContext->SetAttribute(dictAttr.Name, dictAttr.DefaultValue);
		}

		if (dictAttr.RuleTable.length() > 0)
			dictAttr.PossibleValues = m_ROMContext->_getPossibleValues(dictAttr.RuleTable, dictAttr.Name);

		m_dict[dictAttr.Name] = dictAttr;
	}
}

ROMDictionaryAttribute* ROMDictionary::GetDictionaryAttr(const string& dictAttrName)
{
	ROMDictionaryAttribute* retval = nullptr;

	map<string, ROMDictionaryAttribute>::iterator itFind = m_dict.find(dictAttrName);
	if (itFind != m_dict.end())
		retval = &m_dict[dictAttrName];

	return retval;
}

