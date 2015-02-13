/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2014 Eric D. Schmidt, DigiRule Solutions LLC

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
#include "ROMInterfaces.h"
#include "KnowledgeBase.h"
#include "utilities.h"

using namespace ROMUTIL;
using namespace std;
using namespace EDS;



namespace ROM
{
	class ROMNode;
	typedef function<ROMNode*(const wstring&)> ObjectFactory;

	class ROMNode
	{
	friend class ROMDictionary;
	friend class LinearEngine;
	public:
		virtual ~ROMNode(void);
		ROMNode(){_init();}
		ROMNode(const wstring id, ObjectFactory factory = nullptr) { CreateROMNode(id, factory); }
		ROMNode(string id, ObjectFactory factory = nullptr) { CreateROMNode(id, factory); }
		ROMNode(const ROMNode&) = delete;             // Prevent copy-construction
		ROMNode& operator=(const ROMNode&) = delete;  // Prevent assignment
		void				CreateROMNode(const wstring id, ObjectFactory factory = nullptr);
		void				CreateROMNode(const string id, ObjectFactory factory) { CreateROMNode(ROMUTIL::MBCStrToWStr(id), factory); }
		ObjectFactory		ROMObjectFactory;

		//relational functions
		ROMNode*			GetRoot();
		ROMNode*			GetParent() {return m_parent;}
		vector<ROMNode*>	GetAllChildren(bool recurs);
		vector<ROMNode*>	FindObjects(const wstring& xpath);
		ROMNode*			FindFirstObject(const wstring& xpath);
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
		vector<wstring>		EvaluateTable(const wstring& evalTable, const wstring& output, bool bGetAll) { return _evaluateTable(evalTable, output, bGetAll, this); }
		vector<wstring>		EvaluateTable(const wstring& evalTable, const wstring& output) { return _evaluateTable(evalTable, output, this); }
		map<wstring, vector<wstring> > EvaluateTable(const wstring& evalTable, bool bGetAll) { return _evaluateTable(evalTable, bGetAll, this); }
		map<wstring, vector<wstring> > EvaluateTable(const wstring& evalTable) { return _evaluateTable(evalTable, this); }
		vector<wstring>		EvaluateTableWithParam(const wstring& evalTable, const wstring& output, bool bGetAll, wstring& param) { return _evaluateTableWithParam(evalTable, output, bGetAll, param, this); }
		vector<wstring>		EvaluateTableWithParam(const wstring& evalTable, const wstring& output, wstring& param) { return _evaluateTableWithParam(evalTable, output, param, this); }
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring& evalTable, bool bGetAll, wstring& param) { return _evaluateTableWithParam(evalTable, bGetAll, param, this); }
		map<wstring, vector<wstring> > EvaluateTableWithParam(const wstring& evalTable, wstring& param) { return _evaluateTableWithParam(evalTable, param, this); }
		wstring				GetFirstTableResult(const wstring& tableName, const wstring& output) { return _getFirstTableResult(tableName, output, this); }
		vector<wstring>		ReverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr, bool bGetAll) { return _reverseEvaluateTable(evalTable, inputAttr, bGetAll, this); }
		vector<wstring>		ReverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr) { return _reverseEvaluateTable(evalTable, inputAttr, this); }
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring& evalTable, bool bGetAll) { return _reverseEvaluateTable(evalTable, bGetAll, this); }
		map<wstring, vector<wstring> > ReverseEvaluateTable(const wstring& evalTable) { return _reverseEvaluateTable(evalTable, this); }
		EDS::CKnowledgeBase* GetKnowledgeBase() { ROMNode* owner = nullptr; return _getKnowledge(owner); }
		EDS::CKnowledgeBase* GetKnowledgeBase(ROMNode*& owner) { return _getKnowledge(owner); }
		void				SetKnowledgeBase(EDS::CKnowledgeBase* rules) { m_KnowledgeBase = rules; }


		//IO
		wstring				SaveXML(bool prettyprint);
		static ROMNode*		LoadXML(const wstring& xmlFile, ObjectFactory factory) { return ROMNode::_loadXML(xmlFile, true, factory); }
		static ROMNode*		LoadXMLFromString(const wstring& xmlStr, ObjectFactory factory) { return ROMNode::_loadXML(xmlStr, false, factory); }

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

#ifndef CLR //these internal methods are called by .NET to assist with passing of managed objects
	private:
#else
	public:
#endif
		vector<wstring>		_evaluateTable(const wstring& evalTable, const wstring& output, bool bGetAll, void* context);
		vector<wstring>		_evaluateTable(const wstring& evalTable, const wstring& output, void* context);
		map<wstring, vector<wstring> > _evaluateTable(const wstring& evalTable, bool bGetAll, void* context);
		map<wstring, vector<wstring> > _evaluateTable(const wstring& evalTable, void* context);
		vector<wstring>		_evaluateTableWithParam(const wstring& evalTable, const wstring& output, bool bGetAll, wstring& param, void* context);
		vector<wstring>		_evaluateTableWithParam(const wstring& evalTable, const wstring& output, wstring& param, void* context);
		map<wstring, vector<wstring> > _evaluateTableWithParam(const wstring& evalTable, bool bGetAll, wstring& param, void* context);
		map<wstring, vector<wstring> > _evaluateTableWithParam(const wstring& evalTable, wstring& param, void* context);
		wstring				_getFirstTableResult(const wstring& tableName, const wstring& output, void* context);
		vector<wstring>		_reverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr, bool bGetAll, void* context);
		vector<wstring>		_reverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr, void* context);
		map<wstring, vector<wstring> > _reverseEvaluateTable(const wstring& evalTable, bool bGetAll, void* context);
		map<wstring, vector<wstring> > _reverseEvaluateTable(const wstring& evalTable, void* context);

	private:
		vector<wstring>			_getPossibleValues(const wstring& evalTable, const wstring& outputName);
		ROMNode*				_findObjectGUID(const string& guid);
		void					_findAllChildObjects(vector<ROMNode*>* res);
		void					_findObjects(const wstring& id, bool recurs, vector<ROMNode*>* res);
		bool					_anyHasChanged();
		void					_setAllUnchanged();
		wstring					_generateXML(bool bRegen, bool prettyprint);
		wstring					_generateAttrNode(const wstring& id);
		static ROMNode*			_buildObject(Node objectNode, ObjectFactory factory);
		void					_createXMLDoc(bool bForceLoad, bool prettyprint);
		static wstring			_convertXMLDocToString(bool prettyprint, Document xmlDoc);
		EDS::CKnowledgeBase*	_getKnowledge(ROMNode*& owner);
		EDS::CKnowledgeBase*	_getKnowledge() { ROMNode* owner = nullptr; return _getKnowledge(owner); }
		ROMNode*				_getActiveContext();
		void					_init(ObjectFactory factory = nullptr);
		static ROMNode*			_loadXML(const wstring& xmlStr, bool isFile, ObjectFactory factory);
#ifdef USE_MSXML
		static Document			_createMSXMLDoc();
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
