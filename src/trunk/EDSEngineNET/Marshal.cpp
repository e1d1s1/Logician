/*
This file is part of EDSEngineNET.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

    EDSEngineNET is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    EDSEngineNET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "Marshal.h"

string MarshalStringA(String ^ s) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	string retval = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
	return retval;
}

wstring MarshalString(String ^ s) {
	using namespace Runtime::InteropServices;
	const wchar_t* chars =
		(const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
	wstring retval = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
	return retval;
}

void MarshalDictionaryStringUInt(Dictionary<String^, size_t>^ dict, unordered_map<wstring, size_t> &mp)
{
	for each(KeyValuePair<String^, size_t> kvp in dict)
	{
		wstring key = MarshalString(kvp.Key);
		mp[key] = kvp.Value;
	}
}

array<String^>^ GetArrayFromVectorStrings(const vector<wstring> &vect)
{
	array<String^>^ arr = gcnew array<String^>(vect.size());
	for (size_t i = 0; i < vect.size(); i++)
	{
		arr[i] = gcnew String(vect[i].c_str());
	}
	return arr;
}

Dictionary<String^, array<String^>^>^ GetDictionaryFromMapStrings(const map<wstring, vector<wstring> > &mp)
{
	Dictionary<String^,	array<String^>^>^ dict = gcnew Dictionary<String^, array<String^>^>();

	for (map<wstring, vector<wstring> >::const_iterator it = mp.begin(); it != mp.end(); it++)
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