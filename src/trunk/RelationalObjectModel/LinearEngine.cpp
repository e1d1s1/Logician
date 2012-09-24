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
#include "LinearEngine.h"
#include "utilities.h"

namespace ROM
{
	void LinearEngine::CreateLinearEngine(ROMNode* context, wstring dictionaryTable)
	{
		InvalidateMode = NORMALINVALIDATE;
		TBUATTR = L"TBU_";
		InitializeEngine(dictionaryTable);		
	}

	void LinearEngine::InitializeEngine(wstring dictionaryTable)
	{
		EDS::CKnowledgeBase *knowledge = m_ROMContext->_getKnowledge();
		if (knowledge)
		{
			INVISPREFIX = L"^";
			DEFAULTPREFIX = L"@";
			DISABLEPREFIX = L"#";

			m_vEvalList.clear();
			m_mapTriggers.clear();
			LoadDictionary(dictionaryTable);
			LoadTrackingAttrs();

			//open each attr in dict, load its dependency info to create m_vEvalList, m_mapTriggers
			//build an initial list that matches the dictionary order
			for (map<wstring, ROMDictionaryAttribute>::iterator it = m_dict.begin(); it != m_dict.end(); it++)
			{
				m_vEvalList.push_back(&it->second);
				//triggers			
				vector<wstring> deps = knowledge->GetInputDependencies(it->second.RuleTable);
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
			m_EvalInternal = false;
			OrderDictionary();
			m_vEvalListRecursChecker.clear();
		}
	}

	void LinearEngine::LoadTrackingAttrs()
	{
		MAPWSTRMAP allAttrs = m_ROMContext->GetAllAttributes();
		for (MAPWSTRMAP::iterator it = allAttrs.begin(); it != allAttrs.end(); it++)
		{
			if (ROMUTIL::StringContains(it->first, TBUATTR))
			{
				wstring dictAttrName = ROMUTIL::FindAndReplace(it->first, TBUATTR, L"");
				if (m_dict.find(dictAttrName) != m_dict.end())
					m_dict[dictAttrName].ChangedByUser = true;
			}
		}
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

		for (size_t i = 0; i < evalOrderCopy.size(); i++)
		{
			//if the lists differ, do another sort, otherwise we should be done
			if (evalOrderCopy[i] != m_vEvalList[i] && m_CurrentRecursion < MAX_RECURSION)
			{
				//does it match a previous result (are we flipping between a couple values, circular logic)
				if (m_CurrentRecursion % 2 == 0 && m_vEvalListRecursChecker.size() > 0)
				{
					for (size_t j = 0; j < m_vEvalListRecursChecker.size(); j++)
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
			m_vEvalListRecursChecker = m_vEvalList;
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
		ResetValueChanged();
		
		if (!m_EvalInternal)
		{
			//resets other atts when setting this one
			m_ROMContext->SetAttribute(L"currentattr", dictAttrName);
			vector<wstring> attrsToReset = m_ROMContext->EvaluateTable((wstring)L"reset", (wstring)L"attr");
			for (vector<wstring>::iterator it = attrsToReset.begin(); it != attrsToReset.end(); it++)
			{
				m_ROMContext->SetAttribute(*it, L"");
				RemoveTouchedByUser(*it);
				if (m_dict[*it].AvailableValues.size() > 0)
					m_dict[*it].Valid = false;
			}

			//resets an attr if it has not been touched by the user			
			attrsToReset = m_ROMContext->EvaluateTable((wstring)L"reset_INCBU", (wstring)L"attr");
			for (vector<wstring>::iterator it = attrsToReset.begin(); it != attrsToReset.end(); it++)
			{
				if (!IsTouchedByUser(*it))
				{
					m_ROMContext->SetAttribute(*it, L"");
					RemoveTouchedByUser(*it);
					if (m_dict[*it].AvailableValues.size() > 0)
						m_dict[*it].Valid = false;
				}
			}
			
		}


		map<wstring, ROMDictionaryAttribute>::iterator itFind = m_dict.find(dictAttrName);
		if (itFind != m_dict.end())
		{
			m_dict[dictAttrName].ValueChanged = true;
			bool bChanged = !m_EvalInternal;
			if (bChanged)
				SetTouchedByUser(dictAttrName);

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

			default: //STATIC
				EvalSingleSelect(dictAttrName, newValues[0]);				
				break;
			}

			if (bEvalDependents)
				EvaluateDependencies(dictAttrName);
		}
	}

	void LinearEngine::ResetValueChanged()
	{
		for (vector<ROMDictionaryAttribute*>::iterator it = m_vEvalList.begin(); it != m_vEvalList.end(); it++)
		{
			(*it)->ValueChanged = false;
		}
	}

	bool LinearEngine::IsTouchedByUser(wstring dictAttrName)
	{
		return m_ROMContext->GetAttributeExists(TBUATTR + dictAttrName);
	}

	void LinearEngine::SetTouchedByUser(wstring dictAttrName)
	{
		m_dict[dictAttrName].ChangedByUser = true;
		m_ROMContext->SetAttribute(TBUATTR + dictAttrName, L"Y");
	}
	
	void LinearEngine::RemoveTouchedByUser(wstring dictAttrName)
	{
		m_dict[dictAttrName].ChangedByUser = false;
		m_ROMContext->RemoveAttribute(TBUATTR + dictAttrName);
	}

	void LinearEngine::EvaluateAll()
	{
		m_EvalInternal = true;
		ResetValueChanged();
		for (vector<ROMDictionaryAttribute*>::iterator it = m_vEvalList.begin(); it != m_vEvalList.end(); it++)
		{
			vector<wstring> selectedValues = GetSelectedValues(*it);
			EvaluateForAttribute((*it)->Name, selectedValues, false);
		}
		m_EvalInternal = false;
	}

	bool LinearEngine::DictionaryIsValid()
	{
		bool retval = true;
		for (vector<ROMDictionaryAttribute*>::iterator it = m_vEvalList.begin(); it != m_vEvalList.end(); it++)
		{
			if (!(*it)->Valid)
			{
				retval = false;
				break;
			}
		}
		return retval;
	}

	void LinearEngine::FlagAttrInvalid(std::wstring dictAttrName)
	{
		m_dict[dictAttrName].ChangedByUser = false;
		m_dict[dictAttrName].Valid = false;
	}

	void LinearEngine::EvalBoolean(std::wstring dictAttrName, std::wstring newValue)
	{
		vector<wstring> res = m_ROMContext->EvaluateTable(m_dict[dictAttrName].RuleTable, dictAttrName, false);
		vector<wstring> availableValues;

		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;

		if (prefixes.size() > 0 && prefixes[0].length() > 0 && prefixes[0][0] == L'^')
			m_dict[dictAttrName].Visible = false;
		else
			m_dict[dictAttrName].Visible = true;

		wstring currentValue = m_ROMContext->GetAttribute(dictAttrName, true);
		m_dict[dictAttrName].Valid = true;
		m_dict[dictAttrName].Enabled = true;

		//set a default
		bool bOverrideDisabled = false;
		if (currentValue.length() == 0)
		{
			if (m_dict[dictAttrName].DefaultValue.length() > 0)
				currentValue = m_dict[dictAttrName].DefaultValue;
			else
			{
				currentValue = L"N";	
				if (availableValues.size() == 1 && availableValues[0].length() > 0 &&
					availableValues[0][0] == 'Y')
					bOverrideDisabled = true;
			}
		}

		if (availableValues.size() == 1) //you should only have one value
		{
			if (availableValues[0].length() == 0 || availableValues[0][0] == L'N')
			{
				m_ROMContext->SetAttribute(dictAttrName, L"N");
				RemoveTouchedByUser(dictAttrName);
				m_dict[dictAttrName].Enabled = false;
				return;
			}
			else if (availableValues[0] == L"YN") //allow Yes or No with a default of Y
			{
				if (!IsTouchedByUser(dictAttrName))
				{
					m_ROMContext->SetAttribute(dictAttrName, L"Y");
				}
				else
				{
					m_ROMContext->SetAttribute(dictAttrName, newValue);
				}
			}
			else if (availableValues[0] == L"YY") //force Yes, no other choice
			{
				m_ROMContext->SetAttribute(dictAttrName, L"Y");
				RemoveTouchedByUser(dictAttrName);
				m_dict[dictAttrName].Enabled = false;
			}
			else if (newValue.length() == 1) //Y or N
			{
				m_ROMContext->SetAttribute(dictAttrName, newValue);
			}
			else if (currentValue.length() == 0 && newValue.length() == 0)
			{
				m_ROMContext->SetAttribute(dictAttrName, L"N");
				RemoveTouchedByUser(dictAttrName);
				m_dict[dictAttrName].Enabled = false;
			}
			else
			{
				if (currentValue == L"N" && !bOverrideDisabled)
					m_dict[dictAttrName].Enabled = false;
				m_ROMContext->SetAttribute(dictAttrName, currentValue);
			}
		}
		else if (newValue.length() == 1) //Y or N
		{
			m_ROMContext->SetAttribute(dictAttrName, newValue);
		}
		else if (currentValue.length() == 0 && newValue.length() == 0)
		{
			m_ROMContext->SetAttribute(dictAttrName, L"N");
			RemoveTouchedByUser(dictAttrName);
			m_dict[dictAttrName].Enabled = false;
		}
		else
		{
			if (currentValue == L"N" && !bOverrideDisabled)
				m_dict[dictAttrName].Enabled = false;
			m_ROMContext->SetAttribute(dictAttrName, currentValue);
		}
	}

	void LinearEngine::EvalEdit(std::wstring dictAttrName, std::wstring newValue)
	{
		vector<wstring> res = m_ROMContext->EvaluateTable(m_dict[dictAttrName].RuleTable, dictAttrName);
		vector<wstring> availableValues;

		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;
		m_dict[dictAttrName].Enabled = true;
		m_dict[dictAttrName].Valid = true;

		bool setTheValue = true;
		if (InvalidateMode != NORMALINVALIDATE)
		{
			wstring currentValue = m_ROMContext->GetAttribute(dictAttrName, true);
			setTheValue = currentValue.length() == 0;
		}

		//set the dictionary default on load
		if (newValue.length() == 0)
		{
			if (m_dict[dictAttrName].DefaultValue.length() > 0)
				newValue = m_dict[dictAttrName].DefaultValue;
		}

		if (availableValues.size() == 1)
		{
			if (ROMUTIL::StringContains(prefixes[0], DISABLEPREFIX))
			{
				m_dict[dictAttrName].Enabled = false;
				RemoveTouchedByUser(dictAttrName);
				m_ROMContext->SetAttribute(dictAttrName, prefixes[0]);
			}
			else
				m_dict[dictAttrName].Enabled = true;

			//check table result for range
			if (availableValues[0][0] == L'[')
			{
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
					m_ROMContext->SetAttribute(dictAttrName, newValue);
			
				}
				else if (dNewValue > dMax)
				{
					if (setTheValue)
					{
						wstring wstrMax(vals[1].begin(), vals[1].end());
						m_ROMContext->SetAttribute(dictAttrName, wstrMax);
					}
					else
					{
						FlagAttrInvalid(dictAttrName);
					}
				}
				else if (dNewValue < dMin)
				{
					if (setTheValue)
					{
						wstring wstrMin(vals[0].begin(), vals[0].end());
						m_ROMContext->SetAttribute(dictAttrName, wstrMin);
					}
					else
					{
						FlagAttrInvalid(dictAttrName);
					}
				}
			}
			else if (availableValues[0].length() == 1 && availableValues[0][0] == L'Y')
			{
				m_ROMContext->SetAttribute(dictAttrName, newValue);			
			}
			else if (availableValues[0].length() == 1 && availableValues[0][0] == L'N')
			{
				m_ROMContext->SetAttribute(dictAttrName, L"");
				RemoveTouchedByUser(dictAttrName);
				m_dict[dictAttrName].Enabled = false;
			}
			else if (availableValues[0].length() == 2 && availableValues[0] == L"YY") //user must enter something
			{	
				m_ROMContext->SetAttribute(dictAttrName, newValue);
				m_dict[dictAttrName].Valid = newValue.length() > 0;
			}
			else
			{
				m_ROMContext->SetAttribute(dictAttrName, availableValues[0]);
				RemoveTouchedByUser(dictAttrName);
			}
		}
		else if (availableValues.size() == 0)
		{
			m_ROMContext->SetAttribute(dictAttrName, newValue);
			RemoveTouchedByUser(dictAttrName);
			m_dict[dictAttrName].Enabled = false;
		}
		else if (availableValues.size() == 1 && availableValues[0].length() > 0)
		{
			m_ROMContext->SetAttribute(dictAttrName, availableValues[0]);
			RemoveTouchedByUser(dictAttrName);
		}
		else if (availableValues.size() > 0)
		{
			vector<wstring>::iterator itFind = find(availableValues.begin(), availableValues.end(), newValue);
			if (itFind != availableValues.end())
			{
				m_ROMContext->SetAttribute(dictAttrName, newValue);
			}
			else
			{
				m_ROMContext->SetAttribute(dictAttrName, L"");
				RemoveTouchedByUser(dictAttrName);
			}
		}

		if (prefixes.size() > 0 && ROMUTIL::StringContains(prefixes[0], INVISPREFIX))
			m_dict[dictAttrName].Visible = false;
		else
			m_dict[dictAttrName].Visible = true;
	}

	void LinearEngine::EvalMultiSelect(std::wstring dictAttrName, vector<wstring> newValues)
	{
		//multi-select lists, checkbox lists
		vector<wstring> res = m_ROMContext->EvaluateTable(m_dict[dictAttrName].RuleTable, dictAttrName);
		vector<wstring> availableValues;
		m_dict[dictAttrName].Enabled = true;
		m_dict[dictAttrName].Valid = true;

		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;

		wstring currentValue = m_ROMContext->GetAttribute(dictAttrName, false);
		vector<wstring> currentValues = ROMUTIL::Split(currentValue, L"|");
		vector<wstring> selectedValues;

		bool setTheValue = true;		
		bool bFound = true;		
		if (InvalidateMode != NORMALINVALIDATE)
		{
			setTheValue = currentValues.size() == 0;
			for (vector<wstring>::iterator it = currentValues.begin(); it != currentValues.end(); it++)
			{
				bFound = find(availableValues.begin(), availableValues.end(), *it) != availableValues.end();
				if (bFound)
					break;
			}		
		}

		//set the dictionary default on load
		if (newValues.size() == 0)
		{
			if (m_dict[dictAttrName].DefaultValue.length() > 0)
			{
				newValues.push_back(m_dict[dictAttrName].DefaultValue);
				RemoveTouchedByUser(dictAttrName);
			}
		}

		//if only one is available, force that selection now
		if (availableValues.size() == 1)
		{
			selectedValues.push_back(availableValues[0]);
			RemoveTouchedByUser(dictAttrName);
			bFound = true;
		}
		//if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
		else if (currentValues.size() == 1 && (currentValues[0].length() == 0 || find(availableValues.begin(), availableValues.end(), currentValues[0]) == availableValues.end()) && prefixes.size() > 0)
		{
			for (size_t i = 0; i < prefixes.size(); i++)
			{
				if (ROMUTIL::StringContains(prefixes[i], DEFAULTPREFIX))
				{
					if (InvalidateMode == NORMALINVALIDATE || currentValue.length() == 0)
					{
						selectedValues.push_back(availableValues[i]);
						RemoveTouchedByUser(dictAttrName);
						bFound = true;
					}
				}
			}
		}
		else if (availableValues.size() == 0)
		{
			m_dict[dictAttrName].Enabled = false;
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
		{
			if (!setTheValue)
				setTheValue = bFound;

			if (setTheValue)
				m_ROMContext->SetAttribute(dictAttrName, finalValue);
			else
			{
				if (availableValues.size() > 0)
					FlagAttrInvalid(dictAttrName);
				else
					m_ROMContext->SetAttribute(dictAttrName, finalValue);
			}				
		}
	}

	//drop down list, radio button groups, etc
	void LinearEngine::EvalSingleSelect(wstring dictAttrName, wstring newValue)
	{
		vector<wstring> res = m_ROMContext->EvaluateTable(m_dict[dictAttrName].RuleTable, dictAttrName);
		vector<wstring> availableValues;
		m_dict[dictAttrName].Enabled = true;
		m_dict[dictAttrName].Valid = true;

		//the list of results is what is available for selection in the control
		vector<wstring> prefixes = ParseOutPrefixes(res, availableValues);
		m_dict[dictAttrName].AvailableValues = availableValues;
		bool bFound = find(availableValues.begin(), availableValues.end(), newValue) != availableValues.end();

		wstring currentValue = m_ROMContext->GetAttribute(dictAttrName, true);
		bool setTheValue = currentValue.length() == 0 || InvalidateMode == NORMALINVALIDATE;

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
			bFound = true;
			RemoveTouchedByUser(dictAttrName);
		}

		//if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
		if ((currentValue.length() == 0 || (find(availableValues.begin(), availableValues.end(), currentValue) == availableValues.end())) && prefixes.size() > 0)
		{
			for (size_t i = 0; i < prefixes.size(); i++)
			{
				if (ROMUTIL::StringContains(prefixes[i], DEFAULTPREFIX))
				{
					if (InvalidateMode == NORMALINVALIDATE || currentValue.length() == 0)
					{
						newValue = availableValues[i];						
						bFound = true;			
						RemoveTouchedByUser(dictAttrName);
					}
					break;
				}
			}
		}
		else if (availableValues.size() == 0)
		{
			m_dict[dictAttrName].Enabled = false;
		}

		if (prefixes.size() > 0 && prefixes[0].length() > 0 && ROMUTIL::StringContains(prefixes[0], INVISPREFIX))
			m_dict[dictAttrName].Visible = false;
		else
			m_dict[dictAttrName].Visible = true;
		
		if (newValue.length() > 0 && bFound)
		{
			if (currentValue != newValue)
			{
				if (!setTheValue)
					setTheValue = bFound;

				if (setTheValue)
					m_ROMContext->SetAttribute(dictAttrName, newValue);
				else				
					FlagAttrInvalid(dictAttrName);				
			}
		}
		else
		{
			if (m_dict[dictAttrName].Enabled == true)
				m_dict[dictAttrName].Valid = false;
			if (setTheValue)
				m_ROMContext->SetAttribute(dictAttrName, L"");
			else
			{
				if (availableValues.size() > 0)
					FlagAttrInvalid(dictAttrName);
				else
					m_ROMContext->SetAttribute(dictAttrName, L"");
			}

			RemoveTouchedByUser(dictAttrName);
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
		m_EvalInternal = true;
		if (m_mapTriggers.find(dictAttrName) != m_mapTriggers.end())
		{
			vector<wstring> attrsToEval = m_mapTriggers[dictAttrName];
			for (vector<wstring>::iterator it = attrsToEval.begin(); it != attrsToEval.end(); it++)
			{
				map<wstring, ROMDictionaryAttribute>::iterator itFind = m_dict.find(*it);
				if (itFind != m_dict.end())
				{
					vector<wstring> selectedValues = GetSelectedValues(&itFind->second);
					bool bWasChangedByUser = itFind->second.ChangedByUser;
					EvaluateForAttribute(*it, selectedValues, true);
					m_EvalInternal = true;
					if (bWasChangedByUser)
					{
						bool bValuesRemainSame = true;
						vector<wstring> newSelectedValues = GetSelectedValues(&itFind->second);
						if (newSelectedValues.size() != selectedValues.size())
							bValuesRemainSame = false;
						else for (size_t i = 0; i < selectedValues.size(); i++)
						{
							vector<wstring>::iterator itFind = find(newSelectedValues.begin(), newSelectedValues.end(), selectedValues[i]);
							if (itFind == newSelectedValues.end())
							{
								bValuesRemainSame = false;
								break;
							}
						}

						itFind->second.ChangedByUser = bValuesRemainSame;
					}
				}
			}
		}
		m_EvalInternal = false;
	}

	vector<wstring> LinearEngine::GetSelectedValues(ROMDictionaryAttribute* attr)
	{
		vector<wstring> retval;
		wstring currentValue = m_ROMContext->GetAttribute(attr->Name, true);

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
	LinearEngine::LinearEngine(ROMNode* context, string dictionaryTable):ROMDictionary(context)
	{
		InitializeEngine(ROMUTIL::MBCStrToWStr(dictionaryTable));
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
