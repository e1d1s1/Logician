/*
This file is part of ROMNETWinRT.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

    ROMNETWinRT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    ROMNETWinRT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROMNETWinRT.  If not, see <http://www.gnu.org/licenses/>.
*/
// This is the main DLL file.

#include "pch.h"
#include "ROMWinRT.h"

bool ROMWinRT::ROMNode::CreateROMNode(String^ id)
{
	m_ROMNode = new ROM::ROMNode(id->Data());
	m_canDelete = true;
	if (m_ROMNode)
	{
		m_KnowledgeBase = m_ROMNode->GetKnowledgeBase();
		return true;
	}
	else
		return false;
}

ROMWinRT::ROMNode^ ROMWinRT::ROMNode::GetRoot()
{
	ROMNode^ retval = nullptr;
	if (m_ROMNode)
	{
		ROM::ROMNode *rootNode = m_ROMNode->GetRoot();
		if (rootNode)
		{
			retval = ref new ROMNode((void*)rootNode);
		}
	}
	return retval;
}

ROMWinRT::ROMNode^ ROMWinRT::ROMNode::Parent()
{
	ROMNode^ retval = nullptr;
	if (m_ROMNode)
	{
		ROM::ROMNode *parentNode = m_ROMNode->GetParent();
		if (parentNode)
		{
			retval = ref new ROMNode(parentNode);
		}
	}
	return retval;
}

IVector<ROMWinRT::ROMNode^>^ ROMWinRT::ROMNode::GetAllChildren(bool recurs)
{
	IVector<ROMNode^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<ROM::ROMNode*> vChildren = m_ROMNode->GetAllChildren(recurs);
		retval = GetArrayFromVectorROM(vChildren);
	}
		
	return retval;
}

IVector<ROMWinRT::ROMNode^>^ ROMWinRT::ROMNode::FindObjects(String^ xpath)
{
	IVector<ROMNode^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<ROM::ROMNode*> vChildren = m_ROMNode->FindObjects(xpath->Data());
		retval = GetArrayFromVectorROM(vChildren);
	}

	return retval;
}

IVector<ROMWinRT::ROMNode^>^ ROMWinRT::ROMNode::FindAllObjectsByID(String^ id, bool recurs)
{
	IVector<ROMNode^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<ROM::ROMNode*> vChildren = m_ROMNode->FindAllObjectsByID(id->Data(), recurs);
		retval = GetArrayFromVectorROM(vChildren);
	}
		
	return retval;
}

ROMWinRT::ROMNode^ ROMWinRT::ROMNode::FindObjectByGUID(String^ guid)
{
	ROMNode^ retval = nullptr;
	if (m_ROMNode)
	{
		ROM::ROMNode* node = m_ROMNode->FindObjectByGUID(guid->Data());
		retval = ref new ROMNode((void*)node);
	}

	return retval;
}

bool ROMWinRT::ROMNode::AddChildROMObject(ROMNode^ child)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->AddChildROMObject(child->m_ROMNode);
		child->m_canDelete = false;
	}
	return retval;
}

bool ROMWinRT::ROMNode::RemoveChildROMObject(ROMNode^ child)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->RemoveChildROMObject(child->m_ROMNode);
		child->m_canDelete = true;
	}
	return retval;
}

bool ROMWinRT::ROMNode::RemoveFromParent()
{
	if (m_ROMNode)
	{
		return m_ROMNode->RemoveFromParent();
		m_canDelete = true;
	}
	return false;
}

IVector<ROMWinRT::ROMNode^>^ ROMWinRT::ROMNode::GetAllFriends()
{
	IVector<ROMNode^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<ROM::ROMNode*> vFriends = m_ROMNode->GetAllFriends();
		retval = GetArrayFromVectorROM(vFriends);
	}
	return retval;
}

bool ROMWinRT::ROMNode::AddFriend(ROMNode^ friendObj)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->AddFriend(friendObj->m_ROMNode);
	}
	return retval;
}

bool ROMWinRT::ROMNode::RemoveFriend(ROMNode^ friendObj)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->RemoveFriend(friendObj->m_ROMNode);
	}
	return retval;
}

bool ROMWinRT::ROMNode::RemoveAllFriends()
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->RemoveAllFriends();
	}
	return retval;
}

bool ROMWinRT::ROMNode::DestroyROMObject()
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

ROMWinRT::ROMNode^ ROMWinRT::ROMNode::Clone()
{
	ROMNode^ retval = nullptr;
	if (m_ROMNode)
	{
		ROM::ROMNode* node = m_ROMNode->Clone();
		delete m_ROMNode;
		retval = ref new ROMNode((void*)node);
		retval->m_canDelete = true;  //creates a new root node in new memory space ok to delete
	}
	return retval;
}

//attribute functions
String^ ROMWinRT::ROMNode::GetAttribute(String^ id, String^ name, bool immediate)
{
	String^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring res = m_ROMNode->GetAttribute(id->Data(), name->Data(), immediate);
		retval = ref new String(res.c_str());
	}
	return retval;
}

bool ROMWinRT::ROMNode::GetAttributeExists(String^ id, String^ name)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->GetAttributeExists(id->Data(), name->Data());
	}
	return retval;
}

bool ROMWinRT::ROMNode::SetAttribute(String^ id, String^ name, String^ value)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->SetAttribute(id->Data(), name->Data(), value->Data());
	}
	return retval;
}

bool ROMWinRT::ROMNode::RemoveAttribute(String^ id, String^ name)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->RemoveAttribute(id->Data(), name->Data());
	}
	return retval;
}

bool ROMWinRT::ROMNode::SetROMObjectValue(String^ name, String^ value)
{
	bool retval = false;
	if (m_ROMNode)
	{			
		retval = m_ROMNode->SetROMObjectValue(name->Data(), value->Data());
	}
	return retval;
}

String^ ROMWinRT::ROMNode::GetROMObjectValue(String^ name)
{
	String^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring res = m_ROMNode->GetROMObjectValue(name->Data());
		retval = ref new String(res.c_str());
	}
	return retval;
}

bool ROMWinRT::ROMNode::RemoveROMObjectValue(String^ name)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->RemoveROMObjectValue(name->Data());
	}
	return retval;
}

String^ ROMWinRT::ROMNode::GetROMObjectID()
{
	String^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring res = m_ROMNode->GetROMObjectID();
		retval = ref new String(res.c_str());
	}
	return retval;
}

void ROMWinRT::ROMNode::SetROMObjectID(String^ id)
{
	if (m_ROMNode)
	{
		m_ROMNode->SetROMObjectID(id->Data());
	}
}

String^ ROMWinRT::ROMNode::GetROMGUID()
{
	String^ retval = nullptr;
	if (m_ROMNode)
	{
		string res = m_ROMNode->GetROMGUID();
		retval = ref new String(MBCStrToWStr(res).c_str());
	}
	return retval;
}

IMap<String^, IMap<String^, String^>^>^	ROMWinRT::ROMNode::GetAllAttributes()
{
	IMap<String^, IMap<String^, String^>^>^ retval = ref new Map<String^, IMap<String^, String^>^>();
	if (m_ROMNode)
	{
		auto attrs = m_ROMNode->GetAllAttributes();
		for (auto it = attrs.begin(); it != attrs.end(); it++)
		{
			IMap<String^, String^>^ dictAttrs = ref new Map<String^, String^>();
			String^ key = ref new String(it->first.c_str());
			for (auto itValues = it->second.begin(); itValues != it->second.end(); itValues++)
			{
				dictAttrs->Insert(ref new String(itValues->first.c_str()), ref new String(itValues->second.c_str()));
			}
			retval->Insert(key, dictAttrs);
		}
	}
	return retval;
}

//rules
bool ROMWinRT::ROMNode::LoadRules(String^ knowledge_file)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->LoadRules(knowledge_file->Data());
		m_KnowledgeBase = m_ROMNode->GetKnowledgeBase();
	}
	return retval;
}

bool ROMWinRT::ROMNode::LoadRulesFromString(String^ xmlStr)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->LoadRulesFromString(xmlStr->Data());
		m_KnowledgeBase = m_ROMNode->GetKnowledgeBase();
	}
	return retval;
}
		
IVector<String^>^ ROMWinRT::ROMNode::EvaluateTable(String^ evalTable, String^ output, bool bGetAll)
{
	IVector<String^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<wstring> res = m_ROMNode->EvaluateTable(evalTable->Data(), output->Data(), bGetAll);
		retval = GetIVectorFromVectorStrings(res);
	}
	return retval;
}

IVector<String^>^ ROMWinRT::ROMNode::EvaluateTable(String^ evalTable, String^ output)
{
	IVector<String^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<wstring> res = m_ROMNode->EvaluateTable((wstring)evalTable->Data(), (wstring)output->Data());
		retval = GetIVectorFromVectorStrings(res);
	}
	return retval;
}

IMap<String^, IVector<String^>^>^ ROMWinRT::ROMNode::EvaluateTable(String^ evalTable, bool bGetAll)
{
	IMap<String^, IVector<String^>^>^ retval = nullptr;
	if (m_ROMNode)
	{
		map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTable(evalTable->Data(), bGetAll);
		retval = GetIMapFromMapStrings(res);
	}		
	return retval;
}	

IMap<String^, IVector<String^>^>^ ROMWinRT::ROMNode::EvaluateTable(String^ evalTable)
{
	IMap<String^, IVector<String^>^>^ retval = nullptr;
	if (m_ROMNode)
	{
		map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTable(evalTable->Data());
		retval = GetIMapFromMapStrings(res);
	}		
	return retval;
}

IVector<String^>^ ROMWinRT::ROMNode::EvaluateTableWithParam(String^ evalTable, String^ output, bool bGetAll, String^ paramIn, String^* paramOut)
{
	IVector<String^>^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring para = paramIn->Data();
		vector<wstring> res = m_ROMNode->EvaluateTableWithParam(evalTable->Data(), output->Data(), bGetAll, para);
		*paramOut = ref new String(para.c_str());
		retval = GetIVectorFromVectorStrings(res);
	}
	return retval;
}

IVector<String^>^ ROMWinRT::ROMNode::EvaluateTableWithParam(String^ evalTable, String^ output, String^ paramIn, String^* paramOut)
{
	IVector<String^>^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring para = paramIn->Data();
		vector<wstring> res = m_ROMNode->EvaluateTableWithParam((wstring)evalTable->Data(), (wstring)output->Data(), para);
		*paramOut = ref new String(para.c_str());
		retval = GetIVectorFromVectorStrings(res);
	}
	return retval;
}

IMap<String^, IVector<String^>^>^ ROMWinRT::ROMNode::EvaluateTableWithParam(String^ evalTable, bool bGetAll, String^ paramIn, String^* paramOut)
{
	IMap<String^, IVector<String^>^>^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring para = paramIn->Data();
		map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTableWithParam(evalTable->Data(), bGetAll, para);
		*paramOut = ref new String(para.c_str());
		retval = GetIMapFromMapStrings(res);
	}		
	return retval;
}	

IMap<String^, IVector<String^>^>^ ROMWinRT::ROMNode::EvaluateTableWithParam(String^ evalTable, String^ paramIn, String^* paramOut)
{
	IMap<String^, IVector<String^>^>^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring para = paramIn->Data();
		map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTableWithParam(evalTable->Data(), para);
		*paramOut = ref new String(para.c_str());
		retval = GetIMapFromMapStrings(res);
	}		
	return retval;
}

String^ ROMWinRT::ROMNode::GetFirstTableResult(String^ evalTable, String^ output)
{
	String^ retval = ref new String(L"");
	IVector<String^>^ resAll = EvaluateTable(evalTable, output);
	if (resAll != nullptr && resAll->Size > 0)
		retval = resAll->GetAt(0);
	return retval;
}

IVector<String^>^ ROMWinRT::ROMNode::ReverseEvaluateTable(String^ evalTable, String^ output, bool bGetAll)
{
	IVector<String^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<wstring> res = m_ROMNode->ReverseEvaluateTable(evalTable->Data(), output->Data(), bGetAll);
		retval = GetIVectorFromVectorStrings(res);
	}
	return retval;
}

IVector<String^>^ ROMWinRT::ROMNode::ReverseEvaluateTable(String^ evalTable, String^ output)
{
	IVector<String^>^ retval = nullptr;
	if (m_ROMNode)
	{
		vector<wstring> res = m_ROMNode->ReverseEvaluateTable((wstring)evalTable->Data(), (wstring)output->Data());
		retval = GetIVectorFromVectorStrings(res);
	}
	return retval;
}

IMap<String^, IVector<String^>^>^ ROMWinRT::ROMNode::ReverseEvaluateTable(String^ evalTable, bool bGetAll)
{
	IMap<String^, IVector<String^>^>^ retval = nullptr;
	if (m_ROMNode)
	{
		map<wstring, vector<wstring> > res = m_ROMNode->ReverseEvaluateTable(evalTable->Data(), bGetAll);
		retval = GetIMapFromMapStrings(res);
	}
	return retval;
}

IMap<String^, IVector<String^>^>^ ROMWinRT::ROMNode::ReverseEvaluateTable(String^ evalTable)
{
	IMap<String^, IVector<String^>^>^ retval = nullptr;
	if (m_ROMNode)
	{
		map<wstring, vector<wstring> > res = m_ROMNode->ReverseEvaluateTable(evalTable->Data());
		retval = GetIMapFromMapStrings(res);
	}
	return retval;
}

//IO
String^ ROMWinRT::ROMNode::SaveXML(bool indented)
{
	String^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring res = m_ROMNode->SaveXML(indented);
		retval = ref new String(res.c_str());		
	}
	return retval;
}

bool ROMWinRT::ROMNode::LoadXML(String^ xmlStr)
{
	bool retval = false;
	if (m_ROMNode)
	{
		retval = m_ROMNode->LoadXML(xmlStr->Data());
	}
	return retval;
}

//XPATH
String^ ROMWinRT::ROMNode::EvaluateXPATH(String^ xpath, String^ guid)
{
	String^ retval = nullptr;
	if (m_ROMNode)
	{
		wstring res = m_ROMNode->EvaluateXPATH((wstring)xpath->Data(), ToASCIIString(guid->Data()));
		retval = ref new String(res.c_str());
	}
	return retval;
}


IVector<ROMWinRT::ROMNode^>^ ROMWinRT::ROMNode::GetArrayFromVectorROM(vector<ROM::ROMNode*> vect)
{
	IVector<ROMNode^>^ arr = ref new Vector<ROMNode^>();
	for (size_t i = 0; i < vect.size(); i++)
	{
		arr->Append(ref new ROMNode(vect[i]));
	}
	return arr;
}



//dictionary
void ROMWinRT::ROMDictionary::LoadDictionary(String^ dictionaryTable)
{
	if (m_ROMDictionary)
	{
		m_ROMDictionary->LoadDictionary(dictionaryTable->Data());
	}
}

ROMWinRT::ROMDictionaryAttribute^ ROMWinRT::ROMDictionary::GetDictionaryAttr(String^ dictAttrName)
{
	ROMDictionaryAttribute^ retval = nullptr;
	if (m_ROMDictionary)
	{
		ROM::ROMDictionaryAttribute* attr = m_ROMDictionary->GetDictionaryAttr(dictAttrName->Data());
		if (attr)
		{
			retval = ref new ROMDictionaryAttribute((void*)attr);
		}
	}
	return retval;
}

IMap<String^, ROMWinRT::ROMDictionaryAttribute^>^ ROMWinRT::ROMDictionary::GetAllDictionaryAttrs()
{
	IMap<String^, ROMDictionaryAttribute^>^ retval = ref new Map<String^, ROMDictionaryAttribute^>();
	if (m_ROMDictionary)
	{
		map<wstring, ROM::ROMDictionaryAttribute>* allAttrs = m_ROMDictionary->GetAllDictionaryAttrs();
		for (map<wstring, ROM::ROMDictionaryAttribute>::iterator it = allAttrs->begin(); it != allAttrs->end(); it++)
		{
			String^ key = ref new String(it->first.c_str());
			ROMDictionaryAttribute^ value = ref new ROMDictionaryAttribute((void*)(&(it->second)));
			retval->Insert(key, value);
		}
	}
	return retval;
}

//LinearEngine
void ROMWinRT::LinearEngine::LoadDictionary(String^ dictionaryTable)
{
	if (m_LinearEngine)
	{
		m_LinearEngine->LoadDictionary(dictionaryTable->Data());
	}
}

ROMWinRT::ROMDictionaryAttribute^ ROMWinRT::LinearEngine::GetDictionaryAttr(String^ dictAttrName)
{
	ROMDictionaryAttribute^ retval = nullptr;
	if (m_LinearEngine)
	{
		ROM::ROMDictionaryAttribute* attr = m_LinearEngine->GetDictionaryAttr(dictAttrName->Data());
		if (attr)
		{
			retval = ref new ROMDictionaryAttribute((void*)attr);
		}
	}
	return retval;
}

IMap<String^, ROMWinRT::ROMDictionaryAttribute^>^ ROMWinRT::LinearEngine::GetAllDictionaryAttrs()
{
	IMap<String^, ROMDictionaryAttribute^>^ retval = ref new Map<String^, ROMDictionaryAttribute^>();
	if (m_LinearEngine)
	{
		map<wstring, ROM::ROMDictionaryAttribute>* allAttrs = m_LinearEngine->GetAllDictionaryAttrs();
		for (map<wstring, ROM::ROMDictionaryAttribute>::iterator it = allAttrs->begin(); it != allAttrs->end(); it++)
		{
			String^ key = ref new String(it->first.c_str());
			ROMDictionaryAttribute^ value = ref new ROMDictionaryAttribute((void*)(&(it->second)));
			retval->Insert(key, value);
		}
	}
	return retval;
}

void ROMWinRT::LinearEngine::EvaluateForAttribute(String^ dictAttrName, IVector<String^>^ newValues, bool bEvalDependents)
{
	if (m_LinearEngine)
	{
		vector<wstring> vals = GetVectorFromIVectorStrings(newValues);
		m_LinearEngine->EvaluateForAttribute(dictAttrName->Data(), vals, bEvalDependents);
	}
}

void ROMWinRT::LinearEngine::EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents)
{
	if (m_LinearEngine)
	{
		m_LinearEngine->EvaluateForAttribute(dictAttrName->Data(), newValue->Data(), bEvalDependents);
	}
}

IVector<ROMWinRT::ROMDictionaryAttribute^>^ ROMWinRT::LinearEngine::GetEvalList()
{
	IVector<ROMDictionaryAttribute^>^ retval = nullptr;
	if (m_LinearEngine)
	{
		vector<ROM::ROMDictionaryAttribute*> res = m_LinearEngine->GetEvalList();
		retval = ref new Vector<ROMDictionaryAttribute^>(res.size());
		size_t i = 0;
		for (vector<ROM::ROMDictionaryAttribute*>::iterator it = res.begin(); it != res.end(); it++)
		{
			retval->SetAt(i, ref new ROMDictionaryAttribute((void*)(*it)));
			i++;
		}			
	}
	return retval;
}

IMap<String^, IVector<String^>^>^ ROMWinRT::LinearEngine::GetTriggers()
{
	Map<String^, IVector<String^>^>^ retval = nullptr;
	if (m_LinearEngine)
	{
		map<wstring, vector<wstring> > res = m_LinearEngine->GetTriggers();
		retval = ref new Map<String^, IVector<String^>^>();
		for (map<wstring, vector<wstring> >::iterator it = res.begin(); it != res.end(); it++)
		{
			IVector<String^>^ attrs = ref new Vector<String^>(it->second.size());
			size_t i = 0;
			for (vector<wstring>::iterator itAttr = it->second.begin(); itAttr != it->second.end(); itAttr++)
			{
				attrs->SetAt(i, ref new String((*itAttr).c_str()));
				i++;
			}
			retval->Insert(ref new String(it->first.c_str()), attrs);
		}			
	}
	return retval;
}
