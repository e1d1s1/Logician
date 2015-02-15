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
	ref class ROMNode;
	public delegate ROMNode^ ROMObjectFactoryDelegate(String^ id);

	public ref class ROMNode sealed
	{
	public:
		ROMNode() { CreateROMNode("", nullptr, (IntPtr)nullptr); m_KnowledgeBase = nullptr; }
		ROMNode(String^ id) {CreateROMNode(id, nullptr, (IntPtr)nullptr);}
		bool CreateROMNode(String^ id, ROMObjectFactoryDelegate^ factory, IntPtr ptr);
		virtual ~ROMNode() {DestroyROMObject();}

		property ROMObjectFactoryDelegate^		ROMObjectFactory
		{
			ROMObjectFactoryDelegate^ get()
			{
				return m_factory;
			}
			void set(ROMObjectFactoryDelegate^ value)
			{
				m_factory = value;
			}
		}

		property EDSEngineWinRT::EDSEngine^ Rules
		{
			EDSEngineWinRT::EDSEngine^ get()
			{
				return _getManagedRules();
			}
			void set(EDSEngineWinRT::EDSEngine^ rules)
			{
				m_ROMNode->SetKnowledgeBase((EDS::CKnowledgeBase*)(void*)rules->GetEDSPtr());
				m_KnowledgeBase = rules;
			}
		}
				
		//relational functions
		ROMNode^			GetRoot();
		ROMNode^			Parent();
		IVector<ROMNode^>^	GetAllChildren(bool recurs);
		IVector<ROMNode^>^	FindObjects(String^ xpath);
		ROMNode^			FindFirstObject(String^ xpath);
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
		IVector<String^>^	EvaluateTable(String^ evalTable, String^ output, bool bGetAll, Object^ context);
		IVector<String^>^	EvaluateTable(String^ evalTable, String^ output, bool bGetAll) { return EvaluateTable(evalTable, output, bGetAll, this); }
		IVector<String^>^	EvaluateTable(String^ evalTable, String^ output, Object^ context);
		IVector<String^>^	EvaluateTable(String^ evalTable, String^ output) { return EvaluateTable(evalTable, output, this); }
		IMap<String^, IVector<String^>^>^ EvaluateTable(String^ evalTable, bool bGetAll, Object^ context);
		IMap<String^, IVector<String^>^>^ EvaluateTable(String^ evalTable, bool bGetAll) { return EvaluateTable(evalTable, bGetAll, this); }
		IMap<String^, IVector<String^>^>^ EvaluateTable(String^ evalTable, Object^ context);
		IMap<String^, IVector<String^>^>^ EvaluateTable(String^ evalTable) { return EvaluateTable(evalTable, this); }
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, bool bGetAll, String^ paramIn, String^* paramOut, Object^ context);
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, bool bGetAll, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(evalTable, output, bGetAll, paramIn, paramOut, this); }
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, String^ paramIn, String^* paramOut, Object^ context);
		IVector<String^>^	EvaluateTableWithParam(String^ evalTable, String^ output, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(evalTable, output, paramIn, paramOut); }
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, bool bGetAll, String^ paramIn, String^* paramOut, Object^ context);
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, bool bGetAll, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(evalTable, bGetAll, paramIn, paramOut); }
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, String^ paramIn, String^* paramOut, Object^ context);
		IMap<String^, IVector<String^>^>^ EvaluateTableWithParam(String^ evalTable, String^ paramIn, String^* paramOut) { return EvaluateTableWithParam(evalTable, paramIn, paramOut); }
		String^				GetFirstTableResult(String^ tableName, String^ output, Object^ context);
		String^				GetFirstTableResult(String^ tableName, String^ output) { return GetFirstTableResult(tableName, output, this); }
		IVector<String^>^	ReverseEvaluateTable(String^ evalTable, String^ output, bool bGetAll, Object^ context);
		IVector<String^>^	ReverseEvaluateTable(String^ evalTable, String^ output, bool bGetAll) { return ReverseEvaluateTable(evalTable, output, bGetAll, this); }
		IVector<String^>^	ReverseEvaluateTable(String^ evalTable, String^ output, Object^ context);
		IVector<String^>^	ReverseEvaluateTable(String^ evalTable, String^ output) { return ReverseEvaluateTable(evalTable, output, this); }
		IMap<String^, IVector<String^>^>^ ReverseEvaluateTable(String^ evalTable, bool bGetAll, Object^ context);
		IMap<String^, IVector<String^>^>^ ReverseEvaluateTable(String^ evalTable, bool bGetAll) { return ReverseEvaluateTable(evalTable, bGetAll, this); }
		IMap<String^, IVector<String^>^>^ ReverseEvaluateTable(String^ evalTable, Object^ context);
		IMap<String^, IVector<String^>^>^ ReverseEvaluateTable(String^ evalTable) { return ReverseEvaluateTable(evalTable, this); }

		//IO
		String^				SaveXML(bool indented);
		static ROMNode^		LoadXML(String^ xmlStr, ROMObjectFactoryDelegate^ factory);
		static ROMNode^		LoadXMLFromString(String^ xmlStr, ROMObjectFactoryDelegate^ factory);

		//XPATH
		String^				EvaluateXPATH(String^ xpath, String^ guid);
		String^				EvaluateXPATH(String^ xpath) {return EvaluateXPATH(xpath, GetROMGUID());}		

	internal:
		void*				GetROMPtr() {return (void*)m_ROMNode;}
		void*				GetEDSPtr() {return (void*)m_KnowledgeBase;}

	private:		
		//ROMNode(void* ptr) {m_ROMNode = (ROM::ROMNode*)ptr; m_KnowledgeBase = m_ROMNode->GetKnowledgeBase(); m_canDelete = false; }

		IVector<ROMNode^>^			_getArrayFromVectorROM(vector<ROM::ROMNode*> vect);		
		static ROMNode^				_managedFactory(String^ id);
		static ROMNode^				_loadNode(ROM::ROMNode* node, ROMObjectFactoryDelegate^ factory);
		static String^				_managedGetter(String^ attrName, Object^ context);
		EDSEngineWinRT::EDSEngine^	_getManagedRules();

		ROMObjectFactoryDelegate^				m_factory;
		IMap<String^, ROMNode^>^				m_managedTreeObjects;
		ROM::ROMNode							*m_ROMNode;
		EDSEngineWinRT::EDSEngine^				m_KnowledgeBase;
		bool									m_canDelete;
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
			m_ROMContext = context;
			m_ROMDictionary = new ROM::ROMDictionary((ROM::ROMNode*)context->GetROMPtr());
		}
		virtual ~ROMDictionary() {if (m_ROMDictionary) delete m_ROMDictionary; m_ROMDictionary = NULL;}

		void					LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttribute^	GetDictionaryAttr(String^ dictAttrName);
		IMap<String^, ROMDictionaryAttribute^>^ GetAllDictionaryAttrs();

	private:		
		ROMNode^				m_ROMContext;
		ROM::ROMDictionary		*m_ROMDictionary;
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
			m_ROMContext = context;
			m_LinearEngine = new ROM::LinearEngine((ROM::ROMNode*)context->GetROMPtr(), dictionaryTable->Data());
		}
		void InitializeEngine();
		void ResetEngine() { m_LinearEngine->ResetEngine(); }
		virtual ~LinearEngine() {if (m_LinearEngine) delete m_LinearEngine;}

		void					LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttribute^	GetDictionaryAttr(String^ dictAttrName);
		IMap<String^, ROMDictionaryAttribute^>^ GetAllDictionaryAttrs();

		void EvaluateForAttribute(String^ dictAttrName, IVector<String^>^ newValues, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, IVector<String^>^ newValues) {EvaluateForAttribute(dictAttrName, newValues, true);}
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue) {EvaluateForAttribute(dictAttrName, newValue, true);}
		void EvaluateAll() { if (m_LinearEngine) m_LinearEngine->_evaluateAll((void*)reinterpret_cast<IInspectable*>(m_ROMContext)); }
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
		ROMNode^				m_ROMContext;
		ROM::LinearEngine		*m_LinearEngine;
	};
}