/*
This file is part of EDSEngineNET.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule LLC

    EDSEngineNET is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    EDSEngineNET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
// This is the main DLL file.

#include "stdafx.h"

#include "EDSEngine.h"
#include <vector>
#include <map>
#include <hash_map>

using namespace std;

namespace EDSNET
{
	bool EDSEngine::CreateKnowledgeBase(System::String ^knowledge_file)
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

	size_t EDSEngine::TableCount()
	{
		if (m_KnowledgeBase)
		{
			return m_KnowledgeBase->TableCount();
		}
		else
			return 0;
	}

	bool EDSEngine::IsOpen()
	{
		if (m_KnowledgeBase)
			return m_KnowledgeBase->IsOpen();
		else 
			return false;
	}

	bool EDSEngine::TableHasScript(String^ tableName)
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

	bool EDSEngine::TableIsGetAll(String^ tableName)
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

	array<String^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^ param, bool bGetAll)
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
			PumpDebugMessages();
		}

		return retval;
	}

	Dictionary<String^,	array<String^>^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, String^ param, bool bGetAll)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;
		if (m_KnowledgeBase)
		{
			wstring table, para;
			MarshalString(tableName, table);
			MarshalString(param, para);
			map<wstring, vector<wstring> > res = m_KnowledgeBase->EvaluateTableWithParam(table, para, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	array<String^>^ EDSEngine::EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table, output;
			MarshalString(tableName, table);
			MarshalString(outputAttr, output);
			vector<wstring> res = m_KnowledgeBase->EvaluateTable(table, output, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}

		return retval;
	}

	Dictionary<String^,	array<String^>^>^ EDSEngine::EvaluateTable(String^ tableName, bool bGetAll)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			map<wstring, vector<wstring> > res = m_KnowledgeBase->EvaluateTable(table, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}

		return retval;
	}

	String^ EDSEngine::GetFirstTableResult(String^ tableName, String^ outputAttr)
	{
		String^ retval = gcnew String("");
		if (m_KnowledgeBase)
		{
			wstring table, output;
			MarshalString(tableName, table);
			MarshalString(outputAttr, output);
			wstring res = m_KnowledgeBase->GetFirstTableResult(table, output);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	array<String^>^	EDSEngine::ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table, input;
			MarshalString(tableName, table);
			MarshalString(inputAttr, input);
			vector<wstring> res = m_KnowledgeBase->ReverseEvaluateTable(table, input, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}

		return retval;
	}

	Dictionary<String^,	array<String^>^>^ EDSEngine::ReverseEvaluateTable(String^ tableName, bool bGetAll)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			map<wstring, vector<wstring> > res = m_KnowledgeBase->ReverseEvaluateTable(table, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}

		return retval;
	}

	String^	EDSEngine::GetEvalParameter()
	{
		String^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring param = m_KnowledgeBase->GetEvalParameter();
			retval = gcnew String(param.c_str());
		}

		return retval;
	}

	void EDSEngine::SetInputValues(Dictionary<String^, size_t>^ values)
	{
		if (m_KnowledgeBase)
		{
			stdext::hash_map<wstring, size_t> hash_values;
			MarshalDictionaryStringUInt(values, hash_values);
			m_KnowledgeBase->SetInputValues(hash_values);
		}
	}

	void EDSEngine::SetInputValue(String^ name, String^ value)
	{
		if (m_KnowledgeBase)
		{
			wstring _name, _value;
			MarshalString(name, _name);
			MarshalString(value, _value);
			m_KnowledgeBase->SetInputValue(_name, _value);
		}
	}

	void EDSEngine::ResetTable(String^ tableName)
	{
		if (m_KnowledgeBase)
		{
			wstring table;
			MarshalString(tableName, table);
			m_KnowledgeBase->ResetTable(table);
		}
	}

	array<String^>^ EDSEngine::GetInputAttrs(String^ tableName)
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

	array<String^>^ EDSEngine::GetInputDependencies(String^ tableName)
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

	array<String^>^ EDSEngine::GetOutputAttrs(String^ tableName)
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

	array<String^>^ EDSEngine::GetAllPossibleOutputs(String^ tableName, String^ outputName)
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

	String^	EDSEngine::DeLocalize(String^ localeValue)
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

	String^	EDSEngine::Translate(String^ source, String^ sourceLocale, String^ destLocale)
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

	void EDSEngine::PumpDebugMessages()
	{
		if (m_KnowledgeBase != NULL && DebugDelegate != nullptr)
		{
			wstring msg = m_KnowledgeBase->GetDebugMessages();
			if (msg.length() > 0)
				DebugDelegate(gcnew String(msg.c_str()));
		}
	}
}