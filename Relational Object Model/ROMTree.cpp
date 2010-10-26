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
#include "utilities.h"
#include "ROMTree.h"
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

using namespace std;
using namespace ROM;
using namespace ROMUTIL;

ROMTree::ROMTree(wstring name)
{
	Initialize(name);
}

void ROMTree::Initialize(wstring name)
{
	string guid = MakeGUID();
	name = FindAndReplace(name, L" ", L"_");
	#ifdef USE_MSXML
		CoInitialize(NULL);

		xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
		xmlDoc->setProperty("SelectionLanguage", "XPath");

		::IXMLDOMProcessingInstructionPtr pPI = xmlDoc->createProcessingInstruction
			(X("xml"), X("version='1.0' encoding='UTF-8'"));
		if (name.length() == 0) {name = L"default";}
		_variant_t vNullVal;
		vNullVal.vt = VT_NULL;
		xmlDoc->insertBefore((Node)pPI, vNullVal);

		Node pRootNode = NULL;
		_variant_t varNodeType((short)MSXML2::NODE_ELEMENT);
		pRootNode = xmlDoc->createNode(varNodeType, OBJECT_NODE, "");

		xmlDoc->appendChild(pRootNode);
		m_tree = xmlDoc->documentElement;
		m_tree->setAttribute("id", name.c_str());
		m_tree->setAttribute(X("guid"), guid.c_str());
	#endif

	#ifdef USE_LIBXML
		xmlInitParser();
		xmlDoc = xmlNewDoc((xmlChar*)"1.0");
		Node rootNode = xmlNewNode(NULL, (xmlChar*)WStrToMBCStr(OBJECT_NODE).c_str());

		xmlDocSetRootElement(xmlDoc, rootNode);
		m_tree = rootNode;
		xmlNewProp(rootNode, (xmlChar *)"id", (xmlChar*)WStrToMBCStr(name).c_str());
		xmlNewProp(rootNode, (xmlChar *)"guid", (xmlChar*)guid.c_str());
	#endif
}

ROMTree::~ROMTree(void)
{
	#ifdef USE_LIBXML
		xmlFreeDoc(xmlDoc);
		xmlCleanupParser();
	#endif
}

Node ROMTree::GetRoot()
{
	return (Node)m_tree;
}

Node ROMTree::Parent(Node current)
{
	if (!current)
		return NULL;
	#ifdef USE_LIBXML
			return current->parent;
	#endif

	#ifdef USE_MSXML
			return (Node)current->parentNode;
	#endif
}

vector<Node> ROMTree::Find(Node current, wstring searchStr)
{
	vector<Node>		retval;
	wstring				class_to_find = L"", catnum_to_find =L"";

	if (!current)
		return retval;
	try
	{
		#ifdef USE_MSXML
			NodeList res = current->selectNodes(searchStr.c_str());
			if (res != NULL)
			{
				for (long i = 0; i < res->length; i++)
				{
					retval.push_back(res->item[i]);
				}
			}
		#endif
		#ifdef USE_LIBXML
			xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
			xpathCtx->node = current;
			xmlXPathObjectPtr xpathObjSearch = xmlXPathEvalExpression((xmlChar*)WStrToMBCStr(searchStr).c_str(), xpathCtx);
			NodeList res = xpathObjSearch->nodesetval;
			if (res != NULL)
			{
				for (int i = 0; i < res->nodeNr; i++)
				{
					retval.push_back(res->nodeTab[i]);
				}
			}
			xmlXPathFreeObject(xpathObjSearch);
			xmlXPathFreeContext(xpathCtx);
		#endif
	}
	catch(...)
	{
		//need an error handler
	}

	return retval;
}

Node ROMTree::CreateROMObject(wstring name)
{
	Node retval = NULL;
	string guid = MakeGUID();

	if (!xmlDoc)
		return retval;

	#ifdef USE_MSXML
		Element elem = xmlDoc->createElement(OBJECT_NODE);

		elem->setAttribute(X("id"), name.c_str());

		//guid for each ObjectNode
		elem->setAttribute(X("guid"), guid.c_str());
		retval = (Node)elem;
	#endif

	#ifdef USE_LIBXML
		retval = xmlNewNode(NULL, (xmlChar*)WStrToMBCStr(OBJECT_NODE).c_str());
		xmlNewProp(retval, (xmlChar*)"id", (xmlChar*)WStrToMBCStr(name).c_str());
		xmlNewProp(retval, (xmlChar*)"guid", (xmlChar*)guid.c_str());
	#endif

	return retval;
}

Node ROMTree::GetROMObject(string guid)
{
	string src = "//Object[@guid='";
	src += guid.c_str();
	src += "']";
	Node retval = NULL;
#ifdef USE_MSXML
	retval = xmlDoc->selectSingleNode(src.c_str());
#endif

#ifdef USE_LIBXML
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
	xmlXPathObjectPtr xpathObjSearch = xmlXPathEvalExpression((xmlChar*)src.c_str(), xpathCtx);
	NodeList res = xpathObjSearch->nodesetval;
	if (res != NULL && res->nodeNr == 1)
	{
		retval = res->nodeTab[0];
	}
	xmlXPathFreeObject(xpathObjSearch);
	xmlXPathFreeContext(xpathCtx);
#endif
	return retval;
}

Node ROMTree::AddChildROMObject(Node current, Node child)
{
	Node retval = NULL;
	if (!current || !child)
			return retval;

	#ifdef USE_MSXML
		retval = current->appendChild(child);;
	#endif

	#ifdef USE_LIBXML
		retval = xmlAddChild(current, child);
	#endif
	return retval;
}

wstring ROMTree::GetROMObjectName(Node current)
{
	wstring retval = L"";
	if (!current)
		return retval;

	#ifdef USE_MSXML
		NamedNodeMap attrs = current->attributes;
		Node data = (Node)attrs->getNamedItem(X("id"));

		if (data)
			retval = ToWString(data->nodeValue);
	#endif

	#ifdef USE_LIBXML
		retval = MBCStrToWStr(xmlGetProp(current, (xmlChar*)"id"));
	#endif

	return retval;
}

void ROMTree::SetROMObjectName(Node current, wstring name)
{
	#ifdef USE_MSXML
		NamedNodeMap attrs = current->attributes;
		Node data = (Node)attrs->getNamedItem(X("id"));

		if (data)
			data->nodeValue = name.c_str();
	#endif

	#ifdef USE_LIBXML
		xmlSetProp(current, (xmlChar*)"id", (xmlChar*)name.c_str());
	#endif
}

//attribute interface
//<current><Attribute id = "some_id" value = "val" some_other_name = "something"/></current>
wstring	ROMTree::GetAttribute(Node currentObject, wstring id, wstring name, bool recurs)
{
	wstring retval = L"";
	if (!currentObject)
		return retval;

	#ifdef USE_MSXML
		if (currentObject->hasChildNodes())
		{
			NodeList nodes = (NodeList)currentObject->childNodes;
			for (WORD i = 0; i < nodes->length; i++)
			{
				Node attrNode = nodes->item[i];
				if (attrNode->nodeName == X(ATTRIBUTE_NODE))
				{
					NamedNodeMap attrs = attrNode->attributes;
					Node data = (Node)attrs->getNamedItem(X("id"));

					if (data && ToWString(data->nodeValue) == id)
					{ //got it
						if (name.length() > 0) //getting a sub attr
						{
							Node attr_data = (Node)attrs->getNamedItem(name.c_str());
							if (attr_data)
							{
								retval = ToWString(attr_data->nodeValue);
								break;
							}
						}
						else //getting the main value attr
						{
							Node attr_data = (Node)attrs->getNamedItem("value");
							if (attr_data)
							{
								retval = ToWString(attr_data->nodeValue);
								break;
							}
						}
					}
				}
			}
		}

		if (recurs && retval == L"")
		{
			Element curEle = (Element)currentObject;
			Element rootEle = (Element)this->GetRoot();
			if (ToWString(curEle->getAttribute(X("guid"))) != ToWString(rootEle->getAttribute(X("guid"))))
			//recursively serach
			{
				Node parent = (Node)currentObject->parentNode;
				retval = GetAttribute(parent, id, name, true);
			}
		}
	#endif

	#ifdef USE_LIBXML
		for (Node childNode = currentObject->children; childNode != NULL; childNode = childNode->next)
		{
			if (MBCStrToWStr(childNode->name) == ATTRIBUTE_NODE)
			{
				if (MBCStrToWStr(xmlGetProp(childNode, (xmlChar*)"id")) == id)
				{
					if (name.length() > 0) //getting a sub attr
					{
						retval = MBCStrToWStr(xmlGetProp(childNode, (xmlChar*)WStrToMBCStr(name).c_str()));
						break;
					}
					else //get the main attr
					{
						retval = MBCStrToWStr(xmlGetProp(childNode, (xmlChar*)"value"));
						break;
					}
				}
			}
		}

		if (recurs && retval == L"")
		{
			Node rootNode = xmlDocGetRootElement(xmlDoc);
			wstring rootGUID = MBCStrToWStr(xmlGetProp(rootNode, (xmlChar*)"guid"));
			wstring currentGUID = MBCStrToWStr(xmlGetProp(currentObject, (xmlChar*)"guid"));
			if (rootGUID != currentGUID)
			{
				Node parent = currentObject->parent;
				retval = GetAttribute(parent, id, name, true);
			}
		}
	#endif

	return retval;
}

bool ROMTree::SetROMObjectValue(Node currentObject, wstring name, wstring value)
{
	bool retval = false;

	if (!currentObject)
		return retval;
	#ifdef USE_MSXML
		Element elem = (Element)currentObject;
		elem->setAttribute(name.c_str(), value.c_str());
	#endif

	#ifdef USE_LIBXML
		if (xmlHasProp(currentObject, (xmlChar*)WStrToMBCStr(name).c_str()))
		{
			xmlSetProp(currentObject, (xmlChar*)WStrToMBCStr(name).c_str(), (xmlChar*)WStrToMBCStr(value).c_str());
		}
		else
		{
			xmlNewProp(currentObject, (xmlChar*)WStrToMBCStr(name).c_str(), (xmlChar*)WStrToMBCStr(value).c_str());
		}
	#endif
	retval = true;
	return retval;
}

wstring ROMTree::GetROMObjectValue(Node currentObject, wstring name)
{
	wstring retval = L"";

	if (!currentObject)
		return retval;
	#ifdef USE_MSXML
		Element elem = (Element)currentObject;
		retval = ToWString(elem->getAttribute(name.c_str()));
	#endif

	#ifdef USE_LIBXML
		if (xmlHasProp(currentObject, (xmlChar*)WStrToMBCStr(name).c_str()))
		{
			retval = MBCStrToWStr(xmlGetProp(currentObject, (xmlChar*)WStrToMBCStr(name).c_str()));
		}
	#endif
	return retval;
}

bool ROMTree::SetAttribute(Node currentObject, wstring id, wstring value)
{
	return SetAttribute(currentObject, id, L"value", value);
}

bool ROMTree::SetAttribute(Node currentObject, wstring id, wstring name, wstring value)
{
	bool retval = false;

	if (!currentObject)
		return retval;

	#ifdef USE_MSXML
		//if already exists, just change the value
		if (currentObject->hasChildNodes())
		{
			NodeList nodes = (NodeList)currentObject->childNodes;
			for (WORD i = 0; i < nodes->length; i++)
			{
				Node attrNode = nodes->item[i];
				if (attrNode->nodeName == X(ATTRIBUTE_NODE))
				{
					NamedNodeMap attrs = attrNode->attributes;
					if (attrs)
					{
						Node data = (Node)attrs->getNamedItem(X("id"));
						if (data != NULL && ToWString(data->nodeValue) == id)
						{
							if (name.length() == 0)
							{ //primary attr value
								Element elem = (Element)attrNode;
								elem->setAttribute(X("value"), value.c_str());
								retval = true;
								break;
							}
							else if (name.length() > 0)
							{ //set another attr
								Element elem = (Element)attrNode;
								elem->setAttribute(name.c_str(), value.c_str());
								retval = true;
								break;
							}
						}
					}
				}
			}
		}

		if (!retval && name == L"value")
		{
		  //create new main attribute
			Element elem = xmlDoc->createElement(X(ATTRIBUTE_NODE));
			elem->setAttribute("id", id.c_str());
			elem->setAttribute("value", value.c_str());
			currentObject->appendChild(elem);
			retval = true;
		}
	#endif

	#ifdef USE_LIBXML
		for (Node childNode = currentObject->children; childNode != NULL; childNode = childNode->next)
		{
			if (MBCStrToWStr(childNode->name) == ATTRIBUTE_NODE)
			{
				wstring currentID = MBCStrToWStr(xmlGetProp(childNode, (xmlChar*)"id"));
				if (currentID == id)
				{
					if (name.length() > 0)
					{
						if (xmlHasProp(childNode, (xmlChar*)WStrToMBCStr(name).c_str()))
						{
							xmlSetProp(childNode, (xmlChar*)WStrToMBCStr(name).c_str(), (xmlChar*)WStrToMBCStr(value).c_str());
						}
						else
						{
							xmlNewProp(childNode, (xmlChar*)WStrToMBCStr(name).c_str(), (xmlChar*)WStrToMBCStr(value).c_str());
						}
					}
					else //primary attr value
					{
						if (xmlHasProp(childNode, (xmlChar*)WStrToMBCStr(name).c_str()))
						{
							xmlSetProp(childNode, (xmlChar*)WStrToMBCStr(id).c_str(), (xmlChar*)"value");
						}
						else
						{
							xmlNewProp(childNode, (xmlChar*)WStrToMBCStr(name).c_str(), (xmlChar*)"value");
						}
					}
					retval = true;
					break;
				}
			}
		}

		if (!retval && name == L"value")
		{
			//create new main attribute
			Node attrNode = xmlNewNode(NULL, (xmlChar*)WStrToMBCStr(ATTRIBUTE_NODE).c_str());
			xmlAddChild(currentObject, attrNode);
			xmlNewProp(attrNode, (xmlChar*)"id", (xmlChar*)WStrToMBCStr(id).c_str());
			xmlNewProp(attrNode, (xmlChar*)"value", (xmlChar*)WStrToMBCStr(value).c_str());
		}
	#endif
	return retval;
}

bool ROMTree::RemoveAttribute(Node currentObject, wstring id)
{
	bool retval = false;

	if (!currentObject)
		return retval;

	#ifdef USE_MSXML
		if (currentObject->hasChildNodes())
		{
			NodeList nodes = (NodeList)currentObject->childNodes;
			for (WORD i = 0; i < nodes->length; i++)
			{
				Node attrNode = nodes->item[i];
				if (attrNode->nodeName == X(ATTRIBUTE_NODE))
				{
					NamedNodeMap attrs = attrNode->attributes;
					Node data = (Node)attrs->getNamedItem(id.c_str());

					if (data)
					{ //got it
						currentObject->removeChild(attrNode);
						retval = true;
						break;
					}
				}
			}
		}
	#endif

	#ifdef USE_LIBXML
		for (Node childNode = currentObject->children; childNode != NULL; childNode = currentObject->next)
		{
			if (childNode->name == (xmlChar*)WStrToMBCStr(ATTRIBUTE_NODE).c_str())
			{
				if (xmlGetProp(childNode, (xmlChar*)"id") == (xmlChar*)WStrToMBCStr(id).c_str())
				{
					xmlUnlinkNode(childNode);
					xmlFreeNode(childNode);
					retval = true;
					break;
				}
			}
		}
	#endif
	return retval;
}

bool ROMTree::DestroyROMObject(Node currentObject)
{
	bool retval = true;

	if (!currentObject)
		return false;

	#ifdef USE_MSXML
		Node parent = currentObject->parentNode;
		parent->removeChild(currentObject);
	#endif

	#ifdef USE_LIBXML
		xmlUnlinkNode(currentObject);
		xmlFreeNode(currentObject);
	#endif

	return retval;
}

map<wstring, map<wstring, wstring> > ROMTree::GetAllAttributes(Node currentObject)
{
	map<wstring, map<wstring, wstring> > retval;

#ifdef USE_MSXML
	NodeList res = currentObject->selectNodes("Attribute");
	if (res != NULL)
	{
		for (long i = 0; i < res->length; i++)
		{
			wstring name = ToWString(res->item[i]->attributes->getNamedItem("id")->nodeValue);
			map<wstring, wstring> keysAndValues;
			for (long j = 0; j < res->item[i]->attributes->length; j++)
			{
				NamedNodeMap attrNodeMap = res->item[i]->attributes;
				wstring key = attrNodeMap->item[j]->nodeName;
				wstring value = ToWString(attrNodeMap->item[j]->nodeValue);
				keysAndValues[key] = value;
			}
			retval[name] = keysAndValues;
		}
	}
#endif

#ifdef USE_LIBXML
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
	xpathCtx->node = currentObject;
	xmlXPathObjectPtr xpathObjSearch = xmlXPathEvalExpression((xmlChar*)"Attribute", xpathCtx);
	NodeList res = xpathObjSearch->nodesetval;
	if (res != NULL)
	{
		for (long i = 0; i < res->nodeNr; i++)
		{
			Node ROMAttrNode = res->nodeTab[i];
			wstring name = MBCStrToWStr(xmlGetProp(ROMAttrNode, (xmlChar*)"id"));
			map<wstring, wstring> keysAndValues;
			for (Node attrNode = ROMAttrNode->children; attrNode != NULL; attrNode = attrNode->next)
			{
				if (attrNode->type == XML_ATTRIBUTE_NODE)
				{
					wstring key = MBCStrToWStr(attrNode->name);
					wstring value = MBCStrToWStr(xmlGetProp(ROMAttrNode, attrNode->name));
					keysAndValues[key] = value;
				}
				retval[name] = keysAndValues;
			}
		}
	}
	xmlXPathFreeObject(xpathObjSearch);
	xmlXPathFreeContext(xpathCtx);
#endif
	return retval;
}

//IO
wstring ROMTree::DumpTree(int format)
{
	wstring retval = L"";

#ifdef USE_MSXML
	retval = ToWString(xmlDoc->Getxml().GetBSTR());
#endif
#ifdef USE_LIBXML
	xmlChar *xmlbuff;
	int buffersize = 0;
	xmlDocDumpFormatMemory(xmlDoc, &xmlbuff, &buffersize, format);
	retval = MBCStrToWStr(xmlbuff);
#endif
	return retval;
}

bool ROMTree::LoadTree(wstring xmlStr)
{
	bool retval = false;

#ifdef USE_MSXML
	retval = xmlDoc->loadXML(xmlStr.c_str());
	xmlDoc->setProperty("SelectionLanguage", "XPath");
#endif
#ifdef USE_LIBXML
	xmlDoc = NULL;
	string buff = WStrToMBCStr(xmlStr);
	xmlDoc = xmlParseMemory(buff.c_str(), (int)buff.size());
	if (xmlDoc)
		retval = true;
#endif
	return retval;
}

//rules
bool ROMTree::LoadRules(wstring knowledge_file)
{
	return m_KnowledgeBase.CreateKnowledgeBase(knowledge_file);
}

vector<wstring> ROMTree::EvaluateTable(Node currentObject, wstring evalTable, wstring output, bool bGetAll)
{
	LoadInputs(currentObject, evalTable);
	return m_KnowledgeBase.EvaluateTable(evalTable, output, bGetAll);
}

map<wstring, vector<wstring> > ROMTree::EvaluateTable(Node currentObject, wstring evalTable, bool bGetAll)
{
	LoadInputs(currentObject, evalTable);
	return m_KnowledgeBase.EvaluateTable(evalTable, bGetAll);
}

vector<wstring> ROMTree::LoadInputs(Node currentObject, wstring evalTable)
{
	vector<wstring> inputs = m_KnowledgeBase.GetInputDependencies(evalTable);
	for (vector<wstring>::iterator it = inputs.begin(); it != inputs.end(); it++)
	{
		wstring value = GetATableInputValue(currentObject, *it);
		m_KnowledgeBase.SetInputValue(*it, value);
	}
	return inputs;
}

vector<wstring> ROMTree::GetPossibleValues(Node currentObject, wstring evalTable, wstring outputName)
{
	vector<wstring> outputs = m_KnowledgeBase.GetAllPossibleOutputs(evalTable, outputName);
	return outputs;
}

wstring ROMTree::GetATableInputValue(Node currentObject, wstring input)
{
	wstring retval;
	//parse out any XPATH Queries here and return values
	if (StringContains(input, L"xpath("))
	{
		wstring cmdArg(input.begin() + 6, input.end() - 1);
		retval = EvaluateXPATH(currentObject, cmdArg);
	}
	else
	{
		retval = GetAttribute(currentObject, input);
	}

	return retval;
}

wstring ROMTree::EvaluateXPATH(Node currentObject, wstring xpath)
{
	wstring retval;
#ifdef USE_MSXML
	Node guidNode = currentObject->attributes->getNamedItem(X("guid"));
	wstring currentGUID = ToWString(guidNode->nodeValue);
#endif
#ifdef USE_LIBXML
	wstring currentGUID = MBCStrToWStr(xmlGetProp(currentObject, (xmlChar*)"guid"));
#endif
	wstring match = L"<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'";
	match += currentGUID + L"\']\"><xsl:value-of select=\"";
	wstring xslt_text = XSLT_TOP + match + xpath + XSLT_BOTTOM;

	Document xsltDoc = NULL;
#ifdef USE_MSXML
	xsltDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	xsltDoc->loadXML(xslt_text.c_str());
	retval = ToWString(xmlDoc->transformNode(xsltDoc));
#endif
#ifdef USE_LIBXML
	string buff = WStrToMBCStr(xslt_text);
	xsltDoc = xmlParseMemory(buff.c_str(), (int)buff.length());

	xsltStylesheetPtr xsl = xsltParseStylesheetDoc(xsltDoc);
	Document result = xsltApplyStylesheet(xsl, xmlDoc, NULL);

	xmlChar *xmlbuff;
	int buffersize = 0;
	xsltSaveResultToString(&xmlbuff, &buffersize, result, xsl);
	//xmlDocDumpFormatMemory(result, &xmlbuff, &buffersize, 0);
	retval = MBCStrToWStr(xmlbuff);

	xmlFreeDoc(xsltDoc);
	xmlFreeDoc(result);
#endif
	return retval;
}

//ASCII Overloads
ROMTree::ROMTree(string name)
{
	Initialize(ROMUTIL::MBCStrToWStr(name));
}

vector<Node> ROMTree::Find(Node current, string searchStr)
{
	return Find(current, ROMUTIL::MBCStrToWStr(searchStr));
}

Node ROMTree::CreateROMObject(string name)
{
	return CreateROMObject(ROMUTIL::MBCStrToWStr(name));
}

string ROMTree::GetAttribute(Node currentObject, string id, string name, bool recurs)
{
	return ToASCIIString(GetAttribute(currentObject, ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), recurs));
}

bool ROMTree::SetAttribute(Node currentObject, string id, string name, string value)
{
	return SetAttribute(currentObject, ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value));
}

bool ROMTree::SetAttribute(Node currentObject, string id, string value)
{
	return SetAttribute(currentObject, ROMUTIL::MBCStrToWStr(id), ROMUTIL::MBCStrToWStr(value));
}

bool ROMTree::SetROMObjectValue(Node currentObject, string name, string value)
{
	return SetROMObjectValue(currentObject, ROMUTIL::MBCStrToWStr(name), ROMUTIL::MBCStrToWStr(value));
}

string ROMTree::GetROMObjectValue(Node currentObject, string name)
{
	return ToASCIIString(GetROMObjectValue(currentObject, ROMUTIL::MBCStrToWStr(name)));
}

bool ROMTree::RemoveAttribute(Node current, string id)
{
	return RemoveAttribute(current, ROMUTIL::MBCStrToWStr(id));
}

string ROMTree::GetROMObjectNameA(Node current)
{
	return ToASCIIString(GetROMObjectName(current));
}

void ROMTree::SetROMObjectName(Node current, string name)
{
	SetROMObjectName(current, ROMUTIL::MBCStrToWStr(name));
}

bool ROMTree::LoadRules(string knowledge_file)
{
	return LoadRules(ROMUTIL::MBCStrToWStr(knowledge_file));
}

vector<string> ROMTree::EvaluateTable(Node currentObject, string evalTable, string output, bool bGetAll)
{
	LoadInputs(currentObject, ROMUTIL::MBCStrToWStr(evalTable));
	return m_KnowledgeBase.EvaluateTable(evalTable, output, bGetAll);
}

map<string, vector<string> > ROMTree::EvaluateTable(Node currentObject, string evalTable, bool bGetAll)
{
	LoadInputs(currentObject, ROMUTIL::MBCStrToWStr(evalTable));
	return m_KnowledgeBase.EvaluateTable(evalTable, bGetAll);
}

string ROMTree::DumpTreeA(int format)
{
	return ToASCIIString(DumpTree(format));
}

bool ROMTree::LoadTree(string xmlStr)
{
	return LoadTree(ROMUTIL::MBCStrToWStr(xmlStr));
}

string ROMTree::EvaluateXPATH(Node current, string xpath)
{
	return ToASCIIString(EvaluateXPATH(current, ROMUTIL::MBCStrToWStr(xpath)));
}
