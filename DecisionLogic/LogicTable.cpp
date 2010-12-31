/////////////////////////////////////////////////////////////////////////////
// Name:        LogicTable.cpp
// Purpose:     Manages the data and project related info for each rules table
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2011 Eric D. Schmidt
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
#include "stdafx.h"
#include "LogicTable.h"
#include "utilities.h"

const wstring INPUT_TABLE = L"Inputs";
const wstring OUTPUT_TABLE = L"Outputs";
const wstring STATUS_TABLE = L"Status";
const wstring VALUE_NAME = L"Value";
const wstring ATTR_NAME = L"Attr";
const wstring INPUT_COL = L"Input";
const wstring OUTPUT_COL = L"Output";
const wstring RULE_STATUS = L"RuleStatus";
const string NAMESPACE = "http://filer.cwru.edu/~eds6/DecisionLogic";
const int DEFAULT_COL_CNT = 10;
const int DEFAULT_ROW_CNT = 4;

LogicTable::LogicTable(void)
{
	bIsSaved = true;
}

LogicTable::~LogicTable(void)
{
}

DataSet<wstring>* LogicTable::GetDataSet()
{
	return &m_ds;
}

bool LogicTable::CreateLogicTable(wstring name)
{
	bool retval = false;
	try
	{
		BuildDataSet(name);
	}
	catch(...)
	{
		ReportError("CreateLogicTable");
	}
	return retval;
}

bool LogicTable::CreateLogicTable(wstring name, vector<wstring> inputs, vector<wstring> outputs)
{
	bool retval = false;
	try
	{
		BuildDataSet(name);

		if (m_ds.TableCount() > 0)
		{
			//input table, inputs listed along left col downward
			StringTable<wstring>* inputTable = m_ds.GetTable(INPUT_TABLE);
			inputTable->AddColumn(INPUT_COL);
			for (vector<wstring>::iterator it = inputs.begin(); it != inputs.end(); it++)
			{
				vector<wstring> newRow = inputTable->NewRow();
				newRow[0] = *it;
				inputTable->AddRow(newRow);
			}

			//output table, outputs listed along left col downward
			StringTable<wstring>* outputTable = m_ds.GetTable(OUTPUT_TABLE);
			outputTable->AddColumn(OUTPUT_COL);
			for (vector<wstring>::iterator it = outputs.begin(); it != outputs.end(); it++)
			{
				vector<wstring> newRow = outputTable->NewRow();
				newRow[0] = *it;
				outputTable->AddRow(newRow);
			}
		}
	}
	catch(...)
	{
		ReportError("CreateLogicTable");
		m_ds.Clear();
	}
	return retval;
}

bool LogicTable::CreateLogicTable(wstring name, StringTable<wstring> inputTable, StringTable<wstring> outputTable)
{
	bool retval = false;
	try
	{
		m_ds.Clear();
		m_ds.DataSetName = name;
		m_ds.AddTable(inputTable);
		m_ds.AddTable(outputTable);
		Name = name;
		retval = true;
	}
	catch(...)
	{
		ReportError("CreateLogicTable");
	}
	return retval;
}

void LogicTable::BuildDataSet(wstring name)
{
	int start_col_cnt = DEFAULT_COL_CNT;
    int start_row_cnt = DEFAULT_ROW_CNT;

    Name = name;

    StringTable<wstring> dtInputs(INPUT_TABLE);
    dtInputs.AddColumn(ATTR_NAME);
    for (int i = 1; i <= start_col_cnt; i++)
	{
		dtInputs.AddColumn(VALUE_NAME);
	}
    for (int j = 0; j < start_row_cnt; j++)
	{
		vector<wstring> newRow;
        dtInputs.AddRow(dtInputs.NewRow());
	}

    StringTable<wstring> dtOutputs(OUTPUT_TABLE);
    dtOutputs.AddColumn(ATTR_NAME);
    for (int i = 1; i <= start_col_cnt; i++)
	{
        dtOutputs.AddColumn(VALUE_NAME);
	}
    for (int j = 0; j < start_row_cnt/2; j++)
        dtOutputs.AddRow(dtOutputs.NewRow());

	StringTable<wstring> dtStatus(STATUS_TABLE);    
    for (int i = 0; i < start_col_cnt; i++)
	{
        dtStatus.AddColumn(RULE_STATUS);		
	}
	dtStatus.AddRow(dtStatus.NewRow());
	for (int i = 0; i < start_col_cnt; i++)
	{
		dtStatus.SetItem(0, i, L"Enabled");
	}

    if (name.length() > 0)
    {
		m_ds.Clear();
		m_ds.DataSetName = name;
        m_ds.AddTable(dtInputs);
        m_ds.AddTable(dtOutputs);
		m_ds.AddTable(dtStatus);
    }
}

bool LogicTable::HasData()
{
	if (m_ds.TableCount() > 0)
		return true;
	else
		return false;
}