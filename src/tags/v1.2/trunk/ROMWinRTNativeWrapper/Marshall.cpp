#include "pch.h"
#include "Marshall.h"
#include <collection.h>
#include <algorithm>

using namespace std;
using namespace Platform::Collections;

IVector<String^>^ GetIVectorFromVectorStrings(const vector<wstring> &vect)
{
	Vector<String^>^ retval = ref new Vector<String^>();
	for (auto iter = vect.begin(); iter != vect.end(); iter++)
	{
		String^ val = ref new String(iter->c_str());
		retval->Append(val);
	}
	return retval;
}

vector<wstring> GetVectorFromIVectorStrings(IVector<String^>^ vect)
{
	vector<wstring> retval;
	for (auto iter = begin(vect); iter != end(vect); iter++)
	{
		retval.push_back((*iter)->Data());
	}
	return retval;
}

IMap<String^, IVector<String^>^>^ GetIMapFromMapStrings(const map<wstring, vector<wstring> > &mp)
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