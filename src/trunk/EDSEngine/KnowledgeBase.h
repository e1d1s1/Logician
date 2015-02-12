/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2014 Eric D. Schmidt, DigiRule Solutions LLC

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
		CKnowledgeBase(wstring knowledge_file);
		bool CreateKnowledgeBase(wstring knowledge_file);
		bool CreateKnowledgeBaseFromString(wstring xmlStr);
		size_t TableCount() {return m_TableSet.Count();}
		bool IsOpen() {return m_IsOpen;}
		void EnableRemoteDebugger(bool enable) { m_remoteDebugging = enable; }

		bool TableHasScript(const wstring& tableName);
		bool TableIsGetAll(const wstring& tableName);
		vector<wstring> EvaluateTableWithParam(const wstring& tableName, const wstring& outputAttr, bool bGetAll, wstring& param, void* context = nullptr);
		vector<wstring> EvaluateTableWithParam(const wstring& tableName, const wstring& outputAttr, wstring& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, outputAttr, TableIsGetAll(tableName), param, context); }
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring& tableName, bool bGetAll, wstring& param, void* context = nullptr);
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring& tableName, wstring& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, TableIsGetAll(tableName), param, context); }
		vector<wstring> EvaluateTable(const wstring& tableName, const wstring& outputAttr, bool bGetAll, void* context = nullptr);
		vector<wstring> EvaluateTable(const wstring& tableName, const wstring& outputAttr, void* context = nullptr) { return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName), context); }
		map<wstring, vector<wstring> > EvaluateTable(const wstring& tableName, bool bGetAll, void* context = nullptr);
		map<wstring, vector<wstring> > EvaluateTable(const wstring& tableName, void* context = nullptr) { return EvaluateTable(tableName, TableIsGetAll(tableName), context); }
		wstring GetFirstTableResult(const wstring& tableName, const wstring& outputAttr, void* context = nullptr);
		vector<wstring> ReverseEvaluateTable(const wstring& tableName, const wstring& inputAttr, bool bGetAll, void* context = nullptr);
		vector<wstring> ReverseEvaluateTable(const wstring& tableName, const wstring& inputAttr, void* context = nullptr) { return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName), context); }
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring& tableName, bool bGetAll, void* context = nullptr);
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring& tableName, void* context = nullptr) { return ReverseEvaluateTable(tableName, TableIsGetAll(tableName), context); }


		vector<wstring> GetInputAttrs(const wstring& tableName) { return m_TableSet.GetInputAttrs(tableName); }
		vector<wstring> GetInputDependencies(const wstring& tableName) { return m_TableSet.GetInputDependencies(tableName); }
		vector<wstring> GetOutputAttrs(const wstring& tableName) { return m_TableSet.GetOutputAttrs(tableName); }
		vector<wstring> GetAllPossibleOutputs(const wstring& tableName, const wstring& outputName);

		//Translations
		wstring Localize(const wstring& baseValue, const wstring& locale) { return Translate(baseValue, L"", locale); }
		wstring DeLocalize(const wstring& localeValue);
		wstring Translate(const wstring& source, const wstring& sourceLocale, const wstring& destLocale);

		//ASCII overloads
		CKnowledgeBase(string knowledge_file);
		bool CreateKnowledgeBase(string knowledge_fil);
		bool CreateKnowledgeBaseFromString(string xmlStr);
		bool TableHasScript(const string& tableName);
		bool TableIsGetAll(const string& tableName);
		vector<string> EvaluateTableWithParam(const string& tableName, const string& outputAttr, string& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, outputAttr, TableIsGetAll(tableName), param, context); }
		vector<string> EvaluateTableWithParam(const string& tableName, const string& outputAttr, bool bGetAll, string& param, void* context = nullptr);
		map<string, vector<string> > EvaluateTableWithParam(const string& tableName, string& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, TableIsGetAll(tableName), param, context); }
		map<string, vector<string> > EvaluateTableWithParam(const string& tableName, bool bGetAll, string& param, void* context = nullptr);
		vector<string> EvaluateTable(const string& tableName, const string& outputAttr, void* context = nullptr) { return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName), context); }
		vector<string> EvaluateTable(const string& tableName, const string& outputAttr, bool bGetAll, void* context = nullptr);
		map<string, vector<string> > EvaluateTable(const string& tableName, void* context = nullptr) { return EvaluateTable(tableName, TableIsGetAll(tableName), context); }
		map<string, vector<string> > EvaluateTable(const string& tableName, bool bGetAll, void* context = nullptr);
		string GetFirstTableResult(const string& tableName, const string& outputAttr, void* context = nullptr);
		vector<string> ReverseEvaluateTable(const string& tableName, const string& inputAttr, void* context = nullptr) { return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName), context); }
		vector<string> ReverseEvaluateTable(const string& tableName, const string& inputAttr, bool bGetAll, void* context = nullptr);
		map<string, vector<string> > ReverseEvaluateTable(const string& tableName, void* context = nullptr) { return ReverseEvaluateTable(tableName, TableIsGetAll(tableName), context); }
		map<string, vector<string> > ReverseEvaluateTable(const string& tableName, bool bGetAll, void* context = nullptr);

		vector<string> GetInputAttrs(const string& tableName);
		vector<string> GetInputDependencies(const string& tableName);
		vector<string> GetOutputAttrs(const string& tableName);
		vector<string> GetAllPossibleOutputs(const string& tableName, const string& outputName);

		std::function<void(const wstring&)> DebugHandlerPtr;
		std::function<wstring(const wstring&, void*)> InputValueGetterPtr;

	private:
		bool _parseXML(Document xmlDocument);
		vector<pair<wstring, vector<CRuleCell> > > _getTableRowFromXML(NodeList nodes, Document xmlDocument);
		void _sendToDebugServer(const wstring& msg);
		wstring _XMLSafe(const wstring& str);
		bool _debugThisTable(const wstring& tableName);

		CBimapper m_stringsMap;
		CTableSet m_TableSet;
		int iRecursingDepth;
		bool m_DEBUGGING_MSGS;
		vector<wstring> m_DebugTables;
		wstring m_DEBUGGING_CON;
		bool m_remoteDebugging;
		bool m_IsOpen;
		wstring m_jsCode, m_pyCode;
		unordered_map<size_t, unordered_map<wstring, wstring>> mapBaseIDtoTranslations;

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


