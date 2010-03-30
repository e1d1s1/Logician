#include "stdafx.h"
#include "LinearEngine.h"
#include "utilities.h"

namespace ROM
{
	LinearEngine::LinearEngine(ROMTree* tree, Node context, wstring dictionaryTable):ROMDictionary(tree, context)
	{
		InitializeEngine(tree, context, dictionaryTable);
	}

	void LinearEngine::InitializeEngine(ROMTree* tree, Node context, wstring dictionaryTable)
	{
		INVISPREFIX = L"^";
		DEFAULTPREFIX = L"@";
		DISABLEPREFIX = L"#";

		m_vEvalList.clear();
		m_mapTriggers.clear();
		LoadDictionary(dictionaryTable);

		//open each attr in dict, load its dependency info to create m_vEvalList, m_mapTriggers
		//build an initial list that matches the dictionary order
		for (map<wstring, ROMDictionaryAttribute>::iterator it = m_dict.begin(); it != m_dict.end(); it++)
		{
			m_vEvalList.push_back(&it->second);
			//triggers
			vector<wstring> deps = m_tree->m_KnowledgeBase.GetInputDependencies(it->second.RuleTable);
			for (vector<wstring>::iterator itDeps = deps.begin(); itDeps != deps.end(); itDeps++)
			{
				if (m_mapTriggers.find(*itDeps) != m_mapTriggers.end())
				{
					if (find(m_mapTriggers[*itDeps].begin(), m_mapTriggers[*itDeps].end(), it->second.Name) == m_mapTriggers[*itDeps].end())
						m_mapTriggers[*itDeps].push_back(it->second.Name);
				}
				else if (m_dict.find(*itDeps) != m_dict.end())
				{
					vector<wstring> newVect;
					newVect.push_back(it->second.Name);
					m_mapTriggers[*itDeps] = newVect;
				}
			}
		}

		//based on the triggers, re-order the dictionary
		m_CurrentRecursion = 0;
		OrderDictionary();
	}

	void LinearEngine::OrderDictionary()
	{
		m_CurrentRecursion++;
		//make a copy to see if the order has changed during recursion
		vector<ROMDictionaryAttribute*> evalOrderCopy = m_vEvalList;

		//check for circular logic, outputs that are also inputs must go above the attrs that are dependent on them
		for (map<wstring, vector<wstring> >::iterator it = m_mapTriggers.begin(); it != m_mapTriggers.end(); it++)
		{
			for (vector<wstring>::iterator itDeps = it->second.begin(); itDeps != it->second.end(); itDeps++)
			{
				size_t lowestIndex = 0;
				bool bFoundInputAttr = false;
				for (lowestIndex = 0; lowestIndex < m_vEvalList.size(); lowestIndex++)
				{
					if (m_vEvalList[lowestIndex]->Name == it->first)
					{
						bFoundInputAttr = true;
						break;
					}
				}
				size_t origIndex = lowestIndex;
				if (bFoundInputAttr)
				{
					for (vector<wstring>::iterator itDeps2 = it->second.begin(); itDeps2 != it->second.end(); itDeps2++)
					{
						if (itDeps2->length() > 0)
						{
							size_t currentIndex = 0;
							for (currentIndex = 0; currentIndex < m_vEvalList.size(); currentIndex++)
							{
								if (m_vEvalList[currentIndex]->Name == *itDeps2)
									break;
							}
							if (currentIndex < lowestIndex)
								lowestIndex = currentIndex;
						}
					}

					//make the current input index lower than lowest output
					if (origIndex != lowestIndex)
					{
						ROMDictionaryAttribute* attr = m_vEvalList[origIndex];
						m_vEvalList.erase(m_vEvalList.begin() + origIndex);
						m_vEvalList.insert(m_vEvalList.begin() + lowestIndex, attr);
					}
				}
			}
		}

		for (int i = 0; i < evalOrderCopy.size(); i++)
		{
			//if the lists differ, do another sort, otherwise we should be done
			if (evalOrderCopy[i] != m_vEvalList[i] && m_CurrentRecursion < MAX_RECURSION)
			{
				OrderDictionary();
				break;
			}
		}
	}

	void LinearEngine::EvaluateForAttribute(wstring dictAttrName, wstring newValue, bool bEvalDependents)
	{
		vector<wstring> newValues;
		newValues.push_back(newValue);
		EvaluateForAttribute(dictAttrName, newValues, bEvalDependents);
	}

	void LinearEngine::EvaluateForAttribute(wstring dictAttrName, vector<wstring> newValues, bool bEvalDependents)
	{
		map<wstring, ROMDictionaryAttribute>::iterator itFind = m_dict.find(dictAttrName);
		if (itFind != m_dict.end())
		{
			m_dict[dictAttrName].ChangedByUser = true;
			switch (m_dict[dictAttrName].AttributeType)
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

	void LinearEngine::EvaluateAll()
	{
		for (vector<ROMDictionaryAttribute*>::iterator it = m_vEvalList.begin(); it != m_vEvalList.end(); it++)
		{
			vector<wstring> selectedValues = GetSelectedValues(*it);
			EvaluateForAttribute((*it)->Name, selectedValues, false);
		}
	}

	void LinearEngine::EvalBoolean(std::wstring dictAttrName, std::wstring newValue)
	{
		vector<wstring> res = m_tree->EvaluateTableForAttr(m_context, m_dict[dictAttrName].RuleTable, dictAttrName, false);
		vector<wstring> availableValues;

		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;

		if (prefixes.size() > 0 && prefixes[0].length() > 0 && prefixes[0][0] == L'^')
			m_dict[dictAttrName].Visible = false;
		else
			m_dict[dictAttrName].Visible = true;

		wstring currentValue = m_tree->GetAttribute(m_context, dictAttrName, false);
		m_dict[dictAttrName].Valid = true;
		m_dict[dictAttrName].Enabled = true;

		//set the dictionary default on load
		if (newValue.length() == 0)
		{
			if (m_dict[dictAttrName].DefaultValue.length() > 0)
				newValue = m_dict[dictAttrName].DefaultValue;
		}

		if (availableValues.size() == 0)
		{
			m_tree->SetAttribute(m_context, dictAttrName, L"N");
			m_dict[dictAttrName].ChangedByUser = false;
			return;
		}
		else if (availableValues.size() == 1) //you should only have one value
		{
			if (availableValues[0].length() == 0 || availableValues[0] == L"N")
			{
				m_tree->SetAttribute(m_context, dictAttrName, L"N");
				m_dict[dictAttrName].ChangedByUser = (newValue.length() > 0 && newValue != L"N");
				return;
			}
			else if (availableValues[0] == L"YN") //allow Yes or No with a default of Y
			{
				if (currentValue.length() == 0)
				{
					m_tree->SetAttribute(m_context, dictAttrName, L"Y");
				}
			}
			else if (availableValues[0] == L"YY") //force Yes, no other choice
			{
				m_tree->SetAttribute(m_context, dictAttrName, L"Y");
				m_dict[dictAttrName].ChangedByUser = false;
				m_dict[dictAttrName].Enabled = false;
			}
			else if (availableValues[0] == L"NN") //force No, no other choice
			{
				m_tree->SetAttribute(m_context, dictAttrName, L"N");
				m_dict[dictAttrName].ChangedByUser = false;
				m_dict[dictAttrName].Enabled = false;
			}
			else if (newValue.length() == 1) //Y or N
			{
				m_tree->SetAttribute(m_context, dictAttrName, newValue);
				m_dict[dictAttrName].ChangedByUser = (availableValues[0] == newValue);
			}
		}
	}

	void LinearEngine::EvalEdit(std::wstring dictAttrName, std::wstring newValue)
	{
		vector<wstring> res = m_tree->EvaluateTable(m_context, m_dict[dictAttrName].RuleTable, dictAttrName);
		vector<wstring> availableValues;

		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;
		m_dict[dictAttrName].Enabled = true;

		//set the dictionary default on load
		if (newValue.length() == 0)
		{
			if (m_dict[dictAttrName].DefaultValue.length() > 0)
				newValue = m_dict[dictAttrName].DefaultValue;
		}

		if (availableValues.size() == 1)
		{
			if (ROMUTIL::StringContains(prefixes[0], INVISPREFIX))
				m_dict[dictAttrName].Visible = false;
			else
				m_dict[dictAttrName].Visible = true;

			if (ROMUTIL::StringContains(prefixes[0], DISABLEPREFIX))
			{
				m_dict[dictAttrName].Enabled = false;
				m_dict[dictAttrName].ChangedByUser = false;
				m_tree->SetAttribute(m_context, dictAttrName, prefixes[0]);
			}
			else
				m_dict[dictAttrName].Enabled = true;

			//check table result for range
			if (availableValues[0][0] == L'[')
			{
				m_dict[dictAttrName].ChangedByUser = true;
				double dNewValue = 0, dMin = 0, dMax = 0;
				wstring val = availableValues[0];
				val = ROMUTIL::FindAndReplace(val, L"[", L"");
				val = ROMUTIL::FindAndReplace(val, L"]", L"");
				vector<wstring> vals = ROMUTIL::Split(val, L",");
#ifdef _MSC_VER
				dNewValue = _wtof(newValue.c_str());
				dMin = _wtof(vals[0].c_str());
				dMax = _wtof(vals[1].c_str());
#else
				string strNew(newValue.begin(), newValue.end());
				string strMin(vals[0].begin(), vals[0].end());
				string strMax(vals[1].begin(), vals[1].end());
				dNewValue = atof(strNew.c_str());
				dMin = atof(strMin.c_str());
				dMax = atof(strMax.c_str());
#endif
				if (dNewValue <= dMax && dNewValue >= dMin)
				{
					m_tree->SetAttribute(m_context, dictAttrName, newValue);
				}
				else if (dNewValue > dMax)
				{
					wstring wstrMax(vals[1].begin(), vals[1].end());
					m_tree->SetAttribute(m_context, dictAttrName, wstrMax);
				}
				else if (dNewValue < dMin)
				{
					wstring wstrMin(vals[0].begin(), vals[0].end());
					m_tree->SetAttribute(m_context, dictAttrName, wstrMin);
				}
			}
		}
		else if (availableValues.size() == 0 || (availableValues.size() == 1 && availableValues[0].length() == 1 && availableValues[0][0] == L'Y'))
		{
			m_tree->SetAttribute(m_context, dictAttrName, newValue);
			m_dict[dictAttrName].ChangedByUser = true;
			if (prefixes.size() > 0 && prefixes[0].length() > 0 && ROMUTIL::StringContains(prefixes[0], INVISPREFIX))
				m_dict[dictAttrName].Visible = false;
			else
				m_dict[dictAttrName].Visible = true;;
		}
		else if (availableValues.size() == 1 && availableValues[0].length() > 0)
		{
			m_tree->SetAttribute(m_context, dictAttrName, availableValues[0]);
			m_dict[dictAttrName].ChangedByUser = false;
			if (prefixes[0].length() > 0 && ROMUTIL::StringContains(prefixes[0],INVISPREFIX))
				m_dict[dictAttrName].Visible = false;
			else
				m_dict[dictAttrName].Visible = true;;
		}
	}

	void LinearEngine::EvalMultiSelect(std::wstring dictAttrName, vector<wstring> newValues)
	{
		//multi-select lists, checkbox lists
		vector<wstring> res = m_tree->EvaluateTableForAttr(m_context, m_dict[dictAttrName].RuleTable, dictAttrName);
		vector<wstring> availableValues;

		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;

		wstring currentValue = m_tree->GetAttribute(m_context, dictAttrName, false);
		vector<wstring> currentValues = ROMUTIL::Split(currentValue, L"|");
		vector<wstring> selectedValues;

		m_dict[dictAttrName].ChangedByUser = true;

		//set the dictionary default on load
		if (newValues.size() == 0)
		{
			if (m_dict[dictAttrName].DefaultValue.length() > 0)
			{
				newValues.push_back(m_dict[dictAttrName].DefaultValue);
				m_dict[dictAttrName].ChangedByUser = false;
			}
		}

		//if only one is available, force that selection now
		if (availableValues.size() == 1)
		{
			selectedValues.push_back(availableValues[0]);
			m_dict[dictAttrName].ChangedByUser = false;
		}
		//if the current value is "", and an available value is prefixed with a "@" default, set it now
		else if (currentValues.size() == 1 && currentValues[0].length() == 0 && prefixes.size() > 0)
		{
			for (size_t i = 0; i < prefixes.size(); i++)
			{
				if (ROMUTIL::StringContains(prefixes[i], DEFAULTPREFIX))
				{
					selectedValues.push_back(availableValues[i]);
					m_dict[dictAttrName].ChangedByUser = false;
				}
			}
		}

		if (selectedValues.size() == 0 && currentValues.size() > 0) //compare the new values to what is really available
		{
			for (size_t i = 0; i < newValues.size(); i++)
			{
				vector<wstring>::iterator itFind = find(availableValues.begin(), availableValues.end(), newValues[i]);
				if (itFind != availableValues.end())
				{
					selectedValues.push_back(newValues[i]);
				}
			}
		}

		wstring finalValue = L"";
		for (vector<wstring>::iterator it = selectedValues.begin(); it != selectedValues.end(); it++)
		{
			if (finalValue.length() > 0)
				finalValue+= L"|";
			finalValue+=*it;
		}
		if (finalValue != currentValue)
			m_tree->SetAttribute(m_context, dictAttrName, finalValue);
	}

	//drop down list, radio button groups, etc
	void LinearEngine::EvalSingleSelect(wstring dictAttrName, wstring newValue)
	{
		vector<wstring> res = m_tree->EvaluateTableForAttr(m_context, m_dict[dictAttrName].RuleTable, dictAttrName);
		vector<wstring> availableValues;

		//the list of results is what is available for selection in the control
		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;

		wstring currentValue = m_tree->GetAttribute(m_context, dictAttrName);

		//set the dictionary default on load
		if (newValue.length() == 0)
		{
			if (m_dict[dictAttrName].DefaultValue.length() > 0)
			{
				newValue = m_dict[dictAttrName].DefaultValue;
			}
		}

		//if only one is available, force that selection now
		if (availableValues.size() == 1)
		{
			newValue = availableValues[0];
			m_dict[dictAttrName].ChangedByUser = false;
		}

		//if the current value is "", and an available value is prefixed with a "@" default, set it now
		if (currentValue.length() == 0)
		{
			for (size_t i = 0; i < prefixes.size(); i++)
			{
				if (ROMUTIL::StringContains(prefixes[i], DEFAULTPREFIX))
				{
					newValue = availableValues[i];
					m_dict[dictAttrName].ChangedByUser = false;
					break;
				}
			}
		}

		if (prefixes.size() > 0 && prefixes[0].length() > 0 && ROMUTIL::StringContains(prefixes[0], INVISPREFIX))
			m_dict[dictAttrName].Visible = false;
		else
			m_dict[dictAttrName].Visible = true;

		if (newValue.length() > 0 && find(availableValues.begin(), availableValues.end(), newValue) != availableValues.end())
		{
			m_dict[dictAttrName].Valid = true;
			if (currentValue != newValue)
			{
				m_tree->SetAttribute(m_context, dictAttrName, newValue);
				m_dict[dictAttrName].ChangedByUser = true;
			}
		}
		else
		{
			m_dict[dictAttrName].Valid = false;
			m_tree->SetAttribute(m_context, dictAttrName, L"");
			m_dict[dictAttrName].ChangedByUser = false;
		}
	}

	vector<wstring> LinearEngine::ParseOutPrefixes(vector<wstring> values, vector<wstring> &valuesWithoutPrefixes)
	{
		vector<wstring> origValues = values;
		vector<wstring> prefixes;
		valuesWithoutPrefixes.clear();

		for (vector<wstring>::iterator it = origValues.begin(); it != origValues.end(); it++)
		{
			wstring val = *it;
			wstring fullPrefix;

			//check for leadoff ^ indicating an invisible control
			if (ROMUTIL::StringContains(val, INVISPREFIX))
			{
				fullPrefix+=INVISPREFIX;
				val = ROMUTIL::FindAndReplace(val, INVISPREFIX, L"");
			}

			//check for leadoff @ indicating a default
			if (ROMUTIL::StringContains(val, DEFAULTPREFIX))
			{
				fullPrefix+=DEFAULTPREFIX;
				val = ROMUTIL::FindAndReplace(val, DEFAULTPREFIX, L"");
			}

			//check for leadoff # indicating a locked edit box
			if (ROMUTIL::StringContains(val, DISABLEPREFIX))
			{
				fullPrefix+=DISABLEPREFIX;
				val = ROMUTIL::FindAndReplace(val, DISABLEPREFIX, L"");
			}

			prefixes.push_back(fullPrefix);
			valuesWithoutPrefixes.push_back(val);
		}

		return prefixes;
	}

	void LinearEngine::EvaluateDependencies(wstring dictAttrName)
	{
		if (m_mapTriggers.find(dictAttrName) != m_mapTriggers.end())
		{
			vector<wstring> attrsToEval = m_mapTriggers[dictAttrName];
			for (vector<wstring>::iterator it = attrsToEval.begin(); it != attrsToEval.end(); it++)
			{
				map<wstring, ROMDictionaryAttribute>::iterator itFind = m_dict.find(*it);
				if (itFind != m_dict.end())
				{
					vector<wstring> selectedValues = GetSelectedValues(&itFind->second);
					EvaluateForAttribute(*it, selectedValues);
				}
			}
		}
	}

	vector<wstring> LinearEngine::GetSelectedValues(ROMDictionaryAttribute* attr)
	{
		vector<wstring> retval;
		wstring currentValue = m_tree->GetAttribute(m_context, attr->Name);

		switch (attr->AttributeType)
		{
		case SINGLESELECT:
			retval.push_back(currentValue);
			break;

		case MULTISELECT:
			retval = ROMUTIL::Split(currentValue, L"|");
			break;

		case BOOLEANSELECT:
			retval.push_back(currentValue);
			break;

		case EDIT:
			retval.push_back(currentValue);
			break;

		default:
			retval.push_back(currentValue);
			break;
		}

		return retval;
	}

	//ASCII overloads
	LinearEngine::LinearEngine(ROMTree* tree, Node context, string dictionaryTable):ROMDictionary(tree, context)
	{
		InitializeEngine(tree, context, ROMUTIL::MBCStrToWStr(dictionaryTable));
	}

	void LinearEngine::EvaluateForAttribute(string dictAttrName, vector<string> newValues, bool bEvalDependents)
	{
		EvaluateForAttribute(ROMUTIL::MBCStrToWStr(dictAttrName), ROMUTIL::ToWStringVector(newValues), bEvalDependents);
	}

	void LinearEngine::EvaluateForAttribute(string dictAttrName, string newValue, bool bEvalDependents)
	{
		vector<wstring> newValues;
		newValues.push_back(ROMUTIL::MBCStrToWStr(newValue));
		EvaluateForAttribute(ROMUTIL::MBCStrToWStr(dictAttrName), newValues, bEvalDependents);
	}
}