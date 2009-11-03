/*
This file is part of ROM2NET.
Copyright (C) 2009 Eric D. Schmidt

    ROM2NET is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROM2NET is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with ROM2NET.  If not, see <http://www.gnu.org/licenses/>.
*/
// ROM2NET.h
#pragma once
#pragma unmanaged
#include "KnowledgeBase.h"
#include <vector>
#include <map>
using namespace std;

#pragma managed
//managed code
#using <mscorlib.dll>
#using <System.dll>
#include <vcclr.h>
using namespace System; 

namespace ROM2NET 
{
	public ref class ROMNode
	{
	public:
		ROMNode(System::Xml::XmlNode^ node) {m_Node = node;}
		~ROMNode(){this->!ROMNode();}
		!ROMNode() {}
		void SetNode(System::Xml::XmlNode^ node);
		System::Xml::XmlNode^ GetNode();
	private:		
		System::Xml::XmlNode^ m_Node;
	};

	public ref class ROMNodeList
	{
	public:
		ROMNodeList(System::Xml::XmlNodeList^ list){m_List = list;}
		long Count();
		~ROMNodeList(){this->!ROMNodeList();}
		!ROMNodeList() {}
		ROMNode^ GetItem(System::Int64 Idx);
		void SetNodeList(System::Xml::XmlNodeList^ list);
		System::Xml::XmlNodeList^ GetNodeList();
	private:		
		System::Xml::XmlNodeList^ m_List;
	};

	public ref class ROMTreeNET
	{
	public:
		ROMTreeNET(System::String^ name);
		~ROMTreeNET() {this->!ROMTreeNET();}		
		!ROMTreeNET() {if (m_KnowledgeBase) delete m_KnowledgeBase;}
		ROMNode^ GetRoot();
		ROMNode^ Parent(ROMNode^ current);
		System::Collections::Generic::List<ROMNode^>^ Find(ROMNode^ current, System::String^ searchStr);
		ROMNode^ AddChildROMObject(ROMNode^ current, ROMNode^ child);
		ROMNode^ CreateROMObject(System::String^ name);
		bool DestoryROMObject(ROMNode^ current);

		//attribute interface
		System::String^		GetAttribute(ROMNode^ currentObject, System::String^ id, System::String^ name, System::Boolean^ recurs);
		System::String^		GetAttribute(ROMNode^ currentObject, System::String^ id);
		bool				SetAttribute(ROMNode^ currentObject, System::String^ id, System::String^ name, System::String^ value);
		bool				SetAttribute(ROMNode^ currentObject, System::String^ id, System::String^ value);
		bool				SetROMObjectValue(ROMNode^ currentObject, System::String^ name, System::String^ value);
		System::String^		GetROMObjectValue(ROMNode ^currentObject, System::String ^name);
		bool				RemoveAttribute(ROMNode^ currentObject, System::String^ id);	
		System::String^		GetROMObjectName(ROMNode^ currentObject);
		void				SetROMObjectName(ROMNode^ currentObject, System::String^ name);

		//rules
		bool				LoadRules(System::String^ knowledge_file);
		System::Collections::Generic::List<System::String^>^	EvaluateTable(ROMNode^ currentObject, System::String^ evalTable, System::String^ output, bool bGetAll);
		System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<System::String^>^>^ EvaluateTable(ROMNode^ currentObject, System::String^ evalTable, bool bGetAll);

		//IO
		System::String^		DumpTree();
		bool	LoadTree(System::String^ xmlStr);

		//Xpath
		System::String^		EvaluateXPATH(ROMNode ^currentObject, System::String^ xpath);

	private:
		void				LoadInputs(ROMNode^ currentObject, System::String^ evalTable);
		System::String^ 	GetATableInputValue(ROMNode^ currentObject, System::String^ input);
		System::Xml::XmlElement^				m_DOM;
		System::Xml::XmlDataDocument^			m_DOC;
		EDS::CKnowledgeBase						*m_KnowledgeBase;
	};
}

#define ATTRIBUTE_NODE L"Attribute"
#define OBJECT_NODE L"Object"
#define XSLT_TOP L"<?xml version=\"1.0\"?><xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"><xsl:output method=\"text\" encoding=\"UTF-8\"/>"
#define XSLT_BOTTOM L"\"/></xsl:for-each></xsl:template></xsl:stylesheet>"


