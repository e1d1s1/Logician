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

/// reference path="KnowledgeBase.js"

//stupid IE doesnt do indexOf for array
function GetIndexOfItem(arr, obj)
{
    for(var i=0; i<arr.length; i++){
        if(arr[i]===obj){
            return i;
        }
    }
    return -1;   
}

function ReportError(err) {
    var vDebug = "";
    for (var prop in err)
    { 
       vDebug += "property: "+ prop+ " value: ["+ err[prop]+ "]\n";
    }
    vDebug += "toString(): " + " value: [" + err.toString() + "]";
    status.rawValue = vDebug; 
    alert(vDebug);
}

function IsIE() {
    var ie = (typeof window.ActiveXObject != 'undefined');
    //alert("IE=" + ie);
    return ie;
}

function IsMoz() {
    var moz = (navigator.userAgent.indexOf('Firefox') >= 0);
    //alert("Moz=" + moz);
    return moz;
}

function MakeGUID() 
{
    try 
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
    catch (err) 
    {
        ReportError(err);
    }
}

var ATTRIBUTE_NODE = "Attribute";
var OBJECT_NODE  = "Object";
var XSLT_TOP = "<?xml version=\"1.0\"?><xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"><xsl:output method=\"text\" omit-xml-declaration=\"yes\" encoding=\"UTF-8\"/>"
var XSLT_BOTTOM = "\"/></xsl:for-each></xsl:template></xsl:stylesheet>"
function ROMNode(id)
{
    this.m_xmlDoc = null;
    this.m_KnowledgeBase = null;
    id = id.replace(" ", "_");
    this.m_id = id;
    this.m_guid = MakeGUID();
    this.m_parent = null;
    this.m_children = new Array();
    this.m_bChanged = true;
    this.m_lastContents = "";
    this.m_lastAttrContents = "";
    this.m_attrs = new Array();
    this.m_nodeValues = new Array();
}

ROMNode.prototype.GetRoot = function()
{
    try
    {
        var nextParent = this;
	    do 
	    {
		    if (nextParent != NULL)
			    nextParent = nextParent.GetParent();		
	    } while (nextParent != NULL);
	    return nextParent;
    }
    catch (err) 
    {
        ReportError(err);
    }
    return null;
}

ROMNode.prototype.GetParent = function() 
{
    return this.m_parent;
}

ROMNode.prototype.AddChildROMObject = function(child) 
{
    try 
    {
        if (child.m_parent == null)
        {
            child.m_parent = this;
            this.m_children.push(child);
            return true;
        }
    }
    catch (err) 
    {
        ReportError(err);
    }
    return false;
}

ROMNode.prototype.RemoveChildROMObject = function(child) 
{
    var retval = false;
    try 
    {
        var i = GetIndexOfItem(this.m_children, child);
        if (i >= 0)
        {
            this.m_children.splice(i, 1);
            retval = true;
        }
        this.m_bChanged = retval;
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.DestroyROMObject = function() 
{
    var retval = false;
    try 
    {
        //remove any references to self in parent node
        if (this.m_parent != NULL)
	    {
		    retval = this.m_parent.RemoveChildROMObject(this);
	    }
	    
	    this.m_attrs.clear();
	    this.m_nodeValues.clear();
	    this.m_id = "";
	    this.m_parent = NULL;
	    this.m_bChanged = false;
	    //trigger downstream destructors
	    for (var i = this.m_children.length - 1; i >= 0; i--)
	    {
		    if (i < this.m_children.length)
		    {
			    var node = m_children[i];
			    if (node)
				    delete node;
		    }
	    }
	    this.m_children.clear();	
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.Clone = function()
{
    var newNode = null;
    try
    {
        newNode = new ROMNode(this.m_id);
        newNode.m_attrs = this.m_attrs.slice(0);
        newNode.m_nodeValues = this.m_nodeValues.slice(0);
        newNode.m_bChanged = false;
        for (var i = this.m_children.length - 1; i >= 0; i--)
        {
            if (i < this.m_children.length)
            {
                var node = this.m_children[i];
                if (node != null)
                {
                    var newChild = node.Clone();
                    newNode.AddChildROMObject(newChild);
                }
            }
        }
    }
    catch (err) 
    {
        ReportError(err);
    }
    return newNode;
}

ROMNode.prototype.GetAttributeValue = function(id, name, immediate) 
{
    var retval = "";
    try 
    {
        var bFound = false;
        if (this.m_attrs[id] != null)
        {
            if (this.m_attrs[id][name] != null)
            {
                retval = this.m_attrs[id][name];
                bFound = true;
            }
        }
        
        if (!immediate && !bFound)
        {
            if (this.m_parent != null)
            {
                retval = this.m_parent.GetAttributeValue(id, name, immediate);
            }
        }
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetAttribute = function(id, immediate) 
{
    var retval = "";
    try 
    {
        retval = this.GetAttributeValue(id, "value", immediate);
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetAttributeExists = function(id) 
{
    var retval = false;
    try 
    {
        retval = this.GetAttributeValueExists(id, "value");
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetAttributeValueExists = function(id, name) 
{
    var retval = false;
    try 
    {
        if (this.m_attrs[id] != null)
        {
            if (this.m_attrs[id][name] != null)
            {
                retval = true;
            }
        }
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.SetAttribute = function(id, value)
{
    try
    {
        return this.SetAttributeValue(id, "value", value);
    }
    catch (err)
    {
        ReportError(err);
    }
}

ROMNode.prototype.SetAttributeValue = function(id, name, value)
{

    try
    {   
        if (this.m_attrs[id] === undefined)
            this.m_attrs[id] = new Array();
        
        this.m_attrs[id][name] = value;
        this.m_bChanged = true;
    }
    catch (err)
    {
        ReportError(err);
        return false;
    }
    return true;
}

ROMNode.prototype.RemoveAttribute = function(id) 
{
    var retval = false;
    try 
    {
        retval = this.RemoveAttributeValue(id, "value");
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.RemoveAttributeValue = function(id, name) 
{
    var retval = false;
    try 
    {
        if (this.m_attrs[id] != null)
        {
            if (name == null || name.length == 0 || name == "value")
            {
                var i = GetIndexOfItem(this.m_attrs, id);
                this.m_attrs.splice(i, 1);
                retval = true;
            }
            else
            {
                if (this.m_attrs[id][name] != null)
                {
                    var j = GetIndexOfItem(this.m_attrs[id], name);
                    this.m_attrs[id].splice(j, 1);
                    retval = true;
                }
            }
        }
        this.m_bChanged = retval;
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.SetROMObjectValue = function(id, value) 
{
    try 
    {
        this.m_nodeValues[id] = value;
        this.m_bChanged = true;
        return true;
    }
    catch (err) 
    {
        ReportError(err);
        return false;
    }
}

ROMNode.prototype.GetROMObjectValue = function(id) 
{
    var retval = "";
    try 
    {
        if (this.m_nodeValues[id] != null)
            retval = this.m_nodeValues[id];
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.RemoveROMObjectValue = function(id)
{
    var retval = false;
    try 
    {
        if (this.m_nodeValues[id] != null)
        {
            var i = GetIndexOfItem(this.m_nodeValues, id)
            if (i >= 0)
            {
                this.m_nodeValues.splice(i, 1);
                retval = true;
                this.m_bChanged = retval;
            }            
        }
    }
    catch (err) 
    {
        ReportError(err);
    }
}

ROMNode.prototype.GetROMObjectID = function() 
{
    return this.m_id;
}

ROMNode.prototype.SetROMObjectID = function(id) 
{
    this.m_id = id;
}

ROMNode.prototype.GetAllAttributes = function() 
{
    return this.m_attrs;
}


//rules
ROMNode.prototype.LoadRules = function(knowledge_file) 
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

ROMNode.prototype.SetRulesDebugHandler = function(func)
{
    if (this.m_KnowledgeBase != null)
        this.m_KnowledgeBase.SetDebugHandler(func);
}

ROMNode.prototype.EvaluateTableForAttr = function(evalTable, output, bGetAll)
{
    try
    {        
        var knowledge = this._getKnowledge();
        if (knowledge != null)
        {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            this.LoadInputs(evalTable);
            var retval = knowledge.EvaluateTableForAttr(evalTable, output, bGetAll);
            return retval;
        }
    }
    catch (err)
    {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype.EvaluateTable = function(evalTable, bGetAll)
{
    try
    {
        var knowledge = this._getKnowledge();
        if (knowledge != null)
        {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            this.LoadInputs(evalTable);
            var retval = knowledge.EvaluateTable(evalTable, bGetAll);   
            return retval;    
        } 
    }
    catch (err)
    {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype.ReverseEvaluateTable = function(evalTable, bGetAll)
{
    try
    {
        var knowledge = this._getKnowledge();
        if (knowledge != null)
        {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            this.LoadOutputs(evalTable);
            var retval = knowledge.ReverseEvaluateTable(evalTable, bGetAll);   
            return retval;    
        } 
    }
    catch (err)
    {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype.ReverseEvaluateTableForAttr = function(evalTable, output, bGetAll)
{
    try
    {
        var knowledge = this._getKnowledge();
        if (knowledge != null)
        {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            this.LoadOutputs(evalTable);
            var retval = knowledge.ReverseEvaluateTableForAttr(evalTable, output, bGetAll);
            return retval;
        }
    }
    catch (err)
    {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype._getKnowledge = function()
{
    var knowledge = null;
    try
    {
        var current = this;
        if (current.m_KnowledgeBase != null)
            knowledge = current.m_KnowledgeBase;
        else
        {
            while (knowledge == null)
		    {
			    var parent = current.GetParent();
			    if (parent != null)
			    {
				    if (parent.m_KnowledgeBase != null)
				    {
					    knowledge = parent.m_KnowledgeBase;
					    break;
				    }
				    else
					    current = parent;
			    }
			    else
				    return null;		
		    }
        }
    }
    catch (err)
    {
        ReportError(err);
        return null;
    }
    return knowledge;
}

ROMNode.prototype.LoadInputs = function(evalTable) {
    try 
    {
        var knowledge = this._getKnowledge();
        if (knowledge != null)
        {
            var inputs = knowledge.GetInputDependencies(evalTable);
            if (inputs != null) for (var i = 0; i < inputs.length; i++) {			
                var value = this.GetATableInputValue(inputs[i]);            
                knowledge.SetInputValue(inputs[i], value);
            }  
        }     
    }
    catch (err) 
    {
        ReportError(err);
    }
}

ROMNode.prototype.LoadOutputs = function(evalTable) {
    try 
    {
        var knowledge = this._getKnowledge();
        if (knowledge != null)
        {
            var outputs = knowledge.GetOutputAttrs(evalTable);
            if (outputs != null) for (var i = 0; i < outputs.length; i++) {			
                var value = this.GetATableInputValue(inputs[i]);            
                knowledge.SetInputValue(inputs[i], value);
            }  
        }     
    }
    catch (err) 
    {
        ReportError(err);
    }
}

ROMNode.prototype.EvaluateXPATH = function(xpath, guid)
{    
    var retval = "";
    try
    {
        if (guid === undefined)
            guid = this.m_guid;
        var match = "<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'";
	    match += guid + "\']\"><xsl:value-of select=\"";
	    var xslt_text = XSLT_TOP + match + xpath + XSLT_BOTTOM;
	    
	    var xsltDoc = null;   
	    this._createXMLDoc();  
	    if (this.m_xmlDoc != null)
	    {
            if (IsIE())
            {
                xsltDoc = new ActiveXObject("Microsoft.XMLDOM");
                xsltDoc.async = "false";
                xsltDoc.loadXML(xslt_text);  
                retval = this.m_xmlDoc.transformNode(xsltDoc);
            }
            else
            {
                var parser = new DOMParser();
                xsltDoc = parser.parseFromString(xslt_text, "text/xml");
                var xsltProcessor = new XSLTProcessor();
                xsltProcessor.importStylesheet(xsltDoc);
                var ownerDocument = document.implementation.createDocument("", "xsltDoc", null);
                var solnNode = xsltProcessor.transformToFragment(this.m_xmlDoc, ownerDocument);
                retval = solnNode.firstChild.textContent;
            }
        }
    }
    catch(err)
    {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetATableInputValue = function(input)
{
    var retval = "";    
    try
    {        
        var xpathIndex = input.indexOf("xpath(");
        if (xpathIndex >= 0)
        {
            var cmdArg = input.substr(xpathIndex + 6, input.length - 7);
            retval = this.EvaluateXPATH(cmdArg, this.m_guid);
        }
        else if (input == "CLASSID")
        {
            retval = this.GetROMObjectID();
        }
        else
        {
            retval = this.GetAttribute(input, false);
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
    return retval;
}

ROMNode.prototype.GetPossibleValues = function(currentObject, evalTable, outputName)
{
    try 
    {
        var knowledge = this._getKnowledge();
        if (knowledge != null)
        {
            var res = knowledge.GetAllPossibleOutputs(evalTable, outputName);
            var retval = new Array();
            for (var i = 0; i < res.length; i++)
                retval.push(res[i]);
            return retval;
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
    return null;
}

//IO
ROMNode.prototype.SaveXML = function(indented)
{
    var retval = "";
    if (indented === undefined)
        indented = false;
    try
    {
        this._createXMLDoc();
        retval = this._convertXMLDocToString(indented);
    }
    catch (err)
    {
        ReportError(err);        
    }
    return retval;
}

ROMNode.prototype.LoadXML = function(xmlStr)
{
    var retval = false;
    try
    {
        var rootNode = null;
        if (IsIE()) 
        {
            this.m_xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
            this.m_xmlDoc.async = "false";
            this.m_xmlDoc.setProperty("SelectionLanguage", "XPath");
            this.m_xmlDoc.loadXML(xmlStr);
            rootNode = this.m_xmlDoc.selectSingleNode("Object");            
        }
        else 
        {
            var parser = new DOMParser();
            this.m_xmlDoc = parser.parseFromString(xmlStr, "text/xml");
            rootNode = this.m_xmlDoc.evaluate("Object", this.m_xmlDoc, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null);
        }
        
        if (rootNode != null)
        {
            if (_buildObject(objectNode, null) != null)
                retval = true;
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
    return retval;
}

ROMNode.prototype._buildObject = function(objectNode, parent)
{
    var newNode = NULL;
    try
    {
        var id = "";
        if (IsIE())
            id = objectNode.attributes.getNamedItem("id").nodeValue;
        else
            id = objectNode.getAttribute("id");
            
        if (parent == null)
        {
            this.DestroyROMObject();
            this.m_id = id;
            newNode = this;
        }
        else
        {
            newNode = new ROMNode(id);
        }
        
        //set object values
        for (var i = 0; i < objectNode.attributes.length; i++)
        {
	        var objAttr = objectNode.attributes.Getitem(i);
	        if (objAttr.nodeName != "id")
		        newNode.SetROMObjectValue(objAttr.nodeName, objAttr.nodeValue);
        }
        
        if (IsIE())
        {
	        //set object attributes
	        var attrNodes = objectNode.selectNodes("Attribute");
	        for (var attrCnt = 0; attrCnt < attrNodes.length; attrCnt++)
	        {
		        var attrNode = attrNodes[attrCnt];
		        var idAttr = attrNode.attributes.getNamedItem("id").nodeValue;
		        for (var i = 0; i < objectNode.attributes.length; i++)
		        {
			        var attr = objectNode.attributes[i];
			        if (attr.nodeName != "id")
				        newNode.SetAttribute(idAttr, attr.nodeName, attr.nodeValue);
		        }
	        }

	        //children recursivley
	        var childNodes = objectNode.selectNodes("Object");
	        for (var childCnt = 0; childCnt < childNode.length; childCnt++)
	        {
		        var childNode = childNodes[childCnt];
		        var newChildObject = this._buildObject(childNode, this);
		        if (newChildObject != null && newNode != null)
		        {
			        newNode.AddChildROMObject(newChildObject);
		        }
	        }
        }
        else
        {
	        //set object attributes
	        var attrNodes = this.m_xmlDoc.evaluate("Attribute", this.m_xmlDoc, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
	        for (var attrCnt = 0; attrCnt < attrNodes.snapshotLength; attrCnt++)
	        {
		        var attrNode = attrNodes.snapshotItem(attrCnt);
		        var idAttr = attrNode.getAttribute("id");
		        for (var i = 0; i < objectNode.attributes.length; i++)
		        {
			        var attr = objectNode.attributes[i];
			        if (attr.nodeName != "id")
				        newNode.SetAttribute(idAttr, attr.nodeName, attr.nodeValue);
		        }
	        }

	        //children recursivley
	        var childNodes = this.m_xmlDoc.evaluate("Object", this.m_xmlDoc, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
	        for (var childCnt = 0; childCnt < childNode.snapshotLength; childCnt++)
	        {
		        var childNode = childNodes.snapshotItem(childCnt);
		        var newChildObject = this._buildObject(childNode, this);
		        if (newChildObject != null && newNode != null)
		        {
			        newNode.AddChildROMObject(newChildObject);
		        }
	        }
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
    return newNode;
}

ROMNode.prototype._convertXMLDocToString = function(indented)
{
    var retval = "";
    try
    {
        if (this.m_xmlDoc != null)
        {
            if (IsIE())
            {
                retval = this.m_xmlDoc.xml;
            }
            else
            {
                var serializer = new XMLSerializer();
                if (indented && IsMoz())
                    retval = XML(serializer.serializeToString(this.m_xmlDoc)).toXMLString();
                else
                    retval = serializer.serializeToString(this.m_xmlDoc);
            }
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
    return retval;
}

ROMNode.prototype._createXMLDoc = function()
{
    try
    {
        var bChanged = this._anyHasChanged();
        if (bChanged)
        {
            var genXML = this._generateXML(bChanged);
            if (IsIE()) 
            {
                this.m_xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
                this.m_xmlDoc.async = "false";
                this.m_xmlDoc.setProperty("SelectionLanguage", "XPath");
                this.m_xmlDoc.loadXML(genXML);
            }
            else 
            {
                var parser = new DOMParser();
                this.m_xmlDoc = parser.parseFromString(genXML, "text/xml");
            }
            
            this._setAllUnchanged();
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
}

ROMNode.prototype._setAllUnchanged = function()
{
    try
    {
        this.m_bChanged = false;
        for (var itNode in this.m_children)
        {
            itNode.m_bChanged = false;
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
}
    

ROMNode.prototype._anyHasChanged = function()
{
    var retval = false;
    try
    {
        if (this.m_bChanged)
            retval = true;
        if (!retval)
        {
            for (var itNode in this.m_children)
            {
                if (itNode.m_bChanged)
                {
                    retval = true;
                    break;                    
                }
            }
        }
    }
    catch (err)
    {
        ReportError(err);        
    }
    return retval;
}

ROMNode.prototype._generateXML = function(bRegen)
{
    var retval = "";
    try
    {
        if (bRegen)
	    {
		    //this object
		    var beginObject = "<Object";
		    beginObject+=" id=\"";
		    beginObject+=this.m_id;
		    beginObject+="\" guid=\"";
		    beginObject+=this.m_guid;
		    beginObject+="\"";

		    //object values
		    var objAttrs = " ";
		    for (var itObjValue in this.m_nodeValues)
		    {
			    objAttrs+= itObjValue;
			    objAttrs+="=\"";
			    objAttrs+=this.m_nodeValues[itObjValue];
			    objAttrs+="\" ";
		    }
		    beginObject+=objAttrs;
		    beginObject+=">";
		    retval += beginObject;

		    if (this.m_bChanged)
		    {
			    var allAttrs = "";
			    //attributes of this object
			    for (var it in this.m_attrs)
			    {
				    var attrObject = "<Attribute id=\"";
				    attrObject+=it;
				    attrObject+="\" ";
				    for (var itValue in this.m_attrs[it])
				    {
					    attrObject+=itValue;
					    attrObject+="=\"";
					    attrObject+=this.m_attrs[it][itValue];
					    attrObject+="\" ";
				    }
				    attrObject+="/>";
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
		    for (var itNode in this.m_children)
		    {
		        var node = this.m_children[itNode];
		        if (node != null)
			        retval += node._generateXML(node.m_bChanged);
		    }

		    retval+="</Object>";
		    m_lastContents = retval;
	    }
	    else
	    {
		    retval = m_lastContents;
	    }
    }
    catch (err)
    {
        ReportError(err);        
    }
    return retval;
}

var SINGLESELECT = 0;
var MULTISELECT = 1;
var BOOLEANSELECT = 2;
var EDIT = 3;
var STATIC = 4;
// ROMDictionaryAttribute class////////////////////////////////////////////////////////////////
function ROMDictionaryAttribute() 
{
    this.Name = "";
    this.Description = "";
    this.DefaultValue = "";
    this.RulesTable = "";
    this.AttributeType = SINGLESELECT;
    this.ValueChanged = false;
    this.ChangedByUser = false;
    this.Valid = false;
    this.Visible = true;
    this.Enabled = true;
    this.PossibleValues = new Array();
    this.AvailableValues = new Array();
}

// ROMDictionary class////////////////////////////////////////////////////////////////
function ROMDictionary(node)
{
    this.m_context = node;
    this.m_dict = new Array();
}

ROMDictionary.prototype.LoadDictionary = function(dictionaryTable) 
{
    try 
    {
        this.m_dict = new Array();
        var res = this.m_context.EvaluateTable(dictionaryTable, true);
        var allNames = res["Name"];

        for (var i = 0; i < ArraySize(allNames); i++) 
        {
            var dictAttr = new ROMDictionaryAttribute();
            dictAttr.Name = allNames[i];
            
            if (res["DefaultValue"].length > 0 && res["DefaultValue"][i] != "~") dictAttr.DefaultValue = res["DefaultValue"][i];
		    if (res["Description"].length > 0 && res["Description"][i] != "~") dictAttr.Description = res["Description"][i];
		    if (res["RuleTable"].length > 0 && res["RuleTable"][i] != "~") dictAttr.RuleTable = res["RuleTable"][i];		

		    var strAttrType = "";
		    if (res["AttributeType"].length > 0 && res["AttributeType"][i] != "~") strAttrType = res["AttributeType"][i];
		    strAttrType = strAttrType.toUpperCase();

		    if (strAttrType.length == 0 || strAttrType == "SINGLESELECT")
		    {
			    dictAttr.AttributeType = SINGLESELECT;
		    }
		    else if (strAttrType == "MULTISELECT")
		    {
			    dictAttr.AttributeType = MULTISELECT;
		    }
		    else if (strAttrType == "BOOLEAN")
		    {
			    dictAttr.AttributeType = BOOLEANSELECT;
		    }
		    else if (strAttrType == "EDIT")
		    {
			    dictAttr.AttributeType = EDIT;
			}
			else if (strAttrType == "STATIC")
		    {
			    dictAttr.AttributeType = STATIC;
		    }
		    
		    //on load, just set default values and possibilities
		    //only set a default if there is no rules table and no current value
		    var value = this.m_context.GetAttribute(dictAttr.Name, false);
		    if (((value.length == 0 && dictAttr.RuleTable.length == 0) || dictAttr.AttributeType == STATIC) && dictAttr.DefaultValue.length > 0)
		    {
			    this.m_context.SetAttribute(dictAttr.Name, dictAttr.DefaultValue);
		    }

		    if (dictAttr.RuleTable.length > 0)
			    dictAttr.PossibleValues = this.m_context.GetPossibleValues(dictAttr.RuleTable, dictAttr.Name);

		    this.m_dict[dictAttr.Name] = dictAttr;
        }        
    }
    catch (err) 
    {
        ReportError(err);
    }
}

ROMDictionary.prototype.GetDictionaryAttr = function(dictAttrName) 
{
    var retval = null;
    try 
    {
        if (dictAttrName in this.m_dict)
        {
            retval = this.m_dict[dictAttrName];
        }
    }
    catch (err) 
    {
        ReportError(err);
    }
    return retval;
}

ROMDictionary.prototype.GetAllDictionaryAttrs = function()
{
    try
    {
        return this.m_dict;
    }
    catch (err)
    {
        ReportError(err);        
    }
    return null;
}

// LinearEngine class////////////////////////////////////////////////////////////////
function LinearEngine(context, dictionaryTable)
{
    this.INVISPREFIX = "^";
    this.DEFAULTPREFIX = "@";
    this.DISABLEPREFIX = "#";
    
    this.base = new ROMDictionary(context);
    
    this.m_EvalList = new Array();
    this.m_mapTriggers = new Array();
    this.m_CurrentRecursion = 0;
    this.m_EvalListRecursChecker = new Array();
    this.m_EvalInternal = false;

    this.base.LoadDictionary(dictionaryTable);
    //open each attr in dict, load its dependency info to create m_EvalList, m_mapTriggers
	//build an initial list that matches the dictionary order
    for (var attr in this.base.m_dict)
    {
        this.m_EvalList.push(this.base.m_dict[attr]);
        //triggers
        var knowledge = this.base.m_context._getKnowledge();
        var deps = knowledge.GetInputDependencies(this.base.m_dict[attr].RuleTable);
        if (deps != null) for (var i = 0; i < deps.length; i++)
        {
            var key = deps[i];
            if (key in this.m_mapTriggers)
            {
                var hasAttr = false;
                for (var j = 0; j < this.m_mapTriggers[key].length; j++){
                if (this.m_mapTriggers[key][j] == this.base.m_dict[attr].Name) 
                    hasAttr = true;
                }
                
                if (!hasAttr)
                    this.m_mapTriggers[key].push(this.base.m_dict[attr].Name);
            }
            else if (key in this.base.m_dict)
            {
                var newArray = new Array();
                newArray.push(this.base.m_dict[attr].Name);
                this.m_mapTriggers[key] = newArray;
            }
        }
    }
    
    //based on the triggers, re-order the dictionary
	m_CurrentRecursion = 0;
	this.OrderDictionary();	
	this.m_EvalListRecursChecker = null;
}

LinearEngine.prototype.GetAllDictionaryAttrs = function(){return this.base.GetAllDictionaryAttrs();}
LinearEngine.prototype.GetDictionaryAttr = function(attrName){return this.base.GetDictionaryAttr(attrName);}

LinearEngine.prototype.ResetValueChanged = function()
{
    try
    {
        for (var i = 0; i < this.m_EvalList.length; i++)
        {
            this.m_EvalList[i].ValueChanged = false;
        }
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvaluateForAttribute = function(dictAttrName, newValues, bEvalDependents)
{
    //newValues could be a string or an array
    try
    {
        this.ResetValueChanged();
        var newVals = new Array();
        if (typeof newValues == "string")
            newVals.push(newValues);
        else
            newVals = newValues;
        
        if (dictAttrName in this.base.m_dict)
        {
            this.base.m_dict[dictAttrName].ValueChanged = true;
            this.base.m_dict[dictAttrName].ChangedByUser = !this.m_EvalInternal;         
            switch (this.base.m_dict[dictAttrName].AttributeType)
            {
            case SINGLESELECT:
			        this.EvalSingleSelect(dictAttrName, newVals[0]);				
			        break;

		        case MULTISELECT:
			        this.EvalMultiSelect(dictAttrName, newVals);
			        break;

		        case BOOLEANSELECT:
			        this.EvalBoolean(dictAttrName, newVals[0]);
			        break;

		        case EDIT:
			        this.EvalEdit(dictAttrName, newVals[0]);
			        break;
			        
			    default: //STATIC
			        this.EvalSingleSelect(dictAttrName, newVals[0]);
			        break;
		    }

	        if (bEvalDependents)
		        this.EvaluateDependencies(dictAttrName);              
        }
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvaluateAll = function()
{
    this.m_EvalInternal = true;
    try
    {        
        this.ResetValueChanged();
        for (var i = 0; i < this.m_EvalList.length; i++)
        {
            var selectedValues = this.GetSelectedValues(this.m_EvalList[i]);
            this.EvaluateForAttribute(this.m_EvalList[i].Name, selectedValues, false);
        }        
    }
    catch (err)
    {
        ReportError(err);
    }
    this.m_EvalInternal = false;
}

LinearEngine.prototype.GetEvalList = function()
{
    try
    {
        return this.m_EvalList;
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.DictionaryIsValid = function()
{
    var retval = true;	
	for (var i = 0; i < this.m_EvalList.length; i++)
	{
		if (!this.m_EvalList[i].Valid)
		{
			retval = false;
			break;
		}
	}
	return retval;
}

var MAX_RECURSION = 1000;
LinearEngine.prototype.OrderDictionary = function()
{
    try
    {
        this.m_CurrentRecursion++;
        var evalOrderCopy = this.m_EvalList.slice();
        
        for (var inputAttr in this.m_mapTriggers)
        {
            var outputAttrs = this.m_mapTriggers[inputAttr];
            if (outputAttrs != null && outputAttrs.length > 0)
            {
                var lowestIndex = 0;
                var bFoundInputAttr = false;
			    for (lowestIndex = 0; lowestIndex < this.m_EvalList.length; lowestIndex++)
			    {
				    if (this.m_EvalList[lowestIndex].Name == inputAttr)
				    {
					    bFoundInputAttr = true;
					    break;
				    }
			    }
			    
			    var origIndex = lowestIndex;
			    if (bFoundInputAttr)
			    {
			        for (var i = 0; i < outputAttrs.length; i++)
                    {
                        var attrOutput = outputAttrs[i];
                        if (attrOutput.length > 0)
                        {
                            var currentIndex = 0;
						    for (currentIndex = 0; currentIndex < this.m_EvalList.length; currentIndex++)
						    {					
							    if (this.m_EvalList[currentIndex].Name == attrOutput)							
								    break;							
						    }
                            if (currentIndex < lowestIndex)                      
                                lowestIndex = currentIndex;                          
                        }
                    }

                    //make the current input index lower than lowest output     
                    if (origIndex != lowestIndex)
                    {
					    var attr = this.m_EvalList[origIndex];
                        this.m_EvalList.splice(origIndex, 1);
                        this.m_EvalList.splice(lowestIndex, 0, attr);
                    }
			    }
			    
            }
        }
        
        for (var i = 0; i < evalOrderCopy.length; i++)
        {
            //if the lists differ, do another sort, otherwise we should be done
            if (evalOrderCopy[i] != this.m_EvalList[i] && this.m_CurrentRecursion < MAX_RECURSION)
            {
                //does it match a previous result (are we flipping between a couple values, circular logic)
				if (this.m_CurrentRecursion % 2 == 0 && this.m_EvalListRecursChecker.length > 0)
				{
					for (var j = 0; j < this.m_EvalListRecursChecker.length; j++)
					{
						if (this.m_EvalList[j] != this.m_EvalListRecursChecker[j])
						{
							this.OrderDictionary();
							break;
						}
					}
				}
				else				
					this.OrderDictionary();
            }
        }
        
        if (this.m_CurrentRecursion % 2 == 0)
		{
			this.m_EvalListRecursChecker = this.m_EvalList.slice(0);
		}
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvalBoolean = function(dictAttrName, newValue)
{
    try
    {
        var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
	    var availableValues = new Array();

	    var prefixes = this.ParseOutPrefixes(res, availableValues);
	    this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);

	    if (prefixes.length > 0 && prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
		    this.base.m_dict[dictAttrName].Visible = false;
	    else
		    this.base.m_dict[dictAttrName].Visible = true;

	    var currentValue = this.base.m_context.GetAttributeValue(dictAttrName, false);
	    this.base.m_dict[dictAttrName].Valid = true;
	    this.base.m_dict[dictAttrName].Enabled = true;
	    
	    //set the dictionary default on load
	    if (newValue.length == 0)
	    {
		    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0)
			    newValue = this.base.m_dict[dictAttrName].DefaultValue;
	    }

	    if (availableValues == null || availableValues.length == 0)
	    {
		    this.base.m_context.SetAttribute(dictAttrName, "N");
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
		    return;
	    }
	    else if (availableValues.length == 1) //you should only have one value
	    {			
		    if (availableValues[0].length == 0 || availableValues[0] == "N")
		    {
			    this.base.m_context.SetAttribute(dictAttrName, "N");
			    return;
		    }
		    else if (availableValues[0] == "YN") //allow Yes or No with a default of Y
		    {
			    if (currentValue.length == 0)
			    {
				    this.base.m_context.SetAttribute(dictAttrName, "Y");					
			    }
		    }
		    else if (availableValues[0] == "YY") //force Yes, no other choice
		    {
			    this.base.m_context.SetAttribute(dictAttrName, "Y");
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    this.base.m_dict[dictAttrName].Enabled = false;
		    }
		    else if (availableValues[0] == "NN") //force No, no other choice
		    {
			    this.base.m_context.SetAttribute(dictAttrName, "N");
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    this.base.m_dict[dictAttrName].Enabled = false;
		    }
		    else if (newValue.length == 1) //Y or N
		    {
			    this.base.m_context.SetAttribute(dictAttrName, newValue);
		    }
	    }
	
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvalEdit = function(dictAttrName, newValue)
{
    try
    {
      var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
	    var availableValues = new Array();

	    var prefixes = this.ParseOutPrefixes(res, availableValues);
	    this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
	    this.base.m_dict[dictAttrName].Enabled = true;
	    this.base.m_dict[dictAttrName].Valid = true;

	    var currentValue = this.base.m_context.GetAttribute(dictAttrName, false);

	    //set the dictionary default on load
	    if (newValue.length == 0)
	    {
		    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0)
			    newValue = this.base.m_dict[dictAttrName].DefaultValue;
	    }
	    
	    if (availableValues.length == 1)
	    {
		    if (prefixes[0].indexOf(this.DISABLEPREFIX) >= 0)
		    {
			    this.base.m_dict[dictAttrName].Enabled = false;
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    this.base.m_context.SetAttribute(dictAttrName, prefixes[0]);	
			    return;
		    }
		    else
			    this.base.m_dict[dictAttrName].Enabled = true;

		    //check table result for range
		    if (availableValues[0].charAt(0) == '[')
		    {
			    var dNewValue = 0, dMin = 0, dMax = 0;
			    var val = availableValues[0];
			    val = val.replace("[", "");
			    val = val.replace("]", "");
    			
			    var vals = val.split(",");
			    dNewValue = parseFloat(newValue);
			    dMin = parseFloat(vals[0]);
			    dMax = parseFloat(vals[1]);

			    if (dNewValue <= dMax && dNewValue >= dMin)
			    {
				    this.base.m_context.SetAttribute(dictAttrName, newValue);	
			    }
			    else if (dNewValue > dMax)
			    {
				    var wstrMax = vals[1];
				    this.base.m_context.SetAttribute(dictAttrName, wstrMax);
			    }
			    else if (dNewValue < dMin)
			    {
				    var wstrMin = vals[0];
				    this.base.m_context.SetAttribute(dictAttrName, wstrMin);
			    }
		    }
		    else if (availableValues[0].length == 1 && availableValues[0][0] == 'Y')
		    {
			    this.base.m_context.SetAttribute(dictAttrName, newValue);
		    }		
		    else if (availableValues[0].length == 1 && availableValues[0][0] == 'N')
		    {
			    this.base.m_context.SetAttribute(dictAttrName, "");
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    this.base.m_dict[dictAttrName].Enabled = false;
		    }
		    else if (availableValues[0].length == 2 && availableValues[0] == "YY") //user must enter something
	        {
		        this.base.m_context.SetAttribute(dictAttrName, newValue);
		        this.base.m_dict[dictAttrName].Valid = newValue.length > 0;
	        }
		    else
		    {
			    this.base.m_context.SetAttribute(dictAttrName, availableValues[0]);
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
		    }
	    }	    
	    else if (availableValues.length == 0)
	    {		
		    this.base.m_context.SetAttribute(dictAttrName, "");
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
		    this.base.m_dict[dictAttrName].Enabled = false;
	    }
	    else if (availableValues.length == 1 && availableValues[0].length > 0)
	    {
		    this.base.m_context.SetAttribute(dictAttrName, availableValues[0]);
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
	    }
	    else if (availableValues.length > 0)
	    {
		    if (GetIndexOfItem(availableValues, newValue) >= 0)
		    {
			    this.base.m_context.SetAttribute(dictAttrName, newValue);
		    }
		    else 
		    {
			    this.base.m_context.SetAttribute(dictAttrName, "");
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
		    }
	    }
	    
	    if (prefixes.length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
	    {
		    this.base.m_dict[dictAttrName].Visible = false;	
	    }
	    else
	    {
		    this.base.m_dict[dictAttrName].Visible = true;
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvalMultiSelect = function(dictAttrName, newValues)
{
    try
    {
        //multi-select lists, checkbox lists
	    var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
	    var availableValues = new Array();	    

	    var prefixes = this.ParseOutPrefixes(res, availableValues);
	    this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
	    this.base.m_dict[dictAttrName].Enabled = true;
	    this.base.m_dict[dictAttrName].Valid = true;

	    var currentValue = this.base.m_context.GetAttributeValue(dictAttrName, false);
	    var currentValues = currentValue.split("|");
	    var selectedValues = new Array();
	    
	    //set the dictionary default on load
	    if (newValues.length == 0)
	    {
		    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0)
		    {
			    newValues = new Array();
			    newValues.push(this.base.m_dict[dictAttrName].DefaultValue);
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
		    }
	    }
	    
	    //if only one is available, force that selection now
	    if (availableValues.length == 1)
	    {
		    selectedValues.push(availableValues[0]);
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
		}
	    
	    //if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
	    if (currentValues != null && (currentValues.length == 1 && (currentValues[0].length == 0 || GetIndexOfItem(availableValues, currentValues[0]) >= 0)) && prefixes.length > 0)
	    {
		    for (var i = 0; i < prefixes.length; i++)
		    {
			    if (prefixes[i].indexOf(this.DEFAULTPREFIX) >= 0)
			    {
				    selectedValues.push(availableValues[i]);
				    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    }
		    }
	    }
	    else if (availableValues.length == 0)
	    {
		    this.base.m_dict[dictAttrName].Enabled = false;
	    }
	    
	    if (selectedValues.length == 0 && currentValues.length > 0) //compare the new values to what is really available
	    {
		    for (var i = 0; i < newValues.length; i++)
		    {	
		        if (GetIndexOfItem(availableValues, newValues[i]) >= 0)          
		        {     
                  selectedValues.push(newValues[i]);          
			    }
		    }
	    }

	    var finalValue = "";
	    for (var i = 0; i < selectedValues.length; i++)
	    {
            var val = selectedValues[i];
		    if (finalValue.length > 0)
			    finalValue+= "|";
		    finalValue+=val;
	    }		

	    if (finalValue != currentValue)
	    {
		    this.base.m_context.SetAttribute(dictAttrName, finalValue);
		}
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvalSingleSelect = function(dictAttrName, newValue)
{
    try
    {
        var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
        var availableValues = new Array();
        this.base.m_dict[dictAttrName].Enabled = true;
        this.base.m_dict[dictAttrName].Valid = true;

        //the list of results is what is available for selection in the control
        var prefixes = this.ParseOutPrefixes(res, availableValues);        
        this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);

        var currentValue = this.base.m_context.GetAttribute(dictAttrName, false);
        
        //set the dictionary default on load
	    if (newValue.length == 0)
	    {
		    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0)
		    {
			    newValue = this.base.m_dict[dictAttrName].DefaultValue;
		    }
	    }
	    
	    //if only one is available, force that selection now
	    if (availableValues.length == 1)
	    {
		    newValue = availableValues[0];
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
	    }
	    
	    //if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
	    if ((currentValue.length == 0 || GetIndexOfItem(availableValues, currentValue) >= 0) && prefixes.length > 0)
	    {
		    for (var i = 0; i < prefixes.length; i++)
		    {
			    if (prefixes[i].indexOf(this.DEFAULTPREFIX) >= 0)
			    {
				    newValue = availableValues[i];
				    this.base.m_dict[dictAttrName].ChangedByUser = false;
				    break;
			    }
		    }
	    }
	    else if (availableValues.length == 0)
	    {
		    this.base.m_dict[dictAttrName].Enabled = false;
	    }
	    
	    if (prefixes.length > 0 && prefixes[0].length> 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
		    this.base.m_dict[dictAttrName].Visible = false;
	    else
		    this.base.m_dict[dictAttrName].Visible = true;
		    
	    if (newValue.length > 0 && GetIndexOfItem(availableValues, newValue) >= 0)
	    {
		    if (currentValue != newValue)
		    {
			    this.base.m_context.SetAttribute(dictAttrName, newValue);
		    }
	    }
	    else
	    {
	        if (this.base.m_dict[dictAttrName].Enabled == true)
		        this.base.m_dict[dictAttrName].Valid = false;
		    this.base.m_context.SetAttribute(dictAttrName, "");
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvaluateDependencies = function(dictAttrName)
{
    this.m_EvalInternal = true;
    try
    {        
        if (dictAttrName in this.m_mapTriggers)
	    {
		    var attrsToEval = this.m_mapTriggers[dictAttrName];
		    for (var i = 0; i < attrsToEval.length; i++)
		    {	
                var attrName = attrsToEval[i];
			    if (attrName in this.base.m_dict)
			    {					
				    var selectedValues = this.GetSelectedValues(this.base.m_dict[attrName]);
				    var bWasChangedByUser = this.base.m_dict[attrName].ChangedByUser;
				    this.EvaluateForAttribute(attrName, selectedValues, true);
				    if (bWasChangedByUser)
				    {
					    var bValuesRemainSame = true;
					    var newSelectedValues = this.GetSelectedValues(this.base.m_dict[attrName]);
					    if (newSelectedValues.length != selectedValues.length)
						    bValuesRemainSame = false;
					    else for (var i = 0; i < selectedValues.length; i++)
					    {
						    if (GetIndexOfItem(newSelectedValues, selectedValues[i]) < 0)
						    {
							    bValuesRemainSame = false;
							    break;
						    }
					    }

					    this.base.m_dict[attrName].ChangedByUser = bValuesRemainSame;
				    }
			    }
		    }
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
    this.m_EvalInternal = false;
}

//remove the special character flags from the values
LinearEngine.prototype.ParseOutPrefixes = function(values, valuesWithoutPrefixes)
{    
    var prefixes = new Array();
    try
    {        
      var origValues = values;
      valuesWithoutPrefixes.splice(0);
	    
	    for (var i = 0; i < origValues.length; i++)
	    {
        var val = origValues[i];
        var fullPrefix = "";
		
		    //check for leadoff ^ indicating an invisible control
		    if (val.indexOf(this.INVISPREFIX) >= 0)
		    {
			    fullPrefix+=this.INVISPREFIX;
			    val = val.replace(this.INVISPREFIX, "");
		    }

		    //check for leadoff @ indicating a default
		    if (val.indexOf(this.DEFAULTPREFIX) >= 0)
		    {
			    fullPrefix+=this.DEFAULTPREFIX;
			    val = val.replace(this.DEFAULTPREFIX, "");
		    }

		    //check for leadoff # indicating a locked edit box
		    if (val.indexOf(this.DISABLEPREFIX) >= 0)
		    {
			    fullPrefix+=this.DISABLEPREFIX;
			    val = val.replace(this.DISABLEPREFIX, "");
		    }

		    prefixes.push(fullPrefix);
		    valuesWithoutPrefixes.push(val);
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
    return prefixes;
}

LinearEngine.prototype.GetSelectedValues = function(attr)
{
    var retval = new Array();
    try
    {        
	    var currentValue = this.base.m_context.GetAttribute(attr.Name, false); 
	    switch (attr.AttributeType)
	    {
	    case SINGLESELECT:
		    retval.push(currentValue);
		    break;

	    case MULTISELECT:
		    retval = currentValue.split("|");
		    break;

	    case BOOLEANSELECT:
		    retval.push(currentValue);
		    break;

	    case EDIT:
		    retval.push(currentValue);
		    break;

	    default:
		    retval.push(currentValue);
		    break;
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
    
    return retval;
}