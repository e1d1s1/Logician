/*
This file is part of the Relational Object Model Library.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

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

void ROMNode::CreateROMNode(wstring id)
{
	id = FindAndReplace(id, L" ", L"_");
	m_id = id;
	_init();
}

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

bool ROMNode::RemoveFromParent()
{
	if (m_parent != NULL)
	{
		return m_parent->RemoveChildROMObject(this);
	}
	return false;
}

vector<ROMNode*> ROMNode::GetAllChildren(bool recurs)
{
	vector<ROMNode*> retval;
	if (!recurs)
		retval = m_children;
	else
		_findAllChildObjects(&retval);

	return retval;
}

vector<ROMNode*> ROMNode::GetAllFriends()
{
	return m_friends;
}

bool ROMNode::AddFriend(ROMNode *friendObj)
{
	if (friendObj)
	{
		vector<ROMNode*>::iterator it = find(m_friends.begin(), m_friends.end(), friendObj);
		if (it == m_friends.end())
		{
			m_friends.push_back(friendObj);
			friendObj->m_friends.push_back(this);
			m_bChanged = true;
			return true;
		}
		return false;		
	}
	else
		return false;
}

bool ROMNode::RemoveFriend(ROMNode *friendObj)
{
	bool retval = false;
	if (friendObj)
	{
		vector<ROMNode*>::iterator it = find(m_friends.begin(), m_friends.end(), friendObj);
		if (it != m_friends.end())
		{
			vector<ROMNode*>::iterator it2 = find(friendObj->m_friends.begin(), friendObj->m_friends.end(), this);
			if (it2 != friendObj->m_friends.end())
				friendObj->m_friends.erase(it2);
			m_friends.erase(it);
			retval = true;
			m_bChanged = retval;
		}
	}
	
	return retval;
}

bool ROMNode::RemoveAllFriends()
{
	bool retval = false;
	for (vector<ROMNode*>::iterator it = m_friends.begin(); it != m_friends.end(); it++)
	{
		retval = RemoveFriend(*it);
	}	
	return retval;
}

vector<ROMNode*> ROMNode::FindAllObjectsByID(wstring id, bool recurs)
{
	vector<ROMNode*> retval;
	
	if (m_id == id)
		retval.push_back(this);
	_findObjects(id, recurs, &retval);

	return retval;
}

ROMNode* ROMNode::FindObjectByGUID(string guid)
{
	ROMNode* retval = NULL;

	ROMNode* rootNode = GetRoot();
	if (rootNode->m_guid == guid)
		retval = rootNode;
	else
		retval = rootNode->_findObjectGUID(guid);

	return retval;
}

bool ROMNode::DestroyROMObject()
{
	bool retval = true;

	//clear friends
	for (vector<ROMNode*>::iterator it = m_friends.begin(); it != m_friends.end(); it++)
	{
		ROMNode* friendNode = *it;
		if (friendNode)
		{
			friendNode->RemoveFriend(this);
		}
	}

	//trigger downstream destructors
	for (vector<ROMNode*>::iterator it = m_children.begin(); it != m_children.end(); )
	{
		ROMNode* node = *it;
		if (node)
		{			
			it = m_children.erase(it);
			delete node;
		}
		else
			++it;
	}

	//remove any references to self in parent node
	if (m_parent != NULL)
	{
		retval = m_parent->RemoveChildROMObject(this);
	}

	m_attrs.clear();
	m_nodeValues.clear();
	m_id.clear();
	m_parent = NULL;
	m_bChanged = true;
	
	m_friends.clear();
	m_children.clear();

	return retval;
}

ROMNode* ROMNode::Clone()
{
	ROMNode* newNode = new ROMNode(m_id);
	newNode->m_attrs = m_attrs;
	newNode->m_nodeValues = m_nodeValues;
	newNode->m_bChanged = false;
	for (size_t i = m_children.size() - 1; i >= 0; i--)
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

vector<ROMNode*> ROMNode::FindObjects(wstring xpath)
{
	_createXMLDoc();
	vector<ROMNode*> retval;
	vector<Node> nodes;
#ifdef USE_MSXML
	NodeList res = m_xmlDoc->selectNodes(xpath.c_str());
	if (res != NULL)
	{
		for (long i = 0; i < res->length; i++)
		{
			Node n;
			res->get_item(i, &n);
			nodes.push_back(n);
		}
	}

	for (vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++)
	{		
		Node objNode = (*it);
		wstring guid = objNode->attributes->getNamedItem("guid")->nodeValue.bstrVal;
		if (guid.length() > 0)
		{
			ROMNode* node = FindObjectByGUID(guid);
			if (node != NULL)
				retval.push_back(node);
		}
	}
#endif
#ifdef USE_LIBXML
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(m_xmlDoc);
	xmlXPathObjectPtr xpathObjSearch = xmlXPathEvalExpression((xmlChar*)WStrToMBCStr(xpath).c_str(), xpathCtx);
	NodeList res = xpathObjSearch->nodesetval;
	if (res != NULL)
	{
		for (int i = 0; i < res->nodeNr; i++)
		{
			nodes.push_back(res->nodeTab[i]);
		}
	}
	xmlXPathFreeObject(xpathObjSearch);
	xmlXPathFreeContext(xpathCtx);

	for (vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++)
	{		
		Node objNode = (*it);
		wstring guid = XMLStrToWStr(xmlGetProp(objNode, (xmlChar*)"guid"));
		if (guid.length() > 0)
		{
			ROMNode* node = FindObjectByGUID(guid);
			if (node != NULL)
				retval.push_back(node);
		}
	}
#endif
	return retval;
}

ROMNode* ROMNode::_findObjectGUID(string guid)
{
	ROMNode* retval = NULL;
	for (vector<ROMNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		if ((*it)->m_guid == guid)
		{
			retval = *it;
			break;
		}
		else
		{
			retval = (*it)->_findObjectGUID(guid);
			if (retval != NULL)
				break;
		}
	}
	return retval;
}

void ROMNode::_findObjects(wstring id, bool recurs, vector<ROMNode*>* res)
{
	for (vector<ROMNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		if (recurs && (*it)->m_id == id && res != NULL)
		{
			res->push_back(*it);
			if ((*it)->m_children.size() > 0)
				(*it)->_findObjects(id, recurs, res);
		}
	}
}

void ROMNode::_findAllChildObjects(vector<ROMNode*>* res)
{
	for (vector<ROMNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		if (res != NULL)
		{
			res->push_back(*it);
			if ((*it)->m_children.size() > 0)
				(*it)->_findAllChildObjects(res);
		}
	}
}

void ROMNode::SetTableDebugHandler(DebugHandler debugger)
{
	if (m_KnowledgeBase) 
		m_KnowledgeBase->SetDebugHandler(debugger);
}

void ROMNode::GenerateTableDebugMessages(bool bGenerate)
{
	if (m_KnowledgeBase) 
		m_KnowledgeBase->GenerateDebugMessages(bGenerate);
}

wstring ROMNode::GetTableDebugMessages()
{
	if (m_KnowledgeBase) 
		return m_KnowledgeBase->GetDebugMessages(); 
	else 
		return L"";
}

wstring ROMNode::GetAttribute(wstring id, wstring name, bool immediate)
{
	wstring retval = L"";
	bool bFound = false;

	MAPWSTRMAP::iterator it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		MAPWSTRS::iterator itValue = it->second.find(name);
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
	MAPWSTRMAP::iterator it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		MAPWSTRS::iterator itValue = it->second.find(name);
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

	MAPWSTRMAP::iterator it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		if (name.length() == 0 || name == L"value")
		{
			m_attrs.erase(it);
			retval = true;
		}
		else
		{
			MAPWSTRS::iterator itValue = it->second.find(name);
			if (itValue != it->second.end())
			{
				it->second.erase(itValue);
				retval = true;
			}
		}
		m_bChanged = retval;
	}	

	return retval;
}

bool ROMNode::SetROMObjectValue(wstring name, wstring value)
{
	bool retval = true;

	name = FindAndReplace(name, L" ", L"_");
	m_nodeValues[name] = value;
	m_bChanged = retval;

	return retval;
}

wstring ROMNode::GetROMObjectValue(wstring name)
{
	wstring retval = L"";

	MAPWSTRS::iterator it = m_nodeValues.find(name);
	if (it != m_nodeValues.end())
	{
		retval = it->second;
	}

	return retval;
}

bool ROMNode::RemoveROMObjectValue(wstring name)
{
	bool retval = false;

	MAPWSTRS::iterator it = m_nodeValues.find(name);
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
	{
		delete m_KnowledgeBase;
		m_KnowledgeBase = NULL;
	}

	if (m_parent == NULL) //only the root will have the reference to the rules
	{
		m_KnowledgeBase = new EDS::CKnowledgeBase();
		return m_KnowledgeBase->CreateKnowledgeBase(knowledge_file);
	}
	else
		return false;
}

bool ROMNode::LoadRulesFromString(wstring xmlStr)
{
	if (m_KnowledgeBase)
	{
		delete m_KnowledgeBase;
		m_KnowledgeBase = NULL;
	}

	if (m_parent == NULL) //only the root will have the reference to the rules
	{
		m_KnowledgeBase = new EDS::CKnowledgeBase();
		return m_KnowledgeBase->CreateKnowledgeBaseFromString(xmlStr);
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



vector<wstring> ROMNode::EvaluateTableWithParam(wstring evalTable, wstring output, wstring param, bool bGetAll)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTableWithParam(evalTable, output, param, bGetAll);
	}
	return retval;
}

vector<wstring> ROMNode::EvaluateTableWithParam(wstring evalTable, wstring output, wstring param)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTableWithParam(evalTable, output, param);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTableWithParam(wstring evalTable, wstring param, bool bGetAll)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTableWithParam(evalTable, param, bGetAll);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTableWithParam(wstring evalTable, wstring param)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		LoadInputs(evalTable);
		retval = knowledge->EvaluateTableWithParam(evalTable, param);
	}
	return retval;
}

wstring ROMNode::GetEvalParameter()
{
	wstring retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->GetEvalParameter();
	}
	return retval;
}

wstring ROMNode::GetFirstTableResult(wstring tableName, wstring output)
{
	wstring retval = L"";
	vector<wstring> res = EvaluateTable(tableName, output);
	if (res.size() > 0)
		retval = res[0];
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
	else if (input == L"CLASSID")
	{
		retval = this->GetROMObjectID();
	}
	else
	{
		retval = GetAttribute(input);
	}

	return retval;
}

//IO
void ROMNode::_createXMLDoc(bool bForceLoad)
{
	bool bChanged = (bForceLoad || _anyHasChanged());
	if (bChanged)
	{
		wstring genXML = _generateXML(bChanged);
#ifdef USE_MSXML
		if (m_xmlDoc != NULL)
			m_xmlDoc.Release();
#ifdef NOATL
		m_xmlDoc.CreateInstance(L"MSXML2.DOMDocument.6.0");
#else
		m_xmlDoc.CoCreateInstance(L"MSXML2.DOMDocument.6.0");
#endif
		m_xmlDoc->async = VARIANT_FALSE;
		m_xmlDoc->resolveExternals = VARIANT_FALSE;
		m_xmlDoc->setProperty("SelectionLanguage", "XPath");
		m_xmlDoc->setProperty("SelectionNamespaces", "");
		VARIANT_BOOL res = m_xmlDoc->loadXML(genXML.c_str()); //-1 is true
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
		beginObject+=L"\" ";

		//object values
		wstring objAttrs = L" ";
		for (MAPWSTRS::iterator itObjValue = m_nodeValues.begin(); itObjValue != m_nodeValues.end(); itObjValue++)
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
			for (MAPWSTRMAP::iterator it = m_attrs.begin(); it != m_attrs.end(); it++)
			{
				wstring attrObject = L"<Attribute id=\"";
				attrObject+=it->first;
				attrObject+=L"\" ";
				for (MAPWSTRS::iterator itValue = it->second.begin(); itValue != it->second.end(); itValue++)
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

wstring ROMNode::SaveXML(bool indented)
{
	_createXMLDoc(true);
	return _convertXMLDocToString(indented);
}

bool ROMNode::LoadXML(wstring xmlStr)
{
	bool retval = false;

#ifdef USE_MSXML
	if (m_xmlDoc != NULL)
		m_xmlDoc.Release();

#ifdef NOATL
	m_xmlDoc.CreateInstance(L"MSXML2.DOMDocument.6.0");
#else
	m_xmlDoc.CoCreateInstance(L"MSXML2.DOMDocument.6.0");
#endif
	m_xmlDoc->async = VARIANT_FALSE;
	m_xmlDoc->resolveExternals = VARIANT_FALSE;
	m_xmlDoc->setProperty("SelectionLanguage", "XPath");
	m_xmlDoc->setProperty("SelectionNamespaces", "");

	try
	{
		VARIANT_BOOL ok = m_xmlDoc->loadXML(_bstr_t(xmlStr.c_str()));
		if (ok == VARIANT_TRUE)
		{
			Node objectNode = m_xmlDoc->selectSingleNode("Object");
			if (_buildObject(objectNode, NULL) != NULL)
				retval = true;
		}
		else
		{
			if (m_xmlDoc != NULL && m_xmlDoc->parseError->errorCode != 0)			
				ReportROMError(ToASCIIString((wstring)(m_xmlDoc->parseError->reason)));			
			else
				ReportROMError("Error loading XML");
		}
			
	}
	catch(const _com_error& e)
	{
		ReportROMError(ToASCIIString((wstring)(e.Description())));
	}
	catch (...)
	{
		ReportROMError("Problem parsing XML");
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
			Node objectNode = allObjs->nodeTab[0];
			if (_buildObject(objectNode, NULL) != NULL)
				retval = true;			
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
	wstring guid = objectNode->attributes->getNamedItem("guid")->nodeValue.bstrVal;
	if (parent == NULL)
	{
		DestroyROMObject();
		m_id = id;
		m_guid = ToASCIIString(guid);
		newNode = this;
	}
	else
	{
		newNode = new ROMNode(id);
		newNode->m_guid = ToASCIIString(guid);
	}

	//set object values
	for (long i = 0; i < objectNode->attributes->Getlength(); i++)
	{
		Node objAttr = objectNode->attributes->Getitem(i);
		wstring attrName = objAttr->nodeName.GetBSTR();		
		if (attrName != L"id" && attrName != L"guid")
		{
			wstring attrValue = objAttr->nodeValue.bstrVal;
			newNode->SetROMObjectValue(attrName, attrValue);
		}
	}

	//set object attributes
	NodeList attrNodes = objectNode->selectNodes("Attribute");
	for (long attrCnt = 0; attrCnt < attrNodes->Getlength(); attrCnt++)
	{
		Node attrNode = attrNodes->item[attrCnt];
		wstring idAttr = attrNode->attributes->getNamedItem("id")->nodeValue.bstrVal;
		for (long i = 0; i < attrNode->attributes->Getlength(); i++)
		{
			Node attr = attrNode->attributes->Getitem(i);
			wstring attrName = attr->nodeName.GetBSTR();
			wstring attrValue = attr->nodeValue.bstrVal;
			if (attrName != L"id")
				newNode->SetAttribute(idAttr, attrName, attrValue);
		}
	}

	//children recursivley
	NodeList childNodes = objectNode->selectNodes("Object");
	for (long childCnt = 0; childCnt < childNodes->Getlength(); childCnt++)
	{
		Node childNode = childNodes->item[childCnt];
		ROMNode *newChildObject = _buildObject(childNode, this);
		if (newChildObject != NULL && newNode != NULL)
		{
			newNode->AddChildROMObject(newChildObject);
		}
	}
#endif

#ifdef USE_LIBXML
	wstring id = XMLStrToWStr(xmlGetProp(objectNode, (xmlChar*)"id"));
	wstring guid = XMLStrToWStr(xmlGetProp(objectNode, (xmlChar*)"guid"));
	if (parent == NULL)
	{
		DestroyROMObject();
		m_id = id;
		m_guid = ToASCIIString(guid);
		newNode = this;
	}
	else
	{
		newNode = new ROMNode(id);
		newNode->m_guid = ToASCIIString(guid);
	}

	//set object values
	for (Attribute objValue = objectNode->properties; objValue != NULL; objValue = objValue->next)
	{
		wstring attrName = XMLStrToWStr(objValue->name);
		if (attrName != L"id" && attrName != L"guid")
		{
			wstring value = XMLStrToWStr(xmlGetProp(objectNode, (xmlChar*)WStrToMBCStr(id).c_str()));
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
			wstring idAttr = XMLStrToWStr(xmlGetProp(attrNode, (xmlChar*)"id"));
			for (Attribute attr = attrNode->properties; attr != NULL; attr = attr->next)
			{
				wstring name = XMLStrToWStr(attr->name);
				if (name != L"id")
				{
					wstring value = XMLStrToWStr(xmlGetProp(attrNode, (xmlChar*)WStrToMBCStr(name).c_str()));
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
			ROMNode *newChildObject = _buildObject(childNode, this);
			if (newChildObject != NULL && newNode != NULL)
			{
				newNode->AddChildROMObject(newChildObject);
			}
		}
	}

#endif
#ifdef _DEBUG
	m_bChanged = true;
	_createXMLDoc();
	string str;
	wstring xml = _convertXMLDocToString(true);
	str.assign(xml.begin(), xml.end());
	ReportROMError(str);
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
#ifdef NOATL
		xsltDoc.CreateInstance(L"MSXML2.DOMDocument.6.0");
#else
		xsltDoc.CoCreateInstance(L"MSXML2.DOMDocument.6.0");
#endif
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
		retval = XMLStrToWStr(xmlbuff);

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
		//By default it is writing the encoding = UTF-16. Change the encoding to UTF-8
		// <?xml version="1.0" encoding="UTF-8"?>
		MSXML2::IXMLDOMNodePtr pXMLFirstChild = m_xmlDoc->GetfirstChild();
		MSXML2::IXMLDOMNodePtr pXMLEncodNode = NULL;
		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
		MSXML2::IXMLDOMNamedNodeMapPtr pXMLAttributeMap =  NULL;	
		
		if (pXMLFirstChild != NULL)
		{
			pXMLAttributeMap = pXMLFirstChild->Getattributes();
			pXMLAttributeMap->getNamedItem(bstr_t("encoding")); 			
		}
		if (pXMLEncodNode != NULL)
			pXMLEncodNode->PutnodeValue(bstr_t("utf-8")); //encoding = UTF-8. Serializer usually omits it in output, it is the default encoding
		else
		{
			MSXML2::IXMLDOMElementPtr pXMLRootElem = m_xmlDoc->GetdocumentElement();
			if (pXMLRootElem != NULL)
			{
				MSXML2::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =    
					m_xmlDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");
				_variant_t vtObject;
				vtObject.vt = VT_DISPATCH;
				vtObject.pdispVal = pXMLRootElem;
				vtObject.pdispVal->AddRef();

				m_xmlDoc->insertBefore(pXMLProcessingNode,vtObject);
			}
		}
		retval = (wstring)m_xmlDoc->Getxml();		
#endif
#ifdef USE_LIBXML
		xmlChar *xmlbuff;
		int buffersize = 0;
		int format = 0;
		if (indented)
			format = 1;
		xmlDocDumpFormatMemory(m_xmlDoc, &xmlbuff, &buffersize, format);
		retval = XMLStrToWStr(xmlbuff);
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
