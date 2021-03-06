/*
Copyright (C) 2009-2014 Eric D. Schmidt, DigiRule Solutions LLC

The JavaScript code in this page is free software: you can
redistribute it and/or modify it under the terms of the GNU
General Public License (GNU GPL) as published by the Free Software
Foundation, either version 2 of the License, or (at your option)
any later version.  The code is distributed WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.

As additional permission under GNU GPL version 2 section 3, you
may distribute non-source (e.g., minimized or compacted) forms of
that code without the copy of the GNU GPL normally required by
section 3, provided you include this license notice and a URL
through which recipients can access the Corresponding Source.
<http://www.gnu.org/licenses/>.
*/

/// reference path="KnowledgeBase.js"
/// reference path="javascript-xpath.js"

//stupid IE doesnt do indexOf for array
function GetIndexOfItem(arr, obj) {
    for (var i = 0; i < arr.length; i++) {
        if (arr[i] === obj) {
            return i;
        }
    }
    return -1;
}

function ArraySize(arr) //for associative array sizes
{
    var l = 0;
	if (length in arr)
		return arr.length;
    for (var k in arr) {
        l++;
    }
    return l;
}

function getKeysOfObject(o) { var keys = [], i; for (i in o) { keys.push(i); } return keys; }

function ReportError(err) {
    var vDebug = "";
    for (var prop in err) {
        vDebug += "property: " + prop + " value: [" + err[prop] + "]\n";
    }
    vDebug += "toString(): " + " value: [" + err.toString() + "]";
    status.rawValue = vDebug;
    alert(vDebug);
}

var engineWebKit = "webkit";
var deviceAndroid = "android";
function IsIE() {
    var ie = false;
	if (window.ActiveXObject || "ActiveXObject" in window)
		ie = true;
    return ie;
}

function IsMoz() {
    var moz = (navigator.userAgent.toLowerCase().indexOf('firefox') >= 0 && SupportsXPATH() == true);
    return moz;
}

// Detects if the current browser is based on WebKit.
function DetectWebkit()
{
   if (navigator.userAgent.toLowerCase().search(engineWebKit) > -1)
      return true;
   else
      return false;
}

// Detects if the current device is an Android OS-based device.
function DetectAndroid() {
    if (navigator.userAgent.toLowerCase().search(deviceAndroid) > -1)
        return true;
    else
        return false;
}

// Detects if the current device is an Android OS-based device and
//   the browser is based on WebKit.
function DetectAndroidWebKit() {
    if (DetectAndroid()) {
        if (DetectWebkit())
            return true;
        else
            return false;
    }
    else
        return false;
}

function SupportsXPATH() {
    var res = (document.implementation
            && document.implementation.hasFeature
            && document.implementation.hasFeature("XPath", null));
    return res;
}

//xpath constants
var STRING_TYPE = 2;
var ORDERED_NODE_SNAPSHOT_TYPE = 6;
var FIRST_ORDERED_NODE_TYPE = 9;

function MakeGUID() {
    try {
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
    catch (err) {
        ReportError(err);
    }
}

var xml_special_to_escaped_one_map = {
'&': '&amp;',
'"': '&quot;',
'\'': '&apos;',
'<': '&lt;',
'>': '&gt;'
};

var escaped_one_to_xml_special_map = {
'&amp;': '&',
'&quot;': '"',
'&apos;': '\'',
'&lt;': '<',
'&gt;': '>'
};

function encodeXml(string) {
    return string.replace(/([\&"<>])/g, function(str, item) {
        return xml_special_to_escaped_one_map[item];
    });
}

function decodeXml(string) {
    return string.replace(/(&quot;|&lt;|&gt;|&amp;)/g, function(str, item) {
            return escaped_one_to_xml_special_map[item];
    });
}

var numb = '0123456789';
var lwr = 'abcdefghijklmnopqrstuvwxyz';
var upr = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';

function isValid(parm,val) {
if (parm == "") return true;
for (i=0; i<parm.length; i++) {
if (val.indexOf(parm.charAt(i),0) == -1) return false;
}
return true;
}

function isNumber(parm) { return !isNaN(parseFloat(parm)) && isFinite(parm); }
function isLower(parm) {return isValid(parm,lwr);}
function isUpper(parm) {return isValid(parm,upr);}
function isAlpha(parm) {return isValid(parm,lwr+upr);}
function isAlphanum(parm) {return isValid(parm,lwr+upr+numb);} 

function decodeFromHex(str){
	var r="";
	var e=str.length;
	var s;
	while(e>=0){
		s=e-3;
		r=String.fromCharCode("0x"+str.substring(s,e))+r;
		e=s;
	}
	//return r.substr(1, r.length - 2); //hacky but works
	return r;
}

function cleanString(s) {
  /*
  ** Remove NewLine, CarriageReturn and Tab characters from a String
  **   s  string to be processed
  ** returns new string
  */
  //alert(s.charCodeAt(0).toString() + ":" + s.charCodeAt(1).toString());
  r = "";
  for (i=0; i < s.length; i++) {
    if (s.charAt(i) != '\n' &&
        s.charAt(i) != '\r' &&
        s.charAt(i) != '\t' &&
		s.charCodeAt(i) != 0) {
      r += s.charAt(i);
      }
    }
	
	r = r.replace(/\u202A/g, '');
	r = r.replace(/\u202B/g, '');
	r = r.replace(/\u202D/g, '');
	r = r.replace(/\u202E/g, '');
	r = r.replace(/\u202C/g, '');
	r = r.replace(/\u200E/g, '');
	r = r.replace(/\u200F/g, '');
	r = r.replace(/\uA0/g, '');
		
	return r.replace(/^\s+|\s+$/g, '');
}

function deepClone(src) {
	function mixin(dest, source, copyFunc) {
		var name, s, i, empty = {};
		for(name in source){
			// the (!(name in empty) || empty[name] !== s) condition avoids copying properties in "source"
			// inherited from Object.prototype.	 For example, if dest has a custom toString() method,
			// don't overwrite it with the toString() method that source inherited from Object.prototype
			s = source[name];
			if(!(name in dest) || (dest[name] !== s && (!(name in empty) || empty[name] !== s))){
				dest[name] = copyFunc ? copyFunc(s) : s;
			}
		}
		return dest;
	}

	if(!src || typeof src != "object" || Object.prototype.toString.call(src) === "[object Function]"){
		// null, undefined, any non-object, or function
		return src;	// anything
	}
	if(src.nodeType && "cloneNode" in src){
		// DOM Node
		return src.cloneNode(true); // Node
	}
	if(src instanceof Date){
		// Date
		return new Date(src.getTime());	// Date
	}
	if(src instanceof RegExp){
		// RegExp
		return new RegExp(src);   // RegExp
	}
	var r, i, l;
	if(src instanceof Array){
		// array
		r = [];
		for(i = 0, l = src.length; i < l; ++i){
			if(i in src){
				r.push(deepClone(src[i]));
			}
		}
		// we don't clone functions for performance reasons
		//		}else if(d.isFunction(src)){
		//			// function
		//			r = function(){ return src.apply(this, arguments); };
	}else{
		// generic objects
		r = src.constructor ? new src.constructor() : {};
	}
	return mixin(r, src, deepClone);

}


//////////////////////////////////////////////////////////
function Enum(){}
Enum.INVALIDATEMODE_E = {NORMALINVALIDATE : 0, FLAGINVALIDATE : 1};
Enum.ATTRTYPE_E = { SINGLESELECT : 0, MULTISELECT : 1, BOOLEANSELECT : 2, EDIT : 3, STATIC : 4 };


var ATTRIBUTE_NODE = "Attribute";
var OBJECT_NODE = "Object";
var XSLT_TOP = "<?xml version=\"1.0\"?><xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"><xsl:output method=\"text\" omit-xml-declaration=\"yes\" encoding=\"UTF-8\"/>"
var XSLT_BOTTOM = "\"/></xsl:for-each></xsl:template></xsl:stylesheet>"

function ROMNode(id, factory) {
    this.m_xmlDoc = null;
    this.m_KnowledgeBase = null;
    if (id != undefined && id != null)
        id = id.replace(" ", "_");
    this.m_id = id;
    this.m_guid = MakeGUID();
    this.m_parent = null;
    this.m_children = new Array();
    this.m_friends = new Array();
    this.m_bChanged = true;
    this.m_lastContents = "";
    this.m_lastAttrContents = "";
    this.m_attrs = new Array();
    this.m_nodeValues = new Array();
    if (factory === undefined || factory == null) {
        this.ROMObjectFactory = function (id) {
            return new ROMNode(id);
        };
    }
    else {
        this.ROMObjectFactory = factory;
    }
}
ROMNode.prototype.constructor = ROMNode;

ROMNode.prototype.GetRoot = function () {
    try {
        var lastParent = this;
        var nextParent = null;
        do {
            nextParent = lastParent.GetParent();
            if (nextParent != null) {
                lastParent = nextParent;                
            }
        } while (nextParent != null);

        return lastParent;
    }
    catch (err) {
        ReportError(err);
    }
    return null;
}

ROMNode.prototype.GetParent = function () {
    return this.m_parent;
}

ROMNode.prototype.AddChildROMObject = function (child) {
    try {
        if (child.m_parent == null) {
            child.m_parent = this;
            this.m_children.push(child);
            this.m_bChanged = true;
            return true;
        }
    }
    catch (err) {
        ReportError(err);
    }
    return false;
}
	
ROMNode.prototype.AddFriend = function (friendObj) {
    try {
        if (friendObj != null) {
			var i = GetIndexOfItem(this.m_friends, friendObj);
			if (i < 0)
			{
				this.m_friends.push(friendObj);
				return true;
			}
        }
    }
    catch (err) {
        ReportError(err);
    }
    return false;
}
	
ROMNode.prototype.GetAllFriends = function() {
	return this.m_friends;
}

ROMNode.prototype._findObjects = function (id, recurs, resObject) {
    for (var child in this.m_children) {
        if (recurs && this.m_children[child].m_id == id && resObject != null) {
            resObject.push(this.m_children[child]);
            if (this.m_children[child].m_children.length > 0)
                this.m_children[child]._findObjects(resObject, recurs, resObject);
        }
    }
}

ROMNode.prototype._findAllChildObjects = function (resObject) {
    for (var child in this.m_children) {
        resObject.push(child);
        if (this.m_children[child].m_children.length > 0)
            this.m_children[child]._findAllChildObjects(resObject);
    }
}

ROMNode.prototype.GetAllChildren = function (recurs) {
    var retval = new Array();
    try {
        if (!recurs)
            retval = this.m_children;
        else {
            this._findAllChildObjects(retval);
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.FindObjects = function (xpath) {
    var retval = new Array();
    try {
        this._createXMLDoc();
        var nodes = new Array();
        if (IsIE()) {
            var res = this.m_xmlDoc.selectNodes(xpath);
            for (var i = 0; i < res.length; i++) {
                nodes.push(res[i]);
            }

            for (var nodeItr in nodes) {
                var objNode = nodes[nodeItr];
                var guid = objNode.attributes.getNamedItem("guid").nodeValue;
                if (guid.length > 0) {
                    var node = this.FindObjectByGUID(guid);
                    if (node != null)
                        retval.push(node);
                }
            }
        }
        else {
            if (DetectAndroidWebKit())
            {
                var evaluator = xpathParse(xpath);
                var res = evaluator.evaluate(this.m_xmlDoc).nodeSetValue();
                    for (var i = 0; i < res.length; i++) {
                    nodes.push(res[i]);
                }
                    
                for (var nodeItr in nodes) {
                    var objNode = nodes[nodeItr];
                    var guid = objNode.getAttribute("guid");
                    if (guid.length > 0) {
                        var node = this.FindObjectByGUID(guid);
                        if (node != null)
                            retval.push(node);
                    }
                }
            }
            else
            {
                var res = this.m_xmlDoc.evaluate(xpath, this.m_xmlDoc, null, ORDERED_NODE_SNAPSHOT_TYPE, null);
                for (var i = 0; i < res.snapshotLength; i++) {
                    nodes.push(res.snapshotItem(i));
                }

                for (var nodeItr in nodes) {
                    var objNode = nodes[nodeItr];
                    var guid = objNode.getAttribute("guid");
                    if (guid.length > 0) {
                        var node = this.FindObjectByGUID(guid);
                        if (node != null)
                            retval.push(node);
                    }
                }
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.FindFirstObject = function (xpath) {
  var objs = this.FindObjects(xpath);
  if (objs.length > 0)
    return objs[0];
  else
    return null;
}

ROMNode.prototype.FindAllObjectsByID = function (id, recurs) {
    var retval = new Array();
    try {
        if (this.m_id == id)
            retval.push(this);
        this._findObjects(id, recurs, retval);
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype._findObjectGUID = function (guid) {
    var retval = null;
    try {
        for (var child in this.m_children) {
            if (this.m_children[child].m_guid == guid) {
                retval = this.m_children[child];
                break;
            }
            else {
                retval = this.m_children[child]._findObjectGUID(guid);
                if (retval != null)
                    break;
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.FindObjectByGUID = function (guid) {
    var retval = null;
    try {
        var rootNode = this.GetRoot();
		if (rootNode.m_guid == guid)
			retval = rootNode;
		else
			retval = rootNode._findObjectGUID(guid);
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}


ROMNode.prototype.RemoveChildROMObject = function (child) {
    var retval = false;
    try {
        var i = GetIndexOfItem(this.m_children, child);
        if (i >= 0) {
            this.m_children.splice(i, 1);
            retval = true;
        }
        this.m_bChanged = retval;
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.RemoveFromParent = function() {
    try {
        if (this.m_parent != null) {
            return this.m_parent.RemoveChildROMObject(this);
        }
    }
    catch (err) {
        ReportError(err);
    }
    return false;
}
	
ROMNode.prototype.RemoveFriend = function (friendObj) {
    var retval = false;
    try {
		if (friendObj != null)
			{
			var i = GetIndexOfItem(this.m_friends, friendObj);
			if (i >= 0) {
				var i2 = GetIndexOfItem(friendObj.m_friends, this);
				if (i2 >= 0)
					friendObj.m_friends.splice(i2, 1);
				this.m_friends.splice(i, 1);
				retval = true;
			}
			this.m_bChanged = retval;
		}
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}
	
ROMNode.prototype.RemoveAllFriends = function () {
    var retval = false;
    try {			
		for (var i = 0; i < this.m_friends.length; i++)
		{
			retval = this.RemoveFriend(this.m_friends[i]);
		}				
	}        
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.DestroyROMObject = function () {
    var retval = true;
    try {
        //remove any references to self in parent node
        if (this.m_parent != null) {
            retval = this.m_parent.RemoveChildROMObject(this);
        }
			
		//clean friends
		for (var i = this.m_friends.length - 1; i >= 0; i--) {
			var friendNode = this.m_friends[i];
			if (friendNode != null)
			{
				friendNode.RemoveFriend(this);
			}
		}

        if (this.m_attrs != null)
            delete this.m_attrs;
        this.m_attrs = new Array();
        if (this.m_nodeValues != null)
            delete this.m_nodeValues;
        this.m_nodeValues = new Array();
        this.m_id = "";
        this.m_parent = null;
        this.m_bChanged = false;
        //trigger downstream destructors
        for (var i = this.m_children.length - 1; i >= 0; i--) {
            if (i < this.m_children.length) {
                var node = m_children[i];
                if (node)
                    delete node;
            }
        }
        if (this.m_children != null)
            delete this.m_children;
        this.m_children = new Array();
    }
    catch (err) {
        ReportError(err);
        retval = false;
    }
    return retval;
}

ROMNode.prototype.Clone = function () {
    var newNode = null;
    try {
        newNode = this.ROMObjectFactory(this.m_id);
        newNode.m_attrs = deepClone(this.m_attrs);
        newNode.m_nodeValues = deepClone(this.m_nodeValues);
        newNode.ROMObjectFactory = this.ROMObjectFactory;
        for (var i = 0; i < this.m_children.length; i++) {
			var node = this.m_children[i];
			if (node != null) {
				var newChild = node.Clone();
				newNode.AddChildROMObject(newChild);
			}                
        }
    }
    catch (err) {
        ReportError(err);
    }
    return newNode;
}

ROMNode.prototype.GetAttributeValue = function (id, name, immediate) {
    var retval = "";
    try {
        if (immediate === undefined)
            immediate = false;
        var bFound = false;
        if (this.m_attrs[id] != null) {
            if (this.m_attrs[id][name] != null) {
                retval = this.m_attrs[id][name];
                bFound = true;
            }
        }

        if (!immediate && !bFound) {
            if (this.m_parent != null) {
                retval = this.m_parent.GetAttributeValue(id, name, immediate);
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetAttribute = function (id, immediate) {
    var retval = "";
    try {
        if (immediate === undefined)
            immediate = false;
        retval = this.GetAttributeValue(id, "value", immediate);
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetAttributeExists = function (id, name) {
    var retval = false;
    try {
        if (name === undefined)
            name = "value";
        if (this.m_attrs[id] != null) {
            if (name != "value") {
                if (this.m_attrs[id][name] != null) {
                    retval = true;
                }
            }
            else
                retval = true;
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.SetAttribute = function (id, value) {
    try {
        return this.SetAttributeValue(id, "value", value);
    }
    catch (err) {
        ReportError(err);
    }
    return false;
}

ROMNode.prototype.SetAttributeValue = function (id, name, value) {
    try {
        if (this.m_attrs[id] === undefined)
            this.m_attrs[id] = new Array();

        this.m_attrs[id][name] = value;
        this.m_bChanged = true;
    }
    catch (err) {
        ReportError(err);
        return false;
    }
    return true;
}

ROMNode.prototype.RemoveAttribute = function (id, name) {
    var retval = false;
    if (name === undefined)
        name = "value";
    try {
        if (id in this.m_attrs) {
            if (name == null || name.length == 0 || name == "value") {
                delete this.m_attrs[id];
                retval = true;
            }
            else {
                if (name in this.m_attrs[id]) {
                    delete this.m_attrs[id][name];
                    retval = true;
                }
            }
            this.m_bChanged = retval;
        }            
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.SetROMObjectValue = function (name, value) {
    try {
        name = name.replace(" ", "_");
        this.m_nodeValues[name] = value;
        this.m_bChanged = true;
        return true;
    }
    catch (err) {
        ReportError(err);
        return false;
    }
}

ROMNode.prototype.GetROMObjectValue = function (name) {
    var retval = "";
    try {
        if (this.m_nodeValues[name] != null)
            retval = this.m_nodeValues[name];
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.RemoveROMObjectValue = function (name) {
    var retval = false;
    try {
        if (this.m_nodeValues[name] != null) {
            var i = GetIndexOfItem(this.m_nodeValues, name)
            if (i >= 0) {
                this.m_nodeValues.splice(i, 1);
                retval = true;
                this.m_bChanged = retval;
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
}

ROMNode.prototype.GetROMObjectID = function () {
    return this.m_id;
}

ROMNode.prototype.SetROMObjectID = function (id) {
    this.m_id = id;
}

ROMNode.prototype.GetROMGUID = function () {
    return this.m_guid;
}

ROMNode.prototype.GetAllAttributes = function () {
    return this.m_attrs;
}


//rules
ROMNode.prototype.LoadRules = function (knowledge_file) {
    try {
        this.m_KnowledgeBase = null;

        if (this.m_parent == null) //only the root will have the reference to the rules
        {
            this.m_KnowledgeBase = new KnowledgeBase(knowledge_file);
            if (this.m_KnowledgeBase != null) {
                this.m_KnowledgeBase.InputValueGetter = function (attrName, obj) {
                    return obj.GetATableInputValue(attrName);
                };
                return true;
            }
            else
                return false;
        }
    }
    catch (err) {
        ReportError(err);
    }
    return false;
}

ROMNode.prototype.LoadRulesFromString = function (xml) {
    try {
        this.m_KnowledgeBase = null;

        if (this.m_parent == null) //only the root will have the reference to the rules
        {
            this.m_KnowledgeBase = CreateKnowledgeBaseFromString(xml);
            if (this.m_KnowledgeBase != null) {
                this.m_KnowledgeBase.InputValueGetter = function (attrName, obj) {
                    return obj.GetATableInputValue(attrName);
                };
                return true;
            }
            else
                return false;
        }
    }
    catch (err) {
        ReportError(err);
    }
	return false;
}

ROMNode.prototype.SetTableDebugHandler = function (func) {
    if (this.m_KnowledgeBase != null)
        this.m_KnowledgeBase.DebugHandler = func;
}

ROMNode.prototype._getKnowledge = function () {
    var knowledge = null;
    try {
        var current = this;
        if (current.m_KnowledgeBase != null)
            knowledge = current.m_KnowledgeBase;
        else {
            while (knowledge == null) {
                var parent = current.GetParent();
                if (parent != null) {
                    if (parent.m_KnowledgeBase != null) {
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
    catch (err) {
        ReportError(err);
        return null;
    }
    return knowledge;
}
    
ROMNode.prototype.GetKnowledgeBase = function() {
    return this._getKnowledge();
}

ROMNode.prototype.EvaluateTableForAttr = function (evalTable, output, bGetAll) {
    try {
        var knowledge = this._getKnowledge();
        if (knowledge != null) {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            var retval = knowledge.EvaluateTableForAttr(evalTable, output, bGetAll, this);
            return retval;
        }
    }
    catch (err) {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype.EvaluateTable = function (evalTable, bGetAll) {
    try {
        var knowledge = this._getKnowledge();
        if (knowledge != null) {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            var retval = knowledge.EvaluateTable(evalTable, bGetAll, this);
            return retval;
        }
    }
    catch (err) {
        ReportError(err);
        return null;
    }
}	
	
ROMNode.prototype.EvaluateTableForAttrWithParam = function (evalTable, output, bGetAll, param) {
    try {
        var knowledge = this._getKnowledge();
        if (knowledge != null) {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            var retval = knowledge.EvaluateTableForAttrWithParam(evalTable, output, bGetAll, param, this);
            return retval;
        }
    }
    catch (err) {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype.EvaluateTableWithParam = function (evalTable, bGetAll, param) {
    try {
        var knowledge = this._getKnowledge();
        if (knowledge != null) {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            var retval = knowledge.EvaluateTableWithParam(evalTable, bGetAll, param, this);
            return retval;
        }
    }
    catch (err) {
        ReportError(err);
        return null;
    }
}
	
ROMNode.prototype.GetFirstTableResult = function(tableName, outputAttr)
{
	var retval = "";
		
	var retAll = this.EvaluateTableForAttr(tableName, outputAttr, this);
	if (retAll != null && retAll.length > 0)
		retval = retAll[0];
		
	return retval;
}

ROMNode.prototype.ReverseEvaluateTable = function (evalTable, bGetAll) {
    try {
        var knowledge = this._getKnowledge();
        if (knowledge != null) {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            var retval = knowledge.ReverseEvaluateTable(evalTable, bGetAll, this);
            return retval;
        }
    }
    catch (err) {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype.ReverseEvaluateTableForAttr = function (evalTable, output, bGetAll) {
    try {
        var knowledge = this._getKnowledge();
        if (knowledge != null) {
            if (bGetAll === undefined)
                bGetAll = knowledge.TableIsGetAll(evalTable);
            var retval = knowledge.ReverseEvaluateTableForAttr(evalTable, output, bGetAll, this);
            return retval;
        }
    }
    catch (err) {
        ReportError(err);
        return null;
    }
}

ROMNode.prototype.EvaluateXPATH = function (xpath, guid) {
    var retval = "";
    try {
        if (guid === undefined)
            guid = this.m_guid;
        var match = "<xsl:template match=\"/\"><xsl:for-each select=\"//Object[@guid=\'";
        match += guid + "\']\"><xsl:value-of select=\"";
        var xslt_text = XSLT_TOP + match + xpath + XSLT_BOTTOM;

        var xsltDoc = null;
        this._createXMLDoc();
        if (this.m_xmlDoc != null) {
            if (IsIE()) {
                xsltDoc = new ActiveXObject("Microsoft.XMLDOM");
                xsltDoc.async = "false";
                xsltDoc.loadXML(xslt_text);
                retval = this.m_xmlDoc.transformNode(xsltDoc);
            }
            else {
                if (DetectAndroidWebKit())
                {
                    xsltDoc = xmlParse(xslt_text);
                    retval = xsltProcess(this.m_xmlDoc.node, xsltDoc);
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
                    delete parser;
                    delete xsltProcessor;
                }
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetATableInputValue = function (input) {
    var retval = "";
    try {
        var xpathIndex = input.indexOf("xpath(");
        if (xpathIndex >= 0) {
            var cmdArg = input.substr(xpathIndex + 6, input.length - 7);
            retval = this.EvaluateXPATH(cmdArg, this.m_guid);
        }
        else if (input == "CLASSID") {
            retval = this.GetROMObjectID();
        }
        else {
            retval = this.GetAttribute(input, false);
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype.GetPossibleValues = function (evalTable, outputName) {
    try {
        var knowledge = this._getKnowledge();
        if (knowledge != null) {
            var res = knowledge.GetAllPossibleOutputs(evalTable, outputName);
            var retval = new Array();
            for (var i = 0; i < res.length; i++)
                retval.push(res[i]);
            return retval;
        }
    }
    catch (err) {
        ReportError(err);
    }
    return null;
}

    //IO
ROMNode.prototype._generateXML = function (bRegen, prettyprint) {
    var retval = "";
    if (prettyprint === undefined || prettyprint == null)
        prettyprint = false;
    
    try {
        if (bRegen) {
            //this object
            var beginObject = "<Object";
            beginObject += " id=\"";
            beginObject += this.m_id;
            beginObject += "\" guid=\"";
            beginObject += this.m_guid;
            beginObject += "\"";

            //object values
            var objAttrs = " ";
            for (var itObjValue in this.m_nodeValues) {
                objAttrs += itObjValue;
                objAttrs += "=\"";
                objAttrs += this.m_nodeValues[itObjValue];
                objAttrs += "\" ";
            }
            beginObject += objAttrs;
            beginObject += ">";
            retval += beginObject;

            if (this.m_bChanged) {
                var allAttrs = "";
                //attributes of this object
                if (prettyprint) //sort attrs alphabetically for easy reading
                {
                    var unsortedList = [];
                    for (var it in this.m_attrs)
                        unsortedList.push(it);
                    
                    var sortedList = unsortedList.sort();
                    for (var i = 0; i < sortedList.length; i++) {
                        var attrObject = this._generateAttrNode(sortedList[i]);
                        allAttrs += attrObject;
                    }
                }
                else {
                    for (var it in this.m_attrs) {
                        var attrObject = this._generateAttrNode(it);
                        allAttrs += attrObject;
                    }
                }
                retval += allAttrs;
                this.m_lastAttrContents = allAttrs;
            }
            else {
                retval += this.m_lastAttrContents;
            }

            //child objects
            for (var itNode in this.m_children) {
                var node = this.m_children[itNode];
                if (node != null)
                    retval += node._generateXML(node.m_bChanged, prettyprint);
            }

            retval += "</Object>";
            this.m_lastContents = retval;
        }
        else {
            retval = this.m_lastContents;
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype._generateAttrNode = function(id)
{
    var attrObject = "<Attribute id=\"";
    attrObject += id;
    attrObject += "\" ";
    for (var value in this.m_attrs[id]) {
        attrObject += value;
        attrObject += "=\"";
        attrObject += encodeXml(this.m_attrs[id][value]);
        attrObject += "\" ";
    }
    attrObject += "/>";
    return attrObject;
}

ROMNode.prototype._convertXMLDocToString = function (prettyprint) {
    var retval = "";
    try {
        if (this.m_xmlDoc != null) {
            if (IsIE()) {
                retval = this.m_xmlDoc.xml;
            }
            else 
            {
                if (DetectAndroidWebKit())
                {     
                    retval = xmlText(this.m_xmlDoc.node);
                }
                else
                {           
                    var serializer = new XMLSerializer();
                    //if (prettyprint && IsMoz())
                    //    retval = XML(serializer.serializeToString(this.m_xmlDoc)).toXMLString();
                    //else
                        retval = serializer.serializeToString(this.m_xmlDoc);
                }
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode.prototype._createXMLDoc = function (bForceLoad, prettyprint) {
    try {
		if (bForceLoad === undefined)
			bForceLoad = false;
			
        var bChanged = (bForceLoad || this._anyHasChanged());
        if (bChanged) {
            var genXML = this._generateXML(bChanged, prettyprint);
            if (IsIE()) {
                this.m_xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
                this.m_xmlDoc.async = "false";
                this.m_xmlDoc.setProperty("SelectionLanguage", "XPath");
                this.m_xmlDoc.loadXML(genXML);
            }
            else {
                if (DetectAndroidWebKit())
                {
                    this.m_xmlDoc = new ExprContext(xmlParse(genXML));
                }
                else
                {
                    var parser = new DOMParser();
                    this.m_xmlDoc = parser.parseFromString(genXML, "text/xml");
                    delete parser;
                }
            }

            this._setAllUnchanged();
        }
    }
    catch (err) {
        ReportError(err);
    }
}

ROMNode.prototype.SaveXML = function (prettyprint) {
    var retval = "";
    if (prettyprint === undefined)
        prettyprint = false;
    try {
        this._createXMLDoc(true, prettyprint);
        retval = this._convertXMLDocToString(prettyprint);
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}
	
ROMNode.LoadXML = function (xmlStr, factory) {
	var retval = null;
    try {
        var xmlDoc = null;
        var rootNode = null;
        if (IsIE()) {
            xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
            xmlDoc.async = "false";
            xmlDoc.setProperty("SelectionLanguage", "XPath");
            xmlDoc.loadXML(xmlStr);
            rootNode = xmlDoc.selectSingleNode("Object");
        }
        else {
            if (DetectAndroidWebKit())
            {
                xmlDoc = new ExprContext(xmlParse(xmlStr));
                var rootEval = xpathParse("Object");
                var rootNodes = rootEval.evaluate(xmlDoc).nodeSetValue();
                if (rootNodes != null && rootNodes.length > 0)
                    rootNode = rootNodes[0];
            }
            else
            {
                var parser = new DOMParser();
                xmlDoc = parser.parseFromString(xmlStr, "text/xml");
                rootNode = xmlDoc.evaluate("Object", xmlDoc, null, FIRST_ORDERED_NODE_TYPE, null);
				delete parser;
			}	
		}

        if (rootNode != null) {
            var objectNode = null;
            if (IsIE())
                objectNode = xmlDoc.selectSingleNode("Object");
            else {
                if (DetectAndroidWebKit())
                {
                    var objectNodeEval = xpathParse("Object");
                    var objectNodes = objectNodeEval.evaluate(xmlDoc).nodeSetValue();
                    if (objectNodes != null && objectNodes.length == 1)
                        objectNode = objectNodes[0];
                }
                else
                {
                    var objectNodeSnap = xmlDoc.evaluate("Object", xmlDoc, null, ORDERED_NODE_SNAPSHOT_TYPE, null);
                    if (objectNodeSnap != null && objectNodeSnap.snapshotLength == 1)
                        objectNode = objectNodeSnap.snapshotItem(0);
                }
            }

            if (objectNode != null)
                retval = ROMNode._buildObject(objectNode, xmlDoc, factory);
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMNode._buildObject = function (objectNode, xmlDoc, factory) {
    var newNode = null;
    try {
        var id = "";
        var guid = "";
        if (IsIE()) {
            id = objectNode.attributes.getNamedItem("id").nodeValue;
            guid = objectNode.attributes.getNamedItem("guid").nodeValue;
        }
        else {
            id = objectNode.getAttribute("id");
			guid = objectNode.getAttribute("guid");
        }

        newNode = factory(id);
        newNode.m_guid = guid;        

        //set object values
        for (var i = 0; i < objectNode.attributes.length; i++) {
            var objAttr = objectNode.attributes[i];
            if (objAttr.nodeName != "id" && objAttr.nodeName != "guid")
                newNode.SetROMObjectValue(objAttr.nodeName, objAttr.nodeValue);
        }

        if (IsIE()) {
            //set object attributes
            var attrNodes = objectNode.selectNodes("Attribute");
            for (var attrCnt = 0; attrCnt < attrNodes.length; attrCnt++) {
                var attrNode = attrNodes[attrCnt];
                var idAttr = attrNode.attributes.getNamedItem("id").nodeValue;
                for (var i = 0; i < attrNode.attributes.length; i++) {
                    var attr = attrNode.attributes[i];
                    var attrName = attr.nodeName;
                    var attrValue = attr.nodeValue;
                    if (attrName != "id")
                        newNode.SetAttributeValue(idAttr, attrName, attrValue);
                }
            }

            //children recursivley
            var childNodes = objectNode.selectNodes("Object");
            if (childNodes != null) for (var childCnt = 0; childCnt < childNodes.length; childCnt++) {
                var childNode = childNodes[childCnt];
                var newChildObject = ROMNode._buildObject(childNode, xmlDoc, factory);
                if (newChildObject != null && newNode != null) {
                    newNode.AddChildROMObject(newChildObject);
                }
            }
        }
        else {
            //set object attributes
            if (DetectAndroidWebKit())
            {
                var attrNodeEval = xpathParse("Attribute");
                var ctx = new ExprContext(objectNode);
                var attrNodes = attrNodeEval.evaluate(ctx).nodeSetValue();
                for (var attrCnt = 0; attrCnt < attrNodes.length; attrCnt++) {
                    var attrNode = attrNodes[attrCnt];
                    var idAttr = attrNode.getAttribute("id");
                    for (var i = 0; i < attrNode.attributes.length; i++) {
                        var attr = attrNode.attributes[i];
                        var attrName = attr.nodeName;
                        var attrValue = attr.nodeValue;
                        if (attrName != "id")
                            newNode.SetAttributeValue(idAttr, attrName, attrValue);
                    }
                }
                    
                //children recursivley
                var childNodeEval = xpathParse("Object");                    
                var childNodes = childNodeEval.evaluate(ctx).nodeSetValue();
                if (childNodes != null) for (var childCnt = 0; childCnt < childNodes.length; childCnt++) {
                    var childNode = childNodes[childCnt];
                    var newChildObject = ROMNode._buildObject(childNode, xmlDoc, factory);
                    if (newChildObject != null && newNode != null) {
                        newNode.AddChildROMObject(newChildObject);
                    }
                }
                delete ctx;
            }
            else
            {                    
                var attrNodes = xmlDoc.evaluate("Attribute", objectNode, null, ORDERED_NODE_SNAPSHOT_TYPE, null);
                for (var attrCnt = 0; attrCnt < attrNodes.snapshotLength; attrCnt++) {
                    var attrNode = attrNodes.snapshotItem(attrCnt);
                    var idAttr = attrNode.getAttribute("id");
                    for (var i = 0; i < attrNode.attributes.length; i++) {
                        var attr = attrNode.attributes[i];
                        var attrName = attr.nodeName;
                        var attrValue = attr.nodeValue;
                        if (attrName != "id")
                            newNode.SetAttributeValue(idAttr, attrName, attrValue);
                    }
                }                

                //children recursivley
                var childNodes = xmlDoc.evaluate("Object", objectNode, null, ORDERED_NODE_SNAPSHOT_TYPE, null);
                if (childNodes != null) for (var childCnt = 0; childCnt < childNodes.snapshotLength; childCnt++) {
                    var childNode = childNodes.snapshotItem(childCnt);
                    var newChildObject = ROMNode._buildObject(childNode, xmlDoc, factory);
                    if (newChildObject != null && newNode != null) {
                        newNode.AddChildROMObject(newChildObject);
                    }
                }
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    return newNode;
}

ROMNode.prototype._setAllUnchanged = function () {
    try {
        this.m_bChanged = false;
        for (var itNode in this.m_children) {
            itNode.m_bChanged = false;
        }
    }
    catch (err) {
        ReportError(err);
    }
}

ROMNode.prototype._anyHasChanged = function () {
    var retval = false;
    try {
        if (this.m_bChanged)
            retval = true;
        if (!retval) {
            for (var itNode in this.m_children) {
                if (itNode.m_bChanged) {
                    retval = true;
                    break;
                }
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}


// ROMDictionaryAttribute class////////////////////////////////////////////////////////////////
function ROMDictionaryAttribute() {
    this.Name = "";
    this.Description = "";
    this.DefaultValue = "";
    this.RuleTable = "";
    this.AttributeType = Enum.ATTRTYPE_E.SINGLESELECT;
    this.ValueChanged = false;
    this.ChangedByUser = false;
    this.Valid = false;
    this.Visible = true;
    this.Enabled = true;
    this.PossibleValues = new Array();
    this.AvailableValues = new Array();
    this.Value = "";
    this.Index = 0;
	this.m_guid = MakeGUID();
}

function CreateROMDictionaryAttribute() {
    var dictAttr = new ROMDictionaryAttribute();
	ActiveROMDictionaryAttributes[dictAttr.m_guid] = dictAttr;
    return dictAttr;
}

// ROMDictionary class////////////////////////////////////////////////////////////////
function CreateROMDictionary(node) {
    
	var dict = new ROMDictionary(node);    
	ActiveROMDictObjects[dict.m_guid] = dict;
    return dict;
}

function CreateROMDictionaryByGUID(guid) {
	var objNode = GetROMObject(guid);
	var dict = CreateROMDictionary(objNode);	
	if (dict != null)
		return dict.m_guid;
	else 
		return null;
}

function ROMDictionary(node) {
    this.m_context = node;
    this.m_tableName = "";
    this.m_dict = {};
    this.m_guid = MakeGUID();
}

ROMDictionary.prototype.GenerateTableDebugMessages = function (bGenerate) {
    try {
        if (this.m_context != null)
            this.m_context.GenerateTableDebugMessages(bGenerate);
    }
    catch (err) {
        ReportError(err);
    }
}

ROMDictionary.prototype.LoadDictionary = function (dictionaryTable) {
    try {
        this.m_dict = {};
        this.m_tableName = dictionaryTable;
        var res = this.m_context.EvaluateTable(this.m_tableName, true);
        var allNames = res["Name"];
        for (var i = 0; i < ArraySize(allNames); i++) {
            var dictAttr = new ROMDictionaryAttribute();
            dictAttr.Name = allNames[i];
            dictAttr.Index = i;

            if (res["DefaultValue"].length > 0 && res["DefaultValue"][i] != "~") dictAttr.DefaultValue = res["DefaultValue"][i];
            if (res["Description"].length > 0 && res["Description"][i] != "~") dictAttr.Description = res["Description"][i];
            if (res["RuleTable"].length > 0 && res["RuleTable"][i] != "~") dictAttr.RuleTable = res["RuleTable"][i];

            var strAttrType = "";
            if (res["AttributeType"].length > 0 && res["AttributeType"][i] != "~") strAttrType = res["AttributeType"][i];
            strAttrType = strAttrType.toUpperCase();

            if (strAttrType.length == 0 || strAttrType == "SINGLESELECT") {
                dictAttr.AttributeType = Enum.ATTRTYPE_E.SINGLESELECT;
            }
            else if (strAttrType == "MULTISELECT") {
                dictAttr.AttributeType = Enum.ATTRTYPE_E.MULTISELECT;
            }
            else if (strAttrType == "BOOLEAN") {
                dictAttr.AttributeType = Enum.ATTRTYPE_E.BOOLEANSELECT;
            }
            else if (strAttrType == "EDIT") {
                dictAttr.AttributeType = Enum.ATTRTYPE_E.EDIT;
            }
            else if (strAttrType == "STATIC") {
                dictAttr.AttributeType = Enum.ATTRTYPE_E.STATIC;
            }

            //on load, just set default values and possibilities
            //only set a default if there is no rules table and no current value
            var value = this.m_context.GetAttribute(dictAttr.Name, false);
            if (((value.length == 0 && dictAttr.RuleTable.length == 0) || dictAttr.AttributeType == Enum.ATTRTYPE_E.STATIC) && dictAttr.DefaultValue.length > 0 && dictAttr.DefaultValue != "~") {
                if (dictAttr.AttributeType == Enum.ATTRTYPE_E.BOOLEANSELECT)
                    this.m_context.SetAttribute(dictAttr.Name, dictAttr.DefaultValue.substr(0, 1));                    
                else
                    this.m_context.SetAttribute(dictAttr.Name, dictAttr.DefaultValue);
            }

            if (dictAttr.RuleTable.length > 0)
                dictAttr.PossibleValues = this.m_context.GetPossibleValues(dictAttr.RuleTable, dictAttr.Name);

            this.m_dict[dictAttr.Name] = dictAttr;
        }
    }
    catch (err) {
        ReportError(err);
    }
}

ROMDictionary.prototype.GetDictionaryAttr = function (dictAttrName) {
    var retval = null;
    try {
        if (dictAttrName in this.m_dict) {
            retval = this.m_dict[dictAttrName];
        }
    }
    catch (err) {
        ReportError(err);
    }
    return retval;
}

ROMDictionary.prototype.GetAllDictionaryAttrs = function () {
    try {
        return this.m_dict;
    }
    catch (err) {
        ReportError(err);
    }
    return null;
}

ROMDictionary.prototype.SetTableDebugHandler = function (func) {
    if (this.m_context != null && this.m_context.m_KnowledgeBase != null)
        this.m_context.m_KnowledgeBase.DebugHandler = func;
}

// LinearEngine class////////////////////////////////////////////////////////////////
function CreateLinearEngine(context, dictionaryTable) {

    var engine = new LinearEngine(context, dictionaryTable);
    ActiveEngineObjects[engine.m_guid] = engine;
    return engine;
}

function CreateLinearEngineByGUID(guid, dictionaryTable) {
	var objNode = GetROMObject(guid);
	var dict = CreateLinearEngine(objNode, dictionaryTable);	
	if (dict != null)
		return dict.m_guid;
	else 
		return null;
}

function LinearEngine(context, dictionaryTable) {
    this.INVISPREFIX = "^";
    this.DEFAULTPREFIX = "@";
    this.DISABLEPREFIX = "#";
    this.TBUATTR = "TBU_";
    
    this.InvalidateMode = Enum.INVALIDATEMODE_E.NORMALINVALIDATE;

    ROMDictionary.call(this, context);

    this.m_EvalList = new Array();
    this.m_mapTriggers = {};
    this.m_CurrentRecursion = 0;
    this.m_EvalListRecursChecker = new Array();
    this.m_EvalInternal = false;

    try {
        if (dictionaryTable === undefined)
            return;
        else {            
            this.CreateLinearEngine(dictionaryTable);
        }
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype = new ROMDictionary();

LinearEngine.prototype.GenerateTableDebugMessages = function (bGenerate) {
    try {
        if (this.m_context != null)
            this.m_context.GenerateTableDebugMessages(bGenerate);
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.ResetValueChanged = function () {
    try {
        for (var i = 0; i < this.m_EvalList.length; i++) {
            this.m_EvalList[i].ValueChanged = false;
        }
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.EvaluateForAttribute = function (dictAttrName, newValues, bEvalDependents) {
    //newValues could be a string or an array
    try {        
        if (!this.m_EvalInternal)
        {
			this.ResetValueChanged();
			
            //resets other atts when setting this one
            this.m_context.SetAttribute("currentattr", dictAttrName);
            var attrsToReset = this.m_context.EvaluateTableForAttr("reset", "attr");
            for (var i = 0; i < attrsToReset.length; i++)
            {
                this.m_context.SetAttribute(attrsToReset[i], "");
                this.RemoveTouchedByUser(attrsToReset[i]);
                if (this.m_dict[attrsToReset[i]].AvailableValues.length > 0)
                    this.m_dict[attrsToReset[i]].Valid = false;
            }
                    
            //resets an attr if it has not been touched by the user                    
            attrsToReset = this.m_context.EvaluateTableForAttr("reset_INCBU", "attr");
            for (var i = 0; i < attrsToReset.length; i++)
            {
                if (!this.IsTouchedByUser(attrsToReset[i]))
                {
                    this.m_context.SetAttribute(attrsToReset[i], "");
                    this.RemoveTouchedByUser(attrsToReset[i]);
                    if (this.m_dict[attrsToReset[i]].AvailableValues.length > 0)
                        this.m_dict[attrsToReset[i]].Valid = false;
                }
            }     
        }
        var newVals = new Array();
        if (typeof newValues == "string")
            newVals.push(newValues);
        else
            newVals = newValues;

        if (dictAttrName in this.m_dict) {
            this.m_dict[dictAttrName].ValueChanged = true;
            var bUserChanged = !this.m_EvalInternal;
            if (bUserChanged)
                this.SetTouchedByUser(dictAttrName);
            switch (this.m_dict[dictAttrName].AttributeType) {
                case Enum.ATTRTYPE_E.SINGLESELECT:
                    this.EvalSingleSelect(dictAttrName, newVals[0]);
                    break;

                case Enum.ATTRTYPE_E.MULTISELECT:
                    this.EvalMultiSelect(dictAttrName, newVals);
                    break;

                case Enum.ATTRTYPE_E.BOOLEANSELECT:
                    this.EvalBoolean(dictAttrName, newVals[0]);
                    break;

                case Enum.ATTRTYPE_E.EDIT:
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
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.EvaluateAll = function () {
    this.m_EvalInternal = true;
    try {
        this.ResetValueChanged();
        for (var i = 0; i < this.m_EvalList.length; i++) {
            var selectedValues = this.GetSelectedValues(this.m_EvalList[i]);
            this.EvaluateForAttribute(this.m_EvalList[i].Name, selectedValues, false);
        }
    }
    catch (err) {
        ReportError(err);
    }
    this.m_EvalInternal = false;
}

LinearEngine.prototype.GetEvalList = function () {
    try {
        return this.m_EvalList;
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.GetTriggers = function () {
    try {
        return this.m_mapTriggers;
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.DictionaryIsValid = function () {
    var retval = true;
    for (var i = 0; i < this.m_EvalList.length; i++) {
        if (!this.m_EvalList[i].Valid) {
            retval = false;
            break;
        }
    }
    return retval;
}

var MAX_RECURSION = 1000;
LinearEngine.prototype.OrderDictionary = function () {
    try {
        this.m_CurrentRecursion++;
        var evalOrderCopy = this.m_EvalList.slice();

        for (var inputAttr in this.m_mapTriggers) {
            var outputAttrs = this.m_mapTriggers[inputAttr];
            if (outputAttrs != null && outputAttrs.length > 0) {
                var lowestIndex = 0;
                var bFoundInputAttr = false;
                for (lowestIndex = 0; lowestIndex < this.m_EvalList.length; lowestIndex++) {
                    if (this.m_EvalList[lowestIndex].Name == inputAttr) {
                        bFoundInputAttr = true;
                        break;
                    }
                }

                var origIndex = lowestIndex;
                if (bFoundInputAttr) {
                    for (var i = 0; i < outputAttrs.length; i++) {
                        var attrOutput = outputAttrs[i];
                        if (attrOutput.length > 0) {
                            var currentIndex = 0;
                            for (currentIndex = 0; currentIndex < this.m_EvalList.length; currentIndex++) {
                                if (this.m_EvalList[currentIndex].Name == attrOutput)
                                    break;
                            }
                            if (currentIndex < lowestIndex)
                                lowestIndex = currentIndex;
                        }
                    }

                    //make the current input index lower than lowest output     
                    if (origIndex != lowestIndex) {
                        var attr = this.m_EvalList[origIndex];
                        this.m_EvalList.splice(origIndex, 1);
                        this.m_EvalList.splice(lowestIndex, 0, attr);
                    }
                }

            }
        }

        for (var i = 0; i < evalOrderCopy.length; i++) {
            //if the lists differ, do another sort, otherwise we should be done
            if (evalOrderCopy[i] != this.m_EvalList[i] && this.m_CurrentRecursion < MAX_RECURSION) {
                //does it match a previous result (are we flipping between a couple values, circular logic)
                if (this.m_CurrentRecursion % 2 == 0 && this.m_EvalListRecursChecker.length > 0) {
                    for (var j = 0; j < this.m_EvalListRecursChecker.length; j++) {
                        if (this.m_EvalList[j] != this.m_EvalListRecursChecker[j]) {
                            this.OrderDictionary();
                            break;
                        }
                    }
                }
                else
                    this.OrderDictionary();
            }
        }

        if (this.m_CurrentRecursion % 2 == 0) {
            this.m_EvalListRecursChecker = this.m_EvalList.slice(0);
        }
    }
    catch (err) {
        ReportError(err);
    }
}
        
LinearEngine.prototype.LoadTrackingAttrs = function () {
    var allAttrs = this.m_context.GetAllAttributes();
    for (var it in allAttrs)
    {
        if (it.indexOf(this.TBUATTR) >= 0)
        {
            var dictAttrName = it.substr(this.TBUATTR.length);
	        if (dictAttrName in this.m_dict)	               
	            this.m_dict[dictAttrName].ChangedByUser = true;	                
	    }
    }
}

LinearEngine.prototype.EvalBoolean = function (dictAttrName, newValue) {
    try {
        var res = this.m_context.EvaluateTableForAttr(this.m_dict[dictAttrName].RuleTable, dictAttrName);
        var availableValues = new Array();

        var prefixes = this.ParseOutPrefixes(Enum.ATTRTYPE_E.BOOLEANSELECT, res, availableValues);
        this.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);

        if (prefixes.length > 0 && prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
            this.m_dict[dictAttrName].Visible = false;
        else
            this.m_dict[dictAttrName].Visible = true;

        var currentValue = this.m_context.GetAttribute(dictAttrName, true);
        this.m_dict[dictAttrName].Valid = true;
        this.m_dict[dictAttrName].Enabled = true;

        //set the dictionary default on load
        var bOverrideDisabled = false;
        if (currentValue.length == 0) {
            if (this.m_dict[dictAttrName].DefaultValue != null && this.m_dict[dictAttrName].DefaultValue.length > 0)
                currentValue = this.m_dict[dictAttrName].DefaultValue;
            else
            {
                currentValue = "N";
                if (availableValues.length == 1 && availableValues[0].length > 0 &&
                    availableValues[0][0] == 'Y')
                    bOverrideDisabled = true;
            }
        }

        if (availableValues.length == 1) //you should only have one value
        {
            if (availableValues[0].length == 0 || availableValues[0] == "N") {
                this.m_context.SetAttribute(dictAttrName, "N");
                this.RemoveTouchedByUser(dictAttrName);
                this.m_dict[dictAttrName].Enabled = false;
            }
            else if (availableValues[0] == "YN") //allow Yes or No with a default of Y
            {
                if (!this.IsTouchedByUser(dictAttrName)) {
                    this.m_context.SetAttribute(dictAttrName, "Y");
                }
                else {
                    this.m_context.SetAttribute(dictAttrName, newValue);
                }
            }
            else if (availableValues[0] == "YY") //force Yes, no other choice
            {
                this.m_context.SetAttribute(dictAttrName, "Y");
                this.RemoveTouchedByUser(dictAttrName);
                this.m_dict[dictAttrName].Enabled = false;
            }
            else if (newValue.length == 1) //Y or N
            {
                this.m_context.SetAttribute(dictAttrName, newValue);
            }
            else if (currentValue.length == 0 && newValue.length == 0)
            {
                this.m_context.SetAttribute(dictAttrName, "Y");
                this.RemoveTouchedByUser(dictAttrName);
                this.m_dict[dictAttrName].Enabled = false;
            }
            else
            {
                if (currentValue == "N" && !bOverrideDisabled)
                    this.m_dict[dictAttrName].Enabled = false;
                this.m_context.SetAttribute(dictAttrName, currentValue);
            }
        }
        else if (newValue.length == 1) //Y or N
        {
            this.m_context.SetAttribute(dictAttrName, newValue);
        }
        else if (currentValue.length == 0 && newValue.length == 0)
        {
            this.m_context.SetAttribute(dictAttrName, "Y");
            this.RemoveTouchedByUser(dictAttrName);
            this.m_dict[dictAttrName].Enabled = false;
        }
        else
        {
            if (currentValue == "N" && !bOverrideDisabled)
                this.m_dict[dictAttrName].Enabled = false;
            this.m_context.SetAttribute(dictAttrName, currentValue);
        }

        this.m_dict[dictAttrName].Value = this.m_context.GetAttribute(dictAttrName, true);
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.EvalEdit = function (dictAttrName, newValue) {
    try {
        var res = this.m_context.EvaluateTableForAttr(this.m_dict[dictAttrName].RuleTable, dictAttrName);
        var availableValues = new Array();

        var prefixes = this.ParseOutPrefixes(Enum.ATTRTYPE_E.EDIT, res, availableValues);
        this.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
        this.m_dict[dictAttrName].Enabled = true;
        this.m_dict[dictAttrName].Valid = true;
        //if no rules table defined, allow any value
        if (this.m_dict[dictAttrName].RuleTable.length == 0)
        {
            this.m_context.SetAttribute(dictAttrName, newValue);
            this.m_dict[dictAttrName].Value = newValue;
            return;
        }

        var setTheValue = true;
        var currentValue = this.m_context.GetAttribute(dictAttrName, true);
        if (this.InvalidateMode != Enum.INVALIDATEMODE_E.NORMALINVALIDATE)
        {
            setTheValue = currentValue.length == 0;
        }        

        //set the dictionary default on load
        if (newValue.length == 0) {
            if (this.m_dict[dictAttrName].DefaultValue != null && this.m_dict[dictAttrName].DefaultValue.length > 0)
                newValue = this.m_dict[dictAttrName].DefaultValue;
        }

        if (availableValues.length == 1) {
            if (prefixes[0].indexOf(this.DISABLEPREFIX) >= 0) {
                this.m_dict[dictAttrName].Enabled = false;
                this.RemoveTouchedByUser(dictAttrName);
                this.m_context.SetAttribute(dictAttrName, availableValues[0]);
                this.m_dict[dictAttrName].Value = availableValues[0];
                return;
            }
            else if (prefixes[0].indexOf(this.DEFAULTPREFIX) >= 0 &&
                availableValues[0].indexOf('[') > 0) {
                var defaultValue = availableValues[0].substr(0, availableValues[0].indexOf('['));
                availableValues[0] = availableValues[0].substr(availableValues[0].indexOf('['));
                if (currentValue.length == 0)
                {
                    newValue = defaultValue;
                    this.RemoveTouchedByUser(dictAttrName);
                }
                this.m_dict[dictAttrName].Enabled = true;
            }
            else
                this.m_dict[dictAttrName].Enabled = true;

            //check table result for range
            if (availableValues[0].charAt(0) == '[') {
                var dNewValue = 0, dMin = 0, dMax = 0;
                var val = availableValues[0];
                val = val.replace("[", "");
                val = val.replace("]", "");

                var vals = val.split(",");
                dNewValue = parseFloat(newValue);
                if (isNaN(dNewValue))
                    dNewValue = 0;
                dMin = parseFloat(vals[0]);
                dMax = parseFloat(vals[1]);

                if (dNewValue <= dMax && dNewValue >= dMin) {
                    if (newValue.length == 0 || !isNumber(newValue))
                    {
                        var wstrMin = vals[0];
                        this.m_context.SetAttribute(dictAttrName, wstrMin);
                    }
                    else
                        this.m_context.SetAttribute(dictAttrName, newValue);
                }
                else if (dNewValue > dMax) {
                    if (setTheValue)
                    {
                        var wstrMax = vals[1];
                        this.m_context.SetAttribute(dictAttrName, wstrMax);
                    }
                    else
                        this.FlagAttrInvalid(dictAttrName);
                }
                else if (dNewValue < dMin) {
                    if (setTheValue)
                    {
                        var wstrMin = vals[0];
                        this.m_context.SetAttribute(dictAttrName, wstrMin);
                    }
                    else
                        this.FlagAttrInvalid(dictAttrName);
                }
            }
            else if (availableValues[0].length == 1 && availableValues[0][0] == 'Y') {
                this.m_context.SetAttribute(dictAttrName, newValue);
            }
            else if (availableValues[0].length == 1 && availableValues[0][0] == 'N') {
                this.m_context.SetAttribute(dictAttrName, "");
                this.RemoveTouchedByUser(dictAttrName);
                this.m_dict[dictAttrName].Enabled = false;
            }
            else if (availableValues[0].length == 2 && availableValues[0] == "YY") //user must enter something
            {
                this.m_context.SetAttribute(dictAttrName, newValue);
                this.m_dict[dictAttrName].Valid = newValue.length > 0;
            }
            else {
                this.m_context.SetAttribute(dictAttrName, availableValues[0]);
                this.RemoveTouchedByUser(dictAttrName);
            }
        }
        else if (availableValues.length == 0) {
            this.m_context.SetAttribute(dictAttrName, "");
            this.RemoveTouchedByUser(dictAttrName);
            this.m_dict[dictAttrName].Enabled = false;
        }
        else if (availableValues.length == 1 && availableValues[0].length > 0) {
            this.m_context.SetAttribute(dictAttrName, availableValues[0]);
            this.RemoveTouchedByUser(dictAttrName);
        }
        else if (availableValues.length > 0) {
            if (GetIndexOfItem(availableValues, newValue) >= 0) {
                this.m_context.SetAttribute(dictAttrName, newValue);
            }
            else {
                this.m_context.SetAttribute(dictAttrName, "");
                this.RemoveTouchedByUser(dictAttrName);
            }
        }

        if (prefixes.length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0) {
            this.m_dict[dictAttrName].Visible = false;
        }
        else {
            this.m_dict[dictAttrName].Visible = true;
        }

        this.m_dict[dictAttrName].Value = this.m_context.GetAttribute(dictAttrName, true);
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.EvalMultiSelect = function (dictAttrName, newValues) {
    try {
        //multi-select lists, checkbox lists
        var res = this.m_context.EvaluateTableForAttr(this.m_dict[dictAttrName].RuleTable, dictAttrName);
        var availableValues = new Array();

        var prefixes = this.ParseOutPrefixes(Enum.ATTRTYPE_E.MULTISELECT, res, availableValues);
        this.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
        this.m_dict[dictAttrName].Enabled = true;
        this.m_dict[dictAttrName].Valid = true;

        var currentValue = this.m_context.GetAttribute(dictAttrName, true);
        var currentValues = currentValue.split("|");
        var selectedValues = new Array();
                
        var setTheValue = true;
        var bFound = true;
        if (this.InvalidateMode != Enum.INVALIDATEMODE_E.NORMALINVALIDATE)
        {
            setTheValue = currentValues.length == 0;
            for (var it in currentValues)
            {
                bFound = GetIndexOfItem(availableValues, it) >= 0;
                if (bFound)
                    break;
            }
        }

        //set the dictionary default on load
        if (newValues.length == 0) {
            if (this.m_dict[dictAttrName].DefaultValue != null && this.m_dict[dictAttrName].DefaultValue.length > 0) {
                newValues = new Array();
                newValues.push(this.m_dict[dictAttrName].DefaultValue);
                this.RemoveTouchedByUser(dictAttrName);
            }
        }

        //if only one is available, force that selection now
        if (availableValues.length == 1) {
            selectedValues.push(availableValues[0]);
            this.RemoveTouchedByUser(dictAttrName);
            bFound = true;
        }

        //if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
        if (currentValues != null && (currentValues.length == 1 && (currentValues[0].length == 0 || GetIndexOfItem(availableValues, currentValues[0]) < 0)) && prefixes.length > 0) {
            for (var i = 0; i < prefixes.length; i++) {
                if (prefixes[i].indexOf(this.DEFAULTPREFIX) >= 0) {
                    selectedValues.push(availableValues[i]);
                    this.RemoveTouchedByUser(dictAttrName);
                    bFound = true;
                }
            }
        }
        else if (availableValues.length == 0) {
            this.m_dict[dictAttrName].Enabled = false;
        }

        if (selectedValues.length == 0 && currentValues.length > 0) //compare the new values to what is really available
        {
            for (var i = 0; i < newValues.length; i++) {
                if (GetIndexOfItem(availableValues, newValues[i]) >= 0) {
                    selectedValues.push(newValues[i]);
                }
            }
        }

        var finalValue = "";
        for (var i = 0; i < selectedValues.length; i++) {
            var val = selectedValues[i];
            if (finalValue.length > 0)
                finalValue += "|";
            finalValue += val;
        }

        if (finalValue != currentValue) {
            if (!setTheValue)
                setTheValue = bFound;
                        
            if (setTheValue)
                this.m_context.SetAttribute(dictAttrName, finalValue);
            else
            {
                if (availableValues.length > 0)
                    this.FlagAttrInvalid(dictAttrName);
                else
                    this.m_context.SetAttribute(dictAttrName, finalValue);
            }
        }

        this.m_dict[dictAttrName].Value = this.m_context.GetAttribute(dictAttrName, true);
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.EvalSingleSelect = function (dictAttrName, newValue) {
    try {
        var res = this.m_context.EvaluateTableForAttr(this.m_dict[dictAttrName].RuleTable, dictAttrName);
        var availableValues = new Array();
        this.m_dict[dictAttrName].Enabled = true;
        this.m_dict[dictAttrName].Valid = true;

        //if no rules table defined, allow any value
        if (this.m_dict[dictAttrName].RuleTable.length == 0) {
            this.m_context.SetAttribute(dictAttrName, newValue);
            availableValues.push(newValue);
            this.m_dict[dictAttrName].Value = newValue;
            this.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
            return;
        }

        //the list of results is what is available for selection in the control
        var prefixes = this.ParseOutPrefixes(Enum.ATTRTYPE_E.SINGLESELECT, res, availableValues);
        this.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);

        var currentValue = this.m_context.GetAttribute(dictAttrName, true);
        var bFound = GetIndexOfItem(availableValues, newValue) >= 0;
        var setTheValue = currentValue.length == 0 || this.InvalidateMode == Enum.INVALIDATEMODE_E.NORMALINVALIDATE;

        //set the dictionary default on load
        if (newValue.length == 0) {
            if (this.m_dict[dictAttrName].DefaultValue != null && this.m_dict[dictAttrName].DefaultValue.length > 0) {
                newValue = this.m_dict[dictAttrName].DefaultValue;
            }
        }

        //if only one is available, force that selection now
        if (availableValues.length == 1) {
            newValue = availableValues[0];
            this.RemoveTouchedByUser(dictAttrName);
            bFound = true;
        }

        //if the current value is "" or will become invalid, and an available value is prefixed with a "@" default, set it now
        if ((currentValue.length == 0 || GetIndexOfItem(availableValues, currentValue) < 0) && prefixes.length > 0) {
            for (var i = 0; i < prefixes.length; i++) {
                if (prefixes[i].indexOf(this.DEFAULTPREFIX) >= 0) {
                    if (this.InvalidateMode == Enum.INVALIDATEMODE_E.NORMALINVALIDATE || currentValue.length == 0)
                    {
                        newValue = availableValues[i];
                        this.RemoveTouchedByUser(dictAttrName);
                        bFound = true;
                        break;
                    }
                }
            }
        }
        else if (availableValues.length == 0) {
            this.m_dict[dictAttrName].Enabled = false;
        }

        if (prefixes.length > 0 && prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
            this.m_dict[dictAttrName].Visible = false;
        else
            this.m_dict[dictAttrName].Visible = true;

        if (newValue.length > 0 && bFound) {
            if (currentValue != newValue) {
                if (!setTheValue)
                    setTheValue = bFound;

                if (setTheValue)
                    this.m_context.SetAttribute(dictAttrName, newValue);
                else
                    this.FlagAttrInvalid(dictAttrName);
            }
        }
        else {
            if (this.m_dict[dictAttrName].Enabled == true)
                this.m_dict[dictAttrName].Valid = false;
            if (setTheValue)
                this.m_context.SetAttribute(dictAttrName, "");
            else
            {        
                if (availableValues.length > 0)
                    this.FlagAttrInvalid(dictAttrName);
                else
                    this.m_context.SetAttribute(dictAttrName, "");
            }
                    
            this.RemoveTouchedByUser(dictAttrName);
        }

        this.m_dict[dictAttrName].Value = this.m_context.GetAttribute(dictAttrName, true);
    }
    catch (err) {
        ReportError(err);
    }
}

LinearEngine.prototype.EvaluateDependencies = function (dictAttrName) {
    this.m_EvalInternal = true;
    try {
        if (dictAttrName in this.m_mapTriggers) {
            var attrsToEval = this.m_mapTriggers[dictAttrName];
            for (var i = 0; i < attrsToEval.length; i++) {
                var attrName = attrsToEval[i];
                if (attrName in this.m_dict) {
                    var selectedValues = this.GetSelectedValues(this.m_dict[attrName]);
                    var bWasChangedByUser = this.m_dict[attrName].ChangedByUser;
                    this.EvaluateForAttribute(attrName, selectedValues, true);
                    this.m_EvalInternal = true;
                    if (bWasChangedByUser) {
                        var bValuesRemainSame = true;
                        var newSelectedValues = this.GetSelectedValues(this.m_dict[attrName]);
                        if (newSelectedValues.length != selectedValues.length)
                            bValuesRemainSame = false;
                        else for (var j = 0; j < selectedValues.length; j++) {
                            if (GetIndexOfItem(newSelectedValues, selectedValues[j]) < 0) {
                                bValuesRemainSame = false;
                                break;
                            }
                        }

                        this.m_dict[attrName].ChangedByUser = bValuesRemainSame;
                    }
                }
            }
        }
    }
    catch (err) {
        ReportError(err);
    }
    this.m_EvalInternal = false;
}
        
LinearEngine.prototype.FlagAttrInvalid = function (dictAttrName) {
    this.m_dict[dictAttrName].ChangedByUser = false;
    this.m_dict[dictAttrName].Valid = false;
}
        
LinearEngine.prototype.IsTouchedByUser = function (dictAttrName) {
    return this.m_context.GetAttributeExists(this.TBUATTR + dictAttrName);
}
        
LinearEngine.prototype.SetTouchedByUser = function (dictAttrName) {
    this.m_dict[dictAttrName].ChangedByUser = true;
    this.m_context.SetAttribute(this.TBUATTR + dictAttrName, "Y");
}
        
LinearEngine.prototype.RemoveTouchedByUser = function (dictAttrName) {
    this.m_dict[dictAttrName].ChangedByUser = false;
    this.m_context.RemoveAttribute(this.TBUATTR + dictAttrName);
}      

//remove the special character flags from the values
LinearEngine.prototype.ParseOutPrefixes = function (AttributeType, values, valuesWithoutPrefixes) {
    var prefixes = new Array();
    try {
        var origValues = values;
        valuesWithoutPrefixes.splice(0);

        for (var i = 0; i < origValues.length; i++) {
            var val = origValues[i];
            var fullPrefix = "";

            //check for leadoff ^ indicating an invisible control
            if (val.indexOf(this.INVISPREFIX) == 0) {
                fullPrefix += this.INVISPREFIX;
                val = val.substr(this.INVISPREFIX.length);
            }

            //check for leadoff @ indicating a default
            if (val.indexOf(this.DEFAULTPREFIX) == 0) {
                fullPrefix += this.DEFAULTPREFIX;
                val = val.substr(this.DEFAULTPREFIX.length);
            }

            //check for leadoff # indicating a locked edit box
            if (AttributeType == Enum.ATTRTYPE_E.EDIT && val.indexOf(this.DISABLEPREFIX) == 0) {
                fullPrefix += this.DISABLEPREFIX;
                val = val.substr(this.DISABLEPREFIX.length);
            }

            prefixes.push(fullPrefix);
            valuesWithoutPrefixes.push(val);
        }
    }
    catch (err) {
        ReportError(err);
    }
    return prefixes;
}

LinearEngine.prototype.GetSelectedValues = function (attr) {
    var retval = new Array();
    try {
        var currentValue = this.m_context.GetAttribute(attr.Name, true);
        switch (attr.AttributeType) {
            case Enum.ATTRTYPE_E.SINGLESELECT:
                retval.push(currentValue);
                break;

            case Enum.ATTRTYPE_E.MULTISELECT:
                retval = currentValue.split("|");
                break;

            case Enum.ATTRTYPE_E.BOOLEANSELECT:
                retval.push(currentValue);
                break;

            case Enum.ATTRTYPE_E.EDIT:
                retval.push(currentValue);
                break;

            default:
                retval.push(currentValue);
                break;
        }
    }
    catch (err) {
        ReportError(err);
    }

    return retval;
}

LinearEngine.prototype.SetTableDebugHandler = function (func) {
    if (this.m_context != null && this.m_context.m_KnowledgeBase != null)
        this.m_context.m_KnowledgeBase.DebugHandler = func;
}

LinearEngine.prototype.ResetEngine = function()
{
    this.m_EvalList = new Array();
    this.m_mapTriggers = {};
    this.m_EvalListRecursChecker = new Array();
    this.CreateLinearEngine(this.m_tableName);
    for (var attr in this.m_dict) {
        this.m_context.SetAttribute(this.m_dict[attr].Name, "");        
    }
    this.LoadDictionary(this.m_tableName);
    this.EvaluateAll();
}

LinearEngine.prototype.CreateLinearEngine = function (dictionaryTable) {
    try {
        this.LoadDictionary(dictionaryTable);
        //open each attr in dict, load its dependency info to create m_EvalList, m_mapTriggers
        //build an initial list that matches the dictionary order
        for (var attr in this.m_dict) {
            this.m_EvalList.push(this.m_dict[attr]);
            //triggers
            var knowledge = this.m_context._getKnowledge();
            var deps = knowledge.GetInputDependencies(this.m_dict[attr].RuleTable);
            if (deps != null) for (var i = 0; i < deps.length; i++) {
                var key = deps[i];
                if (key in this.m_mapTriggers) {
                    var hasAttr = false;
                    for (var j = 0; j < this.m_mapTriggers[key].length; j++) {
                        if (this.m_mapTriggers[key][j] == this.m_dict[attr].Name)
                            hasAttr = true;
                    }

                    if (!hasAttr)
                        this.m_mapTriggers[key].push(this.m_dict[attr].Name);
                }
                else if (key in this.m_dict) {
                    var newArray = new Array();
                    newArray.push(this.m_dict[attr].Name);
                    this.m_mapTriggers[key] = newArray;
                }
            }
        }

        this.LoadTrackingAttrs();
        //based on the triggers, re-order the dictionary
        this.m_CurrentRecursion = 0;
        this.m_EvalInternal = false;
        this.OrderDictionary();
    }
    catch (err) {
        ReportError(err);
    }
}

