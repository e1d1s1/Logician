/////////////////////////////////////////////////////////////////////////////
// Name:        utilities.cpp
// Purpose:     Bag of tricks, useful routines
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2010 Eric D. Schmidt
// Licence:     GNU GPLv3
/*
	DecisionLogic is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
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

string UTILS::WStrToMBCStr(wstring wstr)
{
	const int MAX_SIZE = 4*wstr.length() + 1; //should handle UTF-8 largest char????
	char *mbcstr = new char[MAX_SIZE];
	int finalSize = wcstombs(mbcstr, wstr.c_str(), MAX_SIZE);
	string retval = mbcstr;
	delete [] mbcstr;
	return retval;
}

wstring UTILS::MBCStrToWStr(xmlChar* mbStr)
{
	if (mbStr == NULL)
		return L"";

	int requiredSize = mbstowcs(NULL, (const char*)mbStr, 0) + 1;
	wchar_t *wStr = new wchar_t[requiredSize];
	mbstowcs(wStr, (const char*)mbStr, requiredSize);
	wstring retval = wStr;
	delete [] wStr;
	xmlFree(mbStr);
	return retval;
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

