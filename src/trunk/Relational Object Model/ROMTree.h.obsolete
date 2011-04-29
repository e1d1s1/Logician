
//no longer used.  Allowing a XML parser to maintain the state is a bit too slow
//kept for reference purposes

/*
This file is part of the Relational Object Model 2 Library.
Copyright (C) 2009 Eric D. Schmidt

    Relational Object Model 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Relational Object Model 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Relational Object Model 2.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "KnowledgeBase.h"

#ifdef USE_MSXML
#include <comdef.h>
#endif

using namespace std;
using namespace EDS;

//Main ROM class
namespace ROM
{
	class ROMTree
	{
	friend class ROMDictionary;
	friend class LinearEngine;
	public:
		ROMTree(wstring name);
		virtual ~ROMTree(void);		

		//relational interface
		Node GetRoot();
		Node Parent(Node current);
		vector<Node> Find(Node current, wstring searchStr);
		Node AddChildROMObject(Node current, Node child);
		Node CreateROMObject(wstring name);
		Node GetROMObject(string guid);
		bool DestroyROMObject(Node current);
		vector<Node> GetAllChildren(Node current) {return Find(current, L"Object");}

		//attribute interface
		wstring				GetAttribute(Node currentObject, wstring id, wstring name, bool immediate = false);
		wstring				GetAttribute(Node currentObject, wstring id, bool immediate = false) {return GetAttribute(currentObject, id, L"value", immediate);}
		bool				SetAttribute(Node currentObject, wstring id, wstring name, wstring value);
		bool				SetAttribute(Node currentObject, wstring id, wstring value);
		bool				SetAttributeValue(Node currentObject, wstring id, wstring value) {SetAttribute(currentObject, id, value);}
		bool				SetROMObjectValue(Node currentObject, wstring name, wstring value);
		wstring				GetROMObjectValue(Node currentObject, wstring name);
		bool				RemoveAttribute(Node current, wstring id);	
		wstring				GetROMObjectName(Node current);
		void				SetROMObjectName(Node current, wstring name);
		map<wstring, map<wstring, wstring> > GetAllAttributes(Node current);

		//rules
		bool				LoadRules(wstring knowledge_file);
		vector<wstring>		EvaluateTable(Node currentObject, wstring evalTable, wstring output, bool bGetAll = true);
		vector<wstring>		EvaluateTableForAttr(Node currentObject, wstring evalTable, wstring output, bool bGetAll = true) {return EvaluateTable(currentObject, evalTable, output, bGetAll);}
		map<wstring, vector<wstring> > EvaluateTable(Node currentObject, wstring evalTable, bool bGetAll = true);

		//IO
		wstring				DumpTree(int format);
		bool				LoadTree(wstring xmlStr);

		//XPATH
		wstring				EvaluateXPATH(Node currentObject, wstring xpath);

		//ASCII Overloads
		ROMTree(string name);
		vector<Node>		Find(Node current, string searchStr);
		Node				CreateROMObject(string name);		
		string				GetAttribute(Node currentObject, string id, string name, bool immediate = false);
		string				GetAttribute(Node currentObject, string id, bool immediate = false) {return GetAttribute(currentObject, id, "value", immediate);}
		bool				SetAttribute(Node currentObject, string id, string name, string value);
		bool				SetAttribute(Node currentObject, string id, string value);
		bool				SetROMObjectValue(Node currentObject, string name, string value);
		string				GetROMObjectValue(Node currentObject, string name);
		bool				RemoveAttribute(Node current, string id);	
		string				GetROMObjectNameA(Node current);
		void				SetROMObjectName(Node currrent, string name);
		bool				LoadRules(string knowledge_file);
		vector<string>		EvaluateTable(Node currentObject, string evalTable, string output, bool bGetAll = true);
		map<string, vector<string> > EvaluateTable(Node currentObject, string evalTable, bool bGetAll = true);
		string				DumpTreeA(int format);
		bool				LoadTree(string xmlStr);
		string				EvaluateXPATH(Node currentObject, string xpath);

#ifdef USE_LIBXML
		wstring MBCStrToWStr(xmlChar* mbStr)
		{
			if (mbStr == NULL)
				return L"";

			size_t requiredSize = mbstowcs(NULL, (const char*)mbStr, 0) + 1;
			wchar_t *wStr = new wchar_t[requiredSize];
			mbstowcs(wStr, (const char*)mbStr, requiredSize);
			wstring retval = wStr;
			delete [] wStr;
			xmlFree(mbStr);
			return retval;
		}

		wstring MBCStrToWStr(const xmlChar* mbStr)
		{
			if (mbStr == NULL)
				return L"";

			size_t requiredSize = mbstowcs(NULL, (const char*)mbStr, 0) + 1;
			wchar_t *wStr = new wchar_t[requiredSize];
			mbstowcs(wStr, (const char*)mbStr, requiredSize);
			wstring retval = wStr;
			delete [] wStr;
			return retval;
		}
#endif

	private:
		void				Initialize(wstring name);
		vector<wstring>		LoadInputs(Node currentObject, wstring evalTable);
		vector<wstring>		GetPossibleValues(Node currentObject, wstring evalTable, wstring outputName);
		wstring				GetATableInputValue(Node currentObject, wstring input);
		Document			xmlDoc;
#ifdef USE_MSXML
		Element				m_tree;
#endif
#ifdef USE_LIBXML			
		Node				m_tree;	
#endif
		EDS::CKnowledgeBase		m_KnowledgeBase;	
	};	
}