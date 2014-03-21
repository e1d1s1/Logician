/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2013 Eric D. Schmidt, DigiRule Solutions LLC

    Relational Object Model is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    Relational Object Model is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Relational Object Model.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include "KnowledgeBase.h"
#include "utilities.h"

using namespace ROMUTIL;
using namespace std;
using namespace EDS;

namespace ROM
{
	class ROMNode
	{
	friend class ROMDictionary;
	friend class LinearEngine;
	public:
		virtual ~ROMNode(void);
		ROMNode(){_init();}
		ROMNode(wstring id) {CreateROMNode(id);}
		ROMNode(string id) {CreateROMNode(id);}
		ROMNode(const ROMNode&) = delete;             // Prevent copy-construction
		ROMNode& operator=(const ROMNode&) = delete;  // Prevent assignment
		void				CreateROMNode(wstring id);
		void				CreateROMNode(string id) {CreateROMNode(ROMUTIL::MBCStrToWStr(id));}
		void				SetTableDebugHandler(function<void(const wstring&)> debugger);
		void				EnableRemoteDebugger(bool enable) { if (m_KnowledgeBase) m_KnowledgeBase->EnableRemoteDebugger(enable); }

		//relational functions
		ROMNode*			GetRoot();
		ROMNode*			GetParent() {return m_parent;}
		vector<ROMNode*>	GetAllChildren(bool recurs);
		vector<ROMNode*>	FindObjects(const wstring& xpath);
		vector<ROMNode*>	FindAllObjectsByID(const wstring& id, bool recurs);
		ROMNode*			FindObjectByGUID(const wstring& guid){ return FindObjectByGUID(ToASCIIString(guid)); }
		bool				AddChildROMObject(ROMNode *child);
		bool				RemoveChildROMObject(ROMNode *child);
		bool				RemoveFromParent();
		vector<ROMNode*>	GetAllFriends();
		bool				AddFriend(ROMNode *friendObj);
		bool				RemoveFriend(ROMNode *friendObj);
		bool				RemoveAllFriends();
		bool				DestroyROMObject();
		ROMNode*			Clone();

		//attribute functions
		virtual wstring		GetAttribute(const wstring& id, const wstring& name, bool immediate = false);
		virtual wstring		GetAttribute(const wstring& id, bool immediate = false) { return GetAttribute(id, L"value", immediate); }
		virtual bool		GetAttributeExists(const wstring& id, const wstring& name = L"value");
		virtual bool		SetAttribute(const wstring& id, const wstring& name, const wstring& value);
		virtual bool		SetAttribute(const wstring& id, const wstring& value) { return SetAttribute(id, L"value", value); }
		virtual bool		SetAttributeValue(const wstring& id, const wstring& value) { return SetAttribute(id, value); }
		virtual bool		RemoveAttribute(const wstring& id, const wstring& name = L"value");
		virtual bool		SetROMObjectValue(const wstring& name, const wstring& value);
		virtual wstring		GetROMObjectValue(const wstring& name);
		virtual bool		RemoveROMObjectValue(const wstring& name);
		wstring				GetROMObjectID() {return m_id;}
		void				SetROMObjectID(const wstring& id) { m_id = id; }
		string				GetROMGUID() {return m_guid;}
		unordered_map<wstring, std::unordered_map<wstring, wstring>> GetAllAttributes() { return m_attrs; }

		//rules
		bool				LoadRules(const wstring& knowledge_file, size_t threads = 1);
		bool				LoadRulesFromString(const wstring& xmlStr, size_t threads = 1);
		vector<wstring>		EvaluateTable(const wstring& evalTable, const wstring& output, bool bGetAll);
		vector<wstring>		EvaluateTable(const wstring& evalTable, const wstring& output);
		map<wstring, vector<wstring> > EvaluateTable(const wstring& evalTable, bool bGetAll);
		map<wstring, vector<wstring> > EvaluateTable(const wstring& evalTable);
		vector<wstring>		EvaluateTableWithParam(const wstring& evalTable, const wstring& output, bool bGetAll, wstring& param);
		vector<wstring>		EvaluateTableWithParam(const wstring& evalTable, const wstring& output, wstring& param);
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring& evalTable, bool bGetAll, wstring& param);
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring& evalTable, wstring& param);
		wstring				GetFirstTableResult(const wstring& tableName, const wstring& output);
		vector<wstring>		ReverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr, bool bGetAll);
		vector<wstring>		ReverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr);
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring& evalTable, bool bGetAll);
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring& evalTable);
		EDS::CKnowledgeBase* GetKnowledgeBase() {return _getKnowledge();}


		//IO
		wstring				SaveXML(bool indented);
		bool				LoadXML(const wstring& xmlStr);

		//XPATH
		wstring				EvaluateXPATH(const wstring& xpath, const string& guid);
		wstring				EvaluateXPATH(const wstring& xpath) { return EvaluateXPATH(xpath, m_guid); }

		//ascii overloads
		vector<ROMNode*>	FindObjects(const string& xpath){return FindObjects(MBCStrToWStr(xpath));}
		vector<ROMNode*>	FindAllObjectsByID(const string& id, bool recurs){ return FindAllObjectsByID(MBCStrToWStr(id), recurs); }
		ROMNode*			FindObjectByGUID(const string& guid);
		string				GetAttribute(const string& id, const string& name, bool immediate = false) { return ROMUTIL::WStrToMBCStr(GetAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), immediate)); }
		string				GetAttribute(const string& id, bool immediate = false) { return GetAttribute(id, "value", immediate); }
		bool				GetAttributeExists(const string& id, const string& name = "value") { return GetAttributeExists(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name)); }
		bool				SetAttribute(const string& id, const string& name, const string& value) { return SetAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value)); }
		bool				SetAttribute(const string& id, const string& value) { return SetAttribute(id, "value", value); }
		bool				RemoveAttribute(const string& id, const string& name = "value") { return RemoveAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name)); }
		bool				SetROMObjectValue(const string& name, const string& value) { return SetROMObjectValue(ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value)); }
		string				GetROMObjectValue(const string& name) { return ROMUTIL::WStrToMBCStr(GetROMObjectValue(ROMUTIL::MBCStrToWStr(name))); }
		bool				RemoveROMObjectValue(const string& id) { return RemoveROMObjectValue(ROMUTIL::MBCStrToWStr(id)); }
		string				GetROMObjectIDA() {return ROMUTIL::ToASCIIString(m_id);}
		void				SetROMObjectID(const string& name) { m_id = ROMUTIL::MBCStrToWStr(name); }
		bool				LoadRules(const string& knowledge_file) { return LoadRules(ROMUTIL::MBCStrToWStr(knowledge_file)); }
		bool				LoadRulesFromString(const string& xmlStr) { return LoadRulesFromString(ROMUTIL::MBCStrToWStr(xmlStr)); }
		vector<string>		EvaluateTable(const string& evalTable, const string& output, bool bGetAll) { return ROMUTIL::WStrToMBCStrVector(EvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(output), bGetAll)); }
		vector<string>		EvaluateTable(const string& evalTable, const string& output) { return ROMUTIL::WStrToMBCStrVector(EvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(output))); }
		map<string, vector<string> > EvaluateTable(const string& evalTable, bool bGetAll) { return ROMUTIL::WStrToMBCStrMapVector(EvaluateTable(MBCStrToWStr(evalTable), bGetAll)); }
		map<string, vector<string> > EvaluateTable(const string& evalTable) { return ROMUTIL::WStrToMBCStrMapVector(EvaluateTable(MBCStrToWStr(evalTable))); }
		vector<string>		EvaluateTableWithParam(const string& evalTable, const string& output, bool bGetAll, string& param);
		vector<string>		EvaluateTableWithParam(const string& evalTable, const string& output, string& param);
		map<string, vector<string> > EvaluateTableWithParam(const string& evalTable, bool bGetAll, string& param);
		map<string, vector<string> > EvaluateTableWithParam(const string& evalTable, string& param);
		string				GetFirstTableResult(const string& tableName, const string& output) { return WStrToMBCStr(GetFirstTableResult(ROMUTIL::MBCStrToWStr(tableName), ROMUTIL::MBCStrToWStr(output))); }
		vector<string>		ReverseEvaluateTable(const string& evalTable, const string& inputAttr, bool bGetAll) { return ROMUTIL::WStrToMBCStrVector(ReverseEvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(inputAttr), bGetAll)); }
		vector<string>		ReverseEvaluateTable(const string& evalTable, const string& inputAttr) { return ROMUTIL::WStrToMBCStrVector(ReverseEvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(inputAttr))); }
		map<string, vector<string> > ReverseEvaluateTable(const string& evalTable, bool bGetAll) { return ROMUTIL::WStrToMBCStrMapVector(ReverseEvaluateTable(MBCStrToWStr(evalTable), bGetAll)); }
		map<string, vector<string> > ReverseEvaluateTable(const string& evalTable) { return ROMUTIL::WStrToMBCStrMapVector(ReverseEvaluateTable(MBCStrToWStr(evalTable))); }
		string				EvaluateXPATH(const string& xpath, const string& guid) { return ROMUTIL::WStrToMBCStr(EvaluateXPATH(ROMUTIL::MBCStrToWStr(xpath), guid)); }
		string				EvaluateXPATH(const string& xpath) { return ROMUTIL::WStrToMBCStr(EvaluateXPATH(ROMUTIL::MBCStrToWStr(xpath), m_guid)); }

	private:
		vector<wstring>			GetPossibleValues(const wstring& evalTable, const wstring& outputName);
		wstring					GetATableInputValue(const wstring& input);
		ROMNode*				_findObjectGUID(const string& guid);
		void					_findAllChildObjects(vector<ROMNode*>* res);
		void					_findObjects(const wstring& id, bool recurs, vector<ROMNode*>* res);
		bool					_anyHasChanged();
		void					_setAllUnchanged();
		wstring					_generateXML(bool bRegen);
		ROMNode*				_buildObject(Node objectNode, ROMNode* parent);
		void					_createXMLDoc(bool bForceLoad = false);
		wstring					_convertXMLDocToString(bool indented);
		EDS::CKnowledgeBase*	_getKnowledge();
		ROMNode*				_getActiveContext();
		void					_init();
#ifdef USE_MSXML
		Document				_createMSXMLDoc();
#endif

		wstring m_id;
		string m_guid;
		bool m_bChanged;
		Document m_xmlDoc;
		wstring m_lastContents;
		wstring m_lastAttrContents;
		vector<ROMNode*> m_children;
		vector<ROMNode*> m_friends;
		ROMNode* m_parent;
		unordered_map<wstring, std::unordered_map<wstring, wstring> > m_attrs;
		unordered_map<wstring, wstring> m_nodeValues;

		EDS::CKnowledgeBase*	m_KnowledgeBase;
	};
}
