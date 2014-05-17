#pragma once
#include <collection.h>
#include <algorithm>
#include <functional>

#include "Marshall.h"
#include "ROMNode.h"
#include "ROMDictionaryAttribute.h"
#include "ROMDictionary.h"
#include "LinearEngine.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

namespace ROMWinRT
{
	delegate void FireDebugMessageDelegate(const wstring&);
	delegate wstring GetTheValueDelegate(const wstring&, void* context);

	public delegate void DebugHandlerDelegate(String^ msg);
	public delegate String^ InputValueGetterDelegate(String^ attrName, Object^ context);

	public ref class ROMNode sealed
	{
	public:
		ROMNode() {CreateROMNode(""); m_KnowledgeBase = NULL;}		
		ROMNode(String^ id) {CreateROMNode(id);}
		bool CreateROMNode(String^ id);
		virtual ~ROMNode() {DestroyROMObject();}

		//debugger
		property DebugHandlerDelegate^	DebugDelegate
		{
			DebugHandlerDelegate^ get()
			{
				return m_debugger;
			}
			void set(DebugHandlerDelegate^ value)
			{
				m_debugger = value;
				if (m_ROMNode != nullptr)
				{
					if (m_debugger != nullptr)
					{
						function<void(const wstring&)> debugDelegate = [value](const wstring& msg)
						{
							value(ref new String(msg.c_str()));
						};
						m_ROMNode->SetTableDebugHandler(debugDelegate);
					}
					else
					{
						m_ROMNode->SetTableDebugHandler(nullptr);
					}
				}
			}
		}
		void				EnableRemoteDebugger(bool enable) { if (m_ROMNode) m_ROMNode->EnableRemoteDebugger(enable); }


		//relational functions
		ROMNode^			GetRoot();
		ROMNode^			Parent();
		IVector<ROMNode^>^	GetAllChildren(bool recurs);
		IVector<ROMNode^>^	FindObjects(String^ xpath);
		IVector<ROMNode^>^	FindAllObjectsByID(String^ id, bool recurs);
		ROMNode^			FindObjectByGUID(String^ guid);
		bool				AddChildROMObject(ROMNode^ child);
		bool				RemoveChildROMObject(ROMNode^ child);
		bool				RemoveFromParent();
		IVector<ROMNode^>^	GetAllFriends();
		bool				AddFriend(ROMNode^ friendObj);
		bool				RemoveFriend(ROMNode^ friendObj);
		bool				RemoveAllFriends();
		bool				DestroyROMObject();
		ROMNode^			Clone();

		//attribute functions
		String^				GetAttribute(String^ id, String^ name, bool immediate);
		String^				GetAttribute(String^ id, bool immediate) {return GetAttribute(id, "value", immediate);}
		String^				GetAttribute(String^ id) {return GetAttribute(id, "value", false);}
		bool				GetAttributeExists(String^ id, String^ name);
		bool				GetAttributeExists(String^ id) {return GetAttributeExists(id, "value");}
		bool				SetAttribute(String^ id, String^ name, String^ value);
		bool				SetAttribute(String^ id, String^ value) {return SetAttribute(id, L"value", value);}
		bool				RemoveAttribute(String^ id, String^ name);
		bool				RemoveAttribute(String^ id) {return RemoveAttribute(id, "value");} 	
		bool				SetROMObjectValue(String^ name, String^ value);
		String^				GetROMObjectValue(String^ name);
		bool				RemoveROMObjectValue(String^ name);	
		String^				GetROMObjectID();
		void				SetROMObjectID(String^ name);
		String^				GetROMGUID();
		IMap<String^, IMap<String^, String^>^>^	GetAllAttributes();

		//rules
		bool				LoadRules(String^ knowledge_file);
		bool				LoadRulesFromString(String^ xmlStr);		
		IVector<String^>^	EvaluateTable(String^ evalTable, String^ output, bool bGetAll);
		IVector<String^>^	EvaluateTable(String^ evalTable, String^ output);
		IMap<String^, IVector<String^>^>^ EvaluateTable(String^ evalTable, bool bGetAll);
		IMap<String^, IVector<String^>^>^ EvaluateTable(String^ evalTable);
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, bool bGetAll, String^ paramIn, String^* paramOut);
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, String^ paramIn, String^* paramOut);
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, bool bGetAll, String^ paramIn, String^* paramOut);
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, String^ paramIn, String^* paramOut);
		String^				GetFirstTableResult(String^ tableName, String^ output);
		IVector<String^>^	ReverseEvaluateTable(String^ evalTable, String^ output, bool bGetAll);
		IVector<String^>^	ReverseEvaluateTable(String^ evalTable, String^ output);
		IMap<String^, IVector<String^>^>^ ReverseEvaluateTable(String^ evalTable, bool bGetAll);
		IMap<String^, IVector<String^>^>^ ReverseEvaluateTable(String^ evalTable);

		//IO
		String^				SaveXML(bool indented);
		bool				LoadXML(String^ xmlStr);

		//XPATH
		String^				EvaluateXPATH(String^ xpath, String^ guid);
		String^				EvaluateXPATH(String^ xpath) {return EvaluateXPATH(xpath, GetROMGUID());}		

	internal:
		void*				GetROMPtr() {return (void*)m_ROMNode;}
		void*				GetEDSPtr() {return (void*)m_KnowledgeBase;}

	private:		
		ROMNode(void* ptr) {m_ROMNode = (ROM::ROMNode*)ptr; m_KnowledgeBase = m_ROMNode->GetKnowledgeBase(); m_canDelete = false; }

		IVector<ROMNode^>^	GetArrayFromVectorROM(vector<ROM::ROMNode*> vect);		

		DebugHandlerDelegate^	m_debugger;
		ROM::ROMNode			*m_ROMNode;
		EDS::CKnowledgeBase		*m_KnowledgeBase;
		bool					m_canDelete;
	};

	public enum class ATTRTYPE
	{
		SINGLESELECT = ROM::SINGLESELECT,
		MULTISELECT = ROM::MULTISELECT,
		BOOLEANSELECT = ROM::BOOLEANSELECT,
		EDIT = ROM::EDIT,
		STATIC = ROM::STATIC
	};

	public ref class ROMDictionaryAttribute sealed
	{
	friend ref class ROMDictionary;
	friend ref class LinearEngine;
	public:
		ROMDictionaryAttribute() {m_ROMDictionaryAttribute = NULL;}		
		void CreateROMDictionaryAttribute() {m_ROMDictionaryAttribute = new ROM::ROMDictionaryAttribute(); m_canDelete = true;}
		virtual ~ROMDictionaryAttribute() {if (m_ROMDictionaryAttribute && m_canDelete) delete m_ROMDictionaryAttribute; m_ROMDictionaryAttribute = NULL;}

		property String^ Name
		{
			String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->Name.c_str());
				else return "";
			}
			void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Name = value->Data();
			}
		}
		property String^ Description
		{
			String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->Description.c_str());
				else return "";
			}
			void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Description = value->Data();
			}
		}
		property String^ DefaultValue
		{
			String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->DefaultValue.c_str());
				else return "";
			}
			void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->DefaultValue = value->Data();
			}
		}
		property String^ RuleTable
		{
			String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->RuleTable.c_str());
				else return "";
			}
			void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->RuleTable = value->Data();
			}
		}
		property ATTRTYPE AttributeType
		{
			ATTRTYPE get()
			{
				if (m_ROMDictionaryAttribute)
					return static_cast<ATTRTYPE>(m_ROMDictionaryAttribute->AttributeType);
				else return ATTRTYPE::STATIC;
			}
			void set(ATTRTYPE value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->AttributeType = static_cast<ROM::ATTRTYPE_E>(value);
			}
		}
		property int Index
		{
			int get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Index;
				else return 0;
			}
			void set(int value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Index = value;
			}
		}
		property bool ValueChanged
		{
			bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->ValueChanged;
				else return false;
			}
			void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->ValueChanged = value;
			}
		}
		property bool ChangedByUser
		{
			bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->ChangedByUser;
				else return false;
			}
			void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->ChangedByUser = value;
			}
		}
		property bool Valid
		{
			bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Valid;
				else return false;
			}
			void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Valid = value;
			}
		}
		property bool Visible
		{
			bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Visible;
				else return false;
			}
			void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Visible = value;
			}
		}
		property bool Enabled
		{
			bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Enabled;
				else return false;
			}
			void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Enabled = value;
			}
		}
		property IVector<String^>^ PossibleValues
		{
			IVector<String^>^ get()
			{
				if (m_ROMDictionaryAttribute)				
					return GetIVectorFromVectorStrings(m_ROMDictionaryAttribute->PossibleValues);			
				else return ref new Vector<String^>(0);
			}
			void set(IVector<String^>^ value)
			{
				if (m_ROMDictionaryAttribute)
				{
					vector<wstring> vals(value->Size);
					for each (String^ val in value)
					{
						vals.push_back(val->Data());
					}
					m_ROMDictionaryAttribute->PossibleValues = vals;
				}

			}
		}
		property IVector<String^>^ AvailableValues
		{
			IVector<String^>^ get()
			{
				if (m_ROMDictionaryAttribute)				
					return GetIVectorFromVectorStrings(m_ROMDictionaryAttribute->AvailableValues);	
				else return ref new Vector<String^>(0);
			}
			void set(IVector<String^>^ value)
			{
				if (m_ROMDictionaryAttribute)
				{
					vector<wstring> vals(value->Size);
					for each (String^ val in value)
					{
						vals.push_back(val->Data());
					}
					m_ROMDictionaryAttribute->AvailableValues = vals;
				}
			}
		}

	private:
		ROMDictionaryAttribute(void* ptr) {m_ROMDictionaryAttribute = (ROM::ROMDictionaryAttribute*)ptr; m_canDelete = false;}
		ROM::ROMDictionaryAttribute* m_ROMDictionaryAttribute;
		bool m_canDelete;
	};

	public ref class ROMDictionary sealed
	{
	friend ref class LinearEngine;
	public:
		ROMDictionary() {m_ROMDictionary = NULL;}		
		ROMDictionary(ROMNode^ context) {CreateROMDictionary(context);}
		void CreateROMDictionary(ROMNode^ context) 
		{
			m_ROMDictionary = new ROM::ROMDictionary((ROM::ROMNode*)context->GetROMPtr());
		}
		virtual ~ROMDictionary() {if (m_ROMDictionary) delete m_ROMDictionary; m_ROMDictionary = NULL;}

		//debugger
		property DebugHandlerDelegate^	DebugDelegate
		{
			DebugHandlerDelegate^ get()
			{
				return m_debugger;
			}
			void set(DebugHandlerDelegate^ value)
			{
				m_debugger = value;
				if (m_ROMDictionary != nullptr)
				{
					if (m_debugger != nullptr)
					{
						function<void(const wstring&)> debugDelegate = [value](const wstring& msg)
						{
							value(ref new String(msg.c_str()));
						};
						m_ROMDictionary->SetTableDebugHandler(debugDelegate);
					}
					else
					{
						m_ROMDictionary->SetTableDebugHandler(nullptr);
					}
				}
			}
		}
		void					EnableRemoteDebugger(bool enable) { if (m_ROMDictionary) m_ROMDictionary->EnableRemoteDebugger(enable); }
		
		void					LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttribute^	GetDictionaryAttr(String^ dictAttrName);
		IMap<String^, ROMDictionaryAttribute^>^ GetAllDictionaryAttrs();

	private:		
		ROM::ROMDictionary		*m_ROMDictionary;
		DebugHandlerDelegate^	m_debugger;
	};		

	public enum class INVALIDATEMODE
	{
		NORMAL = ROM::NORMALINVALIDATE,
		FLAG = ROM::FLAGINVALIDATE
	};

	public ref class LinearEngine sealed
	{
	public:
		LinearEngine() {m_LinearEngine = NULL;}		
		LinearEngine(ROMNode^ context, String^ dictionaryTable) {CreateLinearEngine(context, dictionaryTable);}
		void CreateLinearEngine(ROMNode^ context, String^ dictionaryTable) 
		{
			m_LinearEngine = new ROM::LinearEngine((ROM::ROMNode*)context->GetROMPtr(), dictionaryTable->Data());
		}
		virtual ~LinearEngine() {if (m_LinearEngine) delete m_LinearEngine;}

		//debugger		
		property DebugHandlerDelegate^	DebugDelegate
		{
			DebugHandlerDelegate^ get()
			{
				return m_debugger;
			}
			void set(DebugHandlerDelegate^ value)
			{
				m_debugger = value;
				if (m_LinearEngine != nullptr)
				{
					if (m_debugger != nullptr)
					{
						function<void(const wstring&)> debugDelegate = [value](const wstring& msg)
						{
							value(ref new String(msg.c_str()));
						};
						m_LinearEngine->SetTableDebugHandler(debugDelegate);
					}
					else
					{
						m_LinearEngine->SetTableDebugHandler(nullptr);
					}
				}
			}
		}
		void					EnableRemoteDebugger(bool enable) { if (m_LinearEngine) m_LinearEngine->EnableRemoteDebugger(enable); }

		void					LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttribute^	GetDictionaryAttr(String^ dictAttrName);
		IMap<String^, ROMDictionaryAttribute^>^ GetAllDictionaryAttrs();

		void EvaluateForAttribute(String^ dictAttrName, IVector<String^>^ newValues, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, IVector<String^>^ newValues) {EvaluateForAttribute(dictAttrName, newValues, true);}
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue) {EvaluateForAttribute(dictAttrName, newValue, true);}
		void EvaluateAll() {if (m_LinearEngine) m_LinearEngine->EvaluateAll();}
		IVector<ROMDictionaryAttribute^>^ GetEvalList();
		IMap<String^, IVector<String^>^>^ GetTriggers();
		property bool DictionaryIsValid
		{
			bool get()
			{
				if (m_LinearEngine)
					return m_LinearEngine->DictionaryIsValid();
				else
					return false;
			}
		}
		property INVALIDATEMODE InvalidateMode
		{
			INVALIDATEMODE get()
			{
				if (m_LinearEngine)
					return static_cast<INVALIDATEMODE>(m_LinearEngine->InvalidateMode);
				else
					return INVALIDATEMODE::NORMAL;
			}
			void set(INVALIDATEMODE value)
			{
				if (m_LinearEngine)
					m_LinearEngine->InvalidateMode = static_cast<ROM::INVALIDATEMODE_E>(value);
			}
		}

	private:
		ROM::LinearEngine			*m_LinearEngine;
		DebugHandlerDelegate^		m_debugger;
	};
}