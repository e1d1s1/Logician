/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

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
#include "stdafx.h"
#include <vector>

class CBimapper
{
public:
	CBimapper(void);
	~CBimapper(void);

	void AddString(size_t id, wstring s);
	size_t AddUserString(wstring s);
	wstring GetStringByID(size_t id);
	size_t GetIDByString(wstring s);
	void ClearUserStrings();

private:
	MAPUINTWSTR m_IndexToStringsMap;
	MAPWSTRUINT m_StringsToIndexMap;
	vector<size_t> userStrings;
	size_t maxID;
};
