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

#include "stdafx.h"
#include "ROM2NET.h"
#include "Marshal.h"


ROM2NET::ROMNode^ ROM2NET::ROMNodeList::GetItem(Int64 Idx)
{
	System::Xml::XmlNode^ xmlNode = m_List->Item(Idx);
	ROMNode^ retval = gcnew ROMNode(xmlNode);	
	return retval;
}

long ROM2NET::ROMNodeList::Count()
{	
	long retval = m_List->Count;
	return retval;
}

void ROM2NET::ROMNodeList::SetNodeList(System::Xml::XmlNodeList^ list)
{
	m_List = list;
}

System::Xml::XmlNodeList^ ROM2NET::ROMNodeList::GetNodeList()
{
	return m_List;
}

void ROM2NET::ROMNode::SetNode(System::Xml::XmlNode^ node)
{
	m_Node = node;
}

System::Xml::XmlNode^ ROM2NET::ROMNode::GetNode()
{
	return m_Node;
}

//ROMTreeNET methods
ROM2NET::ROMTreeNET::ROMTreeNET(System::String^ name)
{
	m_DOC = gcnew System::Xml::XmlDataDocument();
	System::Xml::XmlProcessingInstruction^ pI = m_DOC->CreateProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
	System::Xml::XmlNode^ rootNode = m_DOC->CreateNode(System::Xml::XmlNodeType::Element, "", OBJECT_NODE, "");
	m_DOC->AppendChild(rootNode);
	m_DOC->InsertBefore(pI, m_DOC->DocumentElement);
	m_DOM = m_DOC->DocumentElement;
	m_DOM->SetAttribute("id", name);
	m_DOM->SetAttribute("guid", System::Guid::NewGuid().ToString());
}

ROM2NET::ROMNode^ ROM2NET::ROMTreeNET::GetRoot()
{
	ROMNode^ retval = gcnew ROMNode(m_DOM);
	return retval;
}

ROM2NET::ROMNode^ ROM2NET::ROMTreeNET::Parent(ROMNode^ current)
{
	if (current == nullptr)
		return nullptr;
	ROMNode^ retval = gcnew ROMNode(current->GetNode()->ParentNode);
	return retval;
}

System::Collections::Generic::List<ROM2NET::ROMNode^>^ ROM2NET::ROMTreeNET::Find(ROMNode^ current, System::String^ searchStr)
{
	if (current == nullptr)
		return nullptr;
	System::Xml::XmlNodeList^ list = current->GetNode()->SelectNodes(searchStr);
	System::Collections::Generic::List<ROM2NET::ROMNode^>^ retval = gcnew System::Collections::Generic::List<ROM2NET::ROMNode^>();
	for (size_t i = 0; i < list->Count; i++)
		retval->Add(gcnew ROM2NET::ROMNode(list->Item(i)));
	return retval;
}

ROM2NET::ROMNode^ ROM2NET::ROMTreeNET::CreateROMObject(System::String^ name)
{
	System::Xml::XmlElement^ newEle = (System::Xml::XmlElement^)m_DOC->CreateNode(System::Xml::XmlNodeType::Element, "", OBJECT_NODE, "");
	newEle->SetAttribute("id", name);
	newEle->SetAttribute("guid", System::Guid::NewGuid().ToString());
	ROMNode^ retval = gcnew ROMNode(newEle);
	return retval;
}

ROM2NET::ROMNode^ ROM2NET::ROMTreeNET::AddChildROMObject(ROMNode^ current, ROMNode^ child)
{	
	if (current == nullptr)
		return nullptr;
	
	current->GetNode()->AppendChild(child->GetNode());
	ROMNode^ retval = current;
	return retval;
}

//attribute interface
//<current><Attribute id = "some_id" value = "val" some_other_name = "something"/></current>
System::String^ ROM2NET::ROMTreeNET::GetAttribute(ROM2NET::ROMNode ^currentObject, System::String ^id, System::String ^name, System::Boolean ^recurs)
{
	System::String^ retval = gcnew System::String("");

	if (currentObject == nullptr)
		return retval;

	if (currentObject->GetNode()->HasChildNodes)
	{
		System::Xml::XmlNodeList^ nodes = currentObject->GetNode()->ChildNodes;
		for (unsigned long i = 0; i < nodes->Count; i++)
		{
			System::Xml::XmlNode^ attrNode = nodes->Item(i);
			if (attrNode->Name == ATTRIBUTE_NODE)
			{		
				System::Xml::XmlAttributeCollection^ attrs = attrNode->Attributes;
				System::Xml::XmlNode^ data = attrs->GetNamedItem("id");

				if (data != nullptr && data->Value == id)
				{ //got it
					if (name->Length > 0) //getting a sub attr
					{
						System::Xml::XmlNode^ attr_data = attrs->GetNamedItem(name);
						if (attr_data != nullptr)
						{
							retval = attr_data->Value;	
							break;
						}
					}
					else //getting the main value attr
					{
						System::Xml::XmlNode^ attr_data = attrs->GetNamedItem("value");
						if (attr_data != nullptr)
						{
							retval = attr_data->Value;	
							break;
						}
					}
				}
			}
		}
	}

	if (recurs && retval->Length == 0)
	{		
		System::Xml::XmlElement^ curEle = (System::Xml::XmlElement^)currentObject->GetNode();
		System::Xml::XmlElement^ rootEle = (System::Xml::XmlElement^)this->GetRoot()->GetNode();
		if (curEle->GetAttribute("guid") != rootEle->GetAttribute("guid"))
		//recursively serach
		{
			System::Xml::XmlNode^ parent = currentObject->GetNode()->ParentNode;
			ROM2NET::ROMNode^ parentNode = gcnew ROM2NET::ROMNode(parent);
			retval = GetAttribute(parentNode, id, name, true);
		}
	}	

	return retval;
}

System::String^ ROM2NET::ROMTreeNET::GetAttribute(ROM2NET::ROMNode ^currentObject, System::String ^id)
{
	return ROM2NET::ROMTreeNET::GetAttribute(currentObject, id, "value", true);
}

bool ROM2NET::ROMTreeNET::SetAttribute(ROM2NET::ROMNode ^currentObject, System::String ^id, System::String ^name, System::String ^value)
{
	bool retval = false;

	if (currentObject == nullptr)
		return retval;

	//if already exists, just change the value
	if (currentObject->GetNode()->HasChildNodes)
	{
		System::Xml::XmlNodeList^ nodes = currentObject->GetNode()->ChildNodes;
		for (unsigned long i = 0; i < nodes->Count; i++)
		{
			System::Xml::XmlNode^ attrNode = nodes->Item(i);
			if (attrNode->Name == ATTRIBUTE_NODE)
			{		
				System::Xml::XmlAttributeCollection^ attrs = attrNode->Attributes;
				if (attrs != nullptr && attrs->Count > 0)
				{
					System::Xml::XmlNode^ data = attrs->GetNamedItem("id");
					if (data != nullptr && data->Value == id)
					{
						if (name->Length == 0)
						{ //primary attr value
							System::Xml::XmlElement^ elem = (System::Xml::XmlElement^)attrNode;
							elem->SetAttribute("value", value);
							retval = true;
							break;
						}
						else if (name->Length > 0)
						{ //set another attr
							System::Xml::XmlElement^ elem = (System::Xml::XmlElement^)attrNode;
							elem->SetAttribute(name, value);
							retval = true;
							break;
						}
					}
				}				
			}
		}
	}

	if (!retval && name == "value")
	{
	  //create new main attribute
		System::Xml::XmlElement^ elem = m_DOC->CreateElement("", ATTRIBUTE_NODE, "");
		elem->SetAttribute("id", id);
		elem->SetAttribute("value", value);
		currentObject->GetNode()->AppendChild(elem);
		retval = true;
	}

	return retval;
}

bool ROM2NET::ROMTreeNET::SetAttribute(ROM2NET::ROMNode^ currentObject, System::String ^id, System::String ^value)
{
	return ROM2NET::ROMTreeNET::SetAttribute(currentObject, id, "value", value);
}

bool ROM2NET::ROMTreeNET::SetROMObjectValue(ROM2NET::ROMNode ^currentObject, System::String ^name, System::String ^value)
{
	short retval = false;

	if (currentObject == nullptr)
		return retval;

	System::Xml::XmlElement^ elem = (System::Xml::XmlElement^)currentObject->GetNode();
	elem->SetAttribute(name, value);
	
	retval = true;
}

System::String^ ROM2NET::ROMTreeNET::GetROMObjectValue(ROM2NET::ROMNode ^currentObject, System::String ^name)
{
	System::String^ retval = gcnew System::String("");

	if (currentObject == nullptr)
		return retval;

	System::Xml::XmlNode^ currentNode = currentObject->GetNode();
	if (currentNode->Attributes->Count > 0)
	{
		System::Xml::XmlNode^ attrNode = currentNode->Attributes->GetNamedItem(name);
		if (attrNode != nullptr)
		{
			retval = attrNode->Value;
		}
	}

	return retval;
}

bool ROM2NET::ROMTreeNET::RemoveAttribute(ROM2NET::ROMNode ^currentObject, System::String ^id)
{
	bool retval = false;	

	if (currentObject == nullptr)
		return retval;

	if (currentObject->GetNode()->HasChildNodes)
	{
		System::Xml::XmlNodeList^ nodes = currentObject->GetNode()->ChildNodes;
		for (unsigned long i = 0; i < nodes->Count; i++)
		{
			System::Xml::XmlNode^ attrNode = nodes->Item(i);
			if (attrNode->Name == ATTRIBUTE_NODE)
			{		
				System::Xml::XmlAttributeCollection^ attrs = attrNode->Attributes;
				System::Xml::XmlNode^ data = attrs->GetNamedItem(id);

				if (data != nullptr)
				{ //got it
					currentObject->GetNode()->RemoveChild(attrNode);
					retval = true;
					break;
				}
			}
		}
	}	
	return retval;
}

System::String^ ROM2NET::ROMTreeNET::GetROMObjectName(ROM2NET::ROMNode ^currentObject)
{
	System::String^ retval = gcnew System::String("");

	if (currentObject == nullptr)
		return retval;

	System::Xml::XmlAttributeCollection^ attrs = currentObject->GetNode()->Attributes;
	System::Xml::XmlNode^ data = attrs->GetNamedItem("id");

	if (data != nullptr)
		retval = data->Value;

	return retval;
}

void ROM2NET::ROMTreeNET::SetROMObjectName(ROM2NET::ROMNode ^currentObject, System::String^ name)
{
	if (currentObject == nullptr)
		return;

	System::Xml::XmlAttributeCollection^ attrs = currentObject->GetNode()->Attributes;
	System::Xml::XmlNode^ data = attrs->GetNamedItem("id");

	if (data != nullptr)
		data->Value = name;
}

bool ROM2NET::ROMTreeNET::DestoryROMObject(ROMNode^ current)
{
	if (current == nullptr)
		return false;
	System::Xml::XmlNode^ parent = current->GetNode()->ParentNode;
	parent->RemoveChild(current->GetNode());
	return true;
}

//Rules
bool ROM2NET::ROMTreeNET::LoadRules(System::String ^knowledge_file)
{
	wstring file;
	MarshalString(knowledge_file, file);
	m_KnowledgeBase = new EDS::CKnowledgeBase(file);
	if (m_KnowledgeBase)
		return true;
	else
		return false;
}

System::Collections::Generic::List<System::String^>^ ROM2NET::ROMTreeNET::EvaluateTable(ROM2NET::ROMNode ^currentObject, System::String ^evalTable, System::String ^output, bool bGetAll)
{
	wstring table, out;
	MarshalString(evalTable, table);
	MarshalString(output, out);
	LoadInputs(currentObject, evalTable);
	vector<wstring> res = m_KnowledgeBase->EvaluateTable(table, out, bGetAll);
	System::Collections::Generic::List<System::String^>^ retval = gcnew System::Collections::Generic::List<System::String^>(res.size());
	for (vector<wstring>::iterator it = res.begin(); it != res.end(); it++)
	{
		System::String^ str = gcnew System::String((*it).c_str());
		retval->Add(str);
	}
	return retval;
}

System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<System::String^>^>^ ROM2NET::ROMTreeNET::EvaluateTable(ROM2NET::ROMNode ^currentObject, System::String ^evalTable, bool bGetAll)
{
	LoadInputs(currentObject, evalTable);
	wstring table;
	MarshalString(evalTable, table);
	map<wstring, vector<wstring>> res = m_KnowledgeBase->EvaluateTable(table, bGetAll);
	System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<System::String^>^>^ retval = gcnew System::Collections::Generic::Dictionary<System::String^, System::Collections::Generic::List<System::String^>^>();
	for (map<wstring, vector<wstring>>::iterator it = res.begin(); it != res.end(); it++)
	{
		pair<wstring, vector<wstring>> kvp = *it;
		System::String^ key = gcnew System::String(kvp.first.c_str());
		System::Collections::Generic::List<System::String^>^ value;
		if (!retval->ContainsKey(key))
		{
			value = gcnew System::Collections::Generic::List<System::String^>(kvp.second.size());
		}
		else
			value = retval[key];

		for (vector<wstring>::iterator valIt = kvp.second.begin(); valIt != kvp.second.end(); valIt++)
		{
			System::String^ curStr = gcnew System::String((*valIt).c_str());
			value->Add(curStr);
		}		
	}
	return retval;
}

void ROM2NET::ROMTreeNET::LoadInputs(ROMNode^ currentObject, System::String^ evalTable)
{
	wstring table;
	MarshalString(evalTable, table);
	vector<wstring> inputs = m_KnowledgeBase->GetInputDependencies(table);
	for (vector<wstring>::iterator it = inputs.begin(); it != inputs.end(); it++)
	{
		System::String^ curStr = gcnew System::String((*it).c_str());
		System::String^ value = GetATableInputValue(currentObject, curStr);
		wstring sVal;
		MarshalString(value, sVal);
		m_KnowledgeBase->SetInputValue(*it, sVal);
	}
}

System::String^ ROM2NET::ROMTreeNET::EvaluateXPATH(ROM2NET::ROMNode ^currentObject, System::String ^xpath)
{
	System::String^ retval = gcnew System::String("");
	System::Xml::XmlNode^ guidNode = currentObject->GetNode()->Attributes->GetNamedItem("guid");
	System::String^ match = gcnew System::String("<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'");
	match += guidNode->Value + "\']\"><xsl:value-of select=\"";
	System::String^ xslt_text = gcnew System::String(XSLT_TOP + match + xpath + XSLT_BOTTOM);

	System::Xml::Xsl::XslTransform^ xslt = gcnew System::Xml::Xsl::XslTransform();
	System::IO::StringReader^ sr = gcnew System::IO::StringReader(xslt_text);
	System::Xml::XmlTextReader^ xslReader = gcnew System::Xml::XmlTextReader(sr);
	xslt->Load(xslReader);
	System::IO::StringWriter^ sw = gcnew System::IO::StringWriter();
	System::Xml::XmlTextWriter^ myWriter = gcnew System::Xml::XmlTextWriter(sw) ;
	xslt->Transform(m_DOC, nullptr, myWriter);
	retval = sw->ToString();

	myWriter->Close();
	xslReader->Close();
	return retval;
}

System::String^ ROM2NET::ROMTreeNET::GetATableInputValue(ROMNode^ currentObject, System::String^ input)
{
	System::String^ retval = gcnew System::String("");
	//parse out any XPATH Queries here and return values
	if (input->Contains("xpath("))
	{
		System::String^ sCMD = input->Substring(6, input->Length - 7);
		retval = EvaluateXPATH(currentObject, input);
	}
	else
	{
		retval = GetAttribute(currentObject, input);
	}

	return retval;
}

//IO
bool ROM2NET::ROMTreeNET::LoadTree(System::String^ xmlStr)
{
	if (m_DOC != nullptr)
	{		
		m_DOC->LoadXml(xmlStr);
	}
	else
		return false;

	return true;
}

System::String^	ROM2NET::ROMTreeNET::DumpTree()
{
	System::String^ retval = gcnew String("");
	if (m_DOC != nullptr)
	{
		System::IO::StringWriter^ sr = gcnew System::IO::StringWriter();
		retval = m_DOC->InnerXml;
	}

	return retval;
}
