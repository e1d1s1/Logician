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
#include "Bimapper.h"

CBimapper::CBimapper(void)
{
	maxID = 0;
}

CBimapper::~CBimapper(void)
{
}

void CBimapper::AddString(size_t id, const wstring& s)
{
	//dont assume the ids are lined up
	if (id > maxID)
		maxID = id;
	m_IndexToStringsMap[id] = s;
	m_StringsToIndexMap[s] = id;
}

wstring CBimapper::GetStringByID(size_t id)
{
	wstring retval;

	auto it = m_IndexToStringsMap.find(id);

	if (it != m_IndexToStringsMap.end())
	{
		retval = (*it).second;
	}
	return retval;
}

size_t CBimapper::GetIDByString(const wstring& s)
{
	size_t retval = INVALID_STRING;

	if (s.length() == 0)
	{
		return EMPTY_STRING;
	}

	auto it = m_StringsToIndexMap.find(s);
	if (it != m_StringsToIndexMap.end())
	{
		retval = (*it).second;
	} 

	return retval;
}