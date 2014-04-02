/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2014 Eric D. Schmidt, DigiRule Solutions LLC

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
#pragma once

#include <string>
#include <map>
#include "RuleTable.h"

using namespace std;

//the collection of all active rules tables + some useful functions
class CTableSet
{

public:
	CTableSet(void);
	~CTableSet(void);

	void AddTable(vector<pair<wstring, vector<CRuleCell> > > inputAttrsTests,
		vector<pair<wstring, vector<CRuleCell> > > outputAttrsValues,
		vector<wstring> formulaInputs, CBimapper *stringMap, wstring name = L"defualt", bool GetAll = false);
	void Initialize();
	CRuleTable* GetTable(const wstring& tableName);
	vector<wstring> GetInputAttrs(const wstring& tableName);
	vector<wstring> GetInputDependencies(const wstring& tableName);
	vector<wstring> GetOutputAttrs(const wstring& tableName);
	size_t Count();
	void LoadTableInfo(CRuleTable *table);
private:
	vector<wstring> ParseTablesAndChainsForInputs(const wstring& tableName);
	map<wstring, CRuleTable> m_tables;
	map<wstring, vector<wstring> > m_inputAttrsByTable;
	map<wstring, vector<wstring> > m_inputDependenciesByTable;
	map<wstring, vector<wstring> > m_outputAttrsByTable;
	bool bInitialized;
};
