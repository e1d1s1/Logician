/*
This file is part of ROMNET.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

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
// This is the main DLL file.

#include "stdafx.h"
#include "ROMNET.h"

namespace ROMNET
{
	bool ROMNode::CreateROMNode(System::String^ id)
	{
		wstring wsID = MarshalString(id);
		m_ROMNode = new ROM::ROMNode(wsID);
		m_canDelete = true;
		if (m_ROMNode)
		{
			m_KnowledgeBase = m_ROMNode->GetKnowledgeBase();
			return true;
		}
		else
			return false;
	}

	ROMNode^ ROMNode::GetRoot()
	{
		ROMNode^ retval = nullptr;
		if (m_ROMNode)
		{
			ROM::ROMNode *rootNode = m_ROMNode->GetRoot();
			if (rootNode)
			{
				retval = gcnew ROMNode((IntPtr)rootNode);
			}
		}
		return retval;
	}

	ROMNode^	ROMNode::Parent()
	{
		ROMNode^ retval = nullptr;
		if (m_ROMNode)
		{
			ROM::ROMNode *parentNode = m_ROMNode->GetParent();
			if (parentNode)
			{
				retval = gcnew ROMNode((IntPtr)parentNode);
			}
		}
		return retval;
	}

	array<ROMNode^>^ ROMNode::GetAllChildren(bool recurs)
	{
		array<ROMNode^>^ retval = nullptr;
		if (m_ROMNode)
		{
			vector<ROM::ROMNode*> vChildren = m_ROMNode->GetAllChildren(recurs);
			retval = GetArrayFromVectorROM(vChildren);
		}
		
		return retval;
	}

	array<ROMNode^>^ ROMNode::FindObjects(String^ xpath)
	{
		array<ROMNode^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsxpath = MarshalString(xpath);
			vector<ROM::ROMNode*> vChildren = m_ROMNode->FindObjects(wsxpath);
			retval = GetArrayFromVectorROM(vChildren);
		}

		return retval;
	}

	array<ROMNode^>^ ROMNode::FindAllObjectsByID(String^ id, bool recurs)
	{
		array<ROMNode^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			vector<ROM::ROMNode*> vChildren = m_ROMNode->FindAllObjectsByID(wsID, recurs);
			retval = GetArrayFromVectorROM(vChildren);
		}
		
		return retval;
	}

	ROMNode^ ROMNode::FindObjectByGUID(String^ guid)
	{
		ROMNode^ retval = nullptr;
		if (m_ROMNode)
		{
			string sguid = MarshalStringA(guid);
			ROM::ROMNode* node = m_ROMNode->FindObjectByGUID(sguid);
			retval = gcnew ROMNode((IntPtr)node);
		}

		return retval;
	}

	bool ROMNode::AddChildROMObject(ROMNode^ child)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->AddChildROMObject(child->m_ROMNode);
			child->m_canDelete = false;
		}
		return retval;
	}

	bool ROMNode::RemoveChildROMObject(ROMNode^ child)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->RemoveChildROMObject(child->m_ROMNode);
			child->m_canDelete = true;
		}
		return retval;
	}

	bool ROMNode::RemoveFromParent()
	{
		if (m_ROMNode)
		{
			return m_ROMNode->RemoveFromParent();
			m_canDelete = true;
		}
		return false;
	}

	array<ROMNode^>^ ROMNode::GetAllFriends()
	{
		array<ROMNode^>^ retval = nullptr;
		if (m_ROMNode)
		{
			vector<ROM::ROMNode*> vFriends = m_ROMNode->GetAllFriends();
			retval = GetArrayFromVectorROM(vFriends);
		}
		return retval;
	}

	bool ROMNode::AddFriend(ROMNode^ friendObj)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->AddFriend(friendObj->m_ROMNode);
		}
		return retval;
	}

	bool ROMNode::RemoveFriend(ROMNode^ friendObj)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->RemoveFriend(friendObj->m_ROMNode);
		}
		return retval;
	}

	bool ROMNode::RemoveAllFriends()
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->RemoveAllFriends();
		}
		return retval;
	}

	bool ROMNode::DestroyROMObject()
	{
		bool retval = false;
		if (m_ROMNode)
		{			
			if (m_canDelete)
				delete m_ROMNode;
			m_ROMNode = NULL;
		}
		return retval;
	}

	ROMNode^ ROMNode::Clone()
	{
		ROMNode^ retval = nullptr;
		if (m_ROMNode)
		{
			ROM::ROMNode* node = m_ROMNode->Clone();
			retval = gcnew ROMNode((IntPtr)node);
			retval->m_canDelete = true;  //creates a new root node in new memory space ok to delete
		}
		return retval;
	}

	//attribute functions
	String^ ROMNode::GetAttribute(String^ id, String^ name, bool immediate)
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			wstring wsName = MarshalString(name);
			wstring res = m_ROMNode->GetAttribute(wsID, wsName, immediate);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	bool ROMNode::GetAttributeExists(String^ id, String^ name)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			wstring wsName = MarshalString(name);
			retval = m_ROMNode->GetAttributeExists(wsID, wsName);
		}
		return retval;
	}

	bool ROMNode::SetAttribute(String^ id, String^ name, String^ value)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			wstring wsName = MarshalString(name);
			wstring wsValue = MarshalString(value);
			retval = m_ROMNode->SetAttribute(wsID, wsName, wsValue);
		}
		return retval;
	}

	bool ROMNode::RemoveAttribute(String^ id, String^ name)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			wstring wsName = MarshalString(name);
			retval = m_ROMNode->RemoveAttribute(wsID, wsName);
		}
		return retval;
	}

	bool ROMNode::SetROMObjectValue(String^ name, String^ value)
	{
		bool retval = false;
		if (m_ROMNode)
		{			
			wstring wsName = MarshalString(name);
			wstring wsValue = MarshalString(value);
			retval = m_ROMNode->SetROMObjectValue(wsName, wsValue);
		}
		return retval;
	}

	String^ ROMNode::GetROMObjectValue(String^ name)
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsName = MarshalString(name);
			wstring res = m_ROMNode->GetROMObjectValue(wsName);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	bool ROMNode::RemoveROMObjectValue(String^ name)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsName = MarshalString(name);
			retval = m_ROMNode->RemoveROMObjectValue(wsName);
		}
		return retval;
	}

	String^ ROMNode::GetROMObjectID()
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring res = m_ROMNode->GetROMObjectID();
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	void ROMNode::SetROMObjectID(String^ id)
	{
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			m_ROMNode->SetROMObjectID(wsID);
		}
	}

	String^ ROMNode::GetROMGUID()
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			string res = m_ROMNode->GetROMGUID();
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	Dictionary<String^, Dictionary<String^, String^>^>^	ROMNode::GetAllAttributes()
	{
		Dictionary<String^, Dictionary<String^, String^>^>^ retval = gcnew Dictionary<String^, Dictionary<String^, String^>^>();
		if (m_ROMNode)
		{
			MAPWSTRMAP attrs = m_ROMNode->GetAllAttributes();
			for (MAPWSTRMAP::iterator it = attrs.begin(); it != attrs.end(); it++)
			{
				Dictionary<String^, String^>^ dictAttrs = gcnew Dictionary<String^, String^>();
				String^ key = gcnew String(it->first.c_str());
				for (MAPWSTRS::iterator itValues = it->second.begin(); itValues != it->second.end(); itValues++)
				{
					dictAttrs->Add(gcnew String(itValues->first.c_str()), gcnew String(itValues->second.c_str()));
				}
				retval->Add(key, dictAttrs);
			}
		}
		return retval;
	}

	//rules
	bool ROMNode::LoadRules(String^ knowledge_file)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsFile = MarshalString(knowledge_file);
			retval = m_ROMNode->LoadRules(wsFile);
			m_KnowledgeBase = m_ROMNode->GetKnowledgeBase();
		}
		return retval;
	}

	bool ROMNode::LoadRulesFromString(String^ xmlStr)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsXML = MarshalString(xmlStr);
			retval = m_ROMNode->LoadRulesFromString(wsXML);
			m_KnowledgeBase = m_ROMNode->GetKnowledgeBase();
		}
		return retval;
	}
		
	array<String^>^ ROMNode::EvaluateTable(String^ evalTable, String^ output, bool bGetAll)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			vector<wstring> res = m_ROMNode->EvaluateTable(wsTable, wsOutput, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	array<String^>^ ROMNode::EvaluateTable(String^ evalTable, String^ output)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			vector<wstring> res = m_ROMNode->EvaluateTable(wsTable, wsOutput);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	Dictionary<String^, array<String^>^>^ ROMNode::EvaluateTable(String^ evalTable, bool bGetAll)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTable(wsTable, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}		
		return retval;
	}	

	Dictionary<String^, array<String^>^>^ ROMNode::EvaluateTable(String^ evalTable)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTable(wsTable);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}		
		return retval;
	}

	array<String^>^ ROMNode::EvaluateTableWithParam(String^ evalTable, String^ output, String^ param, bool bGetAll)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			wstring par = MarshalString(param);
			vector<wstring> res = m_ROMNode->EvaluateTableWithParam(wsTable, wsOutput, par, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	array<String^>^ ROMNode::EvaluateTableWithParam(String^ evalTable, String^ output, String^ param)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			wstring par = MarshalString(param);
			vector<wstring> res = m_ROMNode->EvaluateTableWithParam(wsTable, wsOutput, par);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	Dictionary<String^, array<String^>^>^ ROMNode::EvaluateTableWithParam(String^ evalTable, String^ param, bool bGetAll)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring par = MarshalString(param);
			map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTableWithParam(wsTable, par, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}		
		return retval;
	}	

	Dictionary<String^, array<String^>^>^ ROMNode::EvaluateTableWithParam(String^ evalTable, String^ param)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring par = MarshalString(param);
			map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTableWithParam(wsTable, par);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}		
		return retval;
	}

	String^ ROMNode::GetEvalParameter()
	{
		String^ retval = gcnew String("");
		if (m_ROMNode)
		{
			wstring res = m_ROMNode->GetEvalParameter();
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	String^ ROMNode::GetFirstTableResult(String^ evalTable, String^ output)
	{
		String^ retval = gcnew String("");
		array<String^>^ resAll = EvaluateTable(evalTable, output);
		if (resAll != nullptr && resAll->Length > 0)
			retval = resAll[0];
		return retval;
	}

	array<String^>^ ROMNode::ReverseEvaluateTable(String^ evalTable, String^ output, bool bGetAll)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			vector<wstring> res = m_ROMNode->ReverseEvaluateTable(wsTable, wsOutput, bGetAll);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	array<String^>^ ROMNode::ReverseEvaluateTable(String^ evalTable, String^ output)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			vector<wstring> res = m_ROMNode->ReverseEvaluateTable(wsTable, wsOutput);
			retval = GetArrayFromVectorStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	Dictionary<String^, array<String^>^>^ ROMNode::ReverseEvaluateTable(String^ evalTable, bool bGetAll)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			map<wstring, vector<wstring> > res = m_ROMNode->ReverseEvaluateTable(wsTable, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	Dictionary<String^, array<String^>^>^ ROMNode::ReverseEvaluateTable(String^ evalTable)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			map<wstring, vector<wstring> > res = m_ROMNode->ReverseEvaluateTable(wsTable);
			retval = GetDictionaryFromMapStrings(res);
			PumpDebugMessages();
		}
		return retval;
	}

	//IO
	String^ ROMNode::SaveXML(bool indented)
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring res = m_ROMNode->SaveXML(indented);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	bool ROMNode::LoadXML(String^ xmlStr)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsXML = MarshalString(xmlStr);
			retval = m_ROMNode->LoadXML(wsXML);
		}
		return retval;
	}

	//XPATH
	String^ ROMNode::EvaluateXPATH(String^ xpath, String^ guid)
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsXPATH = MarshalString(xpath);
			string sGuid = MarshalStringA(guid);
			wstring res = m_ROMNode->EvaluateXPATH(wsXPATH, sGuid);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}


	array<ROMNode^>^ ROMNode::GetArrayFromVectorROM(vector<ROM::ROMNode*> vect)
	{
		array<ROMNode^>^ arr = gcnew array<ROMNode^>(vect.size());
		for (size_t i = 0; i < vect.size(); i++)
		{
			arr[i] = gcnew ROMNode((IntPtr)vect[i]);
		}
		return arr;
	}

	void ROMNode::PumpDebugMessages()
	{
		if (m_ROMNode != NULL && DebugDelegate != nullptr)
		{
			wstring msg = m_ROMNode->GetTableDebugMessages();
			if (msg.length() > 0)
				DebugDelegate(gcnew String(msg.c_str()));
		}
	}



	//dictionary
	void ROMDictionary::LoadDictionary(String^ dictionaryTable)
	{
		if (m_ROMDictionary)
		{
			wstring wsDict = MarshalString(dictionaryTable);
			m_ROMDictionary->LoadDictionary(wsDict);
			PumpDebugMessages();
		}
	}

	ROMDictionaryAttribute^ ROMDictionary::GetDictionaryAttr(String^ dictAttrName)
	{
		ROMDictionaryAttribute^ retval = nullptr;
		if (m_ROMDictionary)
		{
			wstring wsName = MarshalString(dictAttrName);
			ROM::ROMDictionaryAttribute* attr = m_ROMDictionary->GetDictionaryAttr(wsName);
			if (attr)
			{
				retval = gcnew ROMDictionaryAttribute((IntPtr)attr);
			}
		}
		return retval;
	}

	Dictionary<String^, ROMDictionaryAttribute^>^ ROMDictionary::GetAllDictionaryAttrs()
	{
		Dictionary<String^, ROMDictionaryAttribute^>^ retval = gcnew Dictionary<String^, ROMDictionaryAttribute^>();
		if (m_ROMDictionary)
		{
			map<wstring, ROM::ROMDictionaryAttribute>* allAttrs = m_ROMDictionary->GetAllDictionaryAttrs();
			for (map<wstring, ROM::ROMDictionaryAttribute>::iterator it = allAttrs->begin(); it != allAttrs->end(); it++)
			{
				String^ key = gcnew String(it->first.c_str());
				ROMDictionaryAttribute^ value = gcnew ROMDictionaryAttribute((IntPtr)(&(it->second)));
				retval->Add(key, value);
			}
		}
		return retval;
	}

	void ROMDictionary::PumpDebugMessages()
	{
		if (m_ROMDictionary != NULL && DebugDelegate != nullptr)
		{
			wstring msg = m_ROMDictionary->GetTableDebugMessages();
			if (msg.length() > 0)
				DebugDelegate(gcnew String(msg.c_str()));
		}
	}

	//LinearEngine
	void LinearEngine::LoadDictionary(String^ dictionaryTable)
	{
		if (m_LinearEngine)
		{
			wstring wsDict = MarshalString(dictionaryTable);
			m_LinearEngine->LoadDictionary(wsDict);
		}
	}

	ROMDictionaryAttribute^ LinearEngine::GetDictionaryAttr(String^ dictAttrName)
	{
		ROMDictionaryAttribute^ retval = nullptr;
		if (m_LinearEngine)
		{
			wstring wsName = MarshalString(dictAttrName);
			ROM::ROMDictionaryAttribute* attr = m_LinearEngine->GetDictionaryAttr(wsName);
			if (attr)
			{
				retval = gcnew ROMDictionaryAttribute((IntPtr)attr);
			}
		}
		return retval;
	}

	Dictionary<String^, ROMDictionaryAttribute^>^ LinearEngine::GetAllDictionaryAttrs()
	{
		Dictionary<String^, ROMDictionaryAttribute^>^ retval = gcnew Dictionary<String^, ROMDictionaryAttribute^>();
		if (m_LinearEngine)
		{
			map<wstring, ROM::ROMDictionaryAttribute>* allAttrs = m_LinearEngine->GetAllDictionaryAttrs();
			for (map<wstring, ROM::ROMDictionaryAttribute>::iterator it = allAttrs->begin(); it != allAttrs->end(); it++)
			{
				String^ key = gcnew String(it->first.c_str());
				ROMDictionaryAttribute^ value = gcnew ROMDictionaryAttribute((IntPtr)(&(it->second)));
				retval->Add(key, value);
			}
		}
		return retval;
	}

	void LinearEngine::EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues, bool bEvalDependents)
	{
		if (m_LinearEngine)
		{
			wstring name = MarshalString(dictAttrName);
			vector<wstring> vals = GetVectorFromArrayStrings(newValues);
			m_LinearEngine->EvaluateForAttribute(name, vals, bEvalDependents);
			PumpDebugMessages();
		}
	}

	void LinearEngine::EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents)
	{
		if (m_LinearEngine)
		{
			wstring name = MarshalString(dictAttrName);
			wstring val = MarshalString(newValue);
			m_LinearEngine->EvaluateForAttribute(name, val, bEvalDependents);
			PumpDebugMessages();
		}
	}

	array<ROMDictionaryAttribute^>^ LinearEngine::GetEvalList()
	{
		array<ROMDictionaryAttribute^>^ retval = nullptr;
		if (m_LinearEngine)
		{
			vector<ROM::ROMDictionaryAttribute*> res = m_LinearEngine->GetEvalList();
			retval = gcnew array<ROMDictionaryAttribute^>(res.size());
			size_t i = 0;
			for (vector<ROM::ROMDictionaryAttribute*>::iterator it = res.begin(); it != res.end(); it++)
			{
				retval[i] = gcnew ROMDictionaryAttribute((IntPtr)(*it));
				i++;
			}			
		}
		return retval;
	}

	Dictionary<String^, array<String^>^>^ LinearEngine::GetTriggers()
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_LinearEngine)
		{
			map<wstring, vector<wstring> > res = m_LinearEngine->GetTriggers();
			retval = gcnew Dictionary<String^, array<String^>^>();
			for (map<wstring, vector<wstring> >::iterator it = res.begin(); it != res.end(); it++)
			{
				array<String^>^ attrs = gcnew array<String^>(it->second.size());
				size_t i = 0;
				for (vector<wstring>::iterator itAttr = it->second.begin(); itAttr != it->second.end(); itAttr++)
				{
					attrs[i] = gcnew String((*itAttr).c_str());
					i++;
				}
				retval->Add(gcnew String(it->first.c_str()), attrs);
			}			
		}
		return retval;
	}

	void LinearEngine::PumpDebugMessages()
	{
		if (m_LinearEngine != NULL && DebugDelegate != nullptr)
		{
			wstring msg = m_LinearEngine->GetTableDebugMessages();
			if (msg.length() > 0)
				DebugDelegate(gcnew String(msg.c_str()));
		}
	}
}
