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

// namespace placeholder
//Extender = {};

///**
// * A function used to extend one class with another
// * 
// * @param {Object} subClass
// * 		The inheriting class, or subclass
// * @param {Object} baseClass
// * 		The class from which to inherit
// */
//Extender.extend = function(subClass, baseClass) {
//   function inheritance() {}
//   inheritance.prototype = baseClass.prototype;

//   subClass.prototype = new inheritance();
//   subClass.prototype.constructor = subClass;
//   subClass.baseConstructor = baseClass;
//   subClass.superClass = baseClass.prototype;
//}


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
    var moz = (typeof document.implementation != 'undefined') && (typeof 
        document.implementation.createDocument != 'undefined');
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
        if (inputs != null) for (var i = 0; i < inputs.length; i++)
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

ROMTree.prototype.GetPossibleValues = function(currentObject, evalTable, outputName)
{
    var res = this.m_KnowledgeBase.GetAllPossibleOutputs(evalTable, outputName);
    var retval = new Array();
    for (var i = 0; i < res.length; i++)
        retval.push(res[i]);
    return retval;
}

var SINGLESELECT = 0;
var MULTISELECT = 1;
var BOOLEANSELECT = 2;
var EDIT = 3;
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
function ROMDictionary(tree, node)
{
    this.m_tree = tree;
    this.m_context = node;
    this.m_dict = new Array();
}

ROMDictionary.prototype.LoadDictionary = function(dictionaryTable) 
{
    try 
    {
        this.m_dict = new Array();
        var res = this.m_tree.EvaluateTable(this.m_context, dictionaryTable, true);
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
		    
		    //on load, just set default values and possibilities
		    //only set a default if there is no rules table and no current value
		    var value = this.m_tree.GetAttributeValue(this.m_context, dictAttr.Name, false);
		    if (value.length == 0 && dictAttr.RuleTable.length == 0 && dictAttr.DefaultValue.length > 0)
		    {
			    this.m_tree.SetAttribute(this.m_context, dictAttr.Name, dictAttr.DefaultValue);
		    }

		    if (dictAttr.RuleTable.length > 0)
			    dictAttr.PossibleValues = this.m_tree.GetPossibleValues(this.m_context, dictAttr.RuleTable, dictAttr.Name);

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
function LinearEngine(tree, context, dictionaryTable)
{
    this.INVISPREFIX = "^";
    this.DEFAULTPREFIX = "@";
    this.DISABLEPREFIX = "#";
    
    this.base = new ROMDictionary(tree, context);
    
    this.m_EvalList = new Array();
    this.m_mapTriggers = new Array();
    this.m_CurrentRecursion = 0;

    this.base.LoadDictionary(dictionaryTable);
    //open each attr in dict, load its dependency info to create m_EvalList, m_mapTriggers
	//build an initial list that matches the dictionary order
    for (var attr in this.base.m_dict)
    {
        this.m_EvalList.push(this.base.m_dict[attr]);
        //triggers
        var r = new ROMTree("a");
        var deps = this.base.m_tree.m_KnowledgeBase.GetInputDependencies(this.base.m_dict[attr].RuleTable);
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
}

LinearEngine.prototype.GetAllDictionaryAttrs = function(){return this.base.GetAllDictionaryAttrs();}
LinearEngine.prototype.GetDictionaryAttr = function(attrName){return this.base.GetDictionaryAttr(attrName);}

LinearEngine.prototype.EvaluateForAttribute = function(dictAttrName, newValues, bEvalDependents)
{
    //newValues could be a string or an array
    try
    {
        var newVals = new Array();
        if (typeof newValues == "string")
            newVals.push(newValues);
        else
            newVals = newValues;
        
        if (dictAttrName in this.base.m_dict)
        {
            this.base.m_dict[dictAttrName].ChangedByUser = true;
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
    try
    {
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

var MAX_RECURSION = 1000;
LinearEngine.prototype.OrderDictionary = function()
{
    try
    {
        m_CurrentRecursion++;
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
            if (evalOrderCopy[i] != this.m_EvalList[i] && m_CurrentRecursion < MAX_RECURSION)
            {
                this.OrderDictionary();
                break;
            }
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
      var res = this.base.m_tree.EvaluateTableForAttr(this.base.m_context, this.base.m_dict[dictAttrName].RuleTable, dictAttrName, true);
      var availableValues = new Array();
      
      //the list of results is what is available for selection in the control
      var prefixes = this.ParseOutPrefixes(res, availableValues);        
      this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
     
      var currentValue = this.base.m_tree.GetAttributeValue(this.base.m_context, dictAttrName, false);
        
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
	    
	    //if the current value is "", and an available value is prefixed with a "@" default, set it now
	    if (currentValue.length == 0)
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
	    
	    if (prefixes.length > 0 && prefixes[0].length> 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
		    this.base.m_dict[dictAttrName].Visible = false;
	    else
		    this.base.m_dict[dictAttrName].Visible = true;
		    
      var srcAvail = availableValues.splice(0);
	    if (newValue.length > 0 && srcAvail.indexOf(newValue) >= 0)
	    {
		    this.base.m_dict[dictAttrName].Valid = true;
		    if (currentValue != newValue)
		    {
			    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, newValue);
			    this.base.m_dict[dictAttrName].ChangedByUser = true;
		    }
	    }
	    else
	    {
		    this.base.m_dict[dictAttrName].Valid = false;
		    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, "");
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
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
	    var res = this.base.m_tree.EvaluateTableForAttr(this.base.m_context, this.base.m_dict[dictAttrName].RuleTable, dictAttrName, true);
	    var availableValues = new Array();	    

	    var prefixes = this.ParseOutPrefixes(res, availableValues);
	    this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);	    

	    var currentValue = this.base.m_tree.GetAttributeValue(this.base.m_context, dictAttrName, false);
	    var currentValues = currentValue.split("|");
	    var selectedValues = new Array();
	    
	    this.base.m_dict[dictAttrName].ChangedByUser = true;
	    
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
	    //if the current value is "", and an available value is prefixed with a "@" default, set it now
	    else if (currentValues != null && currentValues.length == 1 && currentValues[0].length == 0 && prefixes.length > 0)
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
	    
	    if (selectedValues.length == 0 && currentValues.length > 0) //compare the new values to what is really available
	    {
		    for (var i = 0; i < newValues.length; i++)
		    {	
          for (var j = 0; j < availableValues.length; j++)
          {
            if (newValues[i] == availableValues[j])
            {
              selectedValues.push(newValues[i]);
              break;
            }
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
		    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, finalValue);
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
        var res = this.base.m_tree.EvaluateTableForAttr(this.base.m_context, this.base.m_dict[dictAttrName].RuleTable, dictAttrName, false);
	    var availableValues = new Array();

	    var prefixes = this.ParseOutPrefixes(res, availableValues);
	    this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);

	    if (prefixes.length > 0 && prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
		    this.base.m_dict[dictAttrName].Visible = false;
	    else
		    this.base.m_dict[dictAttrName].Visible = true;

	    var currentValue = this.base.m_tree.GetAttributeValue(this.base.m_context, dictAttrName, false);
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
		    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, "N");
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
		    return;
	    }
	    else if (availableValues.length == 1) //you should only have one value
	    {			
		    if (availableValues[0].length == 0 || availableValues[0] == "N")
		    {
			    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, "N");
			    this.base.m_dict[dictAttrName].ChangedByUser = (newValue.length > 0 && newValue != "N");
			    return;
		    }
		    else if (availableValues[0] == "YN") //allow Yes or No with a default of Y
		    {
			    if (currentValue.length == 0)
			    {
				    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, "Y");					
			    }
		    }
		    else if (availableValues[0] == "YY") //force Yes, no other choice
		    {
			    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, "Y");
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    this.base.m_dict[dictAttrName].Enabled = false;
		    }
		    else if (availableValues[0] == "NN") //force No, no other choice
		    {
			    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, "N");
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    this.base.m_dict[dictAttrName].Enabled = false;
		    }
		    else if (newValue.length == 1) //Y or N
		    {
			    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, newValue);
			    this.base.m_dict[dictAttrName].ChangedByUser = (availableValues[0] == newValue);
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
      var res = this.base.m_tree.EvaluateTableForAttr(this.base.m_context, this.base.m_dict[dictAttrName].RuleTable, dictAttrName, true);
	    var availableValues = new Array();

	    var prefixes = this.ParseOutPrefixes(res, availableValues);
	    this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
	    this.base.m_dict[dictAttrName].Enabled = true;

	    var currentValue = this.base.m_tree.GetAttributeValue(this.base.m_context, dictAttrName, false);

	    //set the dictionary default on load
	    if (newValue.length == 0)
	    {
		    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0)
			    newValue = this.base.m_dict[dictAttrName].DefaultValue;
	    }
	    
	    if (availableValues.length == 1)
	    {
		    if (prefixes[0].indexOf(this.INVISPREFIX) >= 0)
		    {
			    this.base.m_dict[dictAttrName].Visible = false;	
		    }
		    else
		    {
			    this.base.m_dict[dictAttrName].Visible = true;
		    }

		    if (prefixes[0].indexOf(this.DISABLEPREFIX) >= 0)
		    {
			    this.base.m_dict[dictAttrName].Enabled = false;
			    this.base.m_dict[dictAttrName].ChangedByUser = false;
			    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, prefixes[0]);	
			    return;
		    }
		    else
			    this.base.m_dict[dictAttrName].Enabled = true;

		    //check table result for range
		    if (availableValues[0].charAt(0) == '[')
		    {
			    this.base.m_dict[dictAttrName].ChangedByUser = true;
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
				    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, newValue);	
			    }
			    else if (dNewValue > dMax)
			    {
				    var wstrMax = vals[1];
				    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, wstrMax);
			    }
			    else if (dNewValue < dMin)
			    {
				    var wstrMin = vals[0];
				    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, wstrMin);
			    }
		    }
	    }
	    else if (availableValues.length == 0 || (availableValues.length == 1 && availableValues[0].length == 1 && availableValues[0].charAt(0) == "Y"))
	    {		
		    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, newValue);
		    this.base.m_dict[dictAttrName].ChangedByUser = true;
		    if (prefixes.length > 0 && prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
			    this.base.m_dict[dictAttrName].Visible = false;
		    else
			    this.base.m_dict[dictAttrName].Visible = true;;

	    }
	    else if (availableValues.length == 1 && availableValues[0].length > 0)
	    {
		    this.base.m_tree.SetAttribute(this.base.m_context, dictAttrName, availableValues[0]);
		    this.base.m_dict[dictAttrName].ChangedByUser = false;
		    if (prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
			    this.base.m_dict[dictAttrName].Visible = false;
		    else
			    this.base.m_dict[dictAttrName].Visible = true;;
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
}

LinearEngine.prototype.EvaluateDependencies = function(dictAttrName)
{
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
				    this.EvaluateForAttribute(attrName, selectedValues, true);
			    }
		    }
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
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
	    var currentValue = this.base.m_tree.GetAttributeValue(this.base.m_context, attr.Name, false); 
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