/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2014 Eric D. Schmidt, DigiRule Solutions LLC

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

#ifndef UTILITIES
#define UTILITIES

#include <string>
#include <vector>
#include "XMLWrapper.h"


static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char trailingBytesForUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };
static const unsigned long offsetsFromUTF8[6] = {
	0x00000000UL, 0x00003080UL, 0x000E2080UL,
	0x03C82080UL, 0xFA082080UL, 0x82082080UL };

using namespace std;

namespace EDSUTIL
{
	string FindAndReplace (const string& source, const string& target, const string& replacement);
	wstring FindAndReplace (const wstring& source, const wstring& target, const wstring& replacement);
	bool StringContains(const wstring& source, const wstring& target);
	vector<string> Split(const string& text, const string& separators);
	vector<wstring> Split(const wstring& text, const wstring& separators);
	bool StringIsNumeric(const wstring& s);
	wstring TrimString(wstring s);
	string WStrToMBCStr(const wstring& wstr);
	vector<string> ToMBCStringVector(vector<wstring> vectWS);
	string stringify(double x);
	string stringify(long x);
	string stringify(size_t x);
	wstring MBCStrToWStr(const string& mbStr);
	string ToASCIIString(const wstring& s); //for integer and ASCII only
	wstring ToWString(const string& s);
#ifdef USE_LIBXML
	wstring XMLStrToWStr(const xmlChar* mbStr);
	wstring XMLStrToWStr(xmlChar* mbStr);
#endif
}
#endif
