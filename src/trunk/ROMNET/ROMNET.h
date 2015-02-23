/*
This file is part of ROMNET.
Copyright (C) 2009-2015 Eric D. Schmidt, DigiRule Solutions LLC

    ROMNET is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    ROMNET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROMNET.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#pragma unmanaged
#include "ROMNode.h"
#include "ROMDictionaryAttribute.h"
#include "ROMDictionary.h"
#include "LinearEngine.h"
#include <vector>
#include <map>
#include "Marshal.h"
using namespace std;

typedef string(__stdcall *VALUECB)(const string&, void*);
typedef void(__stdcall *DEBUGCB)(const string&);

#pragma managed
#using <mscorlib.dll>
#using <System.dll>
#include <vcclr.h>
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

namespace ROMNET {
	ref class ROMNode;
	public delegate ROMNode^ ROMObjectFactoryDelegate(String^ id);

	public ref class ROMNode
	{
	public:
		ROMNode() {CreateROMNode("", nullptr, (IntPtr)nullptr); m_KnowledgeBase = nullptr;}
		ROMNode(String^ id) { CreateROMNode(id, nullptr, (IntPtr)nullptr); }
		ROMNode(String^ id, ROMObjectFactoryDelegate^ factory) { CreateROMNode(id, factory, (IntPtr)nullptr); }
		bool CreateROMNode(System::String^ id, ROMObjectFactoryDelegate^ factory, IntPtr ptr);
		~ROMNode() {this->!ROMNode();}
		!ROMNode() {DestroyROMObject();}

		//some useful operators/casts for the managed/unmanaged boundry
		static bool operator ==(ROMNode^ romObj, ROMNode^ romObj2)
		{
			if (Equals(romObj, romObj2))
				return true;

			if (!Equals(romObj, nullptr) && !Equals(romObj2, nullptr))
				return romObj->m_ROMNode == romObj2->m_ROMNode;

			return false;
		}

		static bool operator !=(ROMNode^ romObj, ROMNode^ romObj2)
		{
			if (Equals(romObj, romObj2))
				return false;

			if (!Equals(romObj, nullptr) && !Equals(romObj2, nullptr))
				return romObj->m_ROMNode != romObj2->m_ROMNode;

			return true;
		}

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

		property EDSNET::EDSEngine^ Rules
		{
			EDSNET::EDSEngine^ get()
			{
				return _getManagedRules();
			}
			void set(EDSNET::EDSEngine^ rules)
			{				
				m_ROMNode->SetKnowledgeBase((EDS::CKnowledgeBase*)rules->GetEDSPtr().ToPointer());				
				m_KnowledgeBase = rules;
			}
		}

		//relational functions
		ROMNode^			GetRoot();
		ROMNode^			Parent();
		array<ROMNode^>^	GetAllChildren(bool recurs);
		array<ROMNode^>^	FindObjects(String^ xpath);
		ROMNode^			FindFirstObject(String^ xpath);
		array<ROMNode^>^	FindAllObjectsByID(String^ id, bool recurs);
		ROMNode^			FindObjectByGUID(String^ guid);
		bool				AddChildROMObject(ROMNode^ child);
		bool				RemoveChildROMObject(ROMNode^ child);
		bool				RemoveFromParent();
		array<ROMNode^>^	GetAllFriends();
		bool				AddFriend(ROMNode^ friendObj);
		bool				RemoveFriend(ROMNode^ friendObj);
		bool				RemoveAllFriends();
		bool				DestroyROMObject();
		ROMNode^			Clone();

		//attribute functions
		virtual String^		GetAttribute(String^ id, String^ name, bool immediate);
		virtual String^		GetAttribute(String^ id, bool immediate) {return GetAttribute(id, "value", immediate);}
		virtual String^		GetAttribute(String^ id) {return GetAttribute(id, "value", false);}
		virtual bool		GetAttributeExists(String^ id, String^ name);
		virtual bool		GetAttributeExists(String^ id) {return GetAttributeExists(id, "value");}
		virtual bool		SetAttribute(String^ id, String^ name, String^ value);
		virtual bool		SetAttribute(String^ id, String^ value) {return SetAttribute(id, L"value", value);}
		virtual bool		RemoveAttribute(String^ id, String^ name);
		virtual bool		RemoveAttribute(String^ id) {return RemoveAttribute(id, "value");} 	
		virtual bool		SetROMObjectValue(String^ name, String^ value);
		virtual String^		GetROMObjectValue(String^ name);
		virtual bool		RemoveROMObjectValue(String^ name);	
		String^				GetROMObjectID();
		void				SetROMObjectID(String^ name);
		String^				GetROMGUID();
		Dictionary<String^, Dictionary<String^, String^>^>^	GetAllAttributes();

		//rules
		array<String^>^		EvaluateTable(String^ evalTable, String^ output, bool bGetAll, Object^ context);
		array<String^>^		EvaluateTable(String^ evalTable, String^ output, bool bGetAll) { return EvaluateTable(evalTable, output, bGetAll, this); }
		array<String^>^		EvaluateTable(String^ evalTable, String^ output, Object^ context);
		array<String^>^		EvaluateTable(String^ evalTable, String^ output) { return EvaluateTable(evalTable, output, this); }
		Dictionary<String^, array<String^>^>^ EvaluateTable(String^ evalTable, bool bGetAll, Object^ context);
		Dictionary<String^, array<String^>^>^ EvaluateTable(String^ evalTable, bool bGetAll) { return EvaluateTable(evalTable, bGetAll, this); }
		Dictionary<String^, array<String^>^>^ EvaluateTable(String^ evalTable, Object^ context);
		Dictionary<String^, array<String^>^>^ EvaluateTable(String^ evalTable) { return EvaluateTable(evalTable, this); }
		array<String^>^		EvaluateTableWithParam(String^ evalTable, String^ output, bool bGetAll, String^ param, Object^ context);
		array<String^>^		EvaluateTableWithParam(String^ evalTable, String^ output, bool bGetAll, String^ param) { return EvaluateTableWithParam(evalTable, output, bGetAll, param, this); }
		array<String^>^		EvaluateTableWithParam(String^ evalTable, String^ output, String^ param, Object^ context);
		array<String^>^		EvaluateTableWithParam(String^ evalTable, String^ output, String^ param) { return EvaluateTableWithParam(evalTable, output, param, this); }
		Dictionary<String^, array<String^>^>^ EvaluateTableWithParam(String^ evalTable, bool bGetAll, String^ param, Object^ context);
		Dictionary<String^, array<String^>^>^ EvaluateTableWithParam(String^ evalTable, bool bGetAll, String^ param) { return EvaluateTableWithParam(evalTable, bGetAll, param, this); }
		Dictionary<String^, array<String^>^>^ EvaluateTableWithParam(String^ evalTable, String^ param, Object^ context);
		Dictionary<String^, array<String^>^>^ EvaluateTableWithParam(String^ evalTable, String^ param) { return EvaluateTableWithParam(evalTable, param, this); }
		String^				GetFirstTableResult(String^ tableName, String^ output, Object^ context);
		String^				GetFirstTableResult(String^ tableName, String^ output) { return GetFirstTableResult(tableName, output, this); }
		array<String^>^		ReverseEvaluateTable(String^ evalTable, String^ output, bool bGetAll, Object^ context);
		array<String^>^		ReverseEvaluateTable(String^ evalTable, String^ output, bool bGetAll) { return ReverseEvaluateTable(evalTable, output, bGetAll, this); }
		array<String^>^		ReverseEvaluateTable(String^ evalTable, String^ output, Object^ context);
		array<String^>^		ReverseEvaluateTable(String^ evalTable, String^ output) { return ReverseEvaluateTable(evalTable, output, this); }
		Dictionary<String^, array<String^>^>^ ReverseEvaluateTable(String^ evalTable, bool bGetAll, Object^ context);
		Dictionary<String^, array<String^>^>^ ReverseEvaluateTable(String^ evalTable, bool bGetAll) { return ReverseEvaluateTable(evalTable, bGetAll, this); }
		Dictionary<String^, array<String^>^>^ ReverseEvaluateTable(String^ evalTable, Object^ context);
		Dictionary<String^, array<String^>^>^ ReverseEvaluateTable(String^ evalTable) { return ReverseEvaluateTable(evalTable, this); }

		//IO
		String^				SaveXML(bool indented);
		static ROMNode^		LoadXML(String^ xmlFile, ROMObjectFactoryDelegate^ factory);
		static ROMNode^		LoadXMLFromString(String^ xmlStr, ROMObjectFactoryDelegate^ factory);

		//XPATH
		String^				EvaluateXPATH(String^ xpath, String^ guid);
		String^				EvaluateXPATH(String^ xpath) {return EvaluateXPATH(xpath, GetROMGUID());}

	public protected:
		IntPtr				GetROMPtr() { return (IntPtr)m_ROMNode; }

	private:		
		array<ROMNode^>^			_getArrayFromVectorROM(vector<ROM::ROMNode*> vect);
		static ROMNode^				_managedFactory(String^ id);
		static ROMNode^				_loadNode(ROM::ROMNode* node, ROMObjectFactoryDelegate^ factory);
		static String^				_managedGetter(String^ attrName, Object^ context);
		EDSNET::EDSEngine^			_getManagedRules();

		ROMObjectFactoryDelegate^				m_factory;
		GCHandle								m_gchFactory;
		ROM::ROMNode							*m_ROMNode;
		EDSNET::EDSEngine^						m_KnowledgeBase;
		bool									m_canDelete;
		Dictionary<String^, ROMNode^>^			m_managedTreeObjects;
	};

	public enum class ATTRTYPE
	{
		SINGLESELECT = ROM::SINGLESELECT,
		MULTISELECT = ROM::MULTISELECT,
		BOOLEANSELECT = ROM::BOOLEANSELECT,
		EDIT = ROM::EDIT,
		STATIC = ROM::STATIC
	};

	public ref class ROMDictionaryAttribute
	{
	public:
		ROMDictionaryAttribute() {m_ROMDictionaryAttribute = NULL;}		
		void CreateROMDictionaryAttribute() {m_ROMDictionaryAttribute = new ROM::ROMDictionaryAttribute(); m_canDelete = true;}
		~ROMDictionaryAttribute() {this->!ROMDictionaryAttribute();}
		!ROMDictionaryAttribute() {if (m_ROMDictionaryAttribute && m_canDelete) delete m_ROMDictionaryAttribute; m_ROMDictionaryAttribute = NULL;}

		property String^ Name
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return gcnew String(m_ROMDictionaryAttribute->Name.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Name = MarshalString(value);
			}
		}
		property String^ Description
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return gcnew String(m_ROMDictionaryAttribute->Description.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Description = MarshalString(value);
			}
		}
		property String^ DefaultValue
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return gcnew String(m_ROMDictionaryAttribute->DefaultValue.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->DefaultValue = MarshalString(value);
			}
		}
		property String^ RuleTable
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return gcnew String(m_ROMDictionaryAttribute->RuleTable.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->RuleTable = MarshalString(value);
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
		property array<String^>^ PossibleValues
		{
			virtual array<String^>^ get()
			{
				if (m_ROMDictionaryAttribute)				
					return GetArrayFromVectorStrings(m_ROMDictionaryAttribute->PossibleValues);			
				else return gcnew array<String^>(0);
			}
			virtual void set(array<String^>^ value)
			{
				if (m_ROMDictionaryAttribute)
				{
					vector<string> vals(value->Length);
					for each (String^ val in value)
					{
						string wsVal = MarshalString(val);
						vals.push_back(wsVal);
					}
					m_ROMDictionaryAttribute->PossibleValues = vals;
				}

			}
		}
		property array<String^>^ AvailableValues
		{
			virtual array<String^>^ get()
			{
				if (m_ROMDictionaryAttribute)				
					return GetArrayFromVectorStrings(m_ROMDictionaryAttribute->AvailableValues);	
				else return gcnew array<String^>(0);
			}
			virtual void set(array<String^>^ value)
			{
				if (m_ROMDictionaryAttribute)
				{
					vector<string> vals(value->Length);
					for each (String^ val in value)
					{
						string wsVal = MarshalString(val);
						vals.push_back(wsVal);
					}
					m_ROMDictionaryAttribute->AvailableValues = vals;
				}
			}
		}

		property String^ Value
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return gcnew String(m_ROMDictionaryAttribute->Value.c_str());
				else return "";
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->Value = MarshalString(value);
			}
		}

	public protected:
		ROMDictionaryAttribute(IntPtr ptr) {m_ROMDictionaryAttribute = (ROM::ROMDictionaryAttribute*)ptr.ToPointer(); m_canDelete = false;}

	private:
		ROM::ROMDictionaryAttribute* m_ROMDictionaryAttribute;
		bool m_canDelete;
	};

	public ref class ROMDictionary
	{
	public:
		ROMDictionary() { m_ROMDictionary = nullptr; m_ROMContext = nullptr; }
		ROMDictionary(ROMNode^ context) {CreateROMDictionary(context);}
		void CreateROMDictionary(ROMNode^ context) 
		{
			m_ROMContext = context;
			m_ROMDictionary = new ROM::ROMDictionary((ROM::ROMNode*)context->GetROMPtr().ToPointer());
		}
		virtual ~ROMDictionary() {this->!ROMDictionary();}
		!ROMDictionary() {if (m_ROMDictionary) delete m_ROMDictionary; m_ROMDictionary = nullptr;}

		void					LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttribute^	GetDictionaryAttr(String^ dictAttrName);
		Dictionary<String^, ROMDictionaryAttribute^>^ GetAllDictionaryAttrs();

	public protected:
		ROMNode^					m_ROMContext;
		ROM::IDictionaryInterface*	m_ROMDictionary;
		
	};		

	public enum class INVALIDATEMODE
	{
		NORMAL = ROM::NORMALINVALIDATE,
		FLAG = ROM::FLAGINVALIDATE
	};

	public ref class LinearEngine : public ROMDictionary
	{
	public:
		LinearEngine() : ROMDictionary() { m_LinearEngine = nullptr; }
		LinearEngine(ROMNode^ context, String^ dictionaryTable) : ROMDictionary() { CreateLinearEngine(context, dictionaryTable); }
		void CreateLinearEngine(ROMNode^ context, String^ dictionaryTable) 
		{
			string dict = MarshalString(dictionaryTable);
			m_ROMContext = context;
			m_LinearEngine = new ROM::LinearEngine((ROM::ROMNode*)context->GetROMPtr().ToPointer(), dict);
			m_ROMDictionary = m_LinearEngine;
		}
		void InitializeEngine();
		void ResetEngine() { m_LinearEngine->ResetEngine(); }
		virtual ~LinearEngine() {this->!LinearEngine();}
		!LinearEngine() { if (m_LinearEngine) delete m_LinearEngine; m_LinearEngine = nullptr; m_ROMDictionary = nullptr; }

		void EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues) {EvaluateForAttribute(dictAttrName, newValues, true);}
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue) {EvaluateForAttribute(dictAttrName, newValue, true);}
		void EvaluateAll();
		array<ROMDictionaryAttribute^>^ GetEvalList();
		Dictionary<String^, array<String^>^>^ GetTriggers();
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
		ROM::LinearEngine		*m_LinearEngine;
	};

	
}
