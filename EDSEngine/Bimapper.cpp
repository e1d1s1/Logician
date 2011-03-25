/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 - 2011 Eric D. Schmidt

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
#include "Bimapper.h"

CBimapper::CBimapper(void)
{
	maxID = 0;
}

CBimapper::~CBimapper(void)
{
}

//strings added through this method remained cached through the application life
void CBimapper::AddString(size_t id, wstring s)
{
	//dont assume the ids are lined up
	if (id > maxID)
		maxID = id;
	m_IndexToStringsMap[id] = s;
	m_StringsToIndexMap[s] = id;
}

size_t CBimapper::AddUserString(wstring s)
{
	size_t retval = maxID + 1;
	m_IndexToStringsMap[retval] = s;
	m_StringsToIndexMap[s] = retval;
	userStrings.push_back(retval);
	maxID = retval;
	return retval;
}

wstring CBimapper::GetStringByID(size_t id)
{
	wstring retval;
#ifdef _MSC_VER
	stdext::hash_map<size_t, wstring>::iterator it = m_IndexToStringsMap.find(id);
#else
	__gnu_cxx::hash_map<size_t, wstring>::iterator it = m_IndexToStringsMap.find(id);
#endif
	if (it != m_IndexToStringsMap.end())
	{
		retval = (*it).second;
	}
	return retval;
}

size_t CBimapper::GetIDByString(std::wstring s)
{
	size_t retval = INVALID_STRING;
#ifdef _MSC_VER
	stdext::hash_map<wstring, size_t>::iterator it = m_StringsToIndexMap.find(s);
#else
	__gnu_cxx::hash_map<wstring, size_t>::iterator it = m_StringsToIndexMap.find(s);
#endif

	if (s.length() == 0)
	{
		retval = EMPTY_STRING;
	}
	else if (it != m_StringsToIndexMap.end())
	{
		retval = (*it).second;
	} 

	return retval;
}

void CBimapper::ClearUserStrings()
{
	for (vector<size_t>::iterator it = userStrings.begin(); it != userStrings.end(); it++)
	{
		size_t id = *it;
		wstring s = GetStringByID(id);

		hash_map<wstring, size_t>::iterator itString = m_StringsToIndexMap.find(s);
		if (itString != m_StringsToIndexMap.end())
			m_StringsToIndexMap.erase(itString);

		hash_map<size_t, wstring>::iterator itID = m_IndexToStringsMap.find(id);
		if (itID != m_IndexToStringsMap.end())
			m_IndexToStringsMap.erase(itID);
	}

	userStrings.clear();
}
