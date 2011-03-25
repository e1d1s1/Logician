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

#include <string>
#include "Bimapper.h"
#include "RuleCell.h"

using namespace std;
#ifdef _MSC_VER
using namespace stdext;
#else
using namespace __gnu_cxx;
#endif

//converts long key to appropriate string value
//checks the string value for logical operators < > !=
class CDecode
{
public:
	CDecode(CRuleCell outputCell, hash_map<wstring, size_t> *inputValues, CBimapper *stringMap);
	CDecode(size_t currentValue, CRuleCell cell, hash_map<wstring, size_t> *inputValues, CBimapper *stringMap);
	~CDecode(void);
	bool EvaluateInputCell();
	vector<wstring> EvaluateOutputCell();

private:
	wstring GetString(size_t lKey);
	wstring ParseStringForGets(size_t lKey, bool bForceZero);
	wstring ReplaceAGet(wstring s, bool bForceZero);
	void CheckForInputGets();

	size_t m_value;
	vector<size_t> m_tests;
	long m_operator;
	CBimapper *m_stringsMap;
	hash_map<wstring, size_t> *m_inputValues;
};

/*
Operator types:
0 - none, direct compare
1 - <>, != not equal
2 - < LT
3 - <= LT Eq
4 - > GT
5 - >= GT Eq
6 - [] Range inclusive
7 - (] Range end inclusive
8 - [) Range start inclusive
9 - () Range not inclusive
10 - Python code
11 - get a value inline get(someOtherAttr)
12 - table chain
*/
#define EQUALS 0x1
#define NOT_EQUAL 0x2
#define LESS_THAN 0x4
#define LESS_THAN_EQUAL 0x8
#define GREATER_THAN 0x10
#define GREATER_THAN_EQUAL 0x20
#define RANGE_INCLUSIVE 0x40
#define RANGE_END_INCLUSIVE 0x80
#define RANGE_START_INCLUSIVE 0x100
#define RANGE_NOT_INCLUSIVE 0x200
#define PYTHON 0x400
#define GETS 0x800
#define CHAIN 0x1000
#define JAVASCRIPT 0x2000


/*
//OR defined with || between values
value1||value2||value3





*/
