/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

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
#include "RuleTable.h"
#include "TableSet.h"
#include "XMLWrapper.h"
#ifdef WIN32
#include <comutil.h>
#endif

using namespace std;

typedef void (*DebugHandler) (wstring); 

namespace EDS
{
	class CKnowledgeBase
	{
	public:
		~CKnowledgeBase(void);
		CKnowledgeBase();
		CKnowledgeBase(const wstring knowledge_file);
		bool CreateKnowledgeBase(const wstring knowledge_file);
		bool CreateKnowledgeBaseFromString(wstring xmlStr);
		size_t TableCount() {return m_TableSet.Count();}
		bool IsOpen() {return m_IsOpen;}
		void SetDebugHandler(DebugHandler debugger);
		void GenerateDebugMessages(bool bGenerate);
		wstring GetDebugMessages();

		bool TableHasScript(const wstring tableName);
		bool TableIsGetAll(const wstring tableName);
		vector<wstring> EvaluateTableWithParam(const wstring tableName, const wstring outputAttr, const wstring param) {return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName));}
		vector<wstring> EvaluateTableWithParam(const wstring tableName, const wstring outputAttr, const wstring param, bool bGetAll);
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring tableName,const  wstring param) {return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName));}
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring tableName, const wstring param, bool bGetAll);
		vector<wstring> EvaluateTable(const wstring tableName, const wstring outputAttr) {return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName));}
		vector<wstring> EvaluateTable(const wstring tableName, const wstring outputAttr, bool bGetAll);
		map<wstring, vector<wstring> > EvaluateTable(const wstring tableName) {return EvaluateTable(tableName, TableIsGetAll(tableName));}
		map<wstring, vector<wstring> > EvaluateTable(const wstring tableName, bool bGetAll);
		wstring GetFirstTableResult(const wstring tableName, const wstring outputAttr);
		vector<wstring> ReverseEvaluateTable(const wstring tableName, const wstring inputAttr) {return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName));}
		vector<wstring> ReverseEvaluateTable(const wstring tableName, const wstring inputAttr, bool bGetAll);
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring tableName) {return ReverseEvaluateTable(tableName, TableIsGetAll(tableName));}
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring tableName, bool bGetAll);

		wstring GetEvalParameter() {return m_StateParameter;}
		void SetInputValues(MAPWSTRUINT values) 
		{
			m_GlobalInputAttrsValues = values;
			m_GlobalInputAttrsValues[L""] = EMPTY_STRING;
			m_GlobalInputAttrsValues[L"NULL"] = EXPLICIT_NULL_STRING;
		}
		void SetInputValue(wstring name, wstring value);
		void ResetTable(wstring tableName);

		vector<wstring> GetInputAttrs(wstring tableName) {return m_TableSet.GetInputAttrs(tableName);}
		vector<wstring> GetInputDependencies(wstring tableName) {return m_TableSet.GetInputDependencies(tableName);}
		vector<wstring> GetOutputAttrs(wstring tableName) {return m_TableSet.GetOutputAttrs(tableName);}
		vector<wstring> GetAllPossibleOutputs(wstring tableName, wstring outputName);

		//Translations
		wstring Localize(wstring baseValue, wstring locale) {return Translate(baseValue, L"", locale);}
		wstring DeLocalize(wstring localeValue);
		wstring Translate(wstring source, wstring sourceLocale, wstring destLocale);

		//ASCII overloads
		CKnowledgeBase(const string knowledge_file);
		bool CreateKnowledgeBase(const string knowledge_file);
		bool CreateKnowledgeBaseFromString(const string xmlStr);
		bool TableHasScript(const string tableName);
		bool TableIsGetAll(const string tableName);
		vector<string> EvaluateTableWithParam(const string tableName, const string outputAttr, const string param) {return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName));}
		vector<string> EvaluateTableWithParam(const string tableName, const string outputAttr, const string param, bool bGetAll);
		map<string, vector<string> > EvaluateTableWithParam(const string tableName, const string param) {return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName));}
		map<string, vector<string> > EvaluateTableWithParam(const string tableName, const string param, bool bGetAll);
		vector<string> EvaluateTable(const string tableName, const string outputAttr) {return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName));}
		vector<string> EvaluateTable(const string tableName, const string outputAttr, bool bGetAll);
		map<string, vector<string> > EvaluateTable(const string tableName) {return EvaluateTable(tableName, TableIsGetAll(tableName));}
		map<string, vector<string> > EvaluateTable(const string tableName, bool bGetAll);
		string GetFirstTableResult(const string tableName, const string outputAttr);
		vector<string> ReverseEvaluateTable(const string tableName, const string inputAttr) {return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName));}
		vector<string> ReverseEvaluateTable(const string tableName, const string inputAttr, bool bGetAll);
		map<string, vector<string> > ReverseEvaluateTable(const string tableName) {return ReverseEvaluateTable(tableName, TableIsGetAll(tableName));}
		map<string, vector<string> > ReverseEvaluateTable(const string tableName, bool bGetAll);
		string GetEvalParameterA();

		void SetInputValues(MAPSTRUINT values);
		void SetInputValue(const string name, const string value);
		size_t GetInputValuesCount() {return m_GlobalInputAttrsValues.size();}
		void ResetTable(const string tableName);
		vector<string> GetInputAttrs(const string tableName);
		vector<string> GetInputDependencies(const string tableName);
		vector<string> GetOutputAttrs(const string tableName);
		vector<string> GetAllPossibleOutputs(const string tableName, const string outputName);


	private:		
		bool _parseXML(Document xmlDocument);
		vector<pair<wstring, vector<CRuleCell> > > GetTableRowFromXML(NodeList nodes, Document xmlDocument);
		void SendToDebugServer(wstring msg);
		wstring XMLSafe(wstring str);
		bool DebugThisTable(wstring tableName);
		MAPWSTRUINT m_GlobalInputAttrsValues;
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
		MAPUINTMAP mapBaseIDtoTranslations;

	//helper functions
	#ifdef WIN32
	wstring VariantToWStr(const _variant_t mVar)
	{
		wstring retval = mVar.bstrVal;
		return retval;
	}
	#endif
	};
}
