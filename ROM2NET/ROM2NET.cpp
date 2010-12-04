// This is the main DLL file.

#include "stdafx.h"
#include "ROM2NET.h"

namespace ROM2NET
{
	bool ROMNodeNET::CreateROMNodeNET(System::String^ id)
	{
		wstring wsID = MarshalString(id);
		m_ROMNode = new ROM::ROMNode(wsID);
		if (m_ROMNode)
			return true;
		else
			return false;
	}

	ROMNodeNET^ ROMNodeNET::GetRoot()
	{
		ROMNodeNET^ retval = nullptr;
		if (m_ROMNode)
		{
			ROM::ROMNode *rootNode = m_ROMNode->GetRoot();
			if (rootNode)
			{
				retval = gcnew ROMNodeNET((IntPtr)rootNode);
			}
		}
		return retval;
	}

	ROMNodeNET^	ROMNodeNET::Parent()
	{
		ROMNodeNET^ retval = nullptr;
		if (m_ROMNode)
		{
			ROM::ROMNode *parentNode = m_ROMNode->GetParent();
			if (parentNode)
			{
				retval = gcnew ROMNodeNET(gcnew String(parentNode->GetROMObjectID().c_str()));
			}
		}
		return retval;
	}

	array<ROMNodeNET^>^ ROMNodeNET::GetAllChildren()
	{
		array<ROMNodeNET^>^ retval = nullptr;
		if (m_ROMNode)
		{
			vector<ROM::ROMNode*> vChildren = m_ROMNode->GetAllChildren();
			retval = GetArrayFromVectorROM(vChildren);
		}
		
		return retval;
	}

	bool ROMNodeNET::AddChildROMObject(ROMNodeNET^ child)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->AddChildROMObject(child->m_ROMNode);
		}
		return retval;
	}

	bool ROMNodeNET::RemoveChildROMObject(ROMNodeNET^ child)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->RemoveChildROMObject(child->m_ROMNode);
		}
		return retval;
	}

	bool ROMNodeNET::DestroyROMObject()
	{
		bool retval = false;
		if (m_ROMNode)
		{
			retval = m_ROMNode->DestroyROMObject();
			delete m_ROMNode;
			m_ROMNode = NULL;
		}
		return retval;
	}

	//attribute functions
	String^ ROMNodeNET::GetAttribute(String^ id, String^ name, bool immediate)
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

	bool ROMNodeNET::GetAttributeExists(String^ id, String^ name)
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

	bool ROMNodeNET::SetAttribute(String^ id, String^ name, String^ value)
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

	bool ROMNodeNET::RemoveAttribute(String^ id, String^ name)
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

	bool ROMNodeNET::SetROMObjectValue(String^ name, String^ value)
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

	String^ ROMNodeNET::GetROMObjectValue(String^ name)
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

	bool ROMNodeNET::RemoveROMObjectValue(String^ id)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			retval = m_ROMNode->RemoveROMObjectValue(wsID);
		}
		return retval;
	}

	String^ ROMNodeNET::GetROMObjectID()
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring res = m_ROMNode->GetROMObjectID();
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	void ROMNodeNET::SetROMObjectID(String^ id)
	{
		if (m_ROMNode)
		{
			wstring wsID = MarshalString(id);
			m_ROMNode->SetROMObjectID(wsID);
		}
	}

	Dictionary<String^, Dictionary<String^, String^>^>^	ROMNodeNET::GetAllAttributes()
	{
		Dictionary<String^, Dictionary<String^, String^>^>^ retval = gcnew Dictionary<String^, Dictionary<String^, String^>^>();
		if (m_ROMNode)
		{
			FASTMAP_MAPS attrs = m_ROMNode->GetAllAttributes();
			for (FASTMAP_MAPS::iterator it = attrs.begin(); it != attrs.end(); it++)
			{
				Dictionary<String^, String^>^ dictAttrs = gcnew Dictionary<String^, String^>();
				String^ key = gcnew String(it->first.c_str());
				for (FASTMAP::iterator itValues = it->second.begin(); itValues != it->second.end(); itValues++)
				{
					dictAttrs->Add(gcnew String(itValues->first.c_str()), gcnew String(itValues->second.c_str()));
				}
				retval->Add(key, dictAttrs);
			}
		}
		return retval;
	}

	//rules
	bool ROMNodeNET::LoadRules(String^ knowledge_file)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsFile = MarshalString(knowledge_file);
			retval = m_ROMNode->LoadRules(wsFile);
		}
		return retval;
	}
		
	array<String^>^ ROMNodeNET::EvaluateTable(String^ evalTable, String^ output, bool bGetAll)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			vector<wstring> res = m_ROMNode->EvaluateTable(wsTable, wsOutput, bGetAll);
			retval = GetArrayFromVectorStrings(res);
		}
		return retval;
	}

	array<String^>^ ROMNodeNET::EvaluateTable(String^ evalTable, String^ output)
	{
		array<String^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			wstring wsOutput = MarshalString(output);
			vector<wstring> res = m_ROMNode->EvaluateTable(wsTable, wsOutput);
			retval = GetArrayFromVectorStrings(res);
		}
		return retval;
	}

	Dictionary<String^, array<String^>^>^ ROMNodeNET::EvaluateTable(String^ evalTable, bool bGetAll)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTable(wsTable, bGetAll);
			retval = GetDictionaryFromMapStrings(res);
		}		
		return retval;
	}

	Dictionary<String^, array<String^>^>^ ROMNodeNET::EvaluateTable(String^ evalTable)
	{
		Dictionary<String^, array<String^>^>^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsTable = MarshalString(evalTable);
			map<wstring, vector<wstring> > res = m_ROMNode->EvaluateTable(wsTable);
			retval = GetDictionaryFromMapStrings(res);
		}		
		return retval;
	}

	//IO
	String^ ROMNodeNET::DumpTree(bool indented)
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring res = m_ROMNode->DumpTree(indented);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}

	bool ROMNodeNET::LoadTree(String^ xmlStr)
	{
		bool retval = false;
		if (m_ROMNode)
		{
			wstring wsXML = MarshalString(xmlStr);
			retval = m_ROMNode->LoadTree(wsXML);
		}
		return retval;
	}

	//XPATH
	String^ ROMNodeNET::EvaluateXPATH(String^ xpath)
	{
		String^ retval = nullptr;
		if (m_ROMNode)
		{
			wstring wsXPATH = MarshalString(xpath);
			wstring res = m_ROMNode->EvaluateXPATH(wsXPATH);
			retval = gcnew String(res.c_str());
		}
		return retval;
	}


	array<ROMNodeNET^>^ ROMNodeNET::GetArrayFromVectorROM(vector<ROM::ROMNode*> vect)
	{
		array<ROMNodeNET^>^ arr = gcnew array<ROMNodeNET^>(vect.size());
		for (size_t i = 0; i < vect.size(); i++)
		{
			arr[i] = gcnew ROMNodeNET((IntPtr)vect[i]);
		}
		return arr;
	}



	//dictionary
	void ROMDictionaryNET::LoadDictionary(String^ dictionaryTable)
	{
		if (m_ROMDictionary)
		{
			wstring wsDict = MarshalString(dictionaryTable);
			m_ROMDictionary->LoadDictionary(wsDict);
		}
	}

	ROMDictionaryAttributeNET^ ROMDictionaryNET::GetDictionaryAttr(String^ dictAttrName)
	{
		ROMDictionaryAttributeNET^ retval = nullptr;
		if (m_ROMDictionary)
		{
			wstring wsName = MarshalString(dictAttrName);
			ROM::ROMDictionaryAttribute* attr = m_ROMDictionary->GetDictionaryAttr(wsName);
			if (attr)
			{
				retval = gcnew ROMDictionaryAttributeNET((IntPtr)attr);
			}
		}
		return retval;
	}

	Dictionary<String^, ROMDictionaryAttributeNET^>^ ROMDictionaryNET::GetAllDictionaryAttrs()
	{
		Dictionary<String^, ROMDictionaryAttributeNET^>^ retval = gcnew Dictionary<String^, ROMDictionaryAttributeNET^>();
		if (m_ROMDictionary)
		{
			map<wstring, ROM::ROMDictionaryAttribute>* allAttrs = m_ROMDictionary->GetAllDictionaryAttrs();
			for (map<wstring, ROM::ROMDictionaryAttribute>::iterator it = allAttrs->begin(); it != allAttrs->end(); it++)
			{
				String^ key = gcnew String(it->first.c_str());
				ROMDictionaryAttributeNET^ value = gcnew ROMDictionaryAttributeNET((IntPtr)(&(it->second)));
				retval->Add(key, value);
			}
		}
		return retval;
	}

	//LinearEngine
	void LinearEngineNET::LoadDictionary(String^ dictionaryTable)
	{
		if (m_LinearEngine)
		{
			wstring wsDict = MarshalString(dictionaryTable);
			m_LinearEngine->LoadDictionary(wsDict);
		}
	}

	ROMDictionaryAttributeNET^ LinearEngineNET::GetDictionaryAttr(String^ dictAttrName)
	{
		ROMDictionaryAttributeNET^ retval = nullptr;
		if (m_LinearEngine)
		{
			wstring wsName = MarshalString(dictAttrName);
			ROM::ROMDictionaryAttribute* attr = m_LinearEngine->GetDictionaryAttr(wsName);
			if (attr)
			{
				retval = gcnew ROMDictionaryAttributeNET((IntPtr)attr);
			}
		}
		return retval;
	}

	Dictionary<String^, ROMDictionaryAttributeNET^>^ LinearEngineNET::GetAllDictionaryAttrs()
	{
		Dictionary<String^, ROMDictionaryAttributeNET^>^ retval = gcnew Dictionary<String^, ROMDictionaryAttributeNET^>();
		if (m_LinearEngine)
		{
			map<wstring, ROM::ROMDictionaryAttribute>* allAttrs = m_LinearEngine->GetAllDictionaryAttrs();
			for (map<wstring, ROM::ROMDictionaryAttribute>::iterator it = allAttrs->begin(); it != allAttrs->end(); it++)
			{
				String^ key = gcnew String(it->first.c_str());
				ROMDictionaryAttributeNET^ value = gcnew ROMDictionaryAttributeNET((IntPtr)(&(it->second)));
				retval->Add(key, value);
			}
		}
		return retval;
	}

	void LinearEngineNET::EvaluateForAttribute(String^ dictAttrName, array<String^>^ newValues, bool bEvalDependents)
	{
		if (m_LinearEngine)
		{
			wstring name = MarshalString(dictAttrName);
			vector<wstring> vals = GetVectorFromArrayStrings(newValues);
			m_LinearEngine->EvaluateForAttribute(name, vals, bEvalDependents);
		}
	}

	void LinearEngineNET::EvaluateForAttribute(String^ dictAttrName, String^ newValue, bool bEvalDependents)
	{
		if (m_LinearEngine)
		{
			wstring name = MarshalString(dictAttrName);
			wstring val = MarshalString(newValue);
			m_LinearEngine->EvaluateForAttribute(name, val, bEvalDependents);
		}
	}

	array<ROMDictionaryAttributeNET^>^ LinearEngineNET::GetEvalList()
	{
		array<ROMDictionaryAttributeNET^>^ retval = nullptr;
		if (m_LinearEngine)
		{
			vector<ROM::ROMDictionaryAttribute*>* res = m_LinearEngine->GetEvalList();
			if (res)
			{
				retval = gcnew array<ROMDictionaryAttributeNET^>(res->size());
				size_t i = 0;
				for (vector<ROM::ROMDictionaryAttribute*>::iterator it = res->begin(); it != res->end(); it++)
				{
					retval[i] = gcnew ROMDictionaryAttributeNET((IntPtr)(*it));
					i++;
				}
			}
		}
		return retval;
	}

	
}
