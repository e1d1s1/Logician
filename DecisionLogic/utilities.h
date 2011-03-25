/////////////////////////////////////////////////////////////////////////////
// Name:        utilities.h
// Purpose:     Bag of tricks, useful routines
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

#ifndef UTILITIES
#define UTILITIES

#include <string>
#include <vector>
#include <libxml/xpath.h>

using namespace std;

namespace UTILS
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
	string stringify(double x);
	string stringify(long x);
	string stringify(size_t x);
	wstring ToWString(string s);
	string WStrToMBCStr(wstring wstr);
	wstring XMLStrToWStr(xmlChar* mbStr);
	bool FileExists(const std::wstring& fileName);
}
#endif
