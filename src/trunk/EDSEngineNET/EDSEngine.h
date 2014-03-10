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
// EDSEngine.h

#pragma once
#pragma unmanaged
#include "KnowledgeBase.h"
#include <vector>
#include <map>
#include "Marshal.h"
using namespace std;

typedef wstring(__stdcall *VALUECB)(const wstring&);
typedef void(__stdcall *DEBUGCB)(const wstring&);

#pragma managed
#using <mscorlib.dll>
#using <System.dll>
#include <vcclr.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace EDSNET {
	delegate void FireDebugMessageDelegate(const wstring&);
	delegate wstring GetTheValueDelegate(const wstring&);
	public delegate void DebugHandlerDelegate(String^ msg);
	public delegate String^ InputValueGetterDelegate(String^ attrName);

	public ref class EDSEngine
	{
	public:
		EDSEngine() { DebugDelegate = nullptr; InputGetterDelegate = nullptr; DebugDelegate = nullptr; }
		EDSEngine(String^ knowledge_file) {CreateKnowledgeBase(knowledge_file);}
		bool CreateKnowledgeBase(String^ knowledge_file);
		~EDSEngine() 
		{
			if (m_gchInput.IsAllocated)			
				m_gchInput.Free();
			if (m_gchDebug.IsAllocated)
				m_gchDebug.Free();
			this->!EDSEngine();
		}		
		!EDSEngine() {if (m_KnowledgeBase) delete m_KnowledgeBase; m_KnowledgeBase = NULL;}

		property DebugHandlerDelegate^			DebugDelegate
		{
			DebugHandlerDelegate^ get()
			{
				return m_debugger;
			}
			void set(DebugHandlerDelegate^ value)
			{
				m_debugger = value;
				if (m_KnowledgeBase != nullptr)
				{
					if (m_debugger != nullptr)
					{
						FireDebugMessageDelegate^ fp = gcnew FireDebugMessageDelegate(this, &EDSEngine::_fireDebug);
						m_gchDebug = GCHandle::Alloc(fp);
						IntPtr ip = Marshal::GetFunctionPointerForDelegate(fp);
						DEBUGCB cb = static_cast<DEBUGCB>(ip.ToPointer());
						m_KnowledgeBase->DebugHandlerPtr = cb;
					}
					else
					{
						m_KnowledgeBase->DebugHandlerPtr = nullptr;
					}
				}
			}
		}

		size_t									TableCount();
		bool									IsOpen();		
		bool									TableHasScript(String^ tableName);
		bool									TableIsGetAll(String^ tableName);

		InputValueGetterDelegate^				InputGetterDelegate;

		array<String^>^							EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^% param, bool bGetAll);		
		array<String^>^							EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^% param) {return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName));}
		Dictionary<String^,	array<String^>^>^	EvaluateTableWithParam(String^ tableName, String^% param) {return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName));}
		Dictionary<String^,	array<String^>^>^	EvaluateTableWithParam(String^ tableName, String^% param, bool bGetAll);
		array<String^>^							EvaluateTable(String^ tableName, String^ outputAttr) {return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName));}
		array<String^>^							EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll);
		Dictionary<String^,	array<String^>^>^	EvaluateTable(String^ tableName) {return EvaluateTable(tableName, TableIsGetAll(tableName));}
		Dictionary<String^,	array<String^>^>^	EvaluateTable(String^ tableName, bool bGetAll);		
		String^									GetFirstTableResult(String^ tableName, String^ ouputAttr);
		array<String^>^							ReverseEvaluateTable(String^ tableName, String^ inputAttr) {return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName));}
		array<String^>^							ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll);
		Dictionary<String^,	array<String^>^>^	ReverseEvaluateTable(String^ tableName) {return ReverseEvaluateTable(tableName, TableIsGetAll(tableName));}
		Dictionary<String^,	array<String^>^>^	ReverseEvaluateTable(String^ tableName, bool bGetAll);	

		array<String^>^							GetInputAttrs(String^ tableName);
		array<String^>^							GetInputDependencies(String^ tableName);
		array<String^>^							GetOutputAttrs(String^ tableName);
		array<String^>^							GetAllPossibleOutputs(String^ tableName, String^ outputName);

		String^									Localize(String^ baseValue, String^ locale) {return Translate(baseValue, gcnew String(""), locale);}
		String^									DeLocalize(String^ localeValue);
		String^									Translate(String^ source, String^ sourceLocale, String^ destLocale);

	private:		
		DebugHandlerDelegate^					m_debugger;
		EDS::CKnowledgeBase						*m_KnowledgeBase;
		GCHandle								m_gchInput, m_gchDebug;
		wstring									_getValue(const wstring& attrName);
		void									_fireDebug(const wstring& msg);
	};
}
