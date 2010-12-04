/*
This file is part of ROM2NET.
Copyright (C) 2009 Eric D. Schmidt

    ROM2NET is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROM2NET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with ROM2NET.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "Marshal.h"

string MarshalStringA ( String ^ s ) {
   using namespace Runtime::InteropServices;
   const char* chars = 
      (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
   string retval = chars;
   Marshal::FreeHGlobal(IntPtr((void*)chars));
   return retval;
}

wstring MarshalString ( String ^ s) {
   using namespace Runtime::InteropServices;
   const wchar_t* chars = 
      (const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
   wstring retval = chars;
   Marshal::FreeHGlobal(IntPtr((void*)chars));
   return retval;
}

stdext::hash_map<wstring, size_t> MarshalDictionaryStringUInt (Dictionary<String^, size_t>^ dict)
{
	stdext::hash_map<wstring, size_t> retval;
	for each(KeyValuePair<String^, size_t> kvp in dict)
	{		
		wstring key = MarshalString(kvp.Key);
		retval[key] = kvp.Value;
	}
	return retval;
}

array<String^>^ GetArrayFromVectorStrings(vector<wstring> vect)
{
	array<String^>^ arr = gcnew array<String^>(vect.size());
	for (size_t i = 0; i < vect.size(); i++)
	{
		arr[i] = gcnew String(vect[i].c_str());
	}
	return arr;
}

vector<wstring> GetVectorFromArrayStrings(array<String^>^ arr)
{
	vector<wstring> retval;
	for each (String^ s in arr)
	{
		wstring ws = MarshalString(s);
		retval.push_back(ws);
	}
	return retval;
}

Dictionary<String^, array<String^>^>^ GetDictionaryFromMapStrings(map<wstring, vector<wstring> > mp)
{
	Dictionary<String^,	array<String^>^>^ dict = gcnew Dictionary<String^, array<String^>^>();

	for (map<wstring, vector<wstring> >::iterator it = mp.begin(); it != mp.end(); it++)
	{
		array<String^>^ arr = gcnew array<String^>(it->second.size());
		for (size_t i = 0; i < it->second.size(); i++)
		{
			arr[i] = gcnew String(it->second[i].c_str());
		}
		String^ key = gcnew String(it->first.c_str());
		dict->Add(key, arr);
	}

	return dict;
}