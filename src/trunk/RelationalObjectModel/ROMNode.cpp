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

void ROMNode::CreateROMNode(const wstring id, ObjectFactory factory)
{
	wstring newid = FindAndReplace(id, L" ", L"_");
	m_id = newid;
	_init(factory);
}

void ROMNode::_init(ObjectFactory factory)
{
#ifdef USE_LIBXML
	xmlInitParser();
#endif
#ifdef USE_MSXML
	CoInitialize(nullptr);
#endif
	m_guid = MakeGUID();
	m_parent = nullptr;
	m_xmlDoc = nullptr;
	m_KnowledgeBase = nullptr;
	m_children.reserve(256);
	m_bChanged = true;

	if (factory == nullptr)
	{
		ROMObjectFactory = [](const wstring& id){
			return new ROMNode(id);
		};
	}
	else
	{
		ROMObjectFactory = factory;
	}
}

ROMNode::~ROMNode(void)
{
	DestroyROMObject();
#ifdef USE_LIBXML
	xmlCleanupParser();
#endif
#ifdef USE_MSXML
	if (m_xmlDoc != nullptr)
		m_xmlDoc.Release();
	CoUninitialize();
#endif
}

ROMNode* ROMNode::GetRoot()
{
	ROMNode *lastParent = this;
	ROMNode *nextParent = nullptr;
	do
	{
		nextParent = lastParent->GetParent();
		if (nextParent != nullptr)
			lastParent = nextParent;
	} while (nextParent != nullptr);

	return lastParent;
}

bool ROMNode::AddChildROMObject(ROMNode *child)
{
	if (child->m_parent == nullptr)
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
	if (m_parent != nullptr)
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

vector<ROMNode*> ROMNode::FindAllObjectsByID(const wstring& id, bool recurs)
{
	vector<ROMNode*> retval;

	if (m_id == id)
		retval.push_back(this);
	_findObjects(id, recurs, &retval);

	return retval;
}

ROMNode* ROMNode::FindObjectByGUID(const string& guid)
{
	ROMNode* retval = nullptr;

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
	if (m_parent != nullptr)
	{
		retval = m_parent->RemoveChildROMObject(this);
	}

	m_attrs.clear();
	m_nodeValues.clear();
	m_id.clear();
	m_parent = nullptr;
	m_bChanged = true;

	m_friends.clear();
	m_children.clear();

	return retval;
}

ROMNode* ROMNode::Clone()
{
	ROMNode* newNode = ROMObjectFactory(m_id);
	newNode->m_attrs = m_attrs;
	newNode->m_nodeValues = m_nodeValues;
	for (ROMNode* node : m_children)
	{
        ROMNode* newChild = node->Clone();
        newNode->AddChildROMObject(newChild);
	}
	return newNode;
}

vector<ROMNode*> ROMNode::FindObjects(const wstring& xpath)
{
	_createXMLDoc(false, false);
	vector<ROMNode*> retval;
	vector<Node> nodes;
#ifdef USE_MSXML
	NodeList res = m_xmlDoc->selectNodes(xpath.c_str());
	if (res != nullptr)
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
			if (node != nullptr)
				retval.push_back(node);
		}
	}
#endif
#ifdef USE_LIBXML
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(m_xmlDoc);
	xmlXPathObjectPtr xpathObjSearch = xmlXPathEvalExpression((xmlChar*)WStrToMBCStr(xpath).c_str(), xpathCtx);
	NodeList res = xpathObjSearch->nodesetval;
	if (res != nullptr)
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
			if (node != nullptr)
				retval.push_back(node);
		}
	}
#endif
	return retval;
}

ROMNode* ROMNode::_findObjectGUID(const string& guid)
{
	ROMNode* retval = nullptr;
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
			if (retval != nullptr)
				break;
		}
	}
	return retval;
}

void ROMNode::_findObjects(const wstring& id, bool recurs, vector<ROMNode*>* res)
{
	for (vector<ROMNode*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		if (recurs && (*it)->m_id == id && res != nullptr)
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
		if (res != nullptr)
		{
			res->push_back(*it);
			if ((*it)->m_children.size() > 0)
				(*it)->_findAllChildObjects(res);
		}
	}
}

void ROMNode::SetTableDebugHandler(std::function<void(const wstring&)> debugger)
{
	if (m_KnowledgeBase)
		m_KnowledgeBase->DebugHandlerPtr = debugger;
}

wstring ROMNode::GetAttribute(const wstring& id, const wstring& name, bool immediate)
{
	wstring retval = L"";
	bool bFound = false;

	auto it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		auto itValue = it->second.find(name);
		if (itValue != it->second.end())
		{
			retval = itValue->second;
			bFound = true;
		}
	}

	if (!immediate && !bFound)
	{
		ROMNode *parent = GetParent();
		if (parent != nullptr)
		{
			retval = parent->GetAttribute(id, name, immediate);
		}
	}

	return retval;
}

bool ROMNode::GetAttributeExists(const wstring& id, const wstring& name)
{
	bool bFound = false;
	auto it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		auto itValue = it->second.find(name);
		if (itValue != it->second.end())
		{
			bFound = true;
		}
	}
	return bFound;
}

bool ROMNode::SetAttribute(const wstring& id, const wstring& name, const wstring& value)
{
	bool retval = true;

	m_attrs[id][name] = value;
	m_bChanged = retval;

	return retval;
}

bool ROMNode::RemoveAttribute(const wstring& id, const wstring& name)
{
	bool retval = false;

	auto it = m_attrs.find(id);
	if (it != m_attrs.end())
	{
		if (name.length() == 0 || name == L"value")
		{
			m_attrs.erase(it);
			retval = true;
		}
		else
		{
			auto itValue = it->second.find(name);
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

bool ROMNode::SetROMObjectValue(const wstring& name, const wstring& value)
{
	bool retval = true;

	wstring newName = FindAndReplace(name, L" ", L"_");
	m_nodeValues[newName] = value;
	m_bChanged = retval;

	return retval;
}

wstring ROMNode::GetROMObjectValue(const wstring& name)
{
	wstring retval = L"";

	auto it = m_nodeValues.find(name);
	if (it != m_nodeValues.end())
	{
		retval = it->second;
	}

	return retval;
}

bool ROMNode::RemoveROMObjectValue(const wstring& name)
{
	bool retval = false;

	auto it = m_nodeValues.find(name);
	if (it != m_nodeValues.end())
	{
		m_nodeValues.erase(it);
		retval = true;
	}
	m_bChanged = retval;

	return retval;
}

//rules
bool ROMNode::LoadRules(const wstring& knowledge_file)
{
	if (m_KnowledgeBase)
	{
		delete m_KnowledgeBase;
		m_KnowledgeBase = nullptr;
	}

	if (m_parent == nullptr) //only the root will have the reference to the rules
	{
		m_KnowledgeBase = new EDS::CKnowledgeBase();
		bool retval = m_KnowledgeBase->CreateKnowledgeBase(knowledge_file);
		m_KnowledgeBase->InputValueGetterPtr = [=](const wstring& attrName, void* obj)
		{
			return ((ROMNode*)obj)->GetATableInputValue(attrName);
		};
		return retval;
	}
	else
		return false;
}

bool ROMNode::LoadRulesFromString(const wstring& xmlStr)
{
	if (m_KnowledgeBase)
	{
		delete m_KnowledgeBase;
		m_KnowledgeBase = nullptr;
	}

	if (m_parent == nullptr) //only the root will have the reference to the rules
	{
		m_KnowledgeBase = new EDS::CKnowledgeBase();
		bool retval = m_KnowledgeBase->CreateKnowledgeBaseFromString(xmlStr);
		m_KnowledgeBase->InputValueGetterPtr = [&](const wstring& attrName, void* obj)
		{
			return ((ROMNode*)obj)->GetATableInputValue(attrName);
		};
		return retval;
	}
	else
		return false;
}

vector<wstring> ROMNode::EvaluateTable(const wstring& evalTable, const wstring& output, bool bGetAll)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTable(evalTable, output, bGetAll, this);
	}
	return retval;
}

vector<wstring> ROMNode::EvaluateTable(const wstring& evalTable, const wstring& output)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTable(evalTable, output, this);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTable(const wstring& evalTable, bool bGetAll)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTable(evalTable, bGetAll, this);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTable(const wstring& evalTable)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTable(evalTable, this);
	}
	return retval;
}

vector<wstring> ROMNode::EvaluateTableWithParam(const wstring& evalTable, const wstring& output, bool bGetAll, wstring& param)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTableWithParam(evalTable, output, bGetAll, param, this);
	}
	return retval;
}

vector<wstring> ROMNode::EvaluateTableWithParam(const wstring& evalTable, const wstring& output, wstring& param)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTableWithParam(evalTable, output, param, this);
	}
	return retval;
}

vector<string> ROMNode::EvaluateTableWithParam(const string& evalTable, const string& output, bool bGetAll, string& param)
{
    wstring wsParam = MBCStrToWStr(param);
    vector<string> retval = ROMUTIL::WStrToMBCStrVector(EvaluateTableWithParam(MBCStrToWStr(evalTable), MBCStrToWStr(output), bGetAll, wsParam));
    param =ROMUTIL::WStrToMBCStr(wsParam);
    return retval;
}

vector<string> ROMNode::EvaluateTableWithParam(const string& evalTable, const string& output, string& param)
{
    wstring wsParam = MBCStrToWStr(param);
    vector<string> retval = ROMUTIL::WStrToMBCStrVector(EvaluateTableWithParam(MBCStrToWStr(evalTable), MBCStrToWStr(output), wsParam));
    param =ROMUTIL::WStrToMBCStr(wsParam);
    return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTableWithParam(const wstring& evalTable, bool bGetAll, wstring& param)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTableWithParam(evalTable, bGetAll, param, this);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::EvaluateTableWithParam(const wstring& evalTable, wstring& param)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->EvaluateTableWithParam(evalTable, param, this);
	}
	return retval;
}

map<string, vector<string> > ROMNode::EvaluateTableWithParam(const string& evalTable, bool bGetAll, string& param)
{
    wstring wsParam = ROMUTIL::MBCStrToWStr(param);
    map<string, vector<string> > retval = ROMUTIL::WStrToMBCStrMapVector(EvaluateTableWithParam(ROMUTIL::MBCStrToWStr(evalTable), bGetAll, wsParam));
    param =ROMUTIL::WStrToMBCStr(wsParam);
    return retval;
}

map<string, vector<string> > ROMNode::EvaluateTableWithParam(const string& evalTable, string& param)
{
    wstring wsParam = ROMUTIL::MBCStrToWStr(param);
    map<string, vector<string> > retval = ROMUTIL::WStrToMBCStrMapVector(EvaluateTableWithParam(ROMUTIL::MBCStrToWStr(evalTable), wsParam));
    param =ROMUTIL::WStrToMBCStr(wsParam);
    return retval;
}

wstring ROMNode::GetFirstTableResult(const wstring& tableName, const wstring& output)
{
	wstring retval = L"";
	vector<wstring> res = EvaluateTable(tableName, output, this);
	if (res.size() > 0)
		retval = res[0];
	return retval;
}

vector<wstring> ROMNode::ReverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr, bool bGetAll)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->ReverseEvaluateTable(evalTable, inputAttr, bGetAll, this);
	}
	return retval;
}

vector<wstring> ROMNode::ReverseEvaluateTable(const wstring& evalTable, const wstring& inputAttr)
{
	vector<wstring> retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->ReverseEvaluateTable(evalTable, inputAttr, this);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::ReverseEvaluateTable(const wstring& evalTable, bool bGetAll)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->ReverseEvaluateTable(evalTable, bGetAll, this);
	}
	return retval;
}

map<wstring, vector<wstring> > ROMNode::ReverseEvaluateTable(const wstring& evalTable)
{
	map<wstring, vector<wstring> > retval;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		retval = knowledge->ReverseEvaluateTable(evalTable, this);
	}
	return retval;
}

vector<wstring> ROMNode::GetPossibleValues(const wstring& evalTable, const wstring& outputName)
{
	vector<wstring> outputs;
	EDS::CKnowledgeBase *knowledge = _getKnowledge();
	if (knowledge)
	{
		outputs = knowledge->GetAllPossibleOutputs(evalTable, outputName);
	}
	return outputs;
}

wstring ROMNode::GetATableInputValue(const wstring& input)
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
void ROMNode::_createXMLDoc(bool bForceLoad, bool prettyprint)
{
	bool bChanged = (bForceLoad || _anyHasChanged());
	if (bChanged)
	{
		wstring genXML = _generateXML(bChanged, prettyprint);
#ifdef USE_MSXML
		if (m_xmlDoc != nullptr)
			m_xmlDoc.Release();
		m_xmlDoc = _createMSXMLDoc();
		VARIANT_BOOL res = m_xmlDoc->loadXML(genXML.c_str()); //-1 is true
#endif
#ifdef USE_LIBXML
		m_xmlDoc = nullptr;
		string buff = WStrToMBCStr(genXML);
		m_xmlDoc = xmlParseMemory(buff.c_str(), (int)buff.size());
#endif
		_setAllUnchanged();
	}
}

#ifdef USE_MSXML
Document ROMNode::_createMSXMLDoc()
{
	Document doc;
	doc = nullptr;
	#ifdef USEATL
		HRESULT hr = doc.CoCreateInstance(L"MSXML2.DOMDocument.6.0");
	#else
		HRESULT hr = doc.CreateInstance(L"MSXML2.DOMDocument.6.0");
	#endif

	#ifdef USEATL
		hr = (hr == S_OK) ? hr : doc.CoCreateInstance(L"MSXML2.DOMDocument.5.0");
	#else
		hr = (hr == S_OK) ? hr : doc.CreateInstance(L"MSXML2.DOMDocument.5.0");
	#endif

	#ifdef USEATL
		hr = (hr == S_OK) ? hr : doc.CoCreateInstance(L"MSXML2.DOMDocument.4.0");
	#else
		hr = (hr == S_OK) ? hr : doc.CreateInstance(L"MSXML2.DOMDocument.4.0");
	#endif

	#ifdef USEATL
		hr = (hr == S_OK) ? hr : doc.CoCreateInstance(L"MSXML2.DOMDocument.3.0");
	#else
		hr = (hr == S_OK) ? hr : doc.CreateInstance(L"MSXML2.DOMDocument.3.0");
	#endif

	if (hr == S_OK)
	{
		doc->async = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;
		doc->setProperty("SelectionLanguage", "XPath");
		doc->setProperty("SelectionNamespaces", "");
	}

	#ifdef USEATL
		hr = (hr == S_OK) ? hr : doc.CoCreateInstance(L"MSXML2.DOMDocument.2.6");
	#else
		hr = (hr == S_OK) ? hr : doc.CreateInstance(L"MSXML2.DOMDocument.2.6");
	#endif

	#ifdef USEATL
		hr = (hr == S_OK) ? hr : doc.CoCreateInstance(L"MSXML2.DOMDocument");
	#else
		hr = (hr == S_OK) ? hr : doc.CreateInstance(L"MSXML2.DOMDocument");
	#endif

	return doc;
}
#endif

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

wstring ROMNode::_generateXML(bool bRegen, bool prettyprint)
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
		for (auto itObjValue = m_nodeValues.begin(); itObjValue != m_nodeValues.end(); itObjValue++)
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
			if (prettyprint)
			{
				vector<wstring> attrList;
				std::transform(m_attrs.begin(), m_attrs.end(), std::back_inserter(attrList), [](const std::unordered_map<wstring, std::unordered_map<wstring, wstring> >::value_type &pair)
					{ return pair.first; });
				std::sort(begin(attrList), end(attrList));
				for (auto& id : attrList)
				{
					wstring attrObject = _generateAttrNode(id);
					allAttrs += attrObject;
				}
			}
			else
			{
				for (auto it = m_attrs.begin(); it != m_attrs.end(); it++)
				{
					wstring attrObject = _generateAttrNode(it->first);
					allAttrs += attrObject;
				}
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
			retval += node->_generateXML(node->m_bChanged, false);
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

wstring ROMNode::_generateAttrNode(const wstring& id)
{
	wstring attrObject = L"<Attribute id=\"";
	attrObject += id;
	attrObject += L"\" ";
	auto mapAttrs = m_attrs[id];
	for (auto itValue = mapAttrs.begin(); itValue != mapAttrs.end(); itValue++)
	{
		attrObject += itValue->first;
		attrObject += L"=\"";
		attrObject += ROMUTIL::encodeForXml(itValue->second);
		attrObject += L"\" ";
	}
	attrObject += L"/>";
	return attrObject;
}

wstring ROMNode::SaveXML(bool prettyprint)
{
	_createXMLDoc(true, prettyprint);
	return _convertXMLDocToString(prettyprint, m_xmlDoc);
}

ROMNode* ROMNode::LoadXML(const wstring& xmlStr, ObjectFactory factory)
{
	ROMNode* retval = nullptr;

	if (factory == nullptr)
	{
		factory = [](const wstring& id){
			return new ROMNode(id);
		};
	}

#ifdef USE_MSXML
	Document xmlDoc = _createMSXMLDoc();

	try
	{
		VARIANT_BOOL ok = xmlDoc->loadXML(_bstr_t(xmlStr.c_str()));
		if (ok == VARIANT_TRUE)
		{
			Node objectNode = xmlDoc->selectSingleNode("Object");
			retval = _buildObject(objectNode, factory);
		}
		else
		{
			if (xmlDoc != nullptr && xmlDoc->parseError->errorCode != 0)
				ReportROMError(ToASCIIString((wstring)(xmlDoc->parseError->reason)));
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
		string buff = WStrToMBCStr(xmlStr);
		Document xmlDoc = xmlParseMemory(buff.c_str(), (int)buff.size());

		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
		xmlChar* objXPath = (xmlChar*)"Object";
		xmlXPathObjectPtr xpathObjs = xmlXPathEvalExpression(objXPath, xpathCtx);
		NodeList allObjs = xpathObjs->nodesetval;
		if (allObjs != nullptr)
		{
			Node objectNode = allObjs->nodeTab[0];
			retval = _buildObject(objectNode, factory);
		}
	}
	catch(...)
	{
		ReportROMError("Problem parsing XML");
	}


#endif

	return retval;
}

ROMNode* ROMNode::_buildObject(Node objectNode, ObjectFactory factory)
{
	//create object
	ROMNode* newNode = nullptr;

#ifdef USE_MSXML
	wstring id = objectNode->attributes->getNamedItem("id")->nodeValue.bstrVal;
	wstring guid = objectNode->attributes->getNamedItem("guid")->nodeValue.bstrVal;

	newNode = factory(id);
	newNode->m_guid = ToASCIIString(guid);	

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
		ROMNode *newChildObject = _buildObject(childNode, factory);
		if (newChildObject != nullptr && newNode != nullptr)
		{
			newNode->AddChildROMObject(newChildObject);
		}
	}
#endif

#ifdef USE_LIBXML
	wstring id = XMLStrToWStr(xmlGetProp(objectNode, (xmlChar*)"id"));
	wstring guid = XMLStrToWStr(xmlGetProp(objectNode, (xmlChar*)"guid"));
	if (parent == nullptr)
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
	for (Attribute objValue = objectNode->properties; objValue != nullptr; objValue = objValue->next)
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
	if (allAttrs != nullptr)
	{
		for (int i = 0; i < allAttrs->nodeNr; i++)
		{
			Node attrNode = allAttrs->nodeTab[i];
			wstring idAttr = XMLStrToWStr(xmlGetProp(attrNode, (xmlChar*)"id"));
			for (Attribute attr = attrNode->properties; attr != nullptr; attr = attr->next)
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
	if (allObjs != nullptr)
	{
		for (int i = 0; i < allObjs->nodeNr; i++)
		{
			Node childNode = allObjs->nodeTab[i];
			ROMNode *newChildObject = _buildObject(childNode, factory);
			if (newChildObject != nullptr && newNode != nullptr)
			{
				newNode->AddChildROMObject(newChildObject);
			}
		}
	}

#endif
#ifdef _DEBUG
	newNode->m_bChanged = true;
	newNode->_createXMLDoc(false, false);
	string str;
	wstring xml = _convertXMLDocToString(true, newNode->m_xmlDoc);
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

wstring	ROMNode::EvaluateXPATH(const wstring& xpath, const string& guid)
{
	wstring retval;
	wstring match = L"<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'";
	match += ROMUTIL::MBCStrToWStr(guid) + L"\']\"><xsl:value-of select=\"";
	wstring xslt_text = XSLT_TOP + match + xpath + XSLT_BOTTOM;

	_createXMLDoc(false, false);
	if (m_xmlDoc != nullptr)
	{
#ifdef USE_MSXML
		Document xsltDoc =_createMSXMLDoc();
		xsltDoc->loadXML(xslt_text.c_str());
		_bstr_t* valPtr = nullptr;
		_bstr_t val = m_xmlDoc->transformNode(xsltDoc);
		valPtr = &val;
		retval = BSTR_T_ToWString(valPtr);
		xsltDoc.Release();
#endif
#ifdef USE_LIBXML
		string buff = WStrToMBCStr(xslt_text);
		Document xsltDoc = xmlParseMemory(buff.c_str(), (int)buff.length());

		xsltStylesheetPtr xsl = xsltParseStylesheetDoc(xsltDoc);
		Document result = xsltApplyStylesheet(xsl, m_xmlDoc, nullptr);

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

wstring ROMNode::_convertXMLDocToString(bool prettyprint, Document xmlDoc)
{
	wstring retval;
	retval.reserve(BIGSTRING);

	if (xmlDoc != nullptr)
	{
#ifdef USE_MSXML
		//By default it is writing the encoding = UTF-16. Change the encoding to UTF-8
		// <?xml version="1.0" encoding="UTF-8"?>
		MSXML2::IXMLDOMNodePtr pXMLFirstChild = xmlDoc->GetfirstChild();
		MSXML2::IXMLDOMNodePtr pXMLEncodNode = nullptr;
		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
		MSXML2::IXMLDOMNamedNodeMapPtr pXMLAttributeMap =  nullptr;

		if (pXMLFirstChild != nullptr)
		{
			pXMLAttributeMap = pXMLFirstChild->Getattributes();
			pXMLAttributeMap->getNamedItem(bstr_t("encoding"));
		}
		if (pXMLEncodNode != nullptr)
			pXMLEncodNode->PutnodeValue(bstr_t("utf-8")); //encoding = UTF-8. Serializer usually omits it in output, it is the default encoding
		else
		{
			MSXML2::IXMLDOMElementPtr pXMLRootElem = xmlDoc->GetdocumentElement();
			if (pXMLRootElem != nullptr)
			{
				MSXML2::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
					xmlDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");
				_variant_t vtObject;
				vtObject.vt = VT_DISPATCH;
				vtObject.pdispVal = pXMLRootElem;
				vtObject.pdispVal->AddRef();

				xmlDoc->insertBefore(pXMLProcessingNode, vtObject);
			}
		}

		MSXML2::IMXWriterPtr pWriter = nullptr;
		pWriter.CreateInstance("MSXML2.MXXMLWriter");

		MSXML2::ISAXXMLReaderPtr pReader = nullptr;
		pReader.CreateInstance("MSXML2.SAXXMLReader");

		MSXML2::ISAXContentHandlerPtr handler = pWriter;

		if (pWriter != nullptr && pReader != nullptr)
		{
			if (prettyprint)
				pWriter->put_indent(VARIANT_TRUE);

			pReader->putContentHandler(handler);
			pReader->parse(xmlDoc.GetInterfacePtr());

			//utf-16 here
			retval = (wstring)pWriter->output.bstrVal;
		}
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
	EDS::CKnowledgeBase *knowledge = nullptr;
	ROMNode* current = this;
	if (current->m_KnowledgeBase != nullptr)
		knowledge = m_KnowledgeBase;
	else
	{
		while (knowledge == nullptr)
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
				return nullptr;
		}
	}
	return knowledge;
}
