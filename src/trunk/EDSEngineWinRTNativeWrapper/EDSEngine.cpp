/*
This file is part of EDSEngineWinRT.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

    EDSEngineWinRT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    EDSEngineWinRT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngineWinRT.  If not, see <http://www.gnu.org/licenses/>.
*/

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
	bool EDSEngine::CreateKnowledgeBase(String ^knowledge_file, int threads)
	{
		m_debugger = nullptr;
		m_KnowledgeBase = new EDS::CKnowledgeBase(knowledge_file->Data());

		if (m_KnowledgeBase)
		{
			m_KnowledgeBase->InputValueGetterPtr = [&](const wstring& attrName, void* context)
			{
				return _getValue(attrName, context);
			};
			return true;
		}		
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

	IVector<String^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, String^ outputAttr, bool bGetAll, String^ paramIn, String^* paramOut, Object^ context)
	{
		vector<wstring> retval;
		if (m_KnowledgeBase)
		{
			wstring para = paramIn->Data();
			retval = m_KnowledgeBase->EvaluateTableWithParam(tableName->Data(), outputAttr->Data(), bGetAll, para);
			*paramOut = ref new String(para.c_str());
		}

		return GetIVectorFromVectorStrings(retval);
	}
	
	IMap<String^, IVector<String^>^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, bool bGetAll, String^ paramIn, String^* paramOut, Object^ context)
	{
		map<wstring, vector<wstring> > retval;
		if (m_KnowledgeBase)
		{
			wstring para = paramIn->Data();
			retval = m_KnowledgeBase->EvaluateTableWithParam(tableName->Data(), bGetAll, para);
			*paramOut = ref new String(para.c_str());
		}

		return GetIMapFromMapStrings(retval);
	}

	IVector<String^>^ EDSEngine::EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll, Object^ context)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->EvaluateTable(tableName->Data(), outputAttr->Data(), bGetAll);
		}

		return GetIVectorFromVectorStrings(retval);
	}

	IMap<String^, IVector<String^>^>^ EDSEngine::EvaluateTable(String^ tableName, bool bGetAll, Object^ context)
	{
		map<wstring, vector<wstring> > retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->EvaluateTable(tableName->Data(), bGetAll);
		}

		return GetIMapFromMapStrings(retval);
	}


	String^ EDSEngine::GetFirstTableResult(String^ tableName, String^ outputAttr, Object^ context)
	{
		String^ retval = "";
		if (m_KnowledgeBase)
		{
			wstring res = m_KnowledgeBase->GetFirstTableResult(tableName->Data(), outputAttr->Data());
			retval = ref new String(res.c_str());
		}
		return retval;
	}

	
	IVector<String^>^ EDSEngine::ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll, Object^ context)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->ReverseEvaluateTable(tableName->Data(), inputAttr->Data(), bGetAll);
		}

		return GetIVectorFromVectorStrings(retval);
	}

	IMap<String^, IVector<String^>^>^ EDSEngine::ReverseEvaluateTable(String^ tableName, bool bGetAll, Object^ context)
	{
		map<wstring, vector<wstring> > retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->ReverseEvaluateTable(tableName->Data(), bGetAll);
		}

		return GetIMapFromMapStrings(retval);
	}

	IVector<String^>^ EDSEngine::GetInputAttrs(String^ tableName)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->GetInputAttrs(tableName->Data());
		}

		return GetIVectorFromVectorStrings(retval);
	}
	
	IVector<String^>^ EDSEngine::GetInputDependencies(String^ tableName)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->GetInputDependencies(tableName->Data());
		}

		return GetIVectorFromVectorStrings(retval);
	}
	
	IVector<String^>^ EDSEngine::GetOutputAttrs(String^ tableName)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->GetOutputAttrs(tableName->Data());
		}

		return GetIVectorFromVectorStrings(retval);
	}
	
	IVector<String^>^ EDSEngine::GetAllPossibleOutputs(String^ tableName, String^ outputName)
	{
		vector<wstring> retval;

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->GetAllPossibleOutputs(tableName->Data(), outputName->Data());
		}

		return GetIVectorFromVectorStrings(retval);
	}
	
	String^	EDSEngine::DeLocalize(String^ localeValue)
	{
		wstring retval = L"";

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->DeLocalize(localeValue->Data());
		}

		return ref new String(retval.c_str());
	}

	String^	EDSEngine::Translate(String^ source, String^ sourceLocale, String^ destLocale)
	{
		wstring retval = L"";

		if (m_KnowledgeBase)
		{
			retval = m_KnowledgeBase->Translate(source->Data(), sourceLocale->Data(), destLocale->Data());
		}

		return ref new String(retval.c_str());
	}

	wstring	EDSEngine::_getValue(const wstring& attrName, void* context)
	{
		wstring retval;
		if (InputGetterDelegate != nullptr)
		{
			Object^ ctxt = nullptr;
			if (context != nullptr)
			{
				ctxt = reinterpret_cast<Object^>(context);
			}
			String^ attrValue = InputGetterDelegate(ref new String(attrName.c_str()), ctxt);
			retval = attrValue->Data();
		}
		return retval;
	}

	void EDSEngine::_fireDebug(const wstring& msg)
	{
		if (DebugDelegate != nullptr)
		{
			DebugDelegate(ref new String(msg.c_str()));
		}
	}
}