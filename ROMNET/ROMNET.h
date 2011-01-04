/*
This file is part of ROMNET.
Copyright (C) 2009-2011 Eric D. Schmidt

    ROMNET is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
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

#pragma managed
#using <mscorlib.dll>
#using <System.dll>
#include <vcclr.h>
using namespace System;
using namespace System::Collections::Generic;

namespace ROMNET {
	public delegate void DebugHandlerDelegate(String^ msg);

	public ref class ROMNodeNET
	{
	public:
		ROMNodeNET() {m_ROMNode = NULL;}
		ROMNodeNET(IntPtr^ ptr) {m_ROMNode = (ROM::ROMNode*)ptr->ToPointer();}
		ROMNodeNET(String^ id) {CreateROMNodeNET(id);}
		bool CreateROMNodeNET(System::String^ id);
		~ROMNodeNET() {DestroyROMObject(); this->!ROMNodeNET();}
		!ROMNodeNET() {}

		//some useful operators/casts for the managed/unmanaged boundry
		static operator long(ROMNodeNET^ romObj)
		{
			if (Equals(romObj, nullptr))
				return 0;
			else if (romObj->m_ROMNode != NULL)
				(long)romObj->m_ROMNode;
			else
				return 0;		
		}

		static bool operator ==(ROMNodeNET^ romObj, ROMNodeNET^ romObj2)
		{
			if (Equals(romObj, romObj2))
				return true;

			if (!Equals(romObj, nullptr) && !Equals(romObj2, nullptr))
				return romObj->m_ROMNode == romObj2->m_ROMNode;

			return false;
		}

		static bool operator !=(ROMNodeNET^ romObj, ROMNodeNET^ romObj2)
		{
			if (Equals(romObj, romObj2))
				return false;

			if (!Equals(romObj, nullptr) && !Equals(romObj2, nullptr))
				return romObj->m_ROMNode != romObj2->m_ROMNode;

			return true;
		}

		//debugger
		DebugHandlerDelegate^	DebugDelegate;
		void					SetDebugging(bool set) {if (m_ROMNode) m_ROMNode->GenerateTableDebugMessages(set);}
		void					PumpDebugMessages();

		//relational functions
		ROMNodeNET^			GetRoot();
		ROMNodeNET^			Parent();
		array<ROMNodeNET^>^	GetAllChildren(bool recurs);
		array<ROMNodeNET^>^ FindAllObjectsOfID(String^ id, bool recurs);
		bool				AddChildROMObject(ROMNodeNET^ child);
		bool				RemoveChildROMObject(ROMNodeNET^ child);
		bool				DestroyROMObject();

		//attribute functions
		String^				GetAttribute(String^ id, String^ name, bool immediate);
		String^				GetAttribute(String^ id, bool immediate) {return GetAttribute(id, "value", immediate);}
		String^				GetAttribute(String^ id) {return GetAttribute(id, "value", false);}
		bool				GetAttributeExists(String^ id, String^ name);
		bool				GetAttributeExists(String^ id) {return GetAttributeExists(id, "value");}
		bool				SetAttribute(String^ id, String^ name, String^ value);
		bool				SetAttribute(String^ id, String^ value) {return SetAttribute(id, L"value", value);}
		bool				SetAttributeValue(String^ id, String^ value) {return SetAttribute(id, value);}
		bool				RemoveAttribute(String^ id, String^ name);
		bool				RemoveAttribute(String^ id) {return RemoveAttribute(id, "value");} 	
		bool				SetROMObjectValue(String^ name, String^ value);
		String^				GetROMObjectValue(String^ name);
		bool				RemoveROMObjectValue(String^ id);	
		String^				GetROMObjectID();
		void				SetROMObjectID(String^ name);
		Dictionary<String^, Dictionary<String^, String^>^>^	GetAllAttributes();

		//rules
		bool				LoadRules(String^ knowledge_file);
		array<String^>^		EvaluateTable(String^ evalTable, String^ output, bool bGetAll);
		array<String^>^		EvaluateTable(String^ evalTable, String^ output);
		array<String^>^		EvaluateTableForAttr(String^ evalTable, String^ output, bool bGetAll) {return EvaluateTable(evalTable, output, bGetAll);}
		array<String^>^		EvaluateTableForAttr(String^ evalTable, String^ output) {return EvaluateTable(evalTable, output);}
		Dictionary<String^, array<String^>^>^ EvaluateTable(String^ evalTable, bool bGetAll);
		Dictionary<String^, array<String^>^>^ EvaluateTable(String^ evalTable);

		//IO
		String^				SaveXML(bool indented);
		bool				LoadXML(String^ xmlStr);

		//XPATH
		String^				EvaluateXPATH(String^ xpath);

		IntPtr^				GetPtr() {return (IntPtr)m_ROMNode;}

	private:		
		array<ROMNodeNET^>^ GetArrayFromVectorROM(vector<ROM::ROMNode*> vect);

		ROM::ROMNode		*m_ROMNode;
	};

	public ref class ROMDictionaryAttributeNET
	{
	public:
		ROMDictionaryAttributeNET() {m_ROMDictionaryAttribute = NULL;}
		ROMDictionaryAttributeNET(IntPtr^ ptr) {m_ROMDictionaryAttribute = (ROM::ROMDictionaryAttribute*)ptr->ToPointer();}
		void CreateROMDictionaryAttributeNET() {m_ROMDictionaryAttribute = new ROM::ROMDictionaryAttribute();}
		~ROMDictionaryAttributeNET() {if (m_ROMDictionaryAttribute) delete m_ROMDictionaryAttribute; this->!ROMDictionaryAttributeNET();}
		!ROMDictionaryAttributeNET() {}

		property String^ Name
		{
			virtual String^ get()
			{
				if (m_ROMDictionaryAttribute)
					return gcnew String(m_ROMDictionaryAttribute->Name.c_str());
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
			}
			virtual void set(String^ value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->RuleTable = MarshalString(value);
			}
		}
		property int AttributeType
		{
			virtual int get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->AttributeType;
			}
			virtual void set(int value)
			{
				if (m_ROMDictionaryAttribute)
					m_ROMDictionaryAttribute->AttributeType = value;
			}
		}
		property bool ValueChanged
		{
			virtual bool get()
			{
				if (m_ROMDictionaryAttribute)
					return m_ROMDictionaryAttribute->ValueChanged;
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
			}
			virtual void set(array<String^>^ value)
			{
				if (m_ROMDictionaryAttribute)
				{
					vector<wstring> vals(value->Length);
					for each (String^ val in value)
					{
						wstring wsVal = MarshalString(val);
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
			}
			virtual void set(array<String^>^ value)
			{
				if (m_ROMDictionaryAttribute)
				{
					vector<wstring> vals(value->Length);
					for each (String^ val in value)
					{
						wstring wsVal = MarshalString(val);
						vals.push_back(wsVal);
					}
					m_ROMDictionaryAttribute->AvailableValues = vals;
				}
			}
		}

	private:
		ROM::ROMDictionaryAttribute* m_ROMDictionaryAttribute;
	};

	public ref class ROMDictionaryNET
	{
	public:
		ROMDictionaryNET() {m_ROMDictionary = NULL;}		
		ROMDictionaryNET(ROMNodeNET^ context) {CreateROMDictionaryNET(context);}
		void CreateROMDictionaryNET(ROMNodeNET^ context) 
		{
			m_ROMDictionary = new ROM::ROMDictionary((ROM::ROMNode*)context->GetPtr()->ToPointer());
		}
		virtual ~ROMDictionaryNET() {if (m_ROMDictionary) delete m_ROMDictionary; this->!ROMDictionaryNET();}
		!ROMDictionaryNET() {}

		//debugger
		DebugHandlerDelegate^	DebugDelegate;
		void					SetDebugging(bool set) {if (m_ROMDictionary) m_ROMDictionary->GenerateTableDebugMessages(set);}
		void					PumpDebugMessages();

		void						LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttributeNET^	GetDictionaryAttr(String^ dictAttrName);
		Dictionary<String^, ROMDictionaryAttributeNET^>^ GetAllDictionaryAttrs();

	private:
		ROMDictionaryNET(IntPtr^ ptr) {m_ROMDictionary = (ROM::ROMDictionary*)ptr->ToPointer();}
		ROM::ROMDictionary *m_ROMDictionary;
	};

	public ref class LinearEngineNET
	{
	public:
		LinearEngineNET() {m_LinearEngine = NULL;}		
		LinearEngineNET(ROMNodeNET^ context, String^ dictionaryTable) {CreateLinearEngineNET(context, dictionaryTable);}
		void CreateLinearEngineNET(ROMNodeNET^ context, String^ dictionaryTable) 
		{
			wstring dict = MarshalString(dictionaryTable);
			m_LinearEngine = new ROM::LinearEngine((ROM::ROMNode*)context->GetPtr()->ToPointer(), dict);
		}
		virtual ~LinearEngineNET() {if (m_LinearEngine) delete m_LinearEngine; this->!LinearEngineNET();}
		!LinearEngineNET() {}

		//debugger
		DebugHandlerDelegate^	DebugDelegate;
		void					SetDebugging(bool set) {if (m_LinearEngine) m_LinearEngine->GenerateTableDebugMessages(set);}
		void					PumpDebugMessages();

		void						LoadDictionary(String^ dictionaryTable);
		ROMDictionaryAttributeNET^	GetDictionaryAttr(String^ dictAttrName);
		Dictionary<String^, ROMDictionaryAttributeNET^>^ GetAllDictionaryAttrs();

		void EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues) {EvaluateForAttribute(dictAttrName, newValues, true);}
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents);
		void EvaluateForAttribute(String^ dictAttrName, String^ newValue) {EvaluateForAttribute(dictAttrName, newValue, true);}
		void EvaluateAll() {if (m_LinearEngine) m_LinearEngine->EvaluateAll();}
		array<ROMDictionaryAttributeNET^>^ GetEvalList();
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
	private:
		LinearEngineNET(IntPtr^ ptr) {m_LinearEngine = (ROM::LinearEngine*)ptr->ToPointer();}
		ROM::LinearEngine *m_LinearEngine;
	};
}
