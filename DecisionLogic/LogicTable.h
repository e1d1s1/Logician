/////////////////////////////////////////////////////////////////////////////
// Name:        LogicTable.h
// Purpose:     Manages the data and project related info for each rules table
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2009
// Copyright:   (c) 2009 Eric D. Schmidt
// Licence:     GNU GPLv3
/*
	DecisionLogic is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DecisionLogic is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DecisionLogic.  If not, see <http://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <vector>
#include "DataSet.h"

using namespace std;

class LogicTable
{
public:
	LogicTable(void);
	~LogicTable(void);
	DataSet<wstring>* GetDataSet();
	bool CreateLogicTable(wstring name);
	bool CreateLogicTable(wstring name, vector<wstring> inputs, vector<wstring> outputs);
	bool CreateLogicTable(wstring name, StringTable<wstring> inputTable, StringTable<wstring> outputTable);
	void LoadDataSet(DataSet<wstring> ds, wstring name, wstring path) {m_ds = ds; Name = name; Path = path;}
	void Clear() {m_ds.Clear(); Name.empty(); Path.empty(); bGetAll = false;}
	bool HasData();

	wstring Name;
	wstring Path;
	bool bIsSaved;
	bool bGetAll;

private:
	void BuildDataSet(wstring name);
	DataSet<wstring> m_ds;
};
