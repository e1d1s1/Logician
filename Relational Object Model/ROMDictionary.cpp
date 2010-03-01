#include "stdafx.h"
#include "utilities.h"
#include "ROMDictionary.h"
#include "ROMTree.h"
#include <algorithm>

using namespace ROM;
using namespace ROMUTIL;

ROMDictionary::ROMDictionary(ROMTree *tree, Node context)
{
	m_tree = tree;
	m_context = context;
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
	map<wstring, vector<wstring> > res = m_tree->EvaluateTable(m_context, dictionaryTable);	
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

		//on load, just set default values and possibilities
		//only set a default if there is no rules table and no current value
		wstring value = m_tree->GetAttribute(m_context, dictAttr.Name);
		if (value.length() == 0 && dictAttr.RuleTable.length() == 0 && dictAttr.DefaultValue.length() > 0)
		{
			m_tree->SetAttribute(m_context, dictAttr.Name, dictAttr.DefaultValue);
		}

		if (dictAttr.RuleTable.length() > 0)
			dictAttr.PossibleValues = m_tree->GetPossibleValues(m_context, dictAttr.RuleTable, dictAttr.Name);

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



