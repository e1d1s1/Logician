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
#include "stdafx.h"
#include "utilities.h"
#include <sstream>
#if defined(POSIX)
#include <uuid/uuid.h>
#else
	#if defined(WIN32)
	#include "Objbase.h"
	#else
	#include <cstdlib>
	#endif	
#endif

using namespace std;

string ROMUTIL::FindAndReplace (const string& source, const string target, const string replacement)
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

wstring ROMUTIL::FindAndReplace (const wstring& source, const wstring target, const wstring replacement)
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

bool ROMUTIL::StringContains(wstring source, wstring target)
{
	bool retval = false;

	if (source.find(target, 0) != string::npos)
		retval = true;

	return retval;
}

vector<string> ROMUTIL::Split(string text, string separators)
{
	vector<string> retval;
	size_t n = text.length();
	size_t start, stop;
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

vector<wstring> ROMUTIL::Split(wstring text, wstring separators)
{
	vector<wstring> retval;
	size_t n = text.length();
	size_t start, stop;
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

bool ROMUTIL::StringIsNumeric(wstring s)
{
	bool retval = false;
	std::wistringstream inpStream(s);
	double inpValue = 0.0;
	if (inpStream >> inpValue || s.length() == 0) //let null string go to a 0
	{
		retval = true;
	}

	return retval;
}

wstring ROMUTIL::TrimString(wstring s)
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

string ROMUTIL::ToASCIIString(wstring s)
{
	string retval(s.begin(), s.end());
	return retval;
}

vector<string> ROMUTIL::ToASCIIStringVector(vector<wstring> vectWS)
{
	vector<string> retval;
	for (vector<wstring>::iterator it = vectWS.begin(); it != vectWS.end(); it++)
	{
		retval.push_back(ROMUTIL::ToASCIIString(*it));
	}
	return retval;
}

vector<string> ROMUTIL::WStrToMBCStrVector(vector<wstring> vectWS)
{
	vector<string> retval;
	for (vector<wstring>::iterator it = vectWS.begin(); it != vectWS.end(); it++)
	{
		retval.push_back(ROMUTIL::WStrToMBCStr(*it));
	}
	return retval;
}

vector<wstring> ROMUTIL::ToWStringVector(vector<string> vStr)
{
	vector<wstring> retval;
	for (vector<string>::iterator it = vStr.begin(); it != vStr.end(); it++)
	{
		wstring wStr = ROMUTIL::MBCStrToWStr(*it);
		retval.push_back(wStr);
	}
	return retval;
}

string ROMUTIL::stringify(double x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(double)");
   return o.str();
}

string ROMUTIL::stringify(long x)
{
   std::ostringstream o;
   if (!(o << x))
     throw std::runtime_error("stringify(long)");
   return o.str();
}

//will replace with a boost:: equivalent when UUID is accepted into the library
string ROMUTIL::MakeGUID()
{
	string guid;
	//guid for each ObjectNode
	#ifdef _MSC_VER
		GUID UIDObj;
		unsigned char* pUIDStr;
		CoCreateGuid(&UIDObj);
		// Convert the GUID to a string
		UuidToString( &UIDObj, &pUIDStr );
		guid = (const char*)pUIDStr;
		RpcStringFree(&pUIDStr);
	#else
		#ifdef POSIX
				uuid_t uid;
				uuid_generate(uid);
				char cGUID[128] = "";
				uuid_unparse(uid, cGUID);
				guid = cGUID;
		#else
			char* pGuidStr = new char[32];
			int i;

			srand(static_cast<unsigned int> (time(NULL))); /*Randomize based on time.*/

			/*Data1 - 8 characters.*/
			*pGuidStr++ = '{';
			for(i = 0; i < 8; i++, pGuidStr++)
			((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

			/*Data2 - 4 characters.*/
			*pGuidStr++ = '-';
			for(i = 0; i < 4; i++, pGuidStr++)
			((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

			/*Data3 - 4 characters.*/
			*pGuidStr++ = '-';
			for(i = 0; i < 4; i++, pGuidStr++)
			((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

			/*Data4 - 4 characters.*/
			*pGuidStr++ = '-';
			for(i = 0; i < 4; i++, pGuidStr++)
			((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

			/*Data5 - 12 characters.*/
			*pGuidStr++ = '-';
			for(i = 0; i < 12; i++, pGuidStr++)
			((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

			*pGuidStr++ = '}';
			*pGuidStr = '\0';

			guid = pGuidStr;
			delete[] pGuidStr;
		#endif
	#endif
	return guid;
}

#ifdef USE_MSXML	
		wstring ROMUTIL::ToWString(_variant_t str)
		{
			return (wstring)str.bstrVal;
		}
#endif
		wstring ROMUTIL::MBCStrToWStr(string mbStr)
		{
			if (mbStr.size() == 0)
				return L"";

			size_t requiredSize = mbstowcs(NULL, mbStr.c_str(), 0) + 1;
			wchar_t *wStr = new wchar_t[requiredSize];
			mbstowcs(wStr, mbStr.c_str(), requiredSize);
			wstring retval = wStr;
			delete [] wStr;
			return retval;
		}

		string ROMUTIL::WStrToMBCStr(wstring wstr)
		{
			mbstate_t *ps = NULL;
			const size_t MAX_SIZE = 4*wstr.length() + 1; //should handle UTF-8 largest char????
			char *mbcstr = new char[MAX_SIZE];
			const wchar_t   *wcsIndirectString = wstr.c_str();
			size_t finalSize = wcstombs(mbcstr, wstr.c_str(), MAX_SIZE);
			string retval = mbcstr;
			delete [] mbcstr;
			return retval;
		}
