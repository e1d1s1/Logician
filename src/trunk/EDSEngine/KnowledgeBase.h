/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2015 Eric D. Schmidt, DigiRule Solutions LLC

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

#include "utilities.h"
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
		CKnowledgeBase(string knowledge_file);
		bool CreateKnowledgeBase(string knowledge_file);
		bool CreateKnowledgeBaseFromString(string xmlStr);
		size_t TableCount() {return m_TableSet.Count();}
		bool IsOpen() {return m_IsOpen;}
		void EnableRemoteDebugger(bool enable) { m_remoteDebugging = enable; }
		void SetMaxThreads(size_t threads);

		bool TableHasScript(const string& tableName);
		bool TableIsGetAll(const string& tableName);
		vector<string> EvaluateTableWithParam(const string& tableName, const string& outputAttr, bool bGetAll, string& param, void* context = nullptr);
		vector<string> EvaluateTableWithParam(const string& tableName, const string& outputAttr, string& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, outputAttr, TableIsGetAll(tableName), param, context); }
		map<string, vector<string> > EvaluateTableWithParam(const string& tableName, bool bGetAll, string& param, void* context = nullptr);
		map<string, vector<string> > EvaluateTableWithParam(const string& tableName, string& param, void* context = nullptr) { return EvaluateTableWithParam(tableName, TableIsGetAll(tableName), param, context); }
		vector<string> EvaluateTable(const string& tableName, const string& outputAttr, bool bGetAll, void* context = nullptr);
		vector<string> EvaluateTable(const string& tableName, const string& outputAttr, void* context = nullptr) { return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName), context); }
		map<string, vector<string> > EvaluateTable(const string& tableName, bool bGetAll, void* context = nullptr);
		map<string, vector<string> > EvaluateTable(const string& tableName, void* context = nullptr) { return EvaluateTable(tableName, TableIsGetAll(tableName), context); }
		string GetFirstTableResult(const string& tableName, const string& outputAttr, void* context = nullptr);
		vector<string> ReverseEvaluateTable(const string& tableName, const string& inputAttr, bool bGetAll, void* context = nullptr);
		vector<string> ReverseEvaluateTable(const string& tableName, const string& inputAttr, void* context = nullptr) { return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName), context); }
		map<string, vector<string> > ReverseEvaluateTable(const string& tableName, bool bGetAll, void* context = nullptr);
		map<string, vector<string> > ReverseEvaluateTable(const string& tableName, void* context = nullptr) { return ReverseEvaluateTable(tableName, TableIsGetAll(tableName), context); }


		vector<string> GetInputAttrs(const string& tableName) { return m_TableSet.GetInputAttrs(tableName); }
		vector<string> GetInputDependencies(const string& tableName) { return m_TableSet.GetInputDependencies(tableName); }
		vector<string> GetOutputAttrs(const string& tableName) { return m_TableSet.GetOutputAttrs(tableName); }
		vector<string> GetAllPossibleOutputs(const string& tableName, const string& outputName);

		//Translations
		string Localize(const string& baseValue, const string& locale) { return Translate(baseValue, "", locale); }
		string DeLocalize(const string& localeValue);
		string Translate(const string& source, const string& sourceLocale, const string& destLocale);

		std::function<void(const string&)> DebugHandlerPtr;
		std::function<string(const string&, void*)> InputValueGetterPtr;

	private:
		bool _parseXML(Document xmlDocument);
		vector<pair<string, vector<CRuleCell> > > _getTableRowFromXML(NodeList nodes, Document xmlDocument);
		void _sendToDebugServer(const string& msg);
		string _XMLSafe(const string& str);
		bool _debugThisTable(const string& tableName);

		CBimapper m_stringsMap;
		CTableSet m_TableSet;
		int iRecursingDepth;
		bool m_DEBUGGING_MSGS;
		vector<string> m_DebugTables;
		string m_DEBUGGING_CON;
		bool m_remoteDebugging;
		bool m_IsOpen;
		string m_jsCode, m_pyCode;
		unordered_map<size_t, unordered_map<string, string>> mapBaseIDtoTranslations;

	//helper functions
	#ifdef WIN32
	string VariantToMBCStr(_variant_t mVar)
	{
		wstring ws = mVar.bstrVal;
		return EDSUTIL::Narrow(ws);
	}
	#endif
	};
}


