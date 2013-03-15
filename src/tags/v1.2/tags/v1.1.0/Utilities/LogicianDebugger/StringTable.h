#ifndef _WX_STRINGTABLE_H_BASE_
#define _WX_STRINGTABLE_H_BASE_

#include <vector>
#include <string>
using namespace std;

template <class T>
class StringTable
{
public:
	StringTable() {}
	StringTable(T name) {TableName = name;}
	~StringTable() {}
	
    T TableName;

	template <class T>
	void Clear()
	{
		m_table.clear();
		m_colNames.clear();
		TableName.clear();
	}

	template <class T>
	void AddColumn(T name)
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
	vector<T> GetColumns()
	{
		return m_colNames;
	}

	template <class T>
	void InsertColumn(T name, size_t col)
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
	void RemoveColumn(T name)
	{
		size_t pos = find(m_colNames.begin(), m_colNames.end(), name) - m_colNames.begin();
		RemoveColumn(pos);
	}

	template <class T>
	void RemoveColumn(size_t col)
	{
		typename vector< vector<T> >::iterator row_iterator;
		m_colNames.erase(m_colNames.begin() + col);
		for (row_iterator = m_table.begin(); row_iterator != m_table.end(); row_iterator++)
		{
			(*row_iterator).erase((*row_iterator).begin() + col);
		}
	}

	template <class T>
	void AddRow(vector<T> newRow)
	{
		if (newRow.size() == m_colNames.size())
		{
			m_table.push_back(newRow);
		}
	}

	template <class T>
	void DeleteRow(size_t index)
	{
		if (index < m_table.size())
			m_table.erase(m_table.begin() + index);
	}

	template <class T>
	void AddRow()
	{
		vector<T> newRow (m_colNames.size());
		m_table.push_back(newRow);
	}

	template <class T>
	vector<T> NewRow()
	{
		vector<T> retval (Columns());
		return retval;
	}

	template <class T>
	void InsertRow(vector<T> newRow, size_t row)
	{
		if (newRow.size() == m_colNames.size())
		{
			m_table.insert(m_table.begin() + row, newRow);
		}
	}

	template <class T>
	size_t Rows()
	{
		return m_table.size();
	}

	template <class T>
	size_t Columns()
	{
		return m_colNames.size();
	}

	template <class T>
	vector<T> GetRow(size_t row)
	{
		return m_table.at(row);
	}

	template <class T>
	T GetItem(size_t row, size_t col)
	{
		return m_table.at(row).at(col);
	}

	template <class T>
	void SetItem(size_t row, size_t col, T value)
	{
		m_table.at(row).at(col) = value;
	}

	template <class T>
	T GetItem(size_t row, T colName)
	{
		size_t pos = std::find(m_colNames.begin(), m_colNames.end(), colName) - m_colNames.begin();
		return m_table.at(row).at(pos);
	}

	template <class T>
	void SetItem(size_t row, T colName, T value)
	{
		size_t pos = std::find(m_colNames.begin(), m_colNames.end(), colName) - m_colNames.begin();
		m_table.at(row).at(pos) = value;
	}

	template <class T>
	void SortByCol(size_t colIndex)
	{
		std::sort(m_table.begin(), m_table.end(), SortByColumn<T>(colIndex));
	}

	template <class T>
	void SortByCol(T colName)
	{
		size_t pos = std::find(m_colNames.begin(), m_colNames.end(), colName) - m_colNames.begin();
		this->SortByCol(pos);
	}

private:
	vector< vector<T> > m_table;
	vector<T> m_colNames;

};



#endif