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
	_init();
}

CRuleTable::~CRuleTable(void)
{

}

void CRuleTable::_init()
{
	m_DEBUGGING = false;
	m_Threads = 1;
	m_Tests = 0;
	bHasChain = false;
	bHasPython = false;
	bHasJavascript = false;
	bGetAll = false;
	m_ThreadingEnabled = false;
}

void CRuleTable::CreateRuleTable(vector<pair<wstring, vector<CRuleCell> > >& inputAttrsTests,
								vector<pair<wstring, vector<CRuleCell> > >& outputAttrsValues,
								vector<wstring>& formulaInputs, CBimapper *stringMap,
								const wstring& name, bool GetAll)
{
	_init();

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

map<wstring, vector<wstring> > CRuleTable::EvaluateTable(bool bGetAll, bool bForward, void* context)
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
		vector<wstring> result = EvaluateTable((*itOut).first, bGetAll, bForward, context);
		retval[(*itOut).first] = result;
	}

	return retval;
}

vector<wstring> CRuleTable::EvaluateTable(const wstring& outputAttr, bool bGetAll, bool bForward, void* context)
{
	vector<wstring> retval;
	vector<CToken> values;
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

	vector<bool> colResults (m_Tests, true); //a table need not have any inputs
	if (inputCollection->size() > 0)
	{
		//get the current values of all input attrs
		for (vector<pair<wstring, vector<CRuleCell> > >::iterator it = inputCollection->begin(); it != inputCollection->end(); it++)
		{
			wstring attrName = it->first;
			CToken token;
			token.Value = InputValueGetter(attrName, context);
			token.ID = m_stringsMap->GetIDByString(token.Value);
			values.push_back(token);
		}

		//sweep down the table for all inputs and do test(s)
		colResults = _runTests(bGetAll, inputCollection, values, context);

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
			CDecode decoder(outputCell, InputValueGetter, m_stringsMap, context);
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

vector<bool> CRuleTable::_runTests(bool bGetAll, vector<pair<wstring, vector<CRuleCell> > >* inputCollection, vector<CToken>& values, void* context)
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
                return _runTestGroup(bGetAll, startIndex, endIndex, inputCollection, values, colResults, context);
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
        _runTestGroup(bGetAll, 0, m_Tests, inputCollection, values, colResults, context);
    }

    return colResults;
}

bool CRuleTable::_runTestGroup(bool bGetAll, size_t startIndex, size_t endIndex, vector<pair<wstring, vector<CRuleCell>>>* inputCollection, vector<CToken>& values, vector<bool>& colResults, void* context)
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
				CDecode decoder(values[inputCnt], (*itTests).second[testIndex], InputValueGetter, m_stringsMap, context);
                bHaveSolution = decoder.EvaluateInputCell();
            }
            inputCnt++;
            if (!bHaveSolution)
                break;
        }
        colResults[testIndex] = bHaveSolution;
        if (bHaveSolution && !bGetAll)
            break;
    } //done column (rule)
    return bHaveSolution;
}

void CRuleTable::DebugEval(const wstring& outputAttr, const vector<CToken>& inputValues, const map<size_t, set<wstring>>& solutions)
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
			xmlBlob += L"<Input name = \"";
			xmlBlob += attr;
			xmlBlob += L"\" value=\"";
			xmlBlob += inputValues[i].Value;
			xmlBlob += L"\"/>";
		}
		xmlBlob += L"</Inputs>";
	} //else something is wrong

	xmlBlob+= L"<Outputs>";
	for (auto it = solutions.begin(); it != solutions.end(); it++)
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

vector<wstring> CRuleTable::GetAllPossibleOutputs(const wstring& outputName)
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

