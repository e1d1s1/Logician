#include "pch.h"
#include "Marshall.h"
#include <collection.h>
#include <algorithm>

using namespace std;
using namespace Platform::Collections;

IVector<String^>^ GetIVectorFromVectorStrings(vector<wstring> vect)
{
	Vector<String^>^ retval = ref new Vector<String^>();
	for (auto iter = vect.begin(); iter != vect.end(); iter++)
	{
		String^ val = ref new String(iter->c_str());
		retval->Append(val);
	}
	return retval;
}

IMap<String^, IVector<String^>^>^ GetIMapFromMapStrings(map<wstring, vector<wstring> > mp)
{
	Map<String^, IVector<String^>^>^ retval = ref new Map<String^, IVector<String^>^>();
	for (auto iter = mp.begin(); iter != mp.end(); iter++)
	{
		String^ key = ref new String(iter->first.c_str());
		IVector<String^>^ vec = GetIVectorFromVectorStrings(iter->second);
		retval->Insert(key, vec);
	}
	return retval;
}

void MarshalIMapStringUInt (IMap<String^, size_t>^ dict, MAPWSTRUINT &mp)
{
	for (IKeyValuePair<String^, size_t>^ item : dict)
	{
		wstring key(item->Key->Data());
		mp[key] = item->Value;
	}
}