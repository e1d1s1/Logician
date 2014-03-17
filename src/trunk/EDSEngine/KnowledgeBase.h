/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

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
#include "stdafx.h"
#include <string>
#include <functional>

#include "RuleTable.h"
#include "TableSet.h"
#include "XMLWrapper.h"
#ifdef WIN32
#include <comutil.h>
#endif

using namespace std;

namespace EDS
{
	class CKnowledgeBase
	{
	public:
		~CKnowledgeBase(void);
		CKnowledgeBase();
		CKnowledgeBase(wstring knowledge_file, size_t threads = 1);
		bool CreateKnowledgeBase(wstring knowledge_file, size_t threads = 1);
		bool CreateKnowledgeBaseFromString(wstring xmlStr, size_t threads = 1);
		size_t TableCount() {return m_TableSet.Count();}
		bool IsOpen() {return m_IsOpen;}

		bool TableHasScript(wstring tableName);
		bool TableIsGetAll(wstring tableName);
		vector<wstring> EvaluateTableWithParam(wstring tableName, wstring outputAttr, wstring& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName), context); }
		vector<wstring> EvaluateTableWithParam(wstring tableName, wstring outputAttr, wstring& param, bool bGetAll, void* context = nullptr);
		map<wstring, vector<wstring> > EvaluateTableWithParam(wstring tableName, wstring& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName), context); }
		map<wstring, vector<wstring> > EvaluateTableWithParam(wstring tableName, wstring& param, bool bGetAll, void* context = nullptr);
		vector<wstring> EvaluateTable(wstring tableName, wstring outputAttr, void* context = nullptr) { return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName), context); }
		vector<wstring> EvaluateTable(wstring tableName, wstring outputAttr, bool bGetAll, void* context = nullptr);
		map<wstring, vector<wstring> > EvaluateTable(wstring tableName, void* context = nullptr) { return EvaluateTable(tableName, TableIsGetAll(tableName), context); }
		map<wstring, vector<wstring> > EvaluateTable(wstring tableName, bool bGetAll, void* context = nullptr);
		wstring GetFirstTableResult(wstring tableName, wstring outputAttr, void* context = nullptr);
		vector<wstring> ReverseEvaluateTable(wstring tableName, wstring inputAttr, void* context = nullptr) { return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName), context); }
		vector<wstring> ReverseEvaluateTable(wstring tableName, wstring inputAttr, bool bGetAll, void* context = nullptr);
		map<wstring, vector<wstring> > ReverseEvaluateTable(wstring tableName, void* context = nullptr) { return ReverseEvaluateTable(tableName, TableIsGetAll(tableName), context); }
		map<wstring, vector<wstring> > ReverseEvaluateTable(wstring tableName, bool bGetAll, void* context = nullptr);

		vector<wstring> GetInputAttrs(wstring tableName) {return m_TableSet.GetInputAttrs(tableName);}
		vector<wstring> GetInputDependencies(wstring tableName) {return m_TableSet.GetInputDependencies(tableName);}
		vector<wstring> GetOutputAttrs(wstring tableName) {return m_TableSet.GetOutputAttrs(tableName);}
		vector<wstring> GetAllPossibleOutputs(wstring tableName, wstring outputName);

		//Translations
		wstring Localize(wstring baseValue, wstring locale) {return Translate(baseValue, L"", locale);}
		wstring DeLocalize(wstring localeValue);
		wstring Translate(wstring source, wstring sourceLocale, wstring destLocale);

		//ASCII overloads
		CKnowledgeBase(string knowledge_file, size_t threads = 1);
		bool CreateKnowledgeBase(string knowledge_file, size_t threads = 1);
		bool CreateKnowledgeBaseFromString(string xmlStr, size_t threads = 1);
		bool TableHasScript(string tableName);
		bool TableIsGetAll(string tableName);
		vector<string> EvaluateTableWithParam(string tableName, string outputAttr, string& param, void* context = nullptr) {return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName), context);}
		vector<string> EvaluateTableWithParam(string tableName, string outputAttr, string& param, bool bGetAll, void* context = nullptr);
		map<string, vector<string> > EvaluateTableWithParam(string tableName, string& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName), context); }
		map<string, vector<string> > EvaluateTableWithParam(string tableName, string& param, bool bGetAll, void* context = nullptr);
		vector<string> EvaluateTable(string tableName, string outputAttr, void* context = nullptr) { return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName), context); }
		vector<string> EvaluateTable(string tableName, string outputAttr, bool bGetAll, void* context = nullptr);
		map<string, vector<string> > EvaluateTable(string tableName, void* context = nullptr) { return EvaluateTable(tableName, TableIsGetAll(tableName), context); }
		map<string, vector<string> > EvaluateTable(string tableName, bool bGetAll, void* context = nullptr);
		string GetFirstTableResult(string tableName, string outputAttr, void* context = nullptr);
		vector<string> ReverseEvaluateTable(string tableName, string inputAttr, void* context = nullptr) { return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName), context); }
		vector<string> ReverseEvaluateTable(string tableName, string inputAttr, bool bGetAll, void* context = nullptr);
		map<string, vector<string> > ReverseEvaluateTable(string tableName, void* context = nullptr) { return ReverseEvaluateTable(tableName, TableIsGetAll(tableName), context); }
		map<string, vector<string> > ReverseEvaluateTable(string tableName, bool bGetAll, void* context = nullptr);

		vector<string> GetInputAttrs(string tableName);
		vector<string> GetInputDependencies(string tableName);
		vector<string> GetOutputAttrs(string tableName);
		vector<string> GetAllPossibleOutputs(string tableName, string outputName);

		std::function<void(const wstring&)> DebugHandlerPtr;
		std::function<wstring(const wstring&, void*)> InputValueGetterPtr;
		
	private:
		bool _parseXML(Document xmlDocument);
		vector<pair<wstring, vector<CRuleCell> > > GetTableRowFromXML(NodeList nodes, Document xmlDocument);
		void SendToDebugServer(wstring msg);
		wstring XMLSafe(wstring str);
		bool DebugThisTable(wstring tableName);

		CBimapper m_stringsMap;
		CTableSet m_TableSet;
		int iRecursingDepth;				
		bool m_DEBUGGING_MSGS;
		vector<wstring> m_DebugTables;
		wstring m_DEBUGGING_CON;
		bool m_IsOpen;
		wstring m_jsCode, m_pyCode;
		unordered_map<size_t, unordered_map<wstring, wstring>> mapBaseIDtoTranslations;
		size_t m_threads;

	//helper functions
	#ifdef WIN32
	wstring VariantToWStr(_variant_t mVar)
	{
		wstring retval = mVar.bstrVal;
		return retval;
	}
	#endif
	};
}


