﻿#pragma once
#include <collection.h>
#include <algorithm>
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
	public delegate void DebugHandlerDelegate(String^ msg);

	public ref class ROMNode sealed
	{
	public:
		ROMNode() {CreateROMNode(""); m_KnowledgeBase = NULL;}		
		ROMNode(String^ id) {CreateROMNode(id);}
		bool CreateROMNode(String^ id);
		~ROMNode() {DestroyROMObject();}

		//debugger
		
		void					SetDebugging(bool set) {if (m_ROMNode) m_ROMNode->GenerateTableDebugMessages(set);}
		void					PumpDebugMessages();
		void					SetDebugHandler(DebugHandlerDelegate^ funct) {m_DebugDelegate = funct;}

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
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, String^ param, bool bGetAll);
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, String^ param);
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, String^ param, bool bGetAll);
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, String^ param);
		String^				GetEvalParameter();
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
		ROMNode(void* ptr) {m_ROMNode = (ROM::ROMNode*)ptr; m_KnowledgeBase = m_ROMNode->GetKnowledgeBase();}

		IVector<ROMNode^>^	GetArrayFromVectorROM(vector<ROM::ROMNode*> vect);		

		DebugHandlerDelegate^	m_DebugDelegate;
		ROM::ROMNode			*m_ROMNode;
		EDS::CKnowledgeBase		*m_KnowledgeBase;
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
		void CreateROMDictionaryAttribute() {m_ROMDictionaryAttribute = new ROM::ROMDictionaryAttribute();}
		~ROMDictionaryAttribute() {if (m_ROMDictionaryAttribute) delete m_ROMDictionaryAttribute;}

		property String^ Name
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->Name.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Name = value->Data();
			}
		}
		property String^ Description
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->Description.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Description = value->Data();
			}
		}
		property String^ DefaultValue
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->DefaultValue.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->DefaultValue = value->Data();
			}
		}
		property String^ RuleTable
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return ref new String(m_ROMDictionaryAttribute->RuleTable.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->RuleTable = value->Data();
			}
		}
		property ATTRTYPE AttributeType
		{
			virtual ATTRTYPE get()
			{
				if (m_ROMDictionaryAttribute)
					return static_cast<ATTRTYPE>(m_ROMDictionaryAttribute->AttributeType);
				else return ATTRTYPE::STATIC;
			}
			virtual void set(ATTRTYPE value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->AttributeType = static_cast<ROM::ATTRTYPE_E>(value);
			}
		}
		property int Index
		{
			virtual int get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Index;
				else return 0;
			}
			virtual void set(int value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Index = value;
			}
		}
		property bool ValueChanged
		{
			virtual bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->ValueChanged;
				else return false;
			}
			virtual void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->ValueChanged = value;
			}
		}
		property bool ChangedByUser
		{
			virtual bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->ChangedByUser;
				else return false;
			}
			virtual void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->ChangedByUser = value;
			}
		}
		property bool Valid
		{
			virtual bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Valid;
				else return false;
			}
			virtual void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Valid = value;
			}
		}
		property bool Visible
		{
			virtual bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Visible;
				else return false;
			}
			virtual void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Visible = value;
			}
		}
		property bool Enabled
		{
			virtual bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->Enabled;
				else return false;
			}
			virtual void set(bool value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Enabled = value;
			}
		}
		property IVector<String^>^ PossibleValues
		{
			virtual IVector<String^>^ get()
			{
				if (m_ROMDictionaryAttribute)				
					return GetIVectorFromVectorStrings(m_ROMDictionaryAttribute->PossibleValues);			
				else return ref new Vector<String^>(0);
			}
			virtual void set(IVector<String^>^ value)
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
			virtual IVector<String^>^ get()
			{
				if (m_ROMDictionaryAttribute)				
					return GetIVectorFromVectorStrings(m_ROMDictionaryAttribute->AvailableValues);	
				else return ref new Vector<String^>(0);
			}
			virtual void set(IVector<String^>^ value)
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
		ROMDictionaryAttribute(void* ptr) {m_ROMDictionaryAttribute = (ROM::ROMDictionaryAttribute*)ptr;}
		ROM::ROMDictionaryAttribute* m_ROMDictionaryAttribute;
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
		virtual ~ROMDictionary() {if (m_ROMDictionary) delete m_ROMDictionary;}

		//debugger
		
		void					SetDebugging(bool set) {if (m_ROMDictionary) m_ROMDictionary->GenerateTableDebugMessages(set);}
		void					PumpDebugMessages();
		void					SetDebugHandler(DebugHandlerDelegate^ funct) {m_DebugDelegate = funct;}

		void					LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttribute^	GetDictionaryAttr(String^ dictAttrName);
		IMap<String^, ROMDictionaryAttribute^>^ GetAllDictionaryAttrs();

	private:
		ROMDictionary(void* ptr) {m_ROMDictionary = (ROM::ROMDictionary*)ptr;}
		ROM::ROMDictionary		*m_ROMDictionary;
		DebugHandlerDelegate^	m_DebugDelegate;
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
		void					SetDebugging(bool set) {if (m_LinearEngine) m_LinearEngine->GenerateTableDebugMessages(set);}
		void					PumpDebugMessages();
		void					SetDebugHandler(DebugHandlerDelegate^ funct) {m_DebugDelegate = funct;}

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
			virtual bool get()
			{
				if (m_LinearEngine)
					return m_LinearEngine->DictionaryIsValid();
				else
					return false;
			}
		}
		property INVALIDATEMODE InvalidateMode
		{
			virtual INVALIDATEMODE get()
			{
				if (m_LinearEngine)
					return static_cast<INVALIDATEMODE>(m_LinearEngine->InvalidateMode);
				else
					return INVALIDATEMODE::NORMAL;
			}
			virtual void set(INVALIDATEMODE value)
			{
				if (m_LinearEngine)
					m_LinearEngine->InvalidateMode = static_cast<ROM::INVALIDATEMODE_E>(value);
			}
		}

	private:
		LinearEngine(void* ptr) {m_LinearEngine = (ROM::LinearEngine*)ptr;}
		ROM::LinearEngine *m_LinearEngine;
		DebugHandlerDelegate^	m_DebugDelegate;
	};
}