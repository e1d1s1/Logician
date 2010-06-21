/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 Eric D. Schmidt

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
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
using namespace EDSUTIL;

CTableSet::CTableSet(void)
{
	bInitialized = false;
}

CTableSet::~CTableSet(void)
{
}

void CTableSet::AddTable(vector<pair<wstring, vector<CRuleCell> > > inputAttrsTests, vector<pair<wstring, vector<CRuleCell> > > outputAttrsValues, vector<wstring> formulaInputs, CBimapper *stringMap, wstring name, bool GetAll)
{
	CRuleTable table (inputAttrsTests, outputAttrsValues, formulaInputs, stringMap, name, GetAll);
	m_tables[table.m_Name] = table;
}

void CTableSet::Initialize()
{
	for (map<wstring, CRuleTable>::iterator it = m_tables.begin(); it != m_tables.end(); it++)
	{
		LoadTableInfo((*it).second);
	}	
	bInitialized = true;
}

void CTableSet::LoadTableInfo(CRuleTable table)
{
	//get the input info for this table
	vector<wstring> inputs = table.GetAllInputAttrNames();	
	if (inputs.size() > 0)
		m_inputAttrsByTable[table.m_Name] = inputs;

	//outputs
	vector<wstring> outputs = table.GetAllOutputAttrNames();	
	if (outputs.size() > 0)
		m_outputAttrsByTable[table.m_Name] = outputs;

	//dependancies
	vector<wstring> inputDeps = table.GetAllInputDependencies();	
	for (vector<wstring>::iterator it = outputs.begin(); it != outputs.end(); it++)
	{
		//check for table chaining
		vector<wstring> values = table.GetAllPossibleOutputs((*it));
		for (vector<wstring>::iterator itValue = values.begin(); itValue != values.end(); itValue++)
		{
			if (StringContains(*itValue, L"eval("))
			{
				wstring cmdArgs((*itValue).begin() + 5, (*itValue).end() - 1);
				vector<wstring> args = Split(cmdArgs, L",");
				if (args.size() > 0)
				{
					vector<wstring> chainInputs = ParseTablesAndChainsForInputs(args[0]); //recursive
					std::copy(chainInputs.begin(), chainInputs.end(), std::back_inserter(inputDeps));
				}
			}
		}
	}
	
	if (inputDeps.size() > 0)
		m_inputDependenciesByTable[table.m_Name] = inputDeps;
	
}

vector<wstring> CTableSet::ParseTablesAndChainsForInputs(wstring tableName)
{
	vector<wstring> retval;

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
					std::copy(chainInputs.begin(), chainInputs.end(), std::back_inserter(retval));
				}
			}
		}
	}

	return retval;
}

CRuleTable* CTableSet::GetTable(std::wstring tableName)
{
	return &m_tables[tableName];
}

vector<wstring> CTableSet::GetInputAttrs(std::wstring tableName)
{
	return m_inputAttrsByTable[tableName];
}

vector<wstring> CTableSet::GetOutputAttrs(std::wstring tableName)
{
	return m_outputAttrsByTable[tableName];
}

vector<wstring> CTableSet::GetInputDependencies(std::wstring tableName)
{
	return m_inputDependenciesByTable[tableName];
}

size_t CTableSet::Count()
{
	return m_tables.size();
}
