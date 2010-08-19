/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 Eric D. Schmidt

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

using namespace std;

namespace EDS
{
	class CKnowledgeBase
	{
	public:
		~CKnowledgeBase(void);
		CKnowledgeBase();
		CKnowledgeBase(wstring knowledge_file);
		bool CreateKnowledgeBase(wstring knowledge_file);
		size_t TableCount() {return m_TableSet.Count();}
		bool IsOpen() {return m_IsOpen;}

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
		void CreateKnowledgeBase(string knowledge_file);
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
		wstring m_StateParameter;
		vector<pair<wstring, vector<CRuleCell> > > GetTableRowFromXML(NodeList nodes, Document xmlDocument);
		void SendToDebugServer(wstring msg);
		wstring XMLSafe(wstring str);
	#ifdef _MSC_VER
		stdext::hash_map<wstring, size_t> m_GlobalInputAttrsValues;
	#else
		__gnu_cxx::hash_map<wstring, size_t> m_GlobalInputAttrsValues;
	#endif
		CBimapper m_stringsMap;
		CTableSet m_TableSet;
		int iRecursingDepth;
		bool m_DEBUGGING_MSGS;
		wstring m_DEBUGGING_CON;
		bool m_IsOpen;
		wstring m_jsCode, m_pyCode;

	#ifdef _MSC_VER
		stdext::hash_map<size_t, stdext::hash_map<wstring, wstring> > mapBaseIDtoTranslations;
	#else
		__gnu_cxx::hash_map<size_t, __gnu_cxx::hash_map<wstring, wstring> > mapBaseIDtoTranslations;
	#endif


	//helper functions
	#ifdef USE_MSXML
	wstring VariantToWStr(const _variant_t mVar)
	{
		wstring retval = mVar.bstrVal;
		return retval;
	}
	#endif

	wstring MBCStrToWStr(const char * mbStr)
	{
		if (mbStr == NULL)
			return L"";

		size_t requiredSize = mbstowcs(NULL, mbStr, 0) + 1;
		wchar_t *wStr = new wchar_t[requiredSize];
		mbstowcs(wStr, mbStr, requiredSize);
		wstring retval = wStr;
		delete [] wStr;
		return retval;
	}

	string WStrToMBCStr(wstring wstr)
	{
		const size_t MAX_SIZE = 4*wstr.length() + 1; //should handle UTF-8 largest char????
		char *mbcstr = new char[MAX_SIZE];
		size_t finalSize = wcstombs(mbcstr, wstr.c_str(), MAX_SIZE);
		string retval = mbcstr;
		delete [] mbcstr;
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
	wstring MBCStrToWStr(const xmlChar* mbStr)
	{
		if (mbStr == NULL)
			return L"";

		size_t requiredSize = mbstowcs(NULL, (const char*)mbStr, 0) + 1;
		wchar_t *wStr = new wchar_t[requiredSize];
		mbstowcs(wStr, (const char*)mbStr, requiredSize);
		wstring retval = wStr;
		delete [] wStr;
		return retval;
	}

	wstring MBCStrToWStr(xmlChar* mbStr)
	{
		if (mbStr == NULL)
			return L"";

		size_t requiredSize = mbstowcs(NULL, (const char*)mbStr, 0) + 1;
		wchar_t *wStr = new wchar_t[requiredSize];
		mbstowcs(wStr, (const char*)mbStr, requiredSize);
		wstring retval = wStr;
		delete [] wStr;
		xmlFree(mbStr);
		return retval;
	}
	#endif
	};
}
