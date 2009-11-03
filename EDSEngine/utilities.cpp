/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 Eric D. Schmidt

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "utilities.h"
#include <sstream>

using namespace std;

string EDSUTIL::FindAndReplace (const string& source, const string target, const string replacement)
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

wstring EDSUTIL::FindAndReplace (const wstring& source, const wstring target, const wstring replacement)
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

bool EDSUTIL::StringContains(wstring source, wstring target)
{
	bool retval = false;

	if (source.find(target, 0) != string::npos)
		retval = true;

	return retval;
}

vector<string> EDSUTIL::Split(string text, string separators)
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

vector<wstring> EDSUTIL::Split(wstring text, wstring separators)
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

bool EDSUTIL::StringIsNumeric(wstring s)
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

wstring EDSUTIL::TrimString(wstring s)
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

string EDSUTIL::ToASCIIString(wstring s)
{
	string retval(s.begin(), s.end());
	return retval;
}

vector<string> EDSUTIL::ToASCIIStringVector(vector<wstring> vectWS)
{
	vector<string> retval;
	for (vector<wstring>::iterator it = vectWS.begin(); it != vectWS.end(); it++)
	{
		retval.push_back(EDSUTIL::ToASCIIString(*it));
	}
	return retval;
}

string EDSUTIL::stringify(double x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(double)");
   return o.str();
}

string EDSUTIL::stringify(size_t x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(size_t)");
   return o.str();
}

string EDSUTIL::stringify(long x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(long)");
   return o.str();
}

//this is only for integer and ascii conversion
wstring EDSUTIL::ToWString(string s)
{
	wstring retval;
	retval.assign(s.begin(), s.end());
	return retval;
}
