/*
This file is part of EDSEngineNET.
Copyright (C) 2009 - 2011 Eric D. Schmidt

    EDSEngineNET is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EDSEngineNET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngineNET.  If not, see <http://www.gnu.org/licenses/>.
*/
// This is the main DLL file.

#include "stdafx.h"

#include "EDSEngineNET.h"
#include <vector>
#include <map>
#include <hash_map>

using namespace std;

namespace EDSNET
{
	bool EDSEngineNET::CreateKnowledgeBase(System::String ^knowledge_file)
	{
		wstring file;
		MarshalString(knowledge_file, file);
		DebugDelegate = nullptr;
		m_KnowledgeBase = new EDS::CKnowledgeBase(file);		
		if (m_KnowledgeBase)
			return true;
		else
			return false;
	}

	size_t EDSEngineNET::TableCount()
	{
		if (m_KnowledgeBase)
		{
			return m_KnowledgeBase->TableCount();
		}
		else
			return 0;
	}

	bool EDSEngineNET::IsOpen()
	{
		if (m_KnowledgeBase)
			return m_KnowledgeBase->IsOpen();
		else 
			return false;
	}

	bool EDSEngineNET::TableHasScript(String^ tableName)
	{
		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			return m_KnowledgeBase->TableHasScript(table);
		}
		else
			return false;
	}

	bool EDSEngineNET::TableIsGetAll(String^ tableName)
	{
		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			return m_KnowledgeBase->TableIsGetAll(table);
		}
		else
			return false;
	}

	array<String^>^ EDSEngineNET::EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^ param, bool bGetAll)
	{
		array<String^>^ retval = nullptr;
		if (m_KnowledgeBase)
		{
			wstring table, output, para;
			MarshalString(tableName, table);
			MarshalString(outputAttr, output);
			MarshalString(param, para);
			vector<wstring> res = m_KnowledgeBase->EvaluateTableWithParam(table, output, para, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			Debug();
		}

		return retval;
	}

	Dictionary<String^,	array<String^>^>^ EDSEngineNET::EvaluateTableWithParam(String^ tableName, String^ param, bool bGetAll)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;
		if (m_KnowledgeBase)
		{
			wstring table, para;
			MarshalString(tableName, table);
			MarshalString(param, para);
			map<wstring, vector<wstring> > res = m_KnowledgeBase->EvaluateTableWithParam(table, para, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			Debug();
		}
		return retval;
	}

	array<String^>^ EDSEngineNET::EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table, output;
			MarshalString(tableName, table);
			MarshalString(outputAttr, output);
			vector<wstring> res = m_KnowledgeBase->EvaluateTable(table, output, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			Debug();
		}

		return retval;
	}

	Dictionary<String^,	array<String^>^>^ EDSEngineNET::EvaluateTable(String^ tableName, bool bGetAll)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			map<wstring, vector<wstring> > res = m_KnowledgeBase->EvaluateTable(table, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			Debug();
		}

		return retval;
	}

	array<String^>^	EDSEngineNET::ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table, input;
			MarshalString(tableName, table);
			MarshalString(inputAttr, input);
			vector<wstring> res = m_KnowledgeBase->ReverseEvaluateTable(table, input, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			Debug();
		}

		return retval;
	}

	Dictionary<String^,	array<String^>^>^ EDSEngineNET::ReverseEvaluateTable(String^ tableName, bool bGetAll)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			map<wstring, vector<wstring> > res = m_KnowledgeBase->ReverseEvaluateTable(table, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			Debug();
		}

		return retval;
	}

	String^	EDSEngineNET::GetEvalParameter()
	{
		String^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring param = m_KnowledgeBase->GetEvalParameter();
			retval = gcnew String(param.c_str());
		}

		return retval;
	}

	void EDSEngineNET::SetInputValues(Dictionary<String^, size_t>^ values)
	{
		if (m_KnowledgeBase)
		{
			stdext::hash_map<wstring, size_t> hash_values;
			MarshalDictionaryStringUInt(values, hash_values);
			m_KnowledgeBase->SetInputValues(hash_values);
		}
	}

	void EDSEngineNET::SetInputValue(String^ name, String^ value)
	{
		if (m_KnowledgeBase)
		{
			wstring _name, _value;
			MarshalString(name, _name);
			MarshalString(value, _value);
			m_KnowledgeBase->SetInputValue(_name, _value);
		}
	}

	void EDSEngineNET::ResetTable(String^ tableName)
	{
		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			m_KnowledgeBase->ResetTable(table);
		}
	}

	array<String^>^ EDSEngineNET::GetInputAttrs(String^ tableName)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			vector<wstring> res = m_KnowledgeBase->GetInputAttrs(table);
			retval = GetArrayFromVectorStrings(res);
		}

		return retval;
	}

	array<String^>^ EDSEngineNET::GetInputDependencies(String^ tableName)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			vector<wstring> res = m_KnowledgeBase->GetInputDependencies(table);
			retval = GetArrayFromVectorStrings(res);
		}

		return retval;
	}

	array<String^>^ EDSEngineNET::GetOutputAttrs(String^ tableName)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			vector<wstring> res = m_KnowledgeBase->GetOutputAttrs(table);
			retval = GetArrayFromVectorStrings(res);
		}

		return retval;
	}

	array<String^>^ EDSEngineNET::GetAllPossibleOutputs(String^ tableName, String^ outputName)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table, output;
			MarshalString(tableName, table);
			MarshalString(outputName, output);
			vector<wstring> res = m_KnowledgeBase->GetAllPossibleOutputs(table, output);
			retval = GetArrayFromVectorStrings(res);
		}

		return retval;
	}

	String^	EDSEngineNET::DeLocalize(String^ localeValue)
	{
		String^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring val;
			MarshalString(localeValue, val);
			wstring res = m_KnowledgeBase->DeLocalize(val);
			retval = gcnew String(res.c_str());
		}

		return retval;
	}

	String^	EDSEngineNET::Translate(String^ source, String^ sourceLocale, String^ destLocale)
	{
		String^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring src, srcLocale, dest;
			MarshalString(source, src);
			MarshalString(sourceLocale, srcLocale);
			MarshalString(destLocale, dest);
			wstring res = m_KnowledgeBase->Translate(src, srcLocale, dest);
			retval = gcnew String(res.c_str());
		}

		return retval;
	}

	void EDSEngineNET::Debug()
	{
		if (m_KnowledgeBase != NULL && DebugDelegate != nullptr)
		{
			wstring msg = m_KnowledgeBase->GetDebugMessages();
			if (msg.length() > 0)
				DebugDelegate(gcnew String(msg.c_str()));
		}
	}
}