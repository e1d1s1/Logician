/////////////////////////////////////////////////////////////////////////////
// Name:        utilities.cpp
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
#include "stdafx.h"
#include "utilities.h"
#include <sstream>
#include <fstream>


using namespace std;

string UTILS::FindAndReplace (const string& source, const string target, const string replacement)
{
	string str = source;
	string::size_type pos = 0,   // where we are now
					found;     // where the found data is

	if (target.size () > 0)   // searching for nothing will cause a loop
	{
		while ((found = str.find (target, pos)) != string::npos)
		{
		  str.replace (found, target.size (), replacement);
		  pos = found + replacement.size ();
		}
	}
	return str;
}

wstring UTILS::FindAndReplace (const wstring& source, const wstring target, const wstring replacement)
{
	wstring str = source;
	wstring::size_type pos = 0,   // where we are now
					found;     // where the found data is

	if (target.size () > 0)   // searching for nothing will cause a loop
	{
		while ((found = str.find (target, pos)) != wstring::npos)
		{
		  str.replace (found, target.size (), replacement);
		  pos = found + replacement.size ();
		}
	}
	return str;
}

bool UTILS::StringContains(wstring source, wstring target)
{
	bool retval = false;

	if (source.find(target, 0) != string::npos)
		retval = true;

	return retval;
}

vector<string> UTILS::Split(string text, string separators)
{
	vector<string> retval;
	int n = text.length();
	int start, stop;
	start = text.find_first_not_of(separators);
	while ((start >= 0) && (start < n))
	{
		stop = text.find_first_of(separators, start);
		if ((stop < 0) || (stop > n)) stop = n;
		retval.push_back(text.substr(start, stop - start));
		start = text.find_first_not_of(separators, stop+1);
	}

	if (retval.size() == 0)
		retval.push_back(text);

	return retval;
}

vector<wstring> UTILS::Split(wstring text, wstring separators)
{
	vector<wstring> retval;
	int n = text.length();
	int start, stop;
	start = text.find_first_not_of(separators);
	while ((start >= 0) && (start < n))
	{
		stop = text.find_first_of(separators, start);
		if ((stop < 0) || (stop > n)) stop = n;
		retval.push_back(text.substr(start, stop - start));
		start = text.find_first_not_of(separators, stop+1);
	}

	if (retval.size() == 0)
		retval.push_back(text);

	return retval;
}

bool UTILS::StringIsNumeric(wstring s)
{
	bool retval = false;
	std::string strBuff(s.begin(), s.end());
	std::istringstream inpStream(strBuff);
	double inpValue = 0.0;
	if (inpStream >> inpValue || s.length() == 0) //let null string go to a 0
	{
		retval = true;
	}

	return retval;
}

wstring UTILS::TrimString(wstring s)
{
  string::size_type pos = s.find_last_not_of(' ');
  if(pos != string::npos) {
    s.erase(pos + 1);
    pos = s.find_first_not_of(' ');
    if(pos != string::npos) s.erase(0, pos);
  }
  else s.erase(s.begin(), s.end());

  return s;
}

string UTILS::ToASCIIString(wstring s)
{
	string retval(s.begin(), s.end());
	return retval;
}

vector<string> UTILS::ToASCIIStringVector(vector<wstring> vectWS)
{
	vector<string> retval;
	for (vector<wstring>::iterator it = vectWS.begin(); it != vectWS.end(); it++)
	{
		retval.push_back(UTILS::ToASCIIString(*it));
	}
	return retval;
}

string UTILS::stringify(double x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(double)");
   return o.str();
}

string UTILS::stringify(long x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(long)");
   return o.str();
}

string UTILS::stringify(size_t x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(size_t)");
   return o.str();
}

//this is only for integer and ascii conversion
wstring UTILS::ToWString(string s)
{
	wstring retval;
	retval.assign(s.begin(), s.end());
	return retval;
}

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
string UTILS::WStrToMBCStr(wstring wstr)
{
	string retval ="";
	for (size_t i = 0; i < wstr.length(); ++i)
	{
		unsigned short bytesToWrite;
		wchar_t ch = wstr[i];

		if (ch < 0x80) bytesToWrite = 1;
		else if (ch < 0x800) bytesToWrite = 2;
		else if (ch < 0x10000) bytesToWrite = 3;
		else if (ch < 0x110000) bytesToWrite = 4;
		else bytesToWrite = 3, ch = 0xFFFD; // replacement character

		char buf[4];
		char* target = &buf[bytesToWrite];
		switch (bytesToWrite)
		{
		case 4: *--target = ((ch | 0x80) & 0xBF); ch >>= 6;
		case 3: *--target = ((ch | 0x80) & 0xBF); ch >>= 6;
		case 2: *--target = ((ch | 0x80) & 0xBF); ch >>= 6;
		case 1: *--target = (char)(ch | firstByteMark[bytesToWrite]);
		}
		retval += std::string(buf, bytesToWrite);
	}
	return retval;
}

wstring UTILS::XMLStrToWStr(xmlChar* mbStr)
{
	if (mbStr == NULL)
		return L"";

	std::wstring result;
	const xmlChar* source = mbStr;
	const xmlChar* sourceEnd = mbStr + strlen((const char*)mbStr);
	while (source < sourceEnd)
	{
		unsigned long ch = 0;
		int extraBytesToRead = trailingBytesForUTF8[*source];
		assert(source + extraBytesToRead < sourceEnd);
		switch (extraBytesToRead)
		{
		case 5: ch += *source++; ch <<= 6;
		case 4: ch += *source++; ch <<= 6;
		case 3: ch += *source++; ch <<= 6;
		case 2: ch += *source++; ch <<= 6;
		case 1: ch += *source++; ch <<= 6;
		case 0: ch += *source++;
		}
		ch -= offsetsFromUTF8[extraBytesToRead];
		// Make sure it fits in a 16-bit wchar_t
		if (ch > 0xFFFF)
			ch = 0xFFFD;

		result += (wchar_t)ch;
	}
	return result;

}

bool UTILS::FileExists(const std::wstring& fileName)
{
  std::wfstream fin;
  #ifdef WIN32
  fin.open(fileName.c_str(),std::ios::in);
  #else
  fin.open(WStrToMBCStr(fileName).c_str(),std::ios::in);
  #endif
  if( fin.is_open() )
  {
    fin.close();
    return true;
  }
  fin.close();
  return false;
}

