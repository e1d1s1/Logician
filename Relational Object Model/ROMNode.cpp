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

#ifdef USE_LIBXML
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xmlmemory.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/extensions.h>
#endif

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
	m_KnowledgeBase = NULL;
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
			nextParent = nextParent->GetParent();
	} while (nextParent != NULL);

	if (!nextParent)
		nextParent = this;
	return nextParent;
}

bool ROMNode::AddChildROMObject(ROMNode *child)
{
	if (child->m_parent == NULL)
	{
		m_children.push_back(child);
		child->m_parent = this;
		return true;
	}
	else
		return false;
}

bool ROMNode::RemoveChildROMObject(ROMNode *child)
{
	bool retval = false;
	vector<ROMNode*>::iterator it = find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end())
	{
		m_children.erase(it);
		retval = true;
		m_bChanged = retval;
	}
	
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
	m_id.clear();
	m_parent = NULL;
	m_bChanged = false;
	//trigger downstream destructors
	for (long i = m_children.size() - 1; i >= 0; i--)
	{
		if (i < m_children.size())
		{
			ROMNode* node = m_children[i];
			if (node)
				delete node;
		}
	}
	m_children.clear();

	return retval;
}

ROMNode* ROMNode::Clone()
{
	ROMNode* newNode = new ROMNode(m_id);
	newNode->m_attrs = m_attrs;
	newNode->m_nodeValues = m_nodeValues;
	newNode->m_bChanged = false;
	for (long i = m_children.size() - 1; i >= 0; i--)
	{
		if (i < m_children.size())
		{
			ROMNode* node = m_children[i];
			if (node)
			{
				ROMNode* newChild = node->Clone();
				newNode->AddChildROMObject(newChild);
			}
		}
	}
	return newNode;
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
		ROMNode *parent = GetParent();
		if (parent != NULL)
		{
			retval = parent->GetAttribute(id, name, immediate);
		}
	}

	return retval;
}

bool ROMNode::GetAttributeExists(wstring id, wstring name)
{
	bool bFound = false;
	FASTMAP_MAPS::iterator it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		FASTMAP::iterator itValue = it->second.find(name);
		if (itValue != it->second.end())
		{
			bFound = true;
		}
	}
	return bFound;
}

bool ROMNode::SetAttribute(wstring id, wstring name, wstring value)
{
	bool retval = true;

	m_attrs[id][name] = value;
	m_bChanged = retval;

	return retval;
}

bool ROMNode::RemoveAttribute(wstring id, wstring name)
{
	bool retval = false;

	FASTMAP_MAPS::iterator it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		if (name.length() == 0 || name == L"value")
		{
			m_attrs.erase(it);
			retval = true;
		}
		else
		{
			FASTMAP::iterator itValue = it->second.find(name);
			if (itValue != it->second.end())
			{
				it->second.erase(itValue);
				retval = true;
			}
		}
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

bool ROMNode::RemoveROMObjectValue(wstring id)
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
bool ROMNode::LoadRules(wstring knowledge_file)
{
	if (m_KnowledgeBase)
		delete m_KnowledgeBase;

	if (m_parent == NULL) //only the root will have the reference to the rules
	{
		m_KnowledgeBase = new EDS::CKnowledgeBase();
		return m_KnowledgeBase->CreateKnowledgeBase(knowledge_file);
	}
	else
		return false;
}

vector<wstring> ROMNode::EvaluateTable(wstring evalTable, wstring output, bool bGetAll)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTable(evalTable, output, bGetAll);
	}
	return retval;
}

vector<wstring> ROMNode::EvaluateTable(wstring evalTable, wstring output)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTable(evalTable, output);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTable(wstring evalTable, bool bGetAll)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTable(evalTable, bGetAll);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTable(wstring evalTable)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTable(evalTable);
	}
	return retval;
}

vector<wstring> ROMNode::ReverseEvaluateTable(wstring evalTable, wstring inputAttr, bool bGetAll)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadOutputs(evalTable);
		retval = knowledge->ReverseEvaluateTable(evalTable, inputAttr, bGetAll);
	}
	return retval;
}

vector<wstring> ROMNode::ReverseEvaluateTable(wstring evalTable, wstring inputAttr)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadOutputs(evalTable);
		retval = knowledge->ReverseEvaluateTable(evalTable, inputAttr);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::ReverseEvaluateTable(wstring evalTable, bool bGetAll)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadOutputs(evalTable);
		retval = knowledge->ReverseEvaluateTable(evalTable, bGetAll);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::ReverseEvaluateTable(wstring evalTable)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadOutputs(evalTable);
		retval = knowledge->ReverseEvaluateTable(evalTable);
	}
	return retval;
}

vector<wstring> ROMNode::LoadInputs(wstring evalTable)
{
	vector<wstring> inputs;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		inputs = knowledge->GetInputDependencies(evalTable);
		for (vector<wstring>::iterator it = inputs.begin(); it != inputs.end(); it++)
		{
			wstring value = GetATableInputValue(*it);
			knowledge->SetInputValue(*it, value);
		}
	}
	return inputs;
}

vector<wstring> ROMNode::LoadOutputs(wstring evalTable)
{
	vector<wstring> outputs;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		outputs = knowledge->GetOutputAttrs(evalTable);
		for (vector<wstring>::iterator it = outputs.begin(); it != outputs.end(); it++)
		{
			wstring value = GetATableInputValue(*it);
			knowledge->SetInputValue(*it, value);
		}
	}
	return outputs;
}

vector<wstring> ROMNode::GetPossibleValues(wstring evalTable, wstring outputName)
{
	vector<wstring> outputs;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		outputs = knowledge->GetAllPossibleOutputs(evalTable, outputName);
	}
	return outputs;
}

wstring ROMNode::GetATableInputValue(wstring input)
{
	wstring retval;
	//parse out any XPATH Queries here and return values
	if (StringContains(input, L"xpath("))
	{
		wstring cmdArg(input.begin() + 6, input.end() - 1);
		//for sibling/child xpath axes to work, eval from root and identify current context by the Object's guid
		retval = this->GetRoot()->EvaluateXPATH(cmdArg, this->m_guid);
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
		beginObject+=m_id;
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

wstring ROMNode::DumpTree(bool indented)
{
	_createXMLDoc();
	return _convertXMLDocToString(indented);
}

bool ROMNode::LoadTree(wstring xmlStr)
{
	bool retval = false;

#ifdef USE_MSXML
	if (m_xmlDoc != NULL)
		m_xmlDoc.Release();

	m_xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	m_xmlDoc->async = VARIANT_FALSE;
	m_xmlDoc->resolveExternals = VARIANT_FALSE;
	m_xmlDoc->setProperty("SelectionLanguage", "XPath");
	m_xmlDoc->setProperty("SelectionNamespaces", "");
	m_xmlDoc->setProperty("NewParser", VARIANT_TRUE);

	try
	{
		VARIANT_BOOL ok = m_xmlDoc->loadXML(xmlStr.c_str());
		if (ok)
		{
			Node objectNode = m_xmlDoc->selectSingleNode("Object");
			_buildObject(objectNode, NULL);
		}
	}
	catch(const _com_error& e)
	{
		ReportROMError(ToASCIIString((wstring)(e.Description())));
	}
#endif



#ifdef USE_LIBXML
	try
	{
		m_xmlDoc = NULL;
		string buff = WStrToMBCStr(xmlStr);
		m_xmlDoc = xmlParseMemory(buff.c_str(), (int)buff.size());

		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(m_xmlDoc);
		xmlChar* objXPath = (xmlChar*)"Object";
		xmlXPathObjectPtr xpathObjs = xmlXPathEvalExpression(objXPath, xpathCtx);
		NodeList allObjs = xpathObjs->nodesetval;
		if (allObjs != NULL)
		{
			for (int i = 0; i < allObjs->nodeNr; i++)
			{
				Node objectNode = allObjs->nodeTab[i];
				_buildObject(objectNode, NULL);
			}
		}
	}
	catch(...)
	{
		ReportROMError("Problem parsing XML");
	}


#endif

	return retval;
}

ROMNode* ROMNode::_buildObject(Node objectNode, ROMNode* parent)
{
	//create object
	ROMNode* newNode = NULL;

#ifdef USE_MSXML
	wstring id = objectNode->attributes->getNamedItem("id")->nodeValue.bstrVal;
	if (parent == NULL)
	{
		DestroyROMObject();
		m_id = id;
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

	//set object attributes
	NodeList attrNodes = objectNode->selectNodes("Attribute");
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
	for (long childCnt = 0; childCnt < childNodes->Getlength(); childCnt++)
	{
		Node childNode = childNodes->item[childCnt];
		ROMNode *newChildObject = _buildObject(childNode, parent);
		if (newChildObject != NULL && newNode != NULL)
		{
			newNode->AddChildROMObject(newChildObject);
		}
	}
#endif

#ifdef USE_LIBXML
	wstring id = MBCStrToWStr(xmlGetProp(objectNode, (xmlChar*)"id"));
	if (parent == NULL)
	{
		DestroyROMObject();
		m_id = id;
		newNode = this;
	}
	else
	{
		newNode = new ROMNode(id);
	}

	//set object values
	for (Attribute objValue = objectNode->properties; objValue != NULL; objValue = objValue->next)
	{
		wstring id = MBCStrToWStr(objValue->name);
		if (id != L"id")
		{
			wstring value = MBCStrToWStr(xmlGetProp(objectNode, (xmlChar*)WStrToMBCStr(id).c_str()));
			newNode->SetROMObjectValue(id, value);
		}
	}

	//set object attributes
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(m_xmlDoc);
	xmlChar* attrXPath = (xmlChar*)"Attribute";
	xmlXPathObjectPtr xpathAttrs = xmlXPathEvalExpression(attrXPath, xpathCtx);
	NodeList allAttrs = xpathAttrs->nodesetval;
	if (allAttrs != NULL)
	{
		for (int i = 0; i < allAttrs->nodeNr; i++)
		{
			Node attrNode = allAttrs->nodeTab[i];
			wstring idAttr = MBCStrToWStr(xmlGetProp(attrNode, (xmlChar*)"id"));
			for (Attribute attr = attrNode->properties; attr != NULL; attr = attr->next)
			{
				wstring name = MBCStrToWStr(attr->name);
				if (name != L"id")
				{
					wstring value = MBCStrToWStr(xmlGetProp(attrNode, (xmlChar*)WStrToMBCStr(name).c_str()));
					newNode->SetAttribute(idAttr, name, value);
				}
			}
		}
	}

	//children recursivley
	xmlChar* objXPath = (xmlChar*)"Object";
	xmlXPathObjectPtr xpathObjects = xmlXPathEvalExpression(objXPath, xpathCtx);
	NodeList allObjs = xpathObjects->nodesetval;
	if (allObjs != NULL)
	{
		for (int i = 0; i < allObjs->nodeNr; i++)
		{
			Node childNode = allObjs->nodeTab[i];
			ROMNode *newChildObject = _buildObject(childNode, parent);
			if (newChildObject != NULL && newNode != NULL)
			{
				newNode->AddChildROMObject(newChildObject);
			}
		}
	}

#endif
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

wstring	ROMNode::EvaluateXPATH(wstring xpath, string guid)
{
	wstring retval;
	wstring match = L"<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'";
	match += ROMUTIL::MBCStrToWStr(guid) + L"\']\"><xsl:value-of select=\"";
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

wstring ROMNode::_convertXMLDocToString(bool indented)
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
		int format = 0;
		if (indented)
			format = 1;
		xmlDocDumpFormatMemory(m_xmlDoc, &xmlbuff, &buffersize, format);
		retval = MBCStrToWStr(xmlbuff);
#endif
	}
	return retval;
}

EDS::CKnowledgeBase* ROMNode::_getKnowledge()
{
	EDS::CKnowledgeBase *knowledge = NULL;
	ROMNode* current = this;
	if (current->m_KnowledgeBase != NULL)
		knowledge = m_KnowledgeBase;
	else
	{		
		while (knowledge == NULL)
		{
			ROMNode* parent = current->GetParent();
			if (parent)
			{
				if (parent->m_KnowledgeBase)
				{
					knowledge = parent->m_KnowledgeBase;
					break;
				}
				else
					current = parent;
			}
			else
				return NULL;		
		}
	}
	return knowledge;
}