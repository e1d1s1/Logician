/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    asize_t with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "RuleTable.h"
#include "Decode.h"
#include <set>
#include <algorithm>
#include <list>
#include <string>
#include <thread>
#include <memory>
#include "utilities.h"

#define THREAD_THRESHOLD 20 //at least this many rules to trigger threading

using namespace std;

CRuleTable::CRuleTable(void)
{
	m_DEBUGGING = false;
	m_Threads = 1;
	ResetTable();
}

CRuleTable::~CRuleTable(void)
{

}

void CRuleTable::ResetTable()
{
	m_Tests = 0;
	bHasChain = false;
	bHasPython = false;
	bHasJavascript = false;
	bGetAll = false;
	bNullSet = false;
	m_ThreadingEnabled = false;

	m_InputAttrsTests.clear();
	m_InputAttrsValues.clear();
	m_OutputAttrsValues.clear();
	m_FormulaInputs.clear();
	DebugMessage.clear();
}

void CRuleTable::CreateRuleTable(vector<pair<wstring, vector<CRuleCell> > > inputAttrsTests,
								vector<pair<wstring, vector<CRuleCell> > > outputAttrsValues,
								vector<wstring> formulaInputs, CBimapper *stringMap,
								wstring name, bool GetAll)
{
	ResetTable();

	m_FormulaInputs = formulaInputs;
	m_InputAttrsTests = inputAttrsTests;
	m_OutputAttrsValues = outputAttrsValues;

	if (m_OutputAttrsValues.size() > 0)
    {
        m_Tests = m_OutputAttrsValues[0].second.size();
        if (m_Threads > 1)
            m_ThreadingEnabled = m_Tests >= THREAD_THRESHOLD;
    }

	m_Name = name;
	m_stringsMap = stringMap;
	bHasChain = false;
	bHasPython = false;
	bHasJavascript = false;

	bGetAll = GetAll;
}

CRuleTable::CRuleTable(vector<pair<wstring, vector<CRuleCell> > > inputAttrsTests,
					   vector<pair<wstring, vector<CRuleCell> > > outputAttrsValues,
					   vector<wstring> formulaInputs, CBimapper *stringMap,
					   wstring name, bool GetAll)
{
	CreateRuleTable(inputAttrsTests, outputAttrsValues, formulaInputs, stringMap, name, GetAll);
}

map<wstring, vector<wstring> > CRuleTable::EvaluateTable(bool bGetAll, bool bForward)
{
	map<wstring, vector<wstring> > retval;
	vector<pair<wstring, vector<CRuleCell> > > *resultCollection;
	if (bForward)
		resultCollection = &m_OutputAttrsValues;
	else
		resultCollection = &m_InputAttrsTests;

	//for all the outputs get the results
	for (vector<pair<wstring, vector<CRuleCell> > >::iterator itOut = resultCollection->begin(); itOut != resultCollection->end(); itOut++)
	{
		vector<wstring> result = EvaluateTable((*itOut).first, bGetAll, bForward);
		retval[(*itOut).first] = result;
	}

	return retval;
}

vector<wstring> CRuleTable::EvaluateTable(wstring outputAttr, bool bGetAll, bool bForward)
{
	vector<wstring> retval;
	vector<size_t> values;
	map<size_t, set<wstring> > solutions;
	vector<pair<wstring, vector<CRuleCell> > > *inputCollection;
	vector<pair<wstring, vector<CRuleCell> > > *outputCollection;
	if (bForward)
	{
		inputCollection = &m_InputAttrsTests;
		outputCollection = &m_OutputAttrsValues;
	}
	else
	{
		inputCollection = &m_OutputAttrsValues;
		outputCollection = &m_InputAttrsTests;
	}


	SetInvalidAttrs();
	vector<bool> colResults (m_Tests, true); //a table need not have any inputs
	if (m_InputAttrsValues.size() > 0 && inputCollection->size() > 0)
	{
		//get the current values of all input attrs
		for (vector<pair<wstring, vector<CRuleCell> > >::iterator it = inputCollection->begin(); it != inputCollection->end(); it++)
		{
			wstring attr = (*it).first;
			MAPWSTRUINT::iterator itFind = m_InputAttrsValues.find(attr);
			if (itFind != m_InputAttrsValues.end())
			{
				values.push_back((*itFind).second);
			}
		}

		//sweep down the table for all inputs and do test(s)
		colResults = _runTests(bGetAll, inputCollection, &values);

	} //done inputs
	else if (inputCollection->size() == 0 && !bGetAll)
	{
		if (colResults.size() > 1)
			for (size_t i = 1; i < colResults.size(); i++)
				colResults[i] = false;
	}

	//for the give output, the reuslts are
	vector<CRuleCell> results;
	for (size_t result = 0; result < outputCollection->size(); result++)
	{
		if ((*outputCollection)[result].first == outputAttr)
		{
			results = (*outputCollection)[result].second;
		}
	}

	//for each true result, add to the solution vector
	for (size_t result = 0; result < m_Tests; result++)
	{
		if (colResults[result] && result < results.size())
		{
			CRuleCell outputCell = results[result];
			CDecode decoder(&outputCell, &m_InputAttrsValues, m_stringsMap);
			if (outputCell.Operation & CHAIN)
				bHasChain = true;
			if (outputCell.Operation & PYTHON)
				bHasPython = true;
			if (outputCell.Operation & JAVASCRIPT)
				bHasJavascript = true;
			vector<wstring> cellOutputs = decoder.EvaluateOutputCell();
			for (vector<wstring>::iterator itOutputs = cellOutputs.begin(); itOutputs != cellOutputs.end(); itOutputs++)
			{
				retval.push_back(*itOutputs);
				if (m_DEBUGGING)
				{
					map<size_t, set<wstring> >::iterator itFind = solutions.find(result);
					if (itFind != solutions.end())
					{
						itFind->second.insert(*itOutputs);
					}
					else
					{
						solutions[result].insert(*itOutputs);
					}
				}
			}
		}
	}


	//report the eval results over a tcp socket connection
	if (m_DEBUGGING)
	{
		DebugEval(outputAttr, values, solutions);
	}

	return retval;
}

vector<bool> CRuleTable::_runTests(bool bGetAll, vector<pair<wstring, vector<CRuleCell> > >* inputCollection, vector<size_t>* values)
{
    //sweep down the table for all inputs and do test(s)
    vector<bool> colResultsDefault (m_Tests, false);
    vector<bool> colResults = colResultsDefault;

    if (m_ThreadingEnabled)
    {
        size_t testsPerThread = m_Tests/m_Threads;
        unique_ptr<thread[]> threads = unique_ptr<thread[]>(new thread[m_Threads]);
        for (size_t i = 0; i < m_Threads; i++)
        {
            size_t startIndex = i * testsPerThread;
            size_t endIndex = i == m_Threads - 1 ? m_Tests : (i + 1) * testsPerThread;
            function<bool(void)> worker = [&]()
            {
                return _runTestGroup(bGetAll, startIndex, endIndex, inputCollection, values, &colResults);
            };
            threads[i] = thread(worker);
        }

        for (size_t i = 0; i < m_Threads; i++)
        {
            threads[i].join();
        }
    }
    else
    {
        _runTestGroup(bGetAll, 0, m_Tests, inputCollection, values, &colResults);
    }

    return colResults;
}

bool CRuleTable::_runTestGroup(bool bGetAll, size_t startIndex, size_t endIndex, vector<pair<wstring, vector<CRuleCell> > >* inputCollection, vector<size_t>* values, vector<bool>* colResults)
{
    bool bHaveSolution = true;
    for (size_t testIndex = startIndex; testIndex < endIndex; testIndex++)
    {
        //sweep through the inputs
        size_t inputCnt = 0;
        for (auto itTests = inputCollection->begin(); itTests != inputCollection->end(); itTests++)
        {
            if ( testIndex < (*itTests).second.size())
            {
                CDecode decoder((*values)[inputCnt], &(*itTests).second[testIndex], &m_InputAttrsValues, m_stringsMap);
                bHaveSolution = decoder.EvaluateInputCell();
            }
            inputCnt++;
            if (!bHaveSolution)
                break;
        }
        (*colResults)[testIndex] = bHaveSolution;
        if (bHaveSolution && !bGetAll)
            break;
    } //done column (rule)
    return bHaveSolution;
}

void CRuleTable::DebugEval(wstring outputAttr, vector<size_t> inputValues, map<size_t, set<wstring> > solutions)
{
	wstring xmlBlob;
	xmlBlob += L"<TableEval name=\"";
	xmlBlob += this->m_Name;
	xmlBlob += L"\" output=\"";
	xmlBlob += outputAttr;
	xmlBlob += L"\">";

	if (m_InputAttrsTests.size() == inputValues.size())
	{
		xmlBlob += L"<Inputs>";
		for (size_t i = 0; i < m_InputAttrsTests.size(); i++)
		{
			pair<wstring, vector<CRuleCell> > currentPair = m_InputAttrsTests[i];
			wstring attr = currentPair.first;
			wstring value = m_stringsMap->GetStringByID(inputValues[i]);
			xmlBlob += L"<Input name = \"";
			xmlBlob += attr;
			xmlBlob += L"\" value=\"";
			xmlBlob += value;
			xmlBlob += L"\"/>";
		}
		xmlBlob += L"</Inputs>";
	} //else something is wrong

	xmlBlob+= L"<Outputs>";
	for (map<size_t, set<wstring> >::iterator it = solutions.begin(); it != solutions.end(); it++)
	{
		size_t index = (*it).first;
		for (set<wstring>::iterator itOut = (*it).second.begin(); itOut != (*it).second.end(); itOut++)
		{
			xmlBlob += L"<Output value=\"";
			xmlBlob += *itOut;
			xmlBlob += L"\" index=\"";
			xmlBlob += EDSUTIL::ToWString(EDSUTIL::stringify(index));
			xmlBlob += L"\"/>";
		}
	}
	xmlBlob+= L"</Outputs>";
	xmlBlob+= L"</TableEval>";

	DebugMessage = xmlBlob;
}

void CRuleTable::SetInputValues(MAPWSTRUINT values)
{
	bNullSet = false;
	m_InputAttrsValues = values;
}

void CRuleTable::SetInvalidAttrs()
{
	if (!bNullSet)
	{
		for (vector<pair<wstring, vector<CRuleCell> > >::iterator it = m_InputAttrsTests.begin(); it != m_InputAttrsTests.end(); it++)
		{
			MAPWSTRUINT::iterator itFind = m_InputAttrsValues.find((*it).first);
			if (itFind == m_InputAttrsValues.end())
			{
				m_InputAttrsValues[(*it).first] = INVALID_STRING;
			}
		}
	}
	bNullSet = true;
}

void CRuleTable::SetInputValue(wstring name, wstring value)
{
	bNullSet = false;

	if (m_InputAttrsTests.size() > 0)
	{
		bool bFoundTableInput = false;
		for (size_t i = 0; i < m_InputAttrsTests.size(); i++)
		{
			if (m_InputAttrsTests[i].first == name)
			{
				bFoundTableInput = true;
				size_t id = m_stringsMap->GetIDByString(value);
				if (id == INVALID_STRING) //wasnt in our existing list
				{
					//add a new tokenized string
					id = m_stringsMap->AddUserString(value);
				}
				m_InputAttrsValues[name] = id;
				break;
			}
		}

		if (!bFoundTableInput) //this came from outside this table, a get() in a cell
		{
			size_t id = m_stringsMap->GetIDByString(value);
			if (id == INVALID_STRING) //wasnt in our existing list
			{
				//add a new tokenized string
				id = m_stringsMap->AddUserString(value);
			}
			m_InputAttrsValues[name] = id;
		}
	}
}

vector<wstring> CRuleTable::GetAllPossibleOutputs(wstring outputName)
{
	vector<wstring> retval;
	std::list<size_t> allValues;

	for (vector<pair<wstring, vector<CRuleCell> > >::iterator it = m_OutputAttrsValues.begin(); it != m_OutputAttrsValues.end(); it++)
	{
		pair<wstring, vector<CRuleCell> > row = *it;

		if (row.first == outputName)
		{
			for (vector<CRuleCell>::iterator itRow = row.second.begin(); itRow != row.second.end(); itRow++)
			{
				for (vector<size_t>::iterator itValue = (*itRow).Values.begin(); itValue != (*itRow).Values.end(); itValue++)
					allValues.push_back(*itValue);
			}
			break;
		}
	}

	allValues.sort();
	allValues.unique();

	for (list<size_t>::iterator it = allValues.begin(); it != allValues.end(); it++)
	{
		retval.push_back(m_stringsMap->GetStringByID(*it));
	}

	return retval;
}

vector<wstring> CRuleTable::GetAllInputAttrNames()
{
	vector<wstring> retval;

	for (vector<pair<wstring, vector<CRuleCell> > >::iterator it = m_InputAttrsTests.begin(); it != m_InputAttrsTests.end(); it++)
	{
		if ((*it).first.size() > 0)
			retval.push_back((*it).first);
	}

	return retval;
}

vector<wstring> CRuleTable::GetAllInputDependencies()
{
	vector<wstring> retval;

	retval = GetAllInputAttrNames();
	for (vector<wstring>::iterator it = m_FormulaInputs.begin(); it != m_FormulaInputs.end(); it++)
	{
		retval.push_back(*it);
	}

	return retval;
}

vector<wstring> CRuleTable::GetAllOutputAttrNames()
{
	vector<wstring> retval;

	for (vector<pair<wstring, vector<CRuleCell> > >::iterator it = m_OutputAttrsValues.begin(); it != m_OutputAttrsValues.end(); it++)
	{
		if ((*it).first.size() > 0)
			retval.push_back((*it).first);
	}

	return retval;
}

