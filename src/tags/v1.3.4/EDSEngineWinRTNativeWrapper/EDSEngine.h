#pragma once
#include "KnowledgeBase.h"
#include <vector>
#include <map>
using namespace std;
using namespace Platform;
using namespace Windows::Foundation::Collections;

namespace EDSEngineWinRT
{
	public delegate void DebugHandlerDelegate(String^ msg);

    public ref class EDSEngine sealed
    {
    public:
        EDSEngine() {m_DebugDelegate = nullptr;}
		EDSEngine(String^ knowledge_file) {CreateKnowledgeBase(knowledge_file);}
		bool CreateKnowledgeBase(String^ knowledge_file);
		virtual ~EDSEngine() {if (m_KnowledgeBase) delete m_KnowledgeBase; m_KnowledgeBase = nullptr;}

		
		void								SetDebugging(bool set) {if (m_KnowledgeBase) m_KnowledgeBase->GenerateDebugMessages(set);}
		void								PumpDebugMessages();
		void								SetDebugHandler(DebugHandlerDelegate^ funct) {m_DebugDelegate = funct;}

		size_t								TableCount();
		bool								IsOpen();		
		bool								TableHasScript(String^ tableName);
		bool								TableIsGetAll(String^ tableName);

		IVector<String^>^					EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^ param, bool bGetAll);	
		IVector<String^>^					EvaluateTableWithParam(String^ tableName, String^ outputAttr, String^ param) {return EvaluateTableWithParam(tableName, outputAttr, param, TableIsGetAll(tableName));}
		IMap<String^,	IVector<String^>^>^	EvaluateTableWithParam(String^ tableName, String^ param) {return EvaluateTableWithParam(tableName, param, TableIsGetAll(tableName));}
		IMap<String^,	IVector<String^>^>^	EvaluateTableWithParam(String^ tableName, String^ param, bool bGetAll);
		IVector<String^>^					EvaluateTable(String^ tableName, String^ outputAttr) {return EvaluateTable(tableName, outputAttr, TableIsGetAll(tableName));}
		IVector<String^>^					EvaluateTable(String^ tableName, String^ outputAttr, bool bGetAll);
		IMap<String^,	IVector<String^>^>^	EvaluateTable(String^ tableName) {return EvaluateTable(tableName, TableIsGetAll(tableName));}
		IMap<String^,	IVector<String^>^>^	EvaluateTable(String^ tableName, bool bGetAll);		
		String^								GetFirstTableResult(String^ tableName, String^ ouputAttr);
		IVector<String^>^					ReverseEvaluateTable(String^ tableName, String^ inputAttr) {return ReverseEvaluateTable(tableName, inputAttr, TableIsGetAll(tableName));}
		IVector<String^>^					ReverseEvaluateTable(String^ tableName, String^ inputAttr, bool bGetAll);
		IMap<String^,	IVector<String^>^>^	ReverseEvaluateTable(String^ tableName) {return ReverseEvaluateTable(tableName, TableIsGetAll(tableName));}
		IMap<String^,	IVector<String^>^>^	ReverseEvaluateTable(String^ tableName, bool bGetAll);	
		String^								GetEvalParameter();
		void								SetInputValues(IMap<String^, size_t>^ values);
		void								SetInputValue(String^ name, String^ value);
		void								ResetTable(String^ tableName);

		IVector<String^>^					GetInputAttrs(String^ tableName);
		IVector<String^>^					GetInputDependencies(String^ tableName);
		IVector<String^>^					GetOutputAttrs(String^ tableName);
		IVector<String^>^					GetAllPossibleOutputs(String^ tableName, String^ outputName);

		String^								Localize(String^ baseValue, String^ locale) {return Translate(baseValue, "", locale);}
		String^								DeLocalize(String^ localeValue);
		String^								Translate(String^ source, String^ sourceLocale, String^ destLocale);

	private:		
		DebugHandlerDelegate^				m_DebugDelegate;
		EDS::CKnowledgeBase					*m_KnowledgeBase;							
    };
}