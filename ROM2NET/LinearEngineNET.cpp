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
#include "LinearEngineNET.h"
#include "Marshal.h"

using namespace ROM2NET;

LinearEngineNET::LinearEngineNET(ROMTreeNET^ tree, ROMNode^ context, String^ dictionaryTable):ROMDictionaryNET(tree, context)
{
	m_vEvalList = gcnew List<ROMDictionaryAttributeNET^>();
	m_mapTriggers = gcnew Dictionary<String^, List<String^>^>();
	LoadDictionary(dictionaryTable);

	//open each attr in dict, load its dependency info to create m_vEvalList, m_mapTriggers
	//build an initial list that matches the dictionary order
	for each (KeyValuePair<String^, ROMDictionaryAttributeNET^> kvp in m_dict)
	{
		m_vEvalList->Add(kvp.Value);
		//triggers		
		wstring strTableName;
		MarshalString(kvp.Value->RuleTable, strTableName);
		vector<wstring> deps = m_tree->m_KnowledgeBase->GetInputDependencies(strTableName);
		for (vector<wstring>::iterator itDeps = deps.begin(); itDeps != deps.end(); itDeps++)
		{
			String^ key = gcnew String((*itDeps).c_str());
			if (m_mapTriggers->ContainsKey(key))
			{				
				if (!m_mapTriggers[key]->Contains(kvp.Value->Name))
					m_mapTriggers[key]->Add(kvp.Value->Name);
			}
			else if (m_dict->ContainsKey(key))
			{
				List<String^>^ newVect = gcnew List<String^>();
				newVect->Add(kvp.Value->Name);
				m_mapTriggers[key] = newVect;
			}
		}
	}

	//based on the triggers, re-order the dictionary
	m_CurrentRecursion = 0;
	OrderDictionary();
	m_vEvalListRecursChecker = nullptr;
	
	m_EvalInternal = false;
}

void LinearEngineNET::OrderDictionary()
{
	m_CurrentRecursion++;
	//make a copy to see if the order has changed during recursion
	List<ROMDictionaryAttributeNET^>^ evalOrderCopy = gcnew List<ROMDictionaryAttributeNET^>(m_vEvalList);

	//check for circular logic, outputs that are also inputs must go above the attrs that are dependent on them
	for each (KeyValuePair<String^, List<String^>^>^ kvp in m_mapTriggers)
    {
        //make sure all the input attrs have a lower index than current
        String^ inputAttr = kvp->Key;
        List<String^>^ outputAttrs = kvp->Value;
        if (outputAttrs != nullptr && outputAttrs->Count > 0)
        {
            int lowestIndex = 0;
			bool bFoundInputAttr = false;
			for (lowestIndex = 0; lowestIndex < m_vEvalList->Count; lowestIndex++)
			{
				if (m_vEvalList[lowestIndex]->Name == inputAttr)
				{
					bFoundInputAttr = true;
					break;
				}
			}

            int origIndex = lowestIndex;
            if (bFoundInputAttr)
            {
                for each (String^ attrOutput in outputAttrs)
                {
                    if (attrOutput->Length > 0)
                    {
                        int currentIndex = 0;
						for (currentIndex = 0; currentIndex < m_vEvalList->Count; currentIndex++)
						{					
							if (m_vEvalList[currentIndex]->Name == attrOutput)							
								break;							
						}
                        if (currentIndex < lowestIndex)                        
                            lowestIndex = currentIndex;
                        
                    }
                }

                //make the current input index lower than lowest output     
                if (origIndex != lowestIndex)
                {
					ROMDictionaryAttributeNET^ attr = m_vEvalList[origIndex];
                    m_vEvalList->RemoveAt(origIndex);
                    m_vEvalList->Insert(lowestIndex, attr);
                }
            }
        }
    }

	for (int i = 0; i < evalOrderCopy->Count; i++)
    {
        //if the lists differ, do another sort, otherwise we should be done
        if (evalOrderCopy[i] != m_vEvalList[i] && m_CurrentRecursion < MAX_RECURSION)
        {
			//does it match a previous result (are we flipping between a couple values, circular logic)
			if (m_CurrentRecursion % 2 == 0 && m_vEvalListRecursChecker != nullptr && m_vEvalListRecursChecker->Length > 0)
			{
				for (int j = 0; j < m_vEvalListRecursChecker->Length; j++)
				{
					if (m_vEvalList[j] != m_vEvalListRecursChecker[j])
					{
						OrderDictionary();
						break;
					}
				}
			}
			else
				OrderDictionary();

            break;
        }
    }

	if (m_CurrentRecursion % 2 == 0)
	{
		m_vEvalListRecursChecker = gcnew array<ROMDictionaryAttributeNET^>(m_vEvalList->Count);
		m_vEvalList->CopyTo(m_vEvalListRecursChecker);
	}
}

void LinearEngineNET::EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents)
{
	array<String^>^ newValues = gcnew array<String^>(1);
	newValues[0] = newValue;
	EvaluateForAttribute(dictAttrName, newValues, bEvalDependents);
}

void LinearEngineNET::EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues, bool bEvalDependents)
{
	ResetValueChanged();
	if (m_dict->ContainsKey(dictAttrName))
	{
		m_dict[dictAttrName]->ValueChanged = true;
		m_dict[dictAttrName]->ChangedByUser = !m_EvalInternal;
		switch (m_dict[dictAttrName]->AttributeType)
		{
		case SINGLESELECT:
			EvalSingleSelect(dictAttrName, newValues[0]);				
			break;

		case MULTISELECT:
			EvalMultiSelect(dictAttrName, newValues);
			break;

		case BOOLEANSELECT:
			EvalBoolean(dictAttrName, newValues[0]);
			break;

		case EDIT:
			EvalEdit(dictAttrName, newValues[0]);
			break;
		}

		if (bEvalDependents)
			EvaluateDependencies(dictAttrName);
	}	
}

void LinearEngineNET::ResetValueChanged()
{
	for each (ROMDictionaryAttributeNET^ attr in m_vEvalList)
	{
		attr->ValueChanged = false;
	}
}

void LinearEngineNET::EvaluateAll()
{
	m_EvalInternal = true;
	ResetValueChanged();
	for each (ROMDictionaryAttributeNET^ attr in m_vEvalList)
	{
		array<String^>^ selectedValues = GetSelectedValues(attr);
		EvaluateForAttribute(attr->Name, selectedValues, false);		
	}
	m_EvalInternal = false;
}

bool LinearEngineNET::DictionaryIsValid()
{
	bool retval = true;	
	for each (ROMDictionaryAttributeNET^ attr in m_vEvalList)
	{
		if (!attr->Valid)
		{
			retval = false;
			break;
		}
	}
	return retval;
}

void LinearEngineNET::EvalBoolean(String^ dictAttrName, String^ newValue)
{
	array<String^>^ res = m_tree->EvaluateTable(m_context, m_dict[dictAttrName]->RuleTable, dictAttrName, false);
	array<String^>^ availableValues;

	array<String^>^ prefixes = ParseOutPrefixes(res, availableValues);
	m_dict[dictAttrName]->AvailableValues = availableValues;

	if (prefixes->Length > 0 && prefixes[0]->Length > 0 && prefixes[0]->Contains(INVISPREFIX))
		m_dict[dictAttrName]->Visible = false;
	else
		m_dict[dictAttrName]->Visible = true;

	String^ currentValue = m_tree->GetAttribute(m_context, dictAttrName, false);
	m_dict[dictAttrName]->Valid = true;
	m_dict[dictAttrName]->Enabled = true;

	//set the dictionary default on load
	if (newValue->Length == 0)
	{
		if (m_dict[dictAttrName]->DefaultValue != nullptr && m_dict[dictAttrName]->DefaultValue->Length > 0)
			newValue = m_dict[dictAttrName]->DefaultValue;
	}

	if (availableValues == nullptr || availableValues->Length == 0)
	{
		m_tree->SetAttribute(m_context, dictAttrName, L"N");
		m_dict[dictAttrName]->ChangedByUser = false;
		return;
	}
	else if (availableValues->Length == 1) //you should only have one value
	{			
		if (availableValues[0]->Length == 0 || availableValues[0] == L"N")
		{
			m_tree->SetAttribute(m_context, dictAttrName, L"N");
			return;
		}
		else if (availableValues[0] == L"YN") //allow Yes or No with a default of Y
		{
			if (currentValue->Length == 0)
			{
				m_tree->SetAttribute(m_context, dictAttrName, L"Y");					
			}
		}
		else if (availableValues[0] == L"YY") //force Yes, no other choice
		{
			m_tree->SetAttribute(m_context, dictAttrName, L"Y");
			m_dict[dictAttrName]->ChangedByUser = false;
			m_dict[dictAttrName]->Enabled = false;
		}
		else if (availableValues[0] == L"NN") //force No, no other choice
		{
			m_tree->SetAttribute(m_context, dictAttrName, L"N");
			m_dict[dictAttrName]->ChangedByUser = false;
			m_dict[dictAttrName]->Enabled = false;
		}
		else if (newValue->Length == 1) //Y or N
		{
			m_tree->SetAttribute(m_context, dictAttrName, newValue);
		}		
	}
}

void LinearEngineNET::EvalEdit(String^ dictAttrName, String^ newValue)
{
	array<String^>^ res = m_tree->EvaluateTable(m_context, m_dict[dictAttrName]->RuleTable, dictAttrName, true);
	array<String^>^ availableValues;

	array<String^>^ prefixes = ParseOutPrefixes(res, availableValues);
	m_dict[dictAttrName]->AvailableValues = availableValues;
	m_dict[dictAttrName]->Enabled = true;
	m_dict[dictAttrName]->Valid = true;

	//set the dictionary default on load
	if (newValue->Length == 0)
	{
		if (m_dict[dictAttrName]->DefaultValue != nullptr && m_dict[dictAttrName]->DefaultValue->Length > 0)
			newValue = m_dict[dictAttrName]->DefaultValue;
	}

	if (availableValues->Length == 1)
	{
		if (prefixes[0]->Contains(DISABLEPREFIX))
		{
			m_dict[dictAttrName]->Enabled = false;
			m_dict[dictAttrName]->ChangedByUser = false;
			m_tree->SetAttribute(m_context, dictAttrName, prefixes[0]);	
			return;
		}
		else
			m_dict[dictAttrName]->Enabled = true;

		//check table result for range
		if (availableValues[0][0] == L'[')
		{
			double dNewValue = 0, dMin = 0, dMax = 0;
			String^ val = availableValues[0];
			val = val->Replace(L"[", L"");
			val = val->Replace(L"]", L"");
			
			array<String^>^ vals = val->Split(L',');
			dNewValue = System::Double::Parse(newValue);
			dMin = System::Double::Parse(vals[0]);
			dMax = System::Double::Parse(vals[1]);

			if (dNewValue <= dMax && dNewValue >= dMin)
			{
				m_tree->SetAttribute(m_context, dictAttrName, newValue);	
			}
			else if (dNewValue > dMax)
			{
				String^ wstrMax = vals[1];
				m_tree->SetAttribute(m_context, dictAttrName, wstrMax);
			}
			else if (dNewValue < dMin)
			{
				String^ wstrMin = vals[0];
				m_tree->SetAttribute(m_context, dictAttrName, wstrMin);
			}
		}
		else if (availableValues[0]->Length == 1 && availableValues[0][0] == L'Y')
		{
			m_tree->SetAttribute(m_context, dictAttrName, newValue);
		}		
		else if (availableValues[0]->Length == 1 && availableValues[0][0] == L'N')
		{
			m_tree->SetAttribute(m_context, dictAttrName, "");
			m_dict[dictAttrName]->ChangedByUser = false;
			m_dict[dictAttrName]->Enabled = false;
		}
		else if (availableValues[0]->Length == 2 && availableValues[0] == L"YY") //user must enter something
		{
			m_tree->SetAttribute(m_context, dictAttrName, newValue);
			m_dict[dictAttrName]->Valid = newValue->Length > 0;
		}
		else
		{
			m_tree->SetAttribute(m_context, dictAttrName, availableValues[0]);
			m_dict[dictAttrName]->ChangedByUser = false;
		}
	}	
	else if (availableValues->Length == 0)
	{		
		m_tree->SetAttribute(m_context, dictAttrName, "");
		m_dict[dictAttrName]->ChangedByUser = false;
		m_dict[dictAttrName]->Enabled = false;
	}
	else if (availableValues->Length == 1 && availableValues[0]->Length > 0)
	{
		m_tree->SetAttribute(m_context, dictAttrName, availableValues[0]);
		m_dict[dictAttrName]->ChangedByUser = false;
	}
	else if (availableValues->Length > 0)
	{
		List<String^>^ available = gcnew List<String^>(availableValues);
		if (available->Contains(newValue))
		{
			m_tree->SetAttribute(m_context, dictAttrName, newValue);
		}
		else 
		{
			m_tree->SetAttribute(m_context, dictAttrName, "");
			m_dict[dictAttrName]->ChangedByUser = false;
		}
	}

	if (prefixes->Length > 0 && prefixes[0]->Length > 0 && prefixes[0]->Contains(INVISPREFIX))
		m_dict[dictAttrName]->Visible = false;
	else
		m_dict[dictAttrName]->Visible = true;;
}

void LinearEngineNET::EvalMultiSelect(String^ dictAttrName, array<String^>^ newValues)
{
	//multi-select lists, checkbox lists
	array<String^>^ res = m_tree->EvaluateTable(m_context, m_dict[dictAttrName]->RuleTable, dictAttrName, true);
	array<String^>^ availableValues;
	m_dict[dictAttrName]->Enabled = true;
	m_dict[dictAttrName]->Valid = true;

	array<String^>^ prefixes = ParseOutPrefixes(res, availableValues);
	m_dict[dictAttrName]->AvailableValues = availableValues;

	String^ currentValue = m_tree->GetAttribute(m_context, dictAttrName, false);
	array<String^>^ currentValues = currentValue->Split(L'|');
	List<String^>^ selectedValues = gcnew List<String^>();

	//set the dictionary default on load
	if (newValues->Length == 0)
	{
		if (m_dict[dictAttrName]->DefaultValue != nullptr && m_dict[dictAttrName]->DefaultValue->Length > 0)
		{
			newValues = gcnew array<String^>(1);
			newValues[0] = m_dict[dictAttrName]->DefaultValue;
			m_dict[dictAttrName]->ChangedByUser = false;
		}
	}

	//if only one is available, force that selection now
	List<String^>^ srcAvail = gcnew List<String^>(availableValues);
	if (availableValues->Length == 1)
	{
		selectedValues->Add(availableValues[0]);
		m_dict[dictAttrName]->ChangedByUser = false;
	}
	//if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
	else if (currentValues != nullptr && (currentValues->Length == 1 && (currentValues[0]->Length == 0 || !srcAvail->Contains(currentValues[0]))) && prefixes->Length > 0)
	{
		for (size_t i = 0; i < prefixes->Length; i++)
		{
			if (prefixes[i]->Contains(DEFAULTPREFIX))
			{
				selectedValues->Add(availableValues[i]);
				m_dict[dictAttrName]->ChangedByUser = false;
			}
		}
	}
	else if (availableValues->Length == 0)
	{
		m_dict[dictAttrName]->Enabled = false;
	}
	
	
	if (selectedValues->Count == 0 && currentValues->Length > 0) //compare the new values to what is really available
	{		
		for (size_t i = 0; i < newValues->Length; i++)
		{	
			if (srcAvail->Contains(newValues[i]))
			{
				selectedValues->Add(newValues[i]);
			}
		}
	}

	String^ finalValue = gcnew String("");
	for each (String^ s in selectedValues)
	{
		if (finalValue->Length > 0)
			finalValue+= L"|";
		finalValue+=s;
	}		

	if (finalValue != currentValue)
		m_tree->SetAttribute(m_context, dictAttrName, finalValue);
}

//drop down list, radio button groups, etc
void LinearEngineNET::EvalSingleSelect(String^ dictAttrName, String^ newValue)
{
	array<String^>^ res = m_tree->EvaluateTable(m_context, m_dict[dictAttrName]->RuleTable, dictAttrName, true);
	array<String^>^ availableValues;
	m_dict[dictAttrName]->Enabled = true;
	m_dict[dictAttrName]->Valid = true;

	//the list of results is what is available for selection in the control
	array<String^>^ prefixes = ParseOutPrefixes(res, availableValues);
	m_dict[dictAttrName]->AvailableValues = availableValues;
	
	String^ currentValue = m_tree->GetAttribute(m_context, dictAttrName, false);

	//set the dictionary default on load
	if (newValue->Length == 0)
	{
		if (m_dict[dictAttrName]->DefaultValue != nullptr && m_dict[dictAttrName]->DefaultValue->Length > 0)
		{
			newValue = m_dict[dictAttrName]->DefaultValue;
		}
	}

	//if only one is available, force that selection now
	if (availableValues->Length == 1)
	{
		newValue = availableValues[0];
		m_dict[dictAttrName]->ChangedByUser = false;
	}

	List<String^>^ srcAvail = gcnew List<String^>(availableValues);
	//if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
	if ((currentValue->Length == 0 || !srcAvail->Contains(currentValue)) && prefixes->Length > 0)
	{
		for (size_t i = 0; i < prefixes->Length; i++)
		{
			if (prefixes[i]->Contains(DEFAULTPREFIX))
			{
				newValue = availableValues[i];
				m_dict[dictAttrName]->ChangedByUser = false;
				break;
			}
		}
	}
	else if (availableValues->Length == 0)
	{
		m_dict[dictAttrName]->Enabled = false;
	}

	if (prefixes->Length > 0 && prefixes[0]->Length> 0 && prefixes[0]->Contains(INVISPREFIX))
		m_dict[dictAttrName]->Visible = false;
	else
		m_dict[dictAttrName]->Visible = true;
	
	if (newValue->Length > 0 && srcAvail->Contains(newValue))
	{		
		if (currentValue != newValue)
		{
			m_tree->SetAttribute(m_context, dictAttrName, newValue);
		}
	}
	else
	{
		if (m_dict[dictAttrName]->Enabled == true)
			m_dict[dictAttrName]->Valid = false;
		m_tree->SetAttribute(m_context, dictAttrName, L"");
		m_dict[dictAttrName]->ChangedByUser = false;
	}
}

array<String^>^ LinearEngineNET::ParseOutPrefixes(array<String^>^ values, array<String^>^% vals)
{
	array<String^>^ origValues = values;
	List<String^>^ prefixes = gcnew List<String^>();
	List<String^>^ valuesWithoutPrefixes = gcnew List<String^>();

	for each (String^ s in origValues)
	{
		String^ val = s;
		String^ fullPrefix = gcnew String("");
		
		//check for leadoff ^ indicating an invisible control
		if (val->Contains(INVISPREFIX))
		{
			fullPrefix+=INVISPREFIX;
			val = val->Replace(INVISPREFIX, L"");
		}

		//check for leadoff @ indicating a default
		if (val->Contains(DEFAULTPREFIX))
		{
			fullPrefix+=DEFAULTPREFIX;
			val = val->Replace(DEFAULTPREFIX, L"");
		}

		//check for leadoff # indicating a locked edit box
		if (val->Contains(DISABLEPREFIX))
		{
			fullPrefix+=DISABLEPREFIX;
			val = val->Replace(DISABLEPREFIX, L"");
		}

		prefixes->Add(fullPrefix);
		valuesWithoutPrefixes->Add(val);
	}
	
	vals = valuesWithoutPrefixes->ToArray();
	return prefixes->ToArray();
}

void LinearEngineNET::EvaluateDependencies(String^ dictAttrName)
{
	m_EvalInternal = true;
	if (m_mapTriggers->ContainsKey(dictAttrName))
	{
		List<String^>^ attrsToEval = m_mapTriggers[dictAttrName];
		for each (String^ s in attrsToEval)
		{	
			if (m_dict->ContainsKey(s))
			{					
				array<String^>^ selectedValues = GetSelectedValues(m_dict[s]);
				bool bWasChangedByUser = m_dict[s]->ChangedByUser;				
				EvaluateForAttribute(s, selectedValues, true);				
				if (bWasChangedByUser)
				{
					bool bValuesRemainSame = true;
					List<String^>^ newSelectedValues = gcnew List<String^>(GetSelectedValues(m_dict[s]));
					if (newSelectedValues->Count != selectedValues->Length)
						bValuesRemainSame = false;
					else for (int i = 0; i < selectedValues->Length; i++)
					{
						if (!newSelectedValues->Contains(selectedValues[i]))
						{
							bValuesRemainSame = false;
							break;
						}
					}

					m_dict[s]->ChangedByUser = bValuesRemainSame;
				}
			}
		}
	}
	m_EvalInternal = false;
}
array<String^>^ LinearEngineNET::GetSelectedValues(ROMDictionaryAttributeNET^ attr)
{
	List<String^>^ retval = gcnew List<String^>();
	String^ currentValue = m_tree->GetAttribute(m_context, attr->Name); 

	switch (attr->AttributeType)
	{
	case SINGLESELECT:
		retval->Add(currentValue);
		break;

	case MULTISELECT:
		retval = gcnew List<String^>(currentValue->Split(L'|'));
		break;

	case BOOLEANSELECT:
		retval->Add(currentValue);
		break;

	case EDIT:
		retval->Add(currentValue);
		break;

	default:
		retval->Add(currentValue);
		break;
	}

	return retval->ToArray();
}