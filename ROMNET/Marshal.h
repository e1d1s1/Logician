/*
This file is part of ROMNET.
Copyright (C) 2009-2011 Eric D. Schmidt

    ROMNET is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROMNET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROMNET.  If not, see <http://www.gnu.org/licenses/>.
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

string MarshalStringA ( String ^ s);
wstring MarshalString ( String ^ s);
stdext::hash_map<wstring, size_t> MarshalDictionaryStringUInt (Dictionary<String^, size_t>^ dict);

array<String^>^ GetArrayFromVectorStrings(vector<wstring> vect);
vector<wstring> GetVectorFromArrayStrings(array<String^>^ arr);
Dictionary<String^,	array<String^>^>^ GetDictionaryFromMapStrings(map<wstring, vector<wstring> > mp);