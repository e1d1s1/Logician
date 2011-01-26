/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2011 Eric D. Schmidt

    Relational Object Model is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
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
#include "KnowledgeBase.h"
#include "utilities.h"

using namespace ROMUTIL;
using namespace std;
using namespace EDS;

#ifdef _MSC_VER
	typedef stdext::hash_map<wstring, wstring> FASTMAP;
	typedef stdext::hash_map<wstring, stdext::hash_map<wstring, wstring> > FASTMAP_MAPS;
#else
	#ifdef __GNUC__
		typedef __gnu_cxx::hash_map<wstring, wstring> FASTMAP;
		typedef __gnu_cxx::hash_map<wstring, __gnu_cxx::hash_map<wstring, wstring> > FASTMAP_MAPS;
	#else
		typedef std::map<wstring, wstring> FASTMAP;
		typedef std::map<wstring, std::map<wstring, wstring> > FASTMAP_MAPS;
	#endif
#endif

namespace ROM
{
	class ROMNode
	{
	friend class ROMDictionary;
	friend class LinearEngine;
	public:
		~ROMNode(void);
		ROMNode(){_init();}
		ROMNode(wstring id) {CreateROMNode(id);}
		ROMNode(string id) {CreateROMNode(id);}
		void CreateROMNode(wstring id);
		void CreateROMNode(string id) {CreateROMNode(ROMUTIL::MBCStrToWStr(id));}
		void				SetTableDebugHandler(DebugHandler debugger);
		void				GenerateTableDebugMessages(bool bGenerate);
		wstring				GetTableDebugMessages();
		

		//relational functions
		ROMNode*			GetRoot();		
		ROMNode*			GetParent() {return m_parent;}
		vector<ROMNode*>	GetAllChildren(bool recurs);
		vector<ROMNode*>	FindObjects(wstring xpath);
		vector<ROMNode*>	FindAllObjectsByID(wstring id, bool recurs);
		ROMNode*			FindObjectByGUID(wstring guid){return FindObjectByGUID(ToASCIIString(guid));}
		bool				AddChildROMObject(ROMNode *child);
		bool				RemoveChildROMObject(ROMNode *child);
		bool				DestroyROMObject();
		ROMNode*			Clone();

		//attribute functions
		wstring				GetAttribute(wstring id, wstring name, bool immediate = false);
		wstring				GetAttribute(wstring id, bool immediate = false) {return GetAttribute(id, L"value", immediate);}
		bool				GetAttributeExists(wstring id, wstring name = L"value");
		bool				SetAttribute(wstring id, wstring name, wstring value);
		bool				SetAttribute(wstring id, wstring value) {return SetAttribute(id, L"value", value);}
		bool				SetAttributeValue(wstring id, wstring value) {return SetAttribute(id, value);}
		bool				RemoveAttribute(wstring id, wstring name = L"value");	
		bool				SetROMObjectValue(wstring name, wstring value);
		wstring				GetROMObjectValue(wstring name);
		bool				RemoveROMObjectValue(wstring name);	
		wstring				GetROMObjectID() {return m_id;}
		void				SetROMObjectID(wstring id) {m_id = id;}
		string				GetROMGUID() {return m_guid;}
		FASTMAP_MAPS		GetAllAttributes() {return m_attrs;}

		//rules
		bool				LoadRules(wstring knowledge_file);
		bool				LoadRulesFromString(wstring xmlStr);
		vector<wstring>		EvaluateTable(wstring evalTable, wstring output, bool bGetAll);
		vector<wstring>		EvaluateTable(wstring evalTable, wstring output);
		vector<wstring>		EvaluateTableForAttr(wstring evalTable, wstring output, bool bGetAll) {return EvaluateTable(evalTable, output, bGetAll);}
		vector<wstring>		EvaluateTableForAttr(wstring evalTable, wstring output) {return EvaluateTable(evalTable, output);}
		map<wstring, vector<wstring> > EvaluateTable(wstring evalTable, bool bGetAll);
		map<wstring, vector<wstring> > EvaluateTable(wstring evalTable);
		vector<wstring>		ReverseEvaluateTable(wstring evalTable, wstring inputAttr, bool bGetAll);
		vector<wstring>		ReverseEvaluateTable(wstring evalTable, wstring inputAttr);
		map<wstring, vector<wstring> > ReverseEvaluateTable(wstring evalTable, bool bGetAll);
		map<wstring, vector<wstring> > ReverseEvaluateTable(wstring evalTable);
		EDS::CKnowledgeBase* GetKnowledgeBase() {return _getKnowledge();}


		//IO
		wstring				SaveXML(bool indented);
		bool				LoadXML(wstring xmlStr);			

		//XPATH
		wstring				EvaluateXPATH(wstring xpath, string guid);
		wstring				EvaluateXPATH(wstring xpath) {return EvaluateXPATH(xpath, m_guid);}

		//ascii overloads
		vector<ROMNode*>	FindObjects(string xpath){return FindObjects(MBCStrToWStr(xpath));}
		vector<ROMNode*>	FindAllObjectsByID(string id, bool recurs){return FindAllObjectsByID(MBCStrToWStr(id), recurs);}
		ROMNode*			FindObjectByGUID(string guid);
		string				GetAttribute(string id, string name, bool immediate = false) {return ROMUTIL::WStrToMBCStr(GetAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), immediate));}
		string				GetAttribute(string id, bool immediate = false) {return GetAttribute(id, "value", immediate);}
		bool				GetAttributeExists(string id, string name = "value") {return GetAttributeExists(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name));}
		bool				SetAttribute(string id, string name, string value) {return SetAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value));}
		bool				SetAttribute(string id, string value) {return SetAttribute(id, "value", value);}
		bool				RemoveAttribute(string id, string name = "value") {return RemoveAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name));}
		bool				SetROMObjectValue(string name, string value) {return SetROMObjectValue(ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value));}
		string				GetROMObjectValue(string name) {return ROMUTIL::WStrToMBCStr(GetROMObjectValue(ROMUTIL::MBCStrToWStr(name)));}
		bool				RemoveROMObjectValue(string id) {return RemoveROMObjectValue(ROMUTIL::MBCStrToWStr(id));}
		string				GetROMObjectIDA() {return ROMUTIL::ToASCIIString(m_id);}
		void				SetROMObjectID(string name) {m_id = ROMUTIL::MBCStrToWStr(name);}
		bool				LoadRules(string knowledge_file) {return LoadRules(ROMUTIL::MBCStrToWStr(knowledge_file));}
		bool				LoadRulesFromString(string xmlStr) {return LoadRulesFromString(ROMUTIL::MBCStrToWStr(xmlStr));}
		vector<string>		EvaluateTable(string evalTable, string output, bool bGetAll) {return ROMUTIL::WStrToMBCStrVector(EvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(output), bGetAll));}
		vector<string>		EvaluateTable(string evalTable, string output) {return ROMUTIL::WStrToMBCStrVector(EvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(output)));}
		map<string, vector<string> > EvaluateTable(string evalTable, bool bGetAll) {return ROMUTIL::WStrToMBCStrMapVector(EvaluateTable(MBCStrToWStr(evalTable), bGetAll));}
		map<string, vector<string> > EvaluateTable(string evalTable) {return ROMUTIL::WStrToMBCStrMapVector(EvaluateTable(MBCStrToWStr(evalTable)));}
		vector<string>		ReverseEvaluateTable(string evalTable, string inputAttr, bool bGetAll) {return ROMUTIL::WStrToMBCStrVector(ReverseEvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(inputAttr), bGetAll));}
		vector<string>		ReverseEvaluateTable(string evalTable, string inputAttr) {return ROMUTIL::WStrToMBCStrVector(ReverseEvaluateTable(MBCStrToWStr(evalTable), MBCStrToWStr(inputAttr)));}
		map<string, vector<string> > ReverseEvaluateTable(string evalTable, bool bGetAll) {return ROMUTIL::WStrToMBCStrMapVector(ReverseEvaluateTable(MBCStrToWStr(evalTable), bGetAll));}
		map<string, vector<string> > ReverseEvaluateTable(string evalTable) {return ROMUTIL::WStrToMBCStrMapVector(ReverseEvaluateTable(MBCStrToWStr(evalTable)));}
		string				EvaluateXPATH(string xpath, string guid) {return ROMUTIL::WStrToMBCStr(EvaluateXPATH(ROMUTIL::MBCStrToWStr(xpath), guid));}
		string				EvaluateXPATH(string xpath) {return ROMUTIL::WStrToMBCStr(EvaluateXPATH(ROMUTIL::MBCStrToWStr(xpath), m_guid));}

	private:
		vector<wstring>			LoadInputs(wstring evalTable);
		vector<wstring>			LoadOutputs(wstring evalTable);
		vector<wstring>			GetPossibleValues(wstring evalTable, wstring outputName);
		wstring					GetATableInputValue(wstring input);
		ROMNode*				_findObjectGUID(string guid);
		void					_findAllChildObjects(vector<ROMNode*>* res);
		void					_findObjects(wstring id, bool recurs, vector<ROMNode*>* res);
		bool					_anyHasChanged();
		void					_setAllUnchanged();
		wstring					_generateXML(bool bRegen);
		ROMNode*				_buildObject(Node objectNode, ROMNode* parent);
		void					_createXMLDoc(bool bForceLoad = false);
		wstring					_convertXMLDocToString(bool indented);
		EDS::CKnowledgeBase*	_getKnowledge();
		void					_init();

		wstring m_id;
		string m_guid;
		bool m_bChanged;
		Document m_xmlDoc;
		wstring m_lastContents;
		wstring m_lastAttrContents;		
		vector<ROMNode*> m_children;
		ROMNode* m_parent;
		FASTMAP_MAPS m_attrs;
		FASTMAP m_nodeValues;

		EDS::CKnowledgeBase		*m_KnowledgeBase;
	};
}