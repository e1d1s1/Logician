// Class1.cpp
#include "pch.h"
#include "EDSEngine.h"
#include "Marshall.h"
#include <collection.h>
#include <algorithm>

using namespace EDSEngineWinRT;
using namespace Platform;
using namespace Platform::Collections;

namespace EDSEngineWinRT
{
	bool EDSEngine::CreateKnowledgeBase(String ^knowledge_file)
	{
		m_DebugDelegate = nullptr;
		m_KnowledgeBase = new EDS::CKnowledgeBase(knowledge_file->Data());		
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
			return m_KnowledgeBase->TableHasScript(tableName->Data());
		}
		else
			return false;
	}

	bool EDSEngine::TableIsGetAll(String^ tableName)
	{
		if (m_KnowledgeBase)
		{
			return m_KnowledgeBase->TableIsGetAll(tableName->Data());
		}
		else
			return false;
	}

	IVector<String^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^ param, bool bGetAll)
	{
		vector<wstring> retval;
		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->EvaluateTableWithParam(tableName->Data(), outputAttr->Data(), param->Data(), bGetAll);
			PumpDebugMessages();
		}

		return GetIVectorFromVectorStrings(retval);
	}
	
	IMap<String^, IVector<String^>^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, String^ param, bool bGetAll)
	{
		map<wstring, vector<wstring> > retval;
		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->EvaluateTableWithParam(tableName->Data(), param->Data(), bGetAll);
			PumpDebugMessages();
		}
		return GetIMapFromMapStrings(retval);
	}

	IVector<String^>^ EDSEngine::EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->EvaluateTable(tableName->Data(), outputAttr->Data(), bGetAll);
			PumpDebugMessages();
		}

		return GetIVectorFromVectorStrings(retval);
	}

	IMap<String^, IVector<String^>^>^ EDSEngine::EvaluateTable(String^ tableName, bool bGetAll)
	{
		map<wstring, vector<wstring> > retval;

		if (m_KnowledgeBase)
		{
			map<wstring, vector<wstring> > res = m_KnowledgeBase->EvaluateTable(tableName->Data(), bGetAll);
			PumpDebugMessages();
		}

		return GetIMapFromMapStrings(retval);
	}


	String^ EDSEngine::GetFirstTableResult(String^ tableName, String^ outputAttr)
	{
		String^ retval = "";
		if (m_KnowledgeBase)
		{
			wstring res = m_KnowledgeBase->GetFirstTableResult(tableName->Data(), outputAttr->Data());
			retval = ref new String(res.c_str());
		}
		return retval;
	}

	
	IVector<String^>^ EDSEngine::ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->ReverseEvaluateTable(tableName->Data(), inputAttr->Data(), bGetAll);
			PumpDebugMessages();
		}

		return GetIVectorFromVectorStrings(retval);
	}

	IMap<String^, IVector<String^>^>^ EDSEngine::ReverseEvaluateTable(String^ tableName, bool bGetAll)
	{
		map<wstring, vector<wstring> > retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->ReverseEvaluateTable(tableName->Data(), bGetAll);
			PumpDebugMessages();
		}

		return GetIMapFromMapStrings(retval);
	}

	String^	EDSEngine::GetEvalParameter()
	{
		String^ retval = "";

		if (m_KnowledgeBase)
		{
			wstring param = m_KnowledgeBase->GetEvalParameter();
			retval = ref new String(param.c_str());
		}

		return retval;
	}
	
	void EDSEngine::SetInputValues(IMap<String^, size_t>^ values)
	{
		if (m_KnowledgeBase)
		{
			stdext::hash_map<wstring, size_t> hash_values;
			MarshalIMapStringUInt(values, hash_values);
			m_KnowledgeBase->SetInputValues(hash_values);
		}
	}
/*
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
		if (m_KnowledgeBase != NULL && m_DebugDelegate != nullptr)
		{
			wstring msg = m_KnowledgeBase->GetDebugMessages();
			if (msg.length() > 0)
				m_DebugDelegate(String(msg.c_str()));
		}
	}*/
}