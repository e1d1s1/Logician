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
#include "stdafx.h"
#include "ROMNode.h"

#define BIGSTRING 16384

using namespace std;
using namespace ROM;

void ROMNode::_init()
{
#ifdef USE_LIBXML
	xmlInitParser();
#endif
#ifdef USE_MSXML
	CoInitialize(NULL);
#endif
	m_guid = MakeGUID();
	m_parent = NULL;
	m_xmlDoc = NULL;
	m_children.reserve(256);
	m_bChanged = true;
}

ROMNode::~ROMNode(void)
{
	DestroyROMObject();	
#ifdef USE_LIBXML
	xmlCleanupParser();
#endif
#ifdef USE_MSXML
	if (m_xmlDoc != NULL)
		m_xmlDoc.Release();
	CoUninitialize();
#endif
}

ROMNode* ROMNode::GetRoot()
{
	ROMNode *nextParent = this;
	do 
	{
		if (nextParent != NULL)
			nextParent = nextParent->Parent();		
	} while (nextParent != NULL);
	return nextParent;
}

bool ROMNode::RemoveChildROMObject(ROMNode *child)
{
	bool retval = false;	
	vector<ROMNode*>::iterator it = find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end())
	{
		m_children.erase(it);
		retval = true;
	}

	m_bChanged = retval;
	return retval;
}

bool ROMNode::DestroyROMObject()
{
	bool retval = false;
	//remove any references to self in parent node
	if (m_parent != NULL)
	{
		retval = m_parent->RemoveChildROMObject(this);
	}

	m_attrs.clear();
	m_nodeValues.clear();
	m_name.clear();
	m_parent = NULL;
	m_bChanged = false;

	//trigger downstream destructors
	for (vector<ROMNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		ROMNode* node = *it;
		node->DestroyROMObject();
		delete node;
	}
	m_children.clear();	

	return retval;
}

wstring ROMNode::GetAttribute(wstring id, wstring name, bool immediate)
{
	wstring retval = L"";
	bool bFound = false;

	FASTMAP_MAPS::iterator it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		FASTMAP::iterator itValue = it->second.find(name);
		if (itValue != it->second.end())
		{
			retval = itValue->second;
			bFound = true;
		}
	}

	if (!immediate && !bFound)
	{
		ROMNode *parent = Parent();
		if (parent != NULL)
		{
			parent->GetAttribute(id, name, immediate);
		}
	}

	return retval;
}

bool ROMNode::SetAttribute(wstring id, wstring name, wstring value)
{
	bool retval = true;

	m_attrs[id][name] = value;
	m_bChanged = retval;

	return retval;
}

bool ROMNode::RemoveAttribute(wstring id)
{
	bool retval = false;
	
	FASTMAP_MAPS::iterator it = m_attrs.find(id);
	if (it != m_attrs.end())
	{		
		m_attrs.erase(it);
		retval = true;
	}
	m_bChanged = retval;

	return retval;
}
	
bool ROMNode::SetROMObjectValue(wstring name, wstring value)
{
	bool retval = true;

	m_nodeValues[name] = value;
	m_bChanged = retval;

	return retval;
}

wstring ROMNode::GetROMObjectValue(wstring name)
{
	wstring retval = L"";

	FASTMAP::iterator it = m_nodeValues.find(name);
	if (it != m_nodeValues.end())
	{		
		retval = it->second;
	}

	return retval;
}

bool ROMNode::RemoveROMObjectAttribute(wstring id)
{
	bool retval = false;

	FASTMAP::iterator it = m_nodeValues.find(id);
	if (it != m_nodeValues.end())
	{		
		m_nodeValues.erase(it);
		retval = true;
	}
	m_bChanged = retval;

	return retval;
}

//rules
vector<wstring> ROMNode::EvaluateTable(wstring evalTable, wstring output, bool bGetAll)
{
	LoadInputs(evalTable);
	return m_KnowledgeBase.EvaluateTable(evalTable, output, bGetAll);
}

map<wstring, vector<wstring> > ROMNode::EvaluateTable(wstring evalTable, bool bGetAll)
{
	LoadInputs(evalTable);
	return m_KnowledgeBase.EvaluateTable(evalTable, bGetAll);
}

vector<wstring> ROMNode::LoadInputs(wstring evalTable)
{
	vector<wstring> inputs = m_KnowledgeBase.GetInputDependencies(evalTable);
	for (vector<wstring>::iterator it = inputs.begin(); it != inputs.end(); it++)
	{
		wstring value = GetATableInputValue(*it);
		m_KnowledgeBase.SetInputValue(*it, value);
	}
	return inputs;
}

vector<wstring> ROMNode::GetPossibleValues(wstring evalTable, wstring outputName)
{
	vector<wstring> outputs = m_KnowledgeBase.GetAllPossibleOutputs(evalTable, outputName);
	return outputs;
}

wstring ROMNode::GetATableInputValue(wstring input)
{
	wstring retval;
	//parse out any XPATH Queries here and return values
	if (StringContains(input, L"xpath("))
	{
		wstring cmdArg(input.begin() + 6, input.end() - 1);
		retval = EvaluateXPATH(cmdArg);
	}
	else
	{
		retval = GetAttribute(input);
	}

	return retval;
}

//IO
void ROMNode::_createXMLDoc()
{
	bool bChanged = _anyHasChanged();
	if (bChanged)
	{
		wstring genXML = _generateXML(bChanged);
#ifdef USE_MSXML
		if (m_xmlDoc != NULL)
			m_xmlDoc.Release();
		m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
		m_xmlDoc->setProperty("SelectionLanguage", "XPath");
		m_xmlDoc->loadXML(genXML.c_str());
#endif
#ifdef USE_LIBXML
		m_xmlDoc = NULL;
		string buff = WStrToMBCStr(genXML);
		m_xmlDoc = xmlParseMemory(buff.c_str(), (int)buff.size());
#endif
		_setAllUnchanged();
	}
}

bool ROMNode::_anyHasChanged()
{
	bool retval = false;

	if (m_bChanged)
		retval = true;
	if (!retval)
	{
		for (vector<ROMNode*>::iterator itNode = m_children.begin(); itNode != m_children.end(); itNode++)
		{
			ROMNode* node = *itNode;
			if (node->m_bChanged)
			{
				retval = true;
				break;
			}
		}
	}

	return retval;
}

wstring ROMNode::_generateXML(bool bRegen)
{
	wstring retval;	
	retval.reserve(BIGSTRING);
	
	if (bRegen)
	{
		//this object
		wstring beginObject = L"<Object";
		beginObject+=L" id=\"";
		beginObject+=m_name;
		beginObject+=L"\" guid=\"";
		beginObject+=ROMUTIL::MBCStrToWStr(m_guid);
		beginObject+=L"\"";
		
		//object values
		wstring objAttrs = L" ";
		for (FASTMAP::iterator itObjValue = m_nodeValues.begin(); itObjValue != m_nodeValues.end(); itObjValue++)
		{
			objAttrs+= itObjValue->first;
			objAttrs+=L"=\"";
			objAttrs+=itObjValue->second;
			objAttrs+=L"\" ";
		}	
		beginObject+=objAttrs;
		beginObject+=L">";
		retval += beginObject;

		if (m_bChanged)
		{
			wstring allAttrs = L"";
			//attributes of this object
			for (FASTMAP_MAPS::iterator it = m_attrs.begin(); it != m_attrs.end(); it++)
			{
				wstring attrObject = L"<Attribute id=\"";
				attrObject+=it->first;
				attrObject+=L"\" ";
				for (FASTMAP::iterator itValue = it->second.begin(); itValue != it->second.end(); itValue++)
				{
					attrObject+=itValue->first;
					attrObject+=L"=\"";
					attrObject+=itValue->second;
					attrObject+=L"\" ";
				}
				attrObject+=L"/>";				
				allAttrs += attrObject;
			}
			retval += allAttrs;
			m_lastAttrContents = allAttrs;
		}
		else
		{
			retval += m_lastAttrContents;
		}

		//child objects
		for (vector<ROMNode*>::iterator itNode = m_children.begin(); itNode != m_children.end(); itNode++)
		{
			ROMNode* node = *itNode;
			retval += node->_generateXML(node->m_bChanged);
		}

		retval+=L"</Object>";
		m_lastContents = retval;
	}
	else
	{
		retval = m_lastContents;
	}
	
	return retval;
}

wstring ROMNode::DumpTree(int format)
{
	_createXMLDoc();	
	return _convertXMLDocToString();	
}

bool ROMNode::LoadTree(wstring xmlStr)
{
	bool retval = false;

	Document xmlDoc;
#ifdef USE_MSXML
	xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	xmlDoc->async = VARIANT_FALSE;
	xmlDoc->resolveExternals = VARIANT_FALSE;
	xmlDoc->setProperty("SelectionLanguage", "XPath");
	xmlDoc->setProperty("SelectionNamespaces", "");
	xmlDoc->setProperty("NewParser", VARIANT_TRUE);
	
	try
	{
		VARIANT_BOOL ok = xmlDoc->loadXML(xmlStr.c_str());
		if (ok)
		{			
			Node objectNode = xmlDoc->selectSingleNode("Object");
			_buildObject(objectNode, NULL);			
		}
	}
	catch(const _com_error& e)
	{
		ReportROMError(ToASCIIString((wstring)(e.Description())));
	}
#endif



#ifdef USE_LIBXML
	xmlDoc = NULL;
	string buff = WStrToMBCStr(xmlStr);
	xmlDoc = xmlParseMemory(buff.c_str(), (int)buff.size());

	//create object

	//set object values

	//set object attributes

#endif

	return retval;
}

ROMNode* ROMNode::_buildObject(Node objectNode, ROMNode* parent)
{
	//create object
	ROMNode* newNode = NULL;
	wstring id = objectNode->attributes->getNamedItem("id")->nodeValue.bstrVal;
	if (parent == NULL)
	{
		DestroyROMObject();
		m_name = id;
		newNode = this;
	}
	else
	{
		newNode = new ROMNode(id);
	}

	//set object values
	for (long i = 0; i < objectNode->attributes->Getlength(); i++)
	{
		Node objAttr = objectNode->attributes->Getitem(i);
		if (objAttr->nodeName.GetBSTR() != L"id")
			newNode->SetROMObjectValue(objAttr->nodeName.GetBSTR(), objAttr->nodeValue.bstrVal);		
	}

	NodeList attrNodes = objectNode->selectNodes("Attribute");
	//set object attributes
	for (long attrCnt = 0; attrCnt < attrNodes->Getlength(); attrCnt++)
	{
		Node attrNode = attrNodes->item[attrCnt];
		wstring idAttr = attrNode->attributes->getNamedItem("id")->nodeValue.bstrVal;
		for (long i = 0; i < objectNode->attributes->Getlength(); i++)
		{
			Node attr = objectNode->attributes->Getitem(i);
			if (attr->nodeName.GetBSTR() != L"id")
				newNode->SetAttribute(idAttr, attr->nodeName.GetBSTR(), attr->nodeValue.bstrVal);		
		}
	}

	//children recursivley
	NodeList childNodes = objectNode->selectNodes("Object");
	for (long childCnt = 0; childCnt < objectNode->attributes->Getlength(); childCnt++)
	{
		Node childNode = childNodes->item[childCnt];
		ROMNode *newChildObject = _buildObject(childNode, parent);
		if (newChildObject != NULL && newNode != NULL)
		{
			newNode->AddChildROMObject(newChildObject);
		}
	}

	return newNode;
}

void ROMNode::_setAllUnchanged()
{
	m_bChanged = false;
	for (vector<ROMNode*>::iterator itNode = m_children.begin(); itNode != m_children.end(); itNode++)
	{
		ROMNode* node = *itNode;
		node->m_bChanged = false;
	}
}

wstring	ROMNode::EvaluateXPATH(wstring xpath)
{
	wstring retval;	
	wstring match = L"<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'";
	match += ROMUTIL::MBCStrToWStr(m_guid) + L"\']\"><xsl:value-of select=\"";
	wstring xslt_text = XSLT_TOP + match + xpath + XSLT_BOTTOM;

	Document xsltDoc = NULL;
	_createXMLDoc();
	if (m_xmlDoc != NULL)
	{
#ifdef USE_MSXML
		xsltDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
		xsltDoc->loadXML(xslt_text.c_str());
		retval = ToWString(m_xmlDoc->transformNode(xsltDoc));
		xsltDoc.Release();
#endif
#ifdef USE_LIBXML
		string buff = WStrToMBCStr(xslt_text);
		xsltDoc = xmlParseMemory(buff.c_str(), (int)buff.length());

		xsltStylesheetPtr xsl = xsltParseStylesheetDoc(xsltDoc);
		Document result = xsltApplyStylesheet(xsl, m_xmlDoc, NULL);

		xmlChar *xmlbuff;
		int buffersize = 0;
		xsltSaveResultToString(&xmlbuff, &buffersize, result, xsl);
		retval = MBCStrToWStr(xmlbuff);

		xmlFreeDoc(xsltDoc);
		xmlFreeDoc(result);
#endif	
	}
	return retval;
}

wstring ROMNode::_convertXMLDocToString()
{
	wstring retval;
	retval.reserve(BIGSTRING);

	if (m_xmlDoc != NULL)
	{
#ifdef USE_MSXML
		retval = ToWString(m_xmlDoc->Getxml().GetBSTR());
#endif
#ifdef USE_LIBXML
		xmlChar *xmlbuff;
		int buffersize = 0;
		xmlDocDumpFormatMemory(xmlDoc, &xmlbuff, &buffersize, format);
		retval = MBCStrToWStr(xmlbuff);
#endif
	}
	return retval;
}