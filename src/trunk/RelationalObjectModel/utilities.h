/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

    Relational Object Model is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Relational Object Model is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Relational Object Model.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#ifndef UTILITIES
#define UTILITIES

#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#ifndef _MSC_VER
#include <time.h>
#endif

using namespace std;
namespace ROMUTIL
{
	string FindAndReplace (const string& source, const string target, const string replacement);
	wstring FindAndReplace (const wstring& source, const wstring target, const wstring replacement);
	bool StringContains(wstring source, wstring target);
	bool StringBeginsWith(wstring source, wstring target);
	vector<string> Split(string text, string separators);
	vector<wstring> Split(wstring text, wstring separators);
	bool StringIsNumeric(wstring s);
	wstring TrimString(wstring s);
	string ToASCIIString(wstring s);
	vector<string> WStrToMBCStrVector(vector<wstring> vectWS);
	map<string, vector<string> > WStrToMBCStrMapVector(map<wstring, vector<wstring> >);
	vector<string> ToASCIIStringVector(vector<wstring> vectWS);
	vector<wstring> ToWStringVector(vector<string> vStr);
	string stringify(double x);
	string stringify(long x);
	string MakeGUID();
	wstring encodeForXml(const std::wstring &sSrc);
#if USE_MSXML
	wstring BSTR_T_ToWString(void* bstr); //void* to avoid reference issues when headers included in WinRT project
#endif
	wstring MBCStrToWStr(string mbStr);
	string WStrToMBCStr(wstring wstr);
#if USE_LIBXML
	wstring XMLStrToWStr(const unsigned char* mbStr);
	wstring XMLStrToWStr(unsigned char* mbStr);
#endif
	
}
#endif