/////////////////////////////////////////////////////////////////////////////
// Name:        StringTable.h
// Purpose:     A minimal handler for table/grid based text data
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2012 Eric D. Schmidt, DigiRule Solutions LLC
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

#include <vector>
#include <string>
#include <algorithm>


using namespace std;

template <class T>
class SortByColumn
{
public:
	SortByColumn(int colIndex) {m_colIndex = colIndex;}
	int operator() (const vector<T> lhs, const vector<T> rhs)
	{
		return lhs.at(m_colIndex) < rhs.at(m_colIndex);
	}
private:
	size_t m_colIndex;
};

template <class T>
class StringTable
{
public:
	StringTable() {}
	StringTable(T name) {TableName = name;}
	~StringTable() {}

	void Clear();
	void AddColumn(T name);
	vector<T> GetColumns();
	void InsertColumn(T name, size_t col);
	void RemoveColumn(T name);
	void RemoveColumn(size_t col);
	void AddRow(vector<T> newRow);
	void AddRow();
	void DeleteRow(size_t index);
	vector<T> NewRow();
	void InsertRow(vector<T> newRow, size_t row);
	size_t Rows();
	size_t Columns();
	vector<T> GetRow(size_t row);
	T GetItem(size_t row, size_t col);
	T GetItem(size_t row, T colName);
	void SetItem(size_t row, T colName, T value);
	void SetItem(size_t row, size_t col, T value);
	void SortByCol(T colName);
	void SortByCol(size_t colIndex);

    T TableName;
private:
	vector< vector<T> > m_table;
	vector<T> m_colNames;
};

template <class T>
void StringTable<T>::Clear()
{
    m_table.clear();
    m_colNames.clear();
    TableName.clear();
}

template <class T>
void StringTable<T>::AddColumn(T name)
{
    typename vector< vector<T> >::iterator row_iterator;
    m_colNames.push_back(name);
    for (row_iterator = m_table.begin(); row_iterator != m_table.end(); row_iterator++)
    {
        vector<T> vect = *row_iterator;
        T val;
        val.empty();
        vect.push_back(val);
    }
}

template <class T>
vector<T> StringTable<T>::GetColumns()
{
    return m_colNames;
}

template <class T>
void StringTable<T>::InsertColumn(T name, size_t col)
{
    typename vector< vector<T> >::iterator row_iterator;
    m_colNames.insert(m_colNames.begin() + col, name);
    for (row_iterator = m_table.begin(); row_iterator != m_table.end(); row_iterator++)
    {
        T val;
        (*row_iterator).insert((*row_iterator).begin() + col, val);
    }
}

template <class T>
void StringTable<T>::RemoveColumn(T name)
{
    size_t pos = find(m_colNames.begin(), m_colNames.end(), name) - m_colNames.begin();
    RemoveColumn(pos);
}

template <class T>
void StringTable<T>::RemoveColumn(size_t col)
{
    typename vector< vector<T> >::iterator row_iterator;
    m_colNames.erase(m_colNames.begin() + col);
    for (row_iterator = m_table.begin(); row_iterator != m_table.end(); row_iterator++)
    {
        (*row_iterator).erase((*row_iterator).begin() + col);
    }
}

template <class T>
void StringTable<T>::AddRow(vector<T> newRow)
{
    if (newRow.size() == m_colNames.size())
    {
        m_table.push_back(newRow);
    }
}

template <class T>
void StringTable<T>::DeleteRow(size_t index)
{
    if (index < m_table.size())
        m_table.erase(m_table.begin() + index);
}

template <class T>
void StringTable<T>::AddRow()
{
    vector<T> newRow (m_colNames.size());
    m_table.push_back(newRow);
}

template <class T>
vector<T> StringTable<T>::NewRow()
{
    vector<T> retval (Columns());
    return retval;
}

template <class T>
void StringTable<T>::InsertRow(vector<T> newRow, size_t row)
{
    if (newRow.size() == m_colNames.size())
    {
        m_table.insert(m_table.begin() + row, newRow);
    }
}

template <class T>
size_t StringTable<T>::Rows()
{
    return m_table.size();
}

template <class T>
size_t StringTable<T>::Columns()
{
    return m_colNames.size();
}

template <class T>
vector<T> StringTable<T>::GetRow(size_t row)
{
    return m_table.at(row);
}

template <class T>
T StringTable<T>::GetItem(size_t row, size_t col)
{
    return m_table.at(row).at(col);
}

template <class T>
void StringTable<T>::SetItem(size_t row, size_t col, T value)
{
    m_table.at(row).at(col) = value;
}

template <class T>
T StringTable<T>::GetItem(size_t row, T colName)
{
    size_t pos = std::find(m_colNames.begin(), m_colNames.end(), colName) - m_colNames.begin();
    return m_table.at(row).at(pos);
}

template <class T>
void StringTable<T>::SetItem(size_t row, T colName, T value)
{
    size_t pos = std::find(m_colNames.begin(), m_colNames.end(), colName) - m_colNames.begin();
    m_table.at(row).at(pos) = value;
}

template <class T>
void StringTable<T>::SortByCol(size_t colIndex)
{
	std::sort(m_table.begin(), m_table.end(), SortByColumn<T>(colIndex));
}

template <class T>
void StringTable<T>::SortByCol(T colName)
{
	size_t pos = std::find(m_colNames.begin(), m_colNames.end(), colName) - m_colNames.begin();
	this->SortByCol(pos);
}
