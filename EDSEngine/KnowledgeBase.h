/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 - 2011 Eric D. Schmidt

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "stdafx.h"
#include <string>
#include "RuleTable.h"
#include "TableSet.h"
#include "XMLWrapper.h"
#ifdef WIN32
#include <comutil.h>
#endif

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

using namespace std;

typedef void (*DebugHandler) (wstring); 

namespace EDS
{
	class CKnowledgeBase
	{
	public:
		~CKnowledgeBase(void);
		CKnowledgeBase();
		CKnowledgeBase(wstring knowledge_file);
		bool CreateKnowledgeBase(wstring knowledge_file);
		bool CreateKnowledgeBaseFromString(wstring xmlStr);
		size_t TableCount() {return m_TableSet.Count();}
		bool IsOpen() {return m_IsOpen;}
		void SetDebugHandler(DebugHandler debugger);
		void GenerateDebugMessages(bool bGenerate);
		wstring GetDebugMessages();

		bool TableHasScript(wstring tableName);
		bool TableIsGetAll(wstring tableName);
		vector<wstring> EvaluateTableWithParam(wstring tableName, wstring outputAttr, wstring param) {return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName));}
		vector<wstring> EvaluateTableWithParam(wstring tableName, wstring outputAttr, wstring param, bool bGetAll);
		map<wstring, vector<wstring> > EvaluateTableWithParam(wstring tableName, wstring param) {return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName));}
		map<wstring, vector<wstring> > EvaluateTableWithParam(wstring tableName, wstring param, bool bGetAll);
		vector<wstring> EvaluateTable(wstring tableName, wstring outputAttr) {return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName));}
		vector<wstring> EvaluateTable(wstring tableName, wstring outputAttr, bool bGetAll);
		map<wstring, vector<wstring> > EvaluateTable(wstring tableName) {return EvaluateTable(tableName, TableIsGetAll(tableName));}
		map<wstring, vector<wstring> > EvaluateTable(wstring tableName, bool bGetAll);
		vector<wstring> ReverseEvaluateTable(wstring tableName, wstring inputAttr) {return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName));}
		vector<wstring> ReverseEvaluateTable(wstring tableName, wstring inputAttr, bool bGetAll);
		map<wstring, vector<wstring> > ReverseEvaluateTable(wstring tableName) {return ReverseEvaluateTable(tableName, TableIsGetAll(tableName));}
		map<wstring, vector<wstring> > ReverseEvaluateTable(wstring tableName, bool bGetAll);

		wstring GetEvalParameter() {return m_StateParameter;}
	#ifdef _MSC_VER
		void SetInputValues(stdext::hash_map<wstring, size_t> values) 
		{
			m_GlobalInputAttrsValues = values;
			m_GlobalInputAttrsValues[L""] = EMPTY_STRING;
			m_GlobalInputAttrsValues[L"NULL"] = EXPLICIT_NULL_STRING;
		}
	#else
		void SetInputValues(__gnu_cxx::hash_map<wstring, size_t> values) 
		{
			m_GlobalInputAttrsValues = values;
		}
	#endif
		void SetInputValue(wstring name, wstring value);
		void ResetTable(wstring tableName);

		vector<wstring> GetInputAttrs(wstring tableName) {return m_TableSet.GetInputAttrs(tableName);}
		vector<wstring> GetInputDependencies(wstring tableName) {return m_TableSet.GetInputDependencies(tableName);}
		vector<wstring> GetOutputAttrs(wstring tableName) {return m_TableSet.GetOutputAttrs(tableName);}
		vector<wstring> GetAllPossibleOutputs(wstring tableName, wstring outputName) {return m_TableSet.GetTable(tableName)->GetAllPossibleOutputs(outputName);}

		//Translations
		wstring Localize(wstring baseValue, wstring locale) {return Translate(baseValue, L"", locale);}
		wstring DeLocalize(wstring localeValue);
		wstring Translate(wstring source, wstring sourceLocale, wstring destLocale);

		//ASCII overloads
		CKnowledgeBase(string knowledge_file);
		bool CreateKnowledgeBase(string knowledge_file);
		bool CreateKnowledgeBaseFromString(string xmlStr);
		bool TableHasScript(string tableName);
		bool TableIsGetAll(string tableName);
		vector<string> EvaluateTableWithParam(string tableName, string outputAttr, string param) {return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName));}
		vector<string> EvaluateTableWithParam(string tableName, string outputAttr, string param, bool bGetAll);
		map<string, vector<string> > EvaluateTableWithParam(string tableName, string param) {return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName));}
		map<string, vector<string> > EvaluateTableWithParam(string tableName, string param, bool bGetAll);
		vector<string> EvaluateTable(string tableName, string outputAttr) {return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName));}
		vector<string> EvaluateTable(string tableName, string outputAttr, bool bGetAll);
		map<string, vector<string> > EvaluateTable(string tableName) {return EvaluateTable(tableName, TableIsGetAll(tableName));}
		map<string, vector<string> > EvaluateTable(string tableName, bool bGetAll);
		vector<string> ReverseEvaluateTable(string tableName, string inputAttr) {return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName));}
		vector<string> ReverseEvaluateTable(string tableName, string inputAttr, bool bGetAll);
		map<string, vector<string> > ReverseEvaluateTable(string tableName) {return ReverseEvaluateTable(tableName, TableIsGetAll(tableName));}
		map<string, vector<string> > ReverseEvaluateTable(string tableName, bool bGetAll);
		string GetEvalParameterA() {return WStrToMBCStr(m_StateParameter);}
	#ifdef _MSC_VER
		void SetInputValues(stdext::hash_map<string, size_t> values);
	#else
		void SetInputValues(__gnu_cxx::hash_map<string, size_t> values);
	#endif
		void SetInputValue(string name, string value);
		size_t GetInputValuesCount() {return m_GlobalInputAttrsValues.size();}
		void ResetTable(string tableName);
		vector<string> GetInputAttrs(string tableName);
		vector<string> GetInputDependencies(string tableName);
		vector<string> GetOutputAttrs(string tableName);
		vector<string> GetAllPossibleOutputs(string tableName, string outputName);


	private:		
		bool _parseXML(Document xmlDocument);
		vector<pair<wstring, vector<CRuleCell> > > GetTableRowFromXML(NodeList nodes, Document xmlDocument);
		void SendToDebugServer(wstring msg);
		wstring XMLSafe(wstring str);
		bool DebugThisTable(wstring tableName);
	#ifdef _MSC_VER
		stdext::hash_map<wstring, size_t> m_GlobalInputAttrsValues;
	#else
		__gnu_cxx::hash_map<wstring, size_t> m_GlobalInputAttrsValues;
	#endif
		wstring m_StateParameter;
		CBimapper m_stringsMap;
		CTableSet m_TableSet;
		int iRecursingDepth;
		bool m_DEBUGGING_MSGS;
		bool m_bGenerateMsg;
		wstring m_LastDebugMessage;
		vector<wstring> m_DebugTables;
		DebugHandler m_DebugHandlerPtr; //to do in-application table debugging
		wstring m_DEBUGGING_CON;
		bool m_IsOpen;
		wstring m_jsCode, m_pyCode;

	#ifdef _MSC_VER
		stdext::hash_map<size_t, stdext::hash_map<wstring, wstring> > mapBaseIDtoTranslations;
	#else
		__gnu_cxx::hash_map<size_t, __gnu_cxx::hash_map<wstring, wstring> > mapBaseIDtoTranslations;
	#endif


	//helper functions
	#ifdef WIN32
	wstring VariantToWStr(const _variant_t mVar)
	{
		wstring retval = mVar.bstrVal;
		return retval;
	}
	#endif

	string WStrToMBCStr(wstring wstr)
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

	wstring MBCStrToWStr(string mbStr)
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

	#ifdef USE_LIBXML	
	wstring XMLStrToWStr(const xmlChar* mbStr)
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

	wstring XMLStrToWStr(xmlChar* mbStr)
	{
		return XMLStrToWStr((const xmlChar*)mbStr);
	}
	#endif
	};
}
