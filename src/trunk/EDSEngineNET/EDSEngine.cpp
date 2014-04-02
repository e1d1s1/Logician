/*
This file is part of EDSEngineNET.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

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
#include <functional>

using namespace std;
using namespace msclr::interop;
using namespace System::Runtime::InteropServices;

namespace EDSNET
{
	bool EDSEngine::CreateKnowledgeBase(System::String ^knowledge_file)
	{
		wstring file = MarshalString(knowledge_file);
		DebugDelegate = nullptr;
		m_KnowledgeBase = new EDS::CKnowledgeBase(file);	

		GetTheValueDelegate^ fp = gcnew GetTheValueDelegate(this, &EDSEngine::_getValue);
		m_gchInput = GCHandle::Alloc(fp);
		IntPtr ip = Marshal::GetFunctionPointerForDelegate(fp);
		VALUECB cb = static_cast<VALUECB>(ip.ToPointer());	

		if (m_KnowledgeBase)
		{			
			m_KnowledgeBase->InputValueGetterPtr = cb;
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
			wstring table = MarshalString(tableName);
			return m_KnowledgeBase->TableHasScript(table);
		}
		else
			return false;
	}

	bool EDSEngine::TableIsGetAll(String^ tableName)
	{
		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			return m_KnowledgeBase->TableIsGetAll(table);
		}
		else
			return false;
	}

	array<String^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, String^ outputAttr, bool bGetAll, String^% param, Object^ context)
	{
		array<String^>^ retval = nullptr;
		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			wstring output = MarshalString(outputAttr);
			wstring para = MarshalString(param);
			void* voidPtr = nullptr;
			if (context)
				voidPtr = GCHandle::ToIntPtr(GCHandle::Alloc(context)).ToPointer();
			vector<wstring> res = m_KnowledgeBase->EvaluateTableWithParam(table, output, bGetAll, para, voidPtr);
			retval = GetArrayFromVectorStrings(res);
			param = gcnew String(para.c_str());
		}

		return retval;
	}

	Dictionary<String^, array<String^>^>^ EDSEngine::EvaluateTableWithParam(String^ tableName, bool bGetAll, String^% param, Object^ context)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;
		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			wstring para = MarshalString(param);
			void* voidPtr = nullptr;
			if (context)
				voidPtr = GCHandle::ToIntPtr(GCHandle::Alloc(context)).ToPointer();
			map<wstring, vector<wstring> > res = m_KnowledgeBase->EvaluateTableWithParam(table, bGetAll, para, voidPtr);
			retval = GetDictionaryFromMapStrings(res);
			param = gcnew String(para.c_str());
		}
		return retval;
	}

	array<String^>^ EDSEngine::EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll, Object^ context)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			wstring output = MarshalString(outputAttr);
			void* voidPtr = nullptr;
			if (context)
				voidPtr = GCHandle::ToIntPtr(GCHandle::Alloc(context)).ToPointer();
			vector<wstring> res = m_KnowledgeBase->EvaluateTable(table, output, bGetAll, voidPtr);
			retval = GetArrayFromVectorStrings(res);
		}

		return retval;
	}

	Dictionary<String^, array<String^>^>^ EDSEngine::EvaluateTable(String^ tableName, bool bGetAll, Object^ context)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			void* voidPtr = nullptr;
			if (context)
				voidPtr = GCHandle::ToIntPtr(GCHandle::Alloc(context)).ToPointer();
			map<wstring, vector<wstring> > res = m_KnowledgeBase->EvaluateTable(table, bGetAll, voidPtr);
			retval = GetDictionaryFromMapStrings(res);
		}

		return retval;
	}

	String^ EDSEngine::GetFirstTableResult(String^ tableName, String^ outputAttr, Object^ context)
	{
		String^ retval = gcnew String("");
		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			wstring output = MarshalString(outputAttr);
			void* voidPtr = nullptr;
			if (context)
				voidPtr = GCHandle::ToIntPtr(GCHandle::Alloc(context)).ToPointer();
			wstring res = m_KnowledgeBase->GetFirstTableResult(table, output, voidPtr);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	array<String^>^	EDSEngine::ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll, Object^ context)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			wstring input = MarshalString(inputAttr);
			void* voidPtr = nullptr;
			if (context)
				voidPtr = GCHandle::ToIntPtr(GCHandle::Alloc(context)).ToPointer();
			vector<wstring> res = m_KnowledgeBase->ReverseEvaluateTable(table, input, bGetAll, voidPtr);
			retval = GetArrayFromVectorStrings(res);
		}

		return retval;
	}

	Dictionary<String^, array<String^>^>^ EDSEngine::ReverseEvaluateTable(String^ tableName, bool bGetAll, Object^ context)
	{
		Dictionary<String^,	array<String^>^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
			void* voidPtr = nullptr;
			if (context)
				voidPtr = GCHandle::ToIntPtr(GCHandle::Alloc(context)).ToPointer();
			map<wstring, vector<wstring> > res = m_KnowledgeBase->ReverseEvaluateTable(table, bGetAll, voidPtr);
			retval = GetDictionaryFromMapStrings(res);
		}

		return retval;
	}

	array<String^>^ EDSEngine::GetInputAttrs(String^ tableName)
	{
		array<String^>^ retval = nullptr;

		if (m_KnowledgeBase)
		{
			wstring table = MarshalString(tableName);
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
			wstring table = MarshalString(tableName);
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
			wstring table = MarshalString(tableName);
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
			wstring table = MarshalString(tableName);
			wstring output = MarshalString(outputName);
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
			wstring val = MarshalString(localeValue);
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
			wstring src = MarshalString(source);
			wstring srcLocale = MarshalString(sourceLocale);
			wstring dest = MarshalString(destLocale);
			wstring res = m_KnowledgeBase->Translate(src, srcLocale, dest);
			retval = gcnew String(res.c_str());
		}

		return retval;
	}

	wstring	EDSEngine::_getValue(const wstring& attrName, void* context)
	{
		wstring retval;
		if (InputGetterDelegate != nullptr)
		{
			Object^ ctxt = nullptr;
			if (context != nullptr)
			{
				GCHandle handle = GCHandle::FromIntPtr(IntPtr(context));
				ctxt = handle.Target;
			}
			String^ attrValue = InputGetterDelegate(gcnew String(attrName.c_str()), ctxt);
			retval = MarshalString(attrValue);
		}
		return retval;
	}

	void EDSEngine::_fireDebug(const wstring& msg)
	{
		if (DebugDelegate != nullptr)
		{
			DebugDelegate(gcnew String(msg.c_str()));
		}
	}
}