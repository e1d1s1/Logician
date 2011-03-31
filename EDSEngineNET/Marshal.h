/*
This file is part of EDSEngineNET.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

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
#pragma once
#pragma unmanaged
#include <vector>
#include <map>
#include <hash_map>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

#pragma managed
//managed code
#using <System.dll>
using namespace System; 
using namespace System::Collections::Generic;

void MarshalString ( String ^ s, string& os );
void MarshalString ( String ^ s, wstring& os );
void MarshalDictionaryStringUInt (Dictionary<String^, size_t>^ dict, stdext::hash_map<wstring, size_t> &mp);

array<String^>^ GetArrayFromVectorStrings(vector<wstring> vect);
Dictionary<String^,	array<String^>^>^ GetDictionaryFromMapStrings(map<wstring, vector<wstring> > mp);