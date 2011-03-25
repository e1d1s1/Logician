/////////////////////////////////////////////////////////////////////////////
// Name:        DataSet.h
// Purpose:     DataSet class implementation.  Manages a collection of tables.
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2011 Eric D. Schmidt
// Licence:     GNU GPLv2
/*
	DecisionLogic is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
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
#include <map>
#include "StringTable.h"

using namespace std;

template <class T>
class DataSet
{
public:
	DataSet(void) {}
	~DataSet(void) {}

	inline void Clear()
	{
		m_tables.clear();
	}

	inline void AddTable(StringTable<T> table)
	{
		m_tables[table.TableName] = table;
	}

	inline size_t TableCount()
	{
		return m_tables.size();
	}

	inline StringTable<T>* GetTable(wstring name)
	{
		return &m_tables[name];
	}

	inline StringTable<T> GetTableCopy(wstring name)
	{
		return m_tables[name];
	}

	wstring DataSetName;

private:
	map< wstring, StringTable<T> > m_tables;
};
