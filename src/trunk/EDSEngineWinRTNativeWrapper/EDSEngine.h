#pragma once
#include "KnowledgeBase.h"
#include <vector>
#include <map>
#include <functional>

using namespace std;
using namespace Platform;
using namespace Windows::Foundation::Collections;

namespace EDSEngineWinRT
{
	delegate void FireDebugMessageDelegate(const wstring&);
	delegate wstring GetTheValueDelegate(const wstring&, void* context);

	public delegate void DebugHandlerDelegate(String^ msg);
	public delegate String^ InputValueGetterDelegate(String^ attrName, Object^ context);

    public ref class EDSEngine sealed
    {
    public:
		EDSEngine() { m_debugger = nullptr; InputGetterDelegate = nullptr; }
		EDSEngine(String^ knowledge_file) { CreateKnowledgeBase(knowledge_file); }
		bool CreateKnowledgeBase(String^ knowledge_file);
		virtual ~EDSEngine() {if (m_KnowledgeBase) delete m_KnowledgeBase; m_KnowledgeBase = nullptr;}

		
		property DebugHandlerDelegate^	DebugDelegate
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
						function<void(const wstring&)> debugDelegate = [value](const wstring& msg)
						{
							value(ref new String(msg.c_str()));
						};
						m_KnowledgeBase->DebugHandlerPtr = debugDelegate;
					}
					else
					{
						m_KnowledgeBase->DebugHandlerPtr = nullptr;
					}
				}
			}
		}
		void								EnableRemoteDebugger(bool enable) { if (m_KnowledgeBase) m_KnowledgeBase->EnableRemoteDebugger(enable); }

		size_t								TableCount();
		bool								IsOpen();		
		bool								TableHasScript(String^ tableName);
		bool								TableIsGetAll(String^ tableName);

		property InputValueGetterDelegate^	InputGetterDelegate;

		IVector<String^>^					EvaluateTableWithParam(String^ tableName, String^ outputAttr, bool bGetAll, String^ paramIn, String^* paramOut, Object^ context);
		IVector<String^>^					EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^ paramIn, String^* paramOut, Object^ context) { return EvaluateTableWithParam(tableName, outputAttr, TableIsGetAll(tableName), paramIn, paramOut, context); }
		IMap<String^, IVector<String^>^>^	EvaluateTableWithParam(String^ tableName, String^ paramIn, String^* paramOut, Object^ context) { return EvaluateTableWithParam(tableName, TableIsGetAll(tableName), paramIn, paramOut, context); }
		IMap<String^, IVector<String^>^>^	EvaluateTableWithParam(String^ tableName, bool bGetAll, String^ paramIn, String^* paramOut, Object^ context);
		IVector<String^>^					EvaluateTable(String^ tableName, String^ outputAttr, Object^ context) { return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName), context); }
		IVector<String^>^					EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll, Object^ context);
		IMap<String^, IVector<String^>^>^	EvaluateTable(String^ tableName, Object^ context) { return EvaluateTable(tableName, TableIsGetAll(tableName), context); }
		IMap<String^, IVector<String^>^>^	EvaluateTable(String^ tableName, bool bGetAll, Object^ context);
		String^								GetFirstTableResult(String^ tableName, String^ ouputAttr, Object^ context);
		IVector<String^>^					ReverseEvaluateTable(String^ tableName, String^ inputAttr, Object^ context) { return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName), context); }
		IVector<String^>^					ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll, Object^ context);
		IMap<String^, IVector<String^>^>^	ReverseEvaluateTable(String^ tableName, Object^ context) { return ReverseEvaluateTable(tableName, TableIsGetAll(tableName), context); }
		IMap<String^, IVector<String^>^>^	ReverseEvaluateTable(String^ tableName, bool bGetAll, Object^ context);

		IVector<String^>^					EvaluateTableWithParam(String^ tableName, String^ outputAttr, bool bGetAll, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(tableName, outputAttr, bGetAll, paramIn, paramOut, nullptr); }
		IVector<String^>^					EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(tableName, outputAttr, TableIsGetAll(tableName), paramIn, paramOut, nullptr); }
		IMap<String^, IVector<String^>^>^	EvaluateTableWithParam(String^ tableName, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(tableName, TableIsGetAll(tableName), paramIn, paramOut, nullptr); }
		IMap<String^, IVector<String^>^>^	EvaluateTableWithParam(String^ tableName, bool bGetAll, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(tableName, bGetAll, paramIn, paramOut, nullptr); }
		IVector<String^>^					EvaluateTable(String^ tableName, String^ outputAttr) { return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName), nullptr); }
		IVector<String^>^					EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll) { return EvaluateTable(tableName, outputAttr, bGetAll, nullptr); }
		IMap<String^, IVector<String^>^>^	EvaluateTable(String^ tableName) { return EvaluateTable(tableName, TableIsGetAll(tableName), nullptr); }
		IMap<String^, IVector<String^>^>^	EvaluateTable(String^ tableName, bool bGetAll) { return EvaluateTable(tableName, bGetAll, nullptr); }
		String^								GetFirstTableResult(String^ tableName, String^ ouputAttr) { return GetFirstTableResult(tableName, ouputAttr, nullptr); }
		IVector<String^>^					ReverseEvaluateTable(String^ tableName, String^ inputAttr) { return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName), nullptr); }
		IVector<String^>^					ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll) { return ReverseEvaluateTable(tableName, inputAttr, bGetAll, nullptr); }
		IMap<String^, IVector<String^>^>^	ReverseEvaluateTable(String^ tableName) { return ReverseEvaluateTable(tableName, TableIsGetAll(tableName), nullptr); }
		IMap<String^, IVector<String^>^>^	ReverseEvaluateTable(String^ tableName, bool bGetAll) { return ReverseEvaluateTable(tableName, bGetAll, nullptr); }

		IVector<String^>^					GetInputAttrs(String^ tableName);
		IVector<String^>^					GetInputDependencies(String^ tableName);
		IVector<String^>^					GetOutputAttrs(String^ tableName);
		IVector<String^>^					GetAllPossibleOutputs(String^ tableName, String^ outputName);

		String^								Localize(String^ baseValue, String^ locale) {return Translate(baseValue, "", locale);}
		String^								DeLocalize(String^ localeValue);
		String^								Translate(String^ source, String^ sourceLocale, String^ destLocale);

	private:		
		wstring								_getValue(const wstring& attrName, void* context);
		void								_fireDebug(const wstring& msg);

		DebugHandlerDelegate^				m_debugger;
		EDS::CKnowledgeBase					*m_KnowledgeBase;							
    };
}