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
using namespace System::Collections::Generic;

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
		ROMTreeNET(String^ name);
		~ROMTreeNET() {this->!ROMTreeNET();}		
		!ROMTreeNET() {if (m_KnowledgeBase) delete m_KnowledgeBase;}
		ROMNode^	GetRoot();
		ROMNode^	Parent(ROMNode^ current);
		array<ROMNode^>^ Find(ROMNode^ current, String^ searchStr);
		ROMNode^	AddChildROMObject(ROMNode^ current, ROMNode^ child);
		ROMNode^	CreateROMObject(String^ name);
		ROMNode^	GetROMObject(Guid^ guid);
		bool		DestoryROMObject(ROMNode^ current);

		//attribute interface
		String^		GetAttribute(ROMNode^ currentObject, String^ id, String^ name, bool recurs);
		String^		GetAttribute(ROMNode^ currentObject, String^ id, bool recurs);
		String^		GetAttribute(ROMNode^ currentObject, String^ id);
		bool		SetAttribute(ROMNode^ currentObject, String^ id, String^ name, String^ value);
		bool		SetAttribute(ROMNode^ currentObject, String^ id, String^ value);
		bool		SetROMObjectValue(ROMNode^ currentObject, String^ name, String^ value);
		String^		GetROMObjectValue(ROMNode ^currentObject, String ^name);
		bool		RemoveAttribute(ROMNode^ currentObject, String^ id);	
		String^		GetROMObjectName(ROMNode^ currentObject);
		void		SetROMObjectName(ROMNode^ currentObject, String^ name);

		//rules
		bool		LoadRules(String^ knowledge_file);
		array<String^>^	EvaluateTable(ROMNode^ currentObject, String^ evalTable, String^ output, bool bGetAll);
		System::Collections::Generic::Dictionary<String^, array<String^>^>^ EvaluateTable(ROMNode^ currentObject, String^ evalTable, bool bGetAll);

		//IO
		String^		DumpTree();
		bool		LoadTree(String^ xmlStr);

		//Xpath
		String^		EvaluateXPATH(ROMNode ^currentObject, String^ xpath);		

	internal:
		array<String^>^	GetPossibleValues(ROMNode^ currentObject, String^ evalTable, String^ outputName);
		void		LoadInputs(ROMNode^ currentObject, String^ evalTable);
		String^ 	GetATableInputValue(ROMNode^ currentObject, String^ input);
		System::Xml::XmlElement^				m_DOM;
		System::Xml::XmlDataDocument^			m_DOC;
		EDS::CKnowledgeBase						*m_KnowledgeBase;
	};
}

#define ATTRIBUTE_NODE L"Attribute"
#define OBJECT_NODE L"Object"
#define XSLT_TOP L"<?xml version=\"1.0\"?><xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"><xsl:output method=\"text\" encoding=\"UTF-8\"/>"
#define XSLT_BOTTOM L"\"/></xsl:for-each></xsl:template></xsl:stylesheet>"


