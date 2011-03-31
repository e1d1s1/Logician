/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "Decode.h"
#include "utilities.h"

using namespace EDSUTIL;

#define _CRT_SECURE_NO_DEPRECATE 1

CDecode::~CDecode(void)
{
}

CDecode::CDecode(CRuleCell outputCell, hash_map<wstring, size_t> *inputValues, CBimapper *stringMap)
{
	m_tests = outputCell.Values;
	m_stringsMap = stringMap;
	m_operator = outputCell.Operation;
	m_inputValues = inputValues;
}

CDecode::CDecode(size_t currentValue, CRuleCell inputCell, hash_map<wstring, size_t> *inputValues, CBimapper *stringMap)
{
	m_value = currentValue;
	m_tests = inputCell.Values;
	m_stringsMap = stringMap;
	m_operator = inputCell.Operation;
	m_inputValues = inputValues;

	CheckForInputGets();
}

wstring CDecode::GetString(size_t lKey)
{
	if (lKey > EMPTY_STRING)
	{
		wstring s = m_stringsMap->GetStringByID(lKey);
		if(s.length() > 0)
			return s;
		else
		{
			ReportError("string not found for index: " + stringify(lKey));
		}
	}
	return L"";
}

bool CDecode::EvaluateInputCell()
{
	bool		retval = true;
	try
	{
		//check by operator
		//no string compares needed here for speed
		if (m_operator & EQUALS)
		{
			for (size_t i = 0; i < m_tests.size(); i++) //possible OR
			{
				size_t test = m_tests[i];

				if (test == m_value && test > 0)
				{
					retval = true;
					break;
				}
				else
					retval = false;

				//explict NULL check
				if (retval == false && test == EXPLICIT_NULL_STRING && m_value == EMPTY_STRING)
					retval = true;
			}
		}
		else if (m_operator & NOT_EQUAL)
		{
			for (size_t i = 0; i < m_tests.size(); i++) //possible OR
			{
				size_t test = m_tests[i];

				if (test == m_value)
				{
					retval = false;
				}

				//explict NULL check
				if (retval == true && test == EXPLICIT_NULL_STRING && m_value == EMPTY_STRING)
					retval = false;
			}
		}
		//the other operations require the strings to be de-tokenized for values tests,
		//there are no "ORs" in these cases
		else if (m_operator & LESS_THAN || m_operator & LESS_THAN_EQUAL || m_operator & GREATER_THAN || m_operator & GREATER_THAN_EQUAL)
		{
			wstring currentTest = GetString(m_tests[0]);
			wstring currentValue = GetString(m_value);
			bool bIsNum = false;

			if (StringIsNumeric(currentTest) == true && StringIsNumeric(currentValue) == true)
			{
				bIsNum = true;
			}
			wstring	testValue;
			double dCurrentValue = 0;
			double dTestValue = 0;

			testValue.assign(currentTest.begin(), currentTest.end());
			if (bIsNum)
			{
				try
				{
#ifdef _MSC_VER
					dCurrentValue = _wtof(currentValue.c_str());
					dTestValue = _wtof(testValue.c_str());
#else
					string strCurrent(currentValue.begin(), currentValue.end());
					string strTest(testValue.begin(), testValue.end());
					dCurrentValue = atof(strCurrent.c_str());
					dTestValue = atof(strTest.c_str());
#endif
				}
				catch(...)
				{
					string error = "Could not create numeric values from string data";
					bIsNum = false;
				}
			}


			if (m_operator & LESS_THAN)
			{
				if (bIsNum)
				{
					retval = dCurrentValue < dTestValue;
				}
				else
				{
					retval = currentValue < testValue;
				}
			}
			else if (m_operator & LESS_THAN_EQUAL)
			{
				if (bIsNum)
				{
					retval = dCurrentValue <= dTestValue;
				}
				else
				{
					retval = currentValue <= testValue;
				}
			}
			else if (m_operator & GREATER_THAN)
			{
				if (bIsNum)
				{
					retval = dCurrentValue > dTestValue;
				}
				else
				{
					retval = currentValue > testValue;
				}
			}
			else if (m_operator & GREATER_THAN_EQUAL)
			{
				if (bIsNum)
				{
					retval = dCurrentValue >= dTestValue;
				}
				else
				{
					retval = currentValue >= testValue;
				}
			}
		}
		//will have to deal with the comma seperator to get values
		else if (m_operator & RANGE_INCLUSIVE || m_operator & RANGE_END_INCLUSIVE ||
			m_operator & RANGE_START_INCLUSIVE ||m_operator & RANGE_NOT_INCLUSIVE)
		{
			wstring testString = GetString(m_tests[0]);
			wstring currentValue = GetString(m_value);
			double min = 0, max = 0, dCurrentValue = 0;
			vector<wstring> parts = EDSUTIL::Split(testString.c_str(), L",");

			if (parts.size() == 2)
			{
				try
				{
#ifdef _MSC_VER
					min = _wtof(parts[0].c_str());
					max = _wtof(parts[1].c_str());
					dCurrentValue = _wtof(currentValue.c_str());
#else
					string part0(parts[0].begin(), parts[0].end());
					string part1(parts[1].begin(), parts[1].end());
					min = atof(part0.c_str());
					max = atof(part1.c_str());
					string sCurVal(currentValue.begin(), currentValue.end());
					dCurrentValue = atof(sCurVal.c_str());
#endif

				}
				catch(...)
				{
					string error = "Could not create numeric values from string data";
					throw error;
				}


				if (m_operator & RANGE_INCLUSIVE)
				{
					retval = (dCurrentValue >= min && dCurrentValue <= max);
				}
				else if (m_operator & RANGE_END_INCLUSIVE)
				{
					retval = (dCurrentValue > min && dCurrentValue <= max);
				}
				else if (m_operator & RANGE_START_INCLUSIVE)
				{
					retval = (dCurrentValue >= min && dCurrentValue < max);
				}
				else if (m_operator & RANGE_NOT_INCLUSIVE)
				{
					retval = (dCurrentValue > min && dCurrentValue < max);
				}

			}
		}

		else if(m_operator & PYTHON)
		{
		    string error = "scripting not supported as an input";
			throw error;
		}

	}
	catch (string)
	{
		retval = false;
	}
	catch(...)
	{
		retval = false;
	}
	return retval;
}

vector<wstring> CDecode::EvaluateOutputCell()
{
	vector<wstring> retval;

	try
	{
		for (vector<size_t>::iterator it = m_tests.begin(); it != m_tests.end(); it++)
		{
			if (m_operator & GETS)
			{
				retval.push_back(ParseStringForGets(*it, false));
			}
			else
			{
				retval.push_back(GetString(*it));
			}
		}
	}
	catch(...)
	{

	}

	return retval;
}

wstring CDecode::ReplaceAGet(wstring s, bool bForceZero)
{
	wstring retval = L"";
	//find the get(xxx) substring.  attrName xxx is between ().
	int iStartPos = s.find(L"get(", 0);
	int iEndPos = s.find_first_of(L")", iStartPos);
	if (iStartPos >= 0 && iEndPos > iStartPos)
	{
		wstring attrName(s.begin() + iStartPos + 4, s.begin() + iEndPos);
		wstring getText = L"get(" + attrName + L")";
		//get the value of the input attr
		bool bFoundAttr = false;
		if (m_inputValues != NULL && m_inputValues->size() > 0)
		{
			hash_map<wstring, size_t>::iterator it = m_inputValues->find(attrName);

			if (it != m_inputValues->end())
			{
				wstring value = GetString((*it).second);
				if (value.size() > 0)
				{
					bFoundAttr = true;
					retval = EDSUTIL::FindAndReplace(s, getText, value);
				}
			}
		}

		if (!bFoundAttr)
		{
			if (bForceZero)
				retval = EDSUTIL::FindAndReplace(s, getText, L"0");
			else
				retval = EDSUTIL::FindAndReplace(s, getText, L"");
		}
	}

	return retval;
}

wstring CDecode::ParseStringForGets(size_t lKey, bool bForceZero)
{
	wstring retval = L"";

	wstring fullString = GetString(lKey);
	//replace with the actual value
	if (StringContains(fullString, L"get("))
	{
		do
		{
			fullString = ReplaceAGet(fullString, bForceZero);
			retval = fullString;
		} while(StringContains(fullString, L"get("));
	}
	else
		retval = fullString;

	return retval;
}

void CDecode::CheckForInputGets()
{
	if (m_operator & GETS)
	for (size_t i = 0; i < m_tests.size(); i++)
	{
		//replace the GET with the actual value (pay a performance penalty here, but convienient)
		wstring val = ParseStringForGets(m_tests[i], false);
		m_tests[i] = m_stringsMap->GetIDByString(val);
	}
}
