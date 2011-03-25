/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 - 2011 Eric D. Schmidt

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#ifdef _MSC_VER
#include <hash_map>
#else
#include <ext/hash_map>
#include <tr1/functional>
// Implement string hash functions so that strings of various flavors can
// be used as keys in STL maps and sets.
namespace __gnu_cxx {

template<>
struct hash<wchar_t*> {
  size_t operator()(const wchar_t* s) const {
    return std::tr1::hash<const wchar_t*>()(s);
  }
};

template<>
struct hash<const wchar_t*> {
  size_t operator()(const wchar_t* s) const {
    return std::tr1::hash<const wchar_t*>()(s);
  }
};

template<>
struct hash<std::wstring> {
  size_t operator()(const std::wstring& s) const {
    return std::tr1::hash<std::wstring>()(s);
  }
};

template<>
struct hash<const std::wstring> {
  size_t operator()(const std::wstring& s) const {
    return std::tr1::hash<std::wstring>()(s);
  }
};

template<>
struct hash<std::string> {
  size_t operator()(const std::string& s) const {
    return std::tr1::hash<std::string>()(s);
  }
};

template<>
struct hash<const std::string> {
  size_t operator()(const std::string& s) const {
    return std::tr1::hash<std::string>()(s);
  }
};
}
namespace std
{
 using namespace __gnu_cxx;
}
#endif

class CBimapper
{
public:
	CBimapper(void);
	~CBimapper(void);

	void AddString(size_t id, wstring s);
	size_t AddUserString(wstring s);
	wstring GetStringByID(size_t id);
	size_t GetIDByString(wstring s);
	void ClearUserStrings();

private:
	hash_map<size_t, wstring> m_IndexToStringsMap;
	hash_map<wstring, size_t> m_StringsToIndexMap;
	vector<size_t> userStrings;
	size_t maxID;
};
