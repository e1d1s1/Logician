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
#include "stdafx.h"
#include "utilities.h"
#include "ROMDictionary.h"
#include <algorithm>

using namespace ROM;
using namespace ROMUTIL;

ROMDictionary::ROMDictionary(ROMNode* context)
{
	m_ROMContext = context;
}

/*A dictionary table should have the following outputs:
Name - ROM attribute name
Description - Visible attribute description in UI
DefaultValue - The initial value to set
AttributeType - default empty value(SINGLESELECT), SINGLESELECT, MULTISELECT, BOOLEAN, EDIT
RuleTable - table to evaluate to obtain the value (will override default if exists). Each rule table should have
	an output column name that matches the attribute name
*/
void ROMDictionary::LoadDictionary(wstring dictionaryTable)
{
	m_dict.clear();
	map<wstring, vector<wstring> > res = m_ROMContext->EvaluateTable(dictionaryTable);	
	vector<wstring> allNames = res[L"Name"];

	for (size_t i = 0; i < allNames.size(); i++)
	{		
		ROMDictionaryAttribute dictAttr;
		dictAttr.Name = allNames[i];		
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
		if (((value.length() == 0 && dictAttr.RuleTable.length() == 0) || dictAttr.AttributeType == STATIC) && dictAttr.DefaultValue.length() > 0)
		{
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

//map<wstring, vector<wstring> > ROMDictionary::_evaluateTable(wstring evalTable, bool bGetAll)
//{
//	map<wstring, vector<wstring> > retval;
//	if (m_ROMContext != NULL)
//	{
//		retval = m_ROMContext->EvaluateTable(evalTable, bGetAll);
//	}
//	else if (m_tree != NULL && m_treeContext != NULL)
//	{
//		retval = m_tree->EvaluateTable(m_treeContext, evalTable, bGetAll);
//	}
//	return retval;
//}
//
//wstring ROMDictionary::m_ROMContext->GetAttribute(wstring name, bool bImmediate)
//{
//	wstring retval = L"";
//	if (m_ROMContext != NULL)
//	{
//		retval = m_ROMContext->GetAttribute(name, bImmediate);
//	}
//	else if (m_tree != NULL && m_treeContext != NULL)
//	{
//		retval = m_tree->GetAttribute(m_treeContext, name, bImmediate);
//	}
//	return retval;
//}
//
//void ROMDictionary::m_ROMContext->SetAttribute(wstring name, wstring value)
//{
//	if (m_ROMContext != NULL)
//	{
//		m_ROMContext->SetAttribute(name, value);
//	}
//	else if (m_tree != NULL && m_treeContext != NULL)
//	{
//		m_tree->SetAttribute(m_treeContext, name, value);
//	}
//}
//
//vector<wstring> ROMDictionary::m_ROMContext->GetPossibleValues(wstring evalTable, wstring outputName)
//{
//	vector<wstring> retval;
//	if (m_ROMContext != NULL)
//	{
//		retval = m_ROMContext->GetPossibleValues(evalTable, outputName);
//	}
//	return retval;
//}
//
//vector<wstring> ROMDictionary::m_ROMContext->EvaluateTableForAttr(wstring evalTable, wstring outputName, bool bGetAll)
//{
//	vector<wstring> retval;	
//	if (m_ROMContext != NULL)
//	{
//		retval = m_ROMContext->EvaluateTableForAttr(evalTable, outputName, bGetAll);
//	}
//	return retval;
//}
//
//EDS::CKnowledgeBase* ROMDictionary::_getKnowledgeBase()
//{
//	EDS::CKnowledgeBase* retval = NULL;	
//	if (m_ROMContext != NULL)
//	{
//		retval = &m_ROMContext->m_KnowledgeBase;
//	}
//	return retval;
//}

