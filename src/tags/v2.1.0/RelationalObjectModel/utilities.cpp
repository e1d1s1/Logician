/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2014 Eric D. Schmidt, DigiRule Solutions LLC

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
#include "stdafx.h"
#include "utilities.h"
#include <sstream>
#include <string.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#ifdef USE_LIBXML
#include <libxml/tree.h>
#endif
#ifdef USE_MSXML
#include <comdef.h>
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

bool ROMUTIL::StringContains(const wstring& source, const wstring& target)
{
	bool retval = false;

	if (source.find(target, 0) != string::npos)
		retval = true;

	return retval;
}

bool ROMUTIL::StringBeginsWith(const wstring& source, const wstring& target)
{
	bool retval = false;

	if (source.find(target, 0) == 0)
		retval = true;

	return retval;
}

vector<string> ROMUTIL::Split(const string& text, const string& separators)
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

vector<wstring> ROMUTIL::Split(const wstring& text, const wstring& separators)
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

bool ROMUTIL::StringIsNumeric(const wstring& s)
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

string ROMUTIL::ToASCIIString(const wstring& s)
{
	string retval(s.begin(), s.end());
	return retval;
}

vector<string> ROMUTIL::ToASCIIStringVector(const vector<wstring>& vectWS)
{
	vector<string> retval;
	for (auto it = vectWS.begin(); it != vectWS.end(); it++)
	{
		retval.push_back(ROMUTIL::ToASCIIString(*it));
	}
	return retval;
}

vector<string> ROMUTIL::WStrToMBCStrVector(vector<wstring> vectWS)
{
	vector<string> retval;
	for (auto it = vectWS.begin(); it != vectWS.end(); it++)
	{
		retval.push_back(ROMUTIL::WStrToMBCStr(*it));
	}
	return retval;
}

map<string, vector<string> > ROMUTIL::WStrToMBCStrMapVector(map<wstring, vector<wstring> > mapVectWS)
{
	map<string, vector<string> > retval;
	for (auto it = mapVectWS.begin(); it != mapVectWS.end(); it++)
	{
		string key = ROMUTIL::WStrToMBCStr(it->first);
		vector<string> vals = ROMUTIL::WStrToMBCStrVector(it->second);
		retval[key] = vals;
	}
	return retval;
}

vector<wstring> ROMUTIL::ToWStringVector(const vector<string>& vStr)
{
	vector<wstring> retval;
	for (auto it = vStr.begin(); it != vStr.end(); it++)
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

string ROMUTIL::MakeGUID()
{
	//guid for each ObjectNode
	string guid;
	boost::uuids::random_generator gen;
	boost::uuids::uuid u = gen();
	std::stringstream ss;
	ss << u;
	guid = ss.str();
	return guid;
}

std::wstring ROMUTIL::encodeForXml(const wstring& sSrc)
{
    wostringstream sRet;
    for( wstring::const_iterator iter = sSrc.begin(); iter!=sSrc.end(); iter++ )
    {
         wchar_t c = (wchar_t)*iter;
         switch( c )
         {
             case L'&': sRet << L"&amp;"; break;
             case L'<': sRet << L"&lt;"; break;
             case L'>': sRet << L"&gt;"; break;
             case L'"': sRet << L"&quot;"; break;
             case L'\'': sRet << L"&apos;"; break;
             default:
				sRet << c;
         }
    }
    return sRet.str();
}

#ifdef USE_MSXML
		wstring ROMUTIL::BSTR_T_ToWString(void *str) //only pass _bstr_t
		{
			if (str != NULL)
			{
				BSTR ms_str = ((_bstr_t*)str)->copy();
				std::wstring ws(ms_str);
				SysFreeString(ms_str);
				return ws;
			}
			return L"";
		}
#endif
		wstring ROMUTIL::MBCStrToWStr(const string& mbStr)
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

		string ROMUTIL::WStrToMBCStr(const wstring& wstr)
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

#if USE_LIBXML
		wstring ROMUTIL::XMLStrToWStr(const unsigned char* mbStr)
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

		wstring ROMUTIL::XMLStrToWStr(unsigned char* mbStr)
		{
			return XMLStrToWStr((const unsigned char*)mbStr);
		}
#endif
