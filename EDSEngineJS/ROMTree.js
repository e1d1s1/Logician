/*
This file is part of the Relational Object Model 2 JS Library.
Copyright (C) 2009 Eric D. Schmidt

Relational Object Model 2 JS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Relational Object Model 2 JS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Relational Object Model 2 JS.  If not, see <http://www.gnu.org/licenses/>.
*/
function ReportError(err) {
    if (IsIE())
        alert("Error:\n" + err.description + "\n" + err.message + "\nline:" + err.number);
    //else if (IsMoz())
    //    alert("Error:\n" + err + " line:" + err.lineNumber);
    else 
        alert("Error:\n" + err);
}

function IsIE() {
    var ie = (typeof window.ActiveXObject != 'undefined');
    //alert("IE=" + ie);
    return ie;
}

function IsMoz() {
    var moz = (typeof document.implementation != 'undefined') && (typeof 
        document.implementation.createDocument != 'undefined');
    //alert("Moz=" + moz);
    return moz;
}


function MakeGUID() 
{
    try 
    {
//        if (IsIE()) //IE7 will prompt for permissions, annoying
//        {
//            var x = new ActiveXObject("Scriptlet.TypeLib");
//            return (x.GUID).substr(0, 38);
//        }
//        else 
        {
            var chars = '0123456789abcdef'.split('');

            var uuid = [], rnd = Math.random, r;
            uuid[8] = uuid[13] = uuid[18] = uuid[23] = '-';
            uuid[14] = '4'; // version 4

            for (var i = 0; i < 36; i++) {
                if (!uuid[i]) {
                    r = 0 | rnd() * 16;
                    uuid[i] = chars[(i == 19) ? (r & 0x3) | 0x8 : r & 0xf];
                }
            }

            return uuid.join('');
        }
    }
    catch (err) 
    {
        ReportError(err);
    }
}

var ATTRIBUTE_NODE = "Attribute";
var OBJECT_NODE  = "Object";
var XSLT_TOP = "<?xml version=\"1.0\"?><xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"><xsl:output method=\"text\" omit-xml-declaration=\"yes\" encoding=\"UTF-8\"/>"
var XSLT_BOTTOM = "\"/></xsl:for-each></xsl:template></xsl:stylesheet>"
//Main ROMTree class////////////////////////////////////////////////////////////////
function ROMTree(name) 
{
    this.xmlDoc = null;
    this.m_tree = null;
    this.m_KnowledgeBase = null;

    var guid = MakeGUID();
    name = name.replace(" ", "_");
    var text = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><Object id=\"" + name + "\" guid=\"" + guid + "\"/>";
    if (IsIE()) 
    {
        this.xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
        this.xmlDoc.async = "false";
        this.xmlDoc.setProperty("SelectionLanguage", "XPath");
        this.xmlDoc.loadXML(text);
    }
    else 
    {
        var parser = new DOMParser();
        this.xmlDoc = parser.parseFromString(text, "text/xml");
    }

    this.m_tree = this.xmlDoc.documentElement;
}

ROMTree.prototype.GetRoot = function()
{
    try
    {
        return this.m_tree;
    }
    catch (err)
    {
        ReportError(err);
    }
}

ROMTree.prototype.Parent = function(current)
{
    var retval = null;
    try
    {        
        if (current)
        {
            return current.parentNode;
        }        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.Find = function(current, searchStr)
{
    var retval = new Array();
    try
    {        
        if (!currrent)
        {
            return retval;
        }
        
        if (IsIE())
        {
            var nodes = currrent.selectNodes(searchStr);
            for (var i = 0; i < nodes.length; i++)
            {
                retval.push(nodes[i]);
            }
        }
        else
        {
            var nodes = this.xmlDoc.evaluate(searchStr, currrent, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
            for (var i = 0; i < nodes.snapshotLength; i++)
            {
                retval.push(nodes.snapshotItem(i));
            }
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.AddChildROMObject = function(current, child)
{
    try
    {
        current.appendChild(child);
        return child;
    }
    catch (err)
    {
        ReportError(err);
    }
}

ROMTree.prototype.CreateROMObject = function(name)
{
    var retval = null;
    try
    {        
        var guid = MakeGUID();
        
        if (!this.xmlDoc)
            return retval;
           
        var elem = this.xmlDoc.createElement(OBJECT_NODE);
        elem.setAttribute("id", name);
        elem.setAttribute("guid", guid);
        retval = elem;        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.DestroyROMObject = function(current)
{
    try
    {       
        if (!current)
            return false;
            
        var parent = current.parentNode;
        parent.removeChild(current);
        delete current;
        
        return true;
    }
    catch (err)
    {
        ReportError(err);
    }
}

ROMTree.prototype.GetAttributeValue = function(currentObject, id, recurs)
{
    try
    {
        return this.GetAttribute(currentObject, id, "value", recurs);
    }
    catch(err)
    {
        ReportError(err);
    }
}

ROMTree.prototype.GetAttribute = function(currentObject, id, name, recurs)
{
    var retval = "";
    try
    {        
        if (!currentObject)
            return retval;
            
        if (currentObject.hasChildNodes())
        {
            var nodes = currentObject.childNodes;
            for (var i = 0; i < nodes.length; i++)
            {
                var attrNode = nodes.item(i);
                if (attrNode.nodeName == ATTRIBUTE_NODE)
                {
                    var data = attrNode.attributes.getNamedItem("id");          
                    if (data != null && data.nodeValue == id)
                    {
                        if (name.length > 0)
                        {
                            var attr_data = attrNode.attributes.getNamedItem(name);
                            if (attr_data != null)
                            {
                                retval = attr_data.nodeValue;
                                break;
                            }
                        }
                        else
                        {
                            var attr_data = attrNode.attributes.getNamedItem("value");
                            if (attr_data != null)
                            {
                                retval = attr_data.nodeValue;
                                break;
                            }
                        }
                    }
                    
                }
            }
        }
        
        if (recurs && retval.length == 0)
        {
            var curEle = currentObject;
            var rootEle = this.GetRoot();
            if (curEle.getAttribute("guid") != rootEle.getAttribute("guid"))
            {   //recrsively search
                var parent = currentObject.parentNode;
                retval = this.GetAttribute(parent, id, name, true);
            }
        }        
        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.SetAttribute = function(currentObject, id, value)
{
    try
    {
        return this.SetAttributeValue(currentObject, id, "value", value);
    }
    catch (err)
    {
        ReportError(err);
    }
}

ROMTree.prototype.SetAttributeValue = function(currentObject, id, name, value)
{
    var retval = false;
    try
    {   
        if (!currentObject)
            return retval;            
        
        if (currentObject.hasChildNodes())
        {
            var nodes = currentObject.childNodes;
            for (var i = 0; i < nodes.length; i++)
            {
                var attrNode = nodes.item(i);
                if (attrNode.nodeName == ATTRIBUTE_NODE)
                {
                    var attrs = attrNode.attributes;
                    if (attrs)
                    {
                        var data = attrs.getNamedItem("id");
                        if (data != null && data.nodeValue == id)
                        {
                            var elem = attrNode;
                            if (name.length == 0)
                            {//primary attr value                                
                                elem.setAttribute("value", value);
                                retval = true;
                                break;
                            }
                            else
                            {//set another attr
                                elem.setAttribute(name, value);
                                retval = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        if (retval == false && name == "value")
        {   //create new main attribute
            var elem = this.xmlDoc.createElement(ATTRIBUTE_NODE);
            elem.setAttribute("id", id);
            elem.setAttribute("value", value);
            currentObject.appendChild(elem);
            retval = true;
        }       
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.SetROMObjectValue = function(currentObject, name, value)
{
    try
    {       
        if (!currentObject)
            return false;
            
        var elem = currentObject;
        elem.setAttribute(name, value);
        return true;
    }
    catch (err)
    {
        ReportError(err);
        return false;
    }
}

ROMTree.prototype.GetROMObjectValue = function(currentObject, name)
{
    var retval = "";
    try
    {        
        if (!currentObject)
            return retval;
        
        var elem = currentObject;
        retval = elem.getAttribute(name);        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.RemoveAttribute = function(currentObject, id)
{
    var retval = false;
    try
    {   
        if (!currentObject)
            return retval;
        
        if (currentObject.hasChildNodes())
        {
            var nodes = currentObject.childNodes;
            for (var i = 0; i < nodes.length; i++)
            {
                var attrNode = nodes.item(i);
                if (attrNode.nodeName == ATTRIBUTE_NODE)
                {
                    var attrs = attrNode.attributes;
                    var data = attrs.getNamedItem(id);                    
                    if (data)
                    {
                        currentObject.removeChild(attrNode);
                        retval = true;
                        break;
                    }
                }
            }
        }       
        
    }
    catch (err)
    {
        ReportError(err);
        return false;
    }
    return retval;
}

ROMTree.prototype.GetROMObjectName = function(current)
{
    var retval = "";
    try
    {        
        if (!current)
            return retval;
            
        var attrs = current.attributes;
        var data = attrs.getNamedItem("id");
        
        if (data)
            retval = data.nodeValue;
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.SetROMObjectName = function(current, name)
{
    try
    {
        var attrs = current.attributes;
        var data = attrs.getNamedItem("id");
        
        if (data)
            data.nodeValue = name;
    }
    catch(err)
    {
        ReportError(err);
    }
}

//rules
ROMTree.prototype.LoadRules = function(knowledge_file) 
{
    try 
    {
        this.m_KnowledgeBase = new KnowledgeBase(knowledge_file);
        if (this.m_KnowledgeBase != null)
            return true;
        else
            return false;
    }
    catch (err) 
    {
        ReportError(err);
    }
}

ROMTree.prototype.SetRulesDebugHandler = function(func)
{
    if (this.m_KnowledgeBase != null)
        this.m_KnowledgeBase.SetDebugHandler(func);
}

ROMTree.prototype.EvaluateTableForAttr = function(currentObject, evalTable, output, bGetAll)
{
    try
    {
        this.LoadInputs(currentObject, evalTable);
        var retval = this.m_KnowledgeBase.EvaluateTableForAttr(evalTable, output, bGetAll);
        return retval;
    }
    catch (err)
    {
        ReportError(err);
        return null;
    }
}

ROMTree.prototype.EvaluateTable = function(currentObject, evalTable, bGetAll)
{
    try
    {
        this.LoadInputs(currentObject, evalTable);
        var retval = this.m_KnowledgeBase.EvaluateTable(evalTable, bGetAll);   
        return retval;     
    }
    catch (err)
    {
        ReportError(err);
        return null;
    }
}

//IO
ROMTree.prototype.DumpTree = function()
{
    var retval = "";  
    try
    {              
        if (IsIE())
        {
            retval = this.xmlDoc.xml;
        }
        else
        {
            retval = (new XMLSerializer()).serializeToString(this.xmlDoc);
        }
    }
    catch (err)
    {
        ReportError(err);
        return "";
    }
    return retval;
}

ROMTree.prototype.LoadTree = function(xmlStr)
{
    try
    {
        if (IsIE())
        {
            var doc = new ActiveXObject("Microsoft.XMLDOM");
            this.xmlDoc.async = "false";
            this.xmlDoc.setProperty("SelectionLanguage", "XPath");
            this.xmlDoc.loadXML(xmlStr); 
        }
        else
        {
            var parser = new DOMParser();
            this.xmlDoc = parser.parseFromString(xmlStr, "text/xml");
        }
        
        return this.xmlDoc;
    }
    catch (err)
    {
        ReportError(err);
    }
}

ROMTree.prototype.LoadInputs = function(currentObject, evalTable)
{
    try
    {
        var inputs = this.m_KnowledgeBase.GetInputDependencies(evalTable);
        for (var i = 0; i < inputs.length; i++)
        {
            var value = this.GetATableInputValue(currentObject, inputs[i]);
            this.m_KnowledgeBase.SetInputValue(inputs[i], value);
        }
    }
    catch (err)
    {
        ReportError(err);
    }
}

ROMTree.prototype.EvaluateXPATH = function(currentObject, xpath)
{
    var retval = "";
    try
    {
        var guidNode = currentObject.attributes.getNamedItem("guid");
        var currentGUID = guidNode.nodeValue;
        var match = "<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'";
	    match += currentGUID + "\']\"><xsl:value-of select=\"";
	    var xslt_text = XSLT_TOP + match + xpath + XSLT_BOTTOM;
	    
	    var xsltDoc = null;            
        if (IsIE())
        {
            xsltDoc = new ActiveXObject("Microsoft.XMLDOM");
            xsltDoc.async = "false";
            xsltDoc.loadXML(xslt_text);  
            retval = this.xmlDoc.transformNode(xsltDoc);
        }
        else
        {
            var parser = new DOMParser();
            xsltDoc = parser.parseFromString(xslt_text, "text/xml");
            var xsltProcessor = new XSLTProcessor();
            xsltProcessor.importStylesheet(xsltDoc);
            var ownerDocument = document.implementation.createDocument("", "xsltDoc", null);
            var solnNode = xsltProcessor.transformToFragment(this.xmlDoc, ownerDocument);
            retval = solnNode.firstChild.textContent;
        }
    }
    catch(err)
    {
        ReportError(err);
    }
    return retval;
}

ROMTree.prototype.GetATableInputValue = function(currentObject, input)
{
    var retval = "";    
    try
    {        
        var xpathIndex = input.indexOf("xpath(");
        if (xpathIndex >= 0)
        {
            var cmdArg = input.substr(xpathIndex + 6, input.length - 7);
            retval = this.EvaluateXPATH(currentObject, cmdArg);
        }
        else
        {
            retval = this.GetAttributeValue(currentObject, input, true);
        }
    }
    catch (err)
    {
        ReportError(err);
        
    }
    return retval;
}