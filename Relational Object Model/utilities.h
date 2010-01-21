/*
This file is part of the Relational Object Model 2 Library.
Copyright (C) 2009 Eric D. Schmidt

    Relational Object Model 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Relational Object Model 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Relational Object Model 2.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#ifndef UTILITIES
#define UTILITIES

#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#ifdef _MSC_VER
#include <comdef.h>
#else
#include <time.h>
#endif

using namespace std;
namespace ROMUTIL
{
	string FindAndReplace (const string& source, const string target, const string replacement);
	wstring FindAndReplace (const wstring& source, const wstring target, const wstring replacement);
	bool StringContains(wstring source, wstring target);
	vector<string> Split(string text, string separators);
	vector<wstring> Split(wstring text, wstring separators);
		bool StringIsNumeric(wstring s);
	wstring TrimString(wstring s);
	string ToASCIIString(wstring s);
	vector<string> ToASCIIStringVector(vector<wstring> vectWS);
	vector<wstring> ToWStringVector(vector<string> vStr);
	string stringify(double x);
	string stringify(long x);
	string MakeGUID();
#if USE_MSXML
	wstring ToWString(_variant_t str);
#endif
	wstring MBCStrToWStr(string mbStr);
	string WStrToMBCStr(wstring wstr);
	
}
#endif