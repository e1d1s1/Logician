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
#ifdef ROM2

#define USE_XERCES
//#define USE_MSXML

#ifdef USE_MSXML
	#import "msxml6.dll" 
	using namespace MSXML2;
	typedef MSXML2::IXMLDOMAttributePtr Attribute;
	typedef MSXML2::IXMLDOMNodePtr Node;
	typedef MSXML2::IXMLDOMNodeListPtr NodeList;
	typedef MSXML2::IXMLDOMDocumentPtr Document;
	typedef MSXML2::IXMLDOMElementPtr Element;	
	typedef MSXML2::IXMLDOMNamedNodeMapPtr NamedNodeMap;
	typedef MSXML2::IXMLDOMProcessingInstructionPtr ProcessingInstruction;

	#define X(str) _bstr_t(str)
#endif

#ifdef USE_XERCES
	//For code compatibility some Xerces types are overloaded with some MSXML-like 
	//accessors/values
	#include <vector>
	using namespace std;

	#include<xercesc/util/PlatformUtils.hpp>

	#include<xercesc/dom/DOM.hpp>
	#include<xercesc/dom/DOMDocument.hpp>
	#include<xercesc/dom/DOMDocumentType.hpp>
	#include<xercesc/dom/DOMElement.hpp>
	#include<xercesc/dom/DOMImplementation.hpp>
	#include<xercesc/dom/DOMImplementationLS.hpp>
	#include<xercesc/dom/DOMNodeIterator.hpp>
	#include<xercesc/dom/DOMNodeList.hpp>
	#include<xercesc/dom/DOMText.hpp>

	#include<xercesc/parsers/XercesDOMParser.hpp>
	#include<xercesc/util/XMLUni.hpp>
	#include <xercesc/util/XMLString.hpp>
	#if defined(_MSC_VER)
		#pragma warning(disable:4786 4239)
		#include <xstring>
	#endif

	XERCES_CPP_NAMESPACE_USE

	class ROMNode;

	class ROMNodeList: public xercesc_3_0::DOMNodeList
	{
	public:
		unsigned long length;
		std::vector<ROMNode*> item;
		ROMNodeList()
		{
			length = this->getLength();
			for (unsigned long i = 0; i < length; i++)
			{
				item.push_back((ROMNode*)this->::xercesc_3_0::DOMNodeList::item(i));
			}
		}
	};
	typedef ROMNodeList* NodeList;

	class ROMNode: public xercesc_3_0::DOMNode
	{	
	public:
		xercesc_3_0::DOMNodeList* childNodes;
		xercesc_3_0::DOMNode* parentNode;
		xercesc_3_0::DOMNamedNodeMap* attributes;
		wstring	nodeName;
		wstring nodeValue;
		ROMNode()
		{
			childNodes = this->getChildNodes();
			parentNode = this->getParentNode();
			nodeName = this->getNodeName();
			attributes = this->getAttributes();
			nodeValue = this->getNodeValue();			
		}
		~ROMNode()
		{
			this->setNodeValue(nodeValue.c_str());
		}
	};	
	typedef ROMNode* Node;	

	class ROMDocument: public xercesc_3_0::DOMDocument
	{
	public:
		xercesc_3_0::DOMElement* documentElement;
		ROMDocument()
		{
			documentElement = this->getDocumentElement();
		}
	};
	typedef ROMDocument* Document;

	typedef xercesc_3_0::DOMAttr Attribute;	
	typedef xercesc_3_0::DOMElement* Element;	
	typedef xercesc_3_0::DOMNamedNodeMap* NamedNodeMap;
	typedef xercesc_3_0::DOMProcessingInstruction* ProcessingInstruction;

	

	// ---------------------------------------------------------------------------
	//  This is a simple class that lets us do easy (though not terribly efficient)
	//  trancoding of char* data to XMLCh data.
	// ---------------------------------------------------------------------------
	class XString
	{
		public :
			// -----------------------------------------------------------------------
			//  Constructors and Destructor
			// -----------------------------------------------------------------------
			XString(const char* const toTranscode)
			{
					// Call the private transcoding method
					fUnicodeForm = XMLString::transcode(toTranscode);
			}
			~XString()
			{
					XMLString::release(&fUnicodeForm);
			}
			// -----------------------------------------------------------------------
			//  Getter methods
			// -----------------------------------------------------------------------
			const XMLCh* unicodeForm() const
			{
					return fUnicodeForm;
			}
		private :
			// -----------------------------------------------------------------------
			//  Private data members
			//
			//  fUnicodeForm
			//      This is the Unicode XMLCh format of the string.
			// -----------------------------------------------------------------------
			XMLCh*   fUnicodeForm;
	};
	#define X(str) XString(str).unicodeForm()
#endif
#endif