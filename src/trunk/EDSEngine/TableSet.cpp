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
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "TableSet.h"
#include "RuleTable.h"
#include "utilities.h"
#include <algorithm>
using namespace EDSUTIL;

CTableSet::CTableSet(void)
{
	bInitialized = false;
	m_threads = 1;
}

CTableSet::~CTableSet(void)
{
}

void CTableSet::AddTable(vector<pair<wstring, vector<CRuleCell> > > inputAttrsTests, vector<pair<wstring, vector<CRuleCell> > > outputAttrsValues, vector<wstring> formulaInputs, CBimapper *stringMap, wstring name, bool GetAll)
{
	CRuleTable table (inputAttrsTests, outputAttrsValues, formulaInputs, stringMap, name, GetAll);
	m_tables[table.m_Name] = table;
}

void CTableSet::Initialize(size_t threads)
{
	m_threads = 1;
	for (map<wstring, CRuleTable>::iterator it = m_tables.begin(); it != m_tables.end(); it++)
	{
		LoadTableInfo(&(*it).second);
	}
	bInitialized = true;
}

void CTableSet::LoadTableInfo(CRuleTable *table)
{
	table->SetThreadCount(m_threads);

	//get the input info for this table
	vector<wstring> inputs = table->GetAllInputAttrNames();
	if (inputs.size() > 0)
		m_inputAttrsByTable[table->m_Name] = inputs;

	//outputs
	vector<wstring> outputs = table->GetAllOutputAttrNames();
	if (outputs.size() > 0)
		m_outputAttrsByTable[table->m_Name] = outputs;

	//dependancies
	vector<wstring> inputDeps = table->GetAllInputDependencies();
	for (vector<wstring>::iterator it = outputs.begin(); it != outputs.end(); it++)
	{
		//check for table chaining
		vector<wstring> values = table->GetAllPossibleOutputs((*it));
		for (vector<wstring>::iterator itValue = values.begin(); itValue != values.end(); itValue++)
		{
			if (StringContains(*itValue, L"eval("))
			{
				wstring cmdArgs((*itValue).begin() + 5, (*itValue).end() - 1);
				vector<wstring> args = Split(cmdArgs, L",");
				if (args.size() > 0)
				{
					vector<wstring> chainInputs = ParseTablesAndChainsForInputs(args[0]); //recursive
					for (vector<wstring>::iterator itChain = chainInputs.begin(); itChain != chainInputs.end(); itChain++)
					{
						if (find(inputDeps.begin(), inputDeps.end(), *itChain) == inputDeps.end())
							inputDeps.push_back(*itChain);
					}
				}
			}
		}
	}

	if (inputDeps.size() > 0)
		m_inputDependenciesByTable[table->m_Name] = inputDeps;

}

vector<wstring> CTableSet::ParseTablesAndChainsForInputs(const wstring& tableName)
{
	vector<wstring> retval;

	if (m_tables.find(tableName) == m_tables.end())
		return retval;

	CRuleTable table = m_tables[tableName];
	retval = table.GetAllInputDependencies();
	vector<wstring> outputs = table.GetAllOutputAttrNames();

	for (vector<wstring>::iterator it = outputs.begin(); it != outputs.end(); it++)
	{
		vector<wstring> values = table.GetAllPossibleOutputs((*it));
		for (vector<wstring>::iterator itValue = values.begin(); itValue != values.end(); itValue++)
		{
			if (StringContains(*itValue, L"eval("))
			{
				wstring cmdArgs((*itValue).begin() + 5, (*itValue).end() - 1);
				vector<wstring> args = Split(cmdArgs, L",");
				if (args.size() > 0)
				{
					vector<wstring> chainInputs = ParseTablesAndChainsForInputs(args[0]);
					for (vector<wstring>::iterator itChain = chainInputs.begin(); itChain != chainInputs.end(); itChain++)
					{
						if (find(retval.begin(), retval.end(), *itChain) == retval.end())
							retval.push_back(*itChain);
					}
				}
			}
		}
	}

	return retval;
}

CRuleTable* CTableSet::GetTable(const wstring& tableName)
{
	if (m_tables.find(tableName) != m_tables.end())
		return &m_tables[tableName];
	else
		return nullptr;
}

vector<wstring> CTableSet::GetInputAttrs(const wstring& tableName)
{
	if (m_inputAttrsByTable.find(tableName) != m_inputAttrsByTable.end())
		return m_inputAttrsByTable[tableName];
	else
		return vector<wstring>();
}

vector<wstring> CTableSet::GetOutputAttrs(const wstring&tableName)
{
	if (m_outputAttrsByTable.find(tableName) != m_outputAttrsByTable.end())
		return m_outputAttrsByTable[tableName];
	else
		return vector<wstring>();
}

vector<wstring> CTableSet::GetInputDependencies(const wstring&tableName)
{
	if (m_outputAttrsByTable.find(tableName) != m_outputAttrsByTable.end())
		return m_inputDependenciesByTable[tableName];
	else
		return vector<wstring>();
}

size_t CTableSet::Count()
{
	return m_tables.size();
}
