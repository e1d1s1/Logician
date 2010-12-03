/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 Eric D. Schmidt

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
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
		ROMNode(wstring id) {m_name = id; _init();}
		ROMNode(string id) {m_name = ROMUTIL::MBCStrToWStr(id); _init();}
		~ROMNode(void);

		//relational interface
		ROMNode*			GetRoot();
		ROMNode*			Parent() {return m_parent;}
		vector<ROMNode*>	GetAllChildren() {return m_children;}
		bool				AddChildROMObject(ROMNode *child) {m_children.push_back(child); return true;}
		bool				RemoveChildROMObject(ROMNode *child);
		bool				DestroyROMObject();

		//attribute interface
		wstring				GetAttribute(wstring id, wstring name, bool immediate = false);
		wstring				GetAttribute(wstring id, bool immediate = false) {return GetAttribute(id, L"value", immediate);}
		bool				SetAttribute(wstring id, wstring name, wstring value);
		bool				SetAttribute(wstring id, wstring value) {return SetAttribute(id, L"value", value);}
		bool				SetAttributeValue(wstring id, wstring value) {return SetAttribute(id, value);}
		bool				RemoveAttribute(wstring id);	
		bool				SetROMObjectValue(wstring name, wstring value);
		wstring				GetROMObjectValue(wstring name);
		bool				RemoveROMObjectAttribute(wstring id);	
		wstring				GetROMObjectName() {return m_name;}
		void				SetROMObjectName(wstring name) {m_name = name;}
		FASTMAP_MAPS		GetAllAttributes() {return m_attrs;}

		//rules
		bool				LoadRules(wstring knowledge_file) {return m_KnowledgeBase.CreateKnowledgeBase(knowledge_file);}
		vector<wstring>		EvaluateTable(wstring evalTable, wstring output, bool bGetAll = true);
		vector<wstring>		EvaluateTableForAttr(wstring evalTable, wstring output, bool bGetAll = true) {return EvaluateTable(evalTable, output, bGetAll);}
		map<wstring, vector<wstring> > EvaluateTable(wstring evalTable, bool bGetAll = true);

		//IO
		wstring				DumpTree(int format);
		bool				LoadTree(wstring xmlStr);			

		//XPATH
		wstring				EvaluateXPATH(wstring xpath);

		//ascii overloads
		string				GetAttribute(string id, string name, bool immediate = false) {return ROMUTIL::WStrToMBCStr(GetAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), immediate));}
		string				GetAttribute(string id, bool immediate = false) {return GetAttribute(id, "value", immediate);}
		bool				SetAttribute(string id, string name, string value) {return SetAttribute(ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value));}
		bool				SetAttribute(string id, string value) {return SetAttribute(id, "value", value);}
		bool				RemoveAttribute(string id) {return RemoveAttribute(ROMUTIL::MBCStrToWStr(id));}
		bool				SetROMObjectValue(string name, string value) {return SetROMObjectValue(ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value));}
		string				GetROMObjectValue(string name) {return ROMUTIL::WStrToMBCStr(GetROMObjectValue(ROMUTIL::MBCStrToWStr(name)));}
		bool				RemoveROMObjectAttribute(string id) {return RemoveROMObjectAttribute(ROMUTIL::MBCStrToWStr(id));}
		string				GetROMObjectNameA() {return ROMUTIL::ToASCIIString(m_name);}
		void				SetROMObjectName(string name) {m_name = ROMUTIL::MBCStrToWStr(name);}
		bool				LoadRules(string knowledge_file) {return LoadRules(ROMUTIL::MBCStrToWStr(knowledge_file));}
		vector<string>		EvaluateTable(string evalTable, string output, bool bGetAll = true) {LoadInputs(ROMUTIL::MBCStrToWStr(evalTable)); return m_KnowledgeBase.EvaluateTable(evalTable, output, bGetAll);}
		map<string, vector<string> > EvaluateTable(string evalTable, bool bGetAll = true) {LoadInputs(ROMUTIL::MBCStrToWStr(evalTable)); return m_KnowledgeBase.EvaluateTable(evalTable, bGetAll);}
		string				EvaluateXPATH(string xpath) {return ROMUTIL::WStrToMBCStr(EvaluateXPATH(ROMUTIL::MBCStrToWStr(xpath)));}

	private:
		vector<wstring>		LoadInputs(wstring evalTable);
		vector<wstring>		GetPossibleValues(wstring evalTable, wstring outputName);
		wstring				GetATableInputValue(wstring input);
		bool				_anyHasChanged();
		void				_setAllUnchanged();
		wstring				_generateXML(bool bRegen);
		ROMNode*			_buildObject(Node objectNode, ROMNode* parent);
		void				_createXMLDoc();
		wstring				_convertXMLDocToString();
		void				_init();

		wstring m_name;
		string m_guid;
		bool m_bChanged;
		Document m_xmlDoc;
		wstring m_lastContents;
		wstring m_lastAttrContents;		
		vector<ROMNode*> m_children;
		ROMNode* m_parent;
		FASTMAP_MAPS m_attrs;
		FASTMAP m_nodeValues;

		EDS::CKnowledgeBase		m_KnowledgeBase;
	};
}