/*
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

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
    var ie = (typeof window.ActiveXObject != 'undefined');
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
// flash support //////////////////////////////////////////////////////////////
var ActiveROMObjects = new Array(); //for flash access, guid keyed objects
var ActiveROMDictionaryAttributes = new Array();
var ActiveROMDictObjects = new Array();
var ActiveEngineObjects = new Array();
function GetROMObject(guid) {
    if (guid in ActiveROMObjects)
        return ActiveROMObjects[guid];	
    else
        return null;
}
function UpdateROMGUID(guid) {
	var ROM = null;
	if (guid in ActiveROMObjects)
		ROM = ActiveROMObjects[guid];
	
	if (ROM != null)
	{
		ActiveROMObjects[ROM.m_guid] = ROM;
		return ROM.m_guid;
	}
	else
		return guid;
}
function GetROMObjectGenericGUID(guid) {
	var ROM = null;
    if (guid in ActiveROMObjects)
        ROM = ActiveROMObjects[guid];	

	if (ROM != null)
	{
		var obj = new Object();
		obj.m_id = ROM.m_id;
		obj.m_guid = ROM.m_guid;
		return obj;
	}
	return null;
}
function GetROMObjectGeneric(obj) {
	if (obj != null)
	{
		if (obj.m_guid != null)
			return GetROMObjectGenericGUID(obj.m_guid);
	}
	return null;
}

function GetROMObjectArray(arr)
{
	var retval = new Array();
	for (var idx in arr)
	{
		var obj = new Object();
		obj.m_id = arr[idx].m_id;
		obj.m_guid = arr[idx].m_guid;
		retval.push(obj);
	}
	return retval;
}
function DestroyROMObject(guid) {
    if (ActiveROMObjects != null && ActiveROMObjects[guid] != null)
        delete ActiveROMObjects[guid];
}
function CleanROMObjects() {
    for (var guid in ActiveROMObjects) {
        DestroyROMObject(guid)
    }
    if (ActiveROMObjects != null) 
        delete ActiveROMObjects;
    ActiveROMObjects = new Array();
}

function GetROMDictAttrObject(guid) {
    if (guid in ActiveROMDictObjects)
        return ActiveROMDictionaryAttributes[guid];
    else
        return null;
}
function GetROMDictAttrArray(arr)
{
	var retval = new Array();
	for (var idx in arr)
	{
		var obj = new Object();
		obj.Name = arr[idx].Name;
		obj.Description = arr[idx].Description;
		obj.DefaultValue = arr[idx].DefaultValue;
		obj.RuleTable = arr[idx].RuleTable;		
		obj.AttributeType = arr[idx].AttributeType;
		obj.ValueChanged = arr[idx].ValueChanged;
		obj.ChangedByUser = arr[idx].ChangedByUser;
		obj.Valid = arr[idx].Valid;
		obj.Visible = arr[idx].Visible;
		obj.Enabled = arr[idx].Enabled;
		obj.PossibleValues = arr[idx].PossibleValues;
		obj.AvailableValues = arr[idx].AvailableValues;		
		obj.m_guid = arr[idx].m_guid;
		retval.push(obj);
	}
	return retval;
}

function DestroyROMDictAttrObject(guid) {
    if (ActiveROMDictionaryAttributes != null && ActiveROMDictionaryAttributes[guid] != null)
        delete ActiveROMDictionaryAttributes[guid];
}
function CleanROMDictAttrObjects() {
    for (var guid in ActiveROMDictionaryAttributes) {
        DestroyROMDictObject(guid)
    }
    if (ActiveROMDictionaryAttributes != null) 
        delete ActiveROMDictionaryAttributes;
    ActiveROMDictionaryAttributes = new Array();
}

function GetROMDictObject(guid) {
    if (guid in ActiveROMDictObjects)
        return ActiveROMDictObjects[guid];
    else
        return null;
}
function DestroyROMDictObject(guid) {
    if (ActiveROMDictObjects != null && ActiveROMODictbjects[guid] != null)
        delete ActiveROMDictObjects[guid];
}
function CleanROMDictObjects() {
    for (var guid in ActiveROMDictObjects) {
        DestroyROMDictObject(guid)
    }
    if (ActiveROMDictObjects != null) 
        delete ActiveROMDictObjects;
    ActiveROMDictObjects = new Array();
}

function GetEngineObject(guid) {
    if (guid in ActiveEngineObjects)
        return ActiveEngineObjects[guid];
    else
        return null;
}
function DestroyEngineObject(guid) {
    if (ActiveEngineObjects != null && ActiveEngineObjects[guid] != null)
        delete ActiveEngineObjects[guid];
}
function CleanEngineObjects() {
    for (var guid in ActiveEngineObjects) {
        DestroyEngineObject(guid)
    }
    if (ActiveEngineObjects != null) 
        delete ActiveEngineObjects;
    ActiveEngineObjects = new Array();
}
function DictionaryToObjArray(dict) {
	var objArray = new Array();
	if (dict != null) for (var key in dict)
	{
		var obj = new Object();
		obj.key = key;
		obj.values = dict[key];
		objArray.push(obj);
	}
	return objArray;
}
function AttributeDictionaryToObjArray(dict) {
	var objArray = new Array();
	if (dict != null) for (var key in dict)
	{
		var obj = new Object();
		obj.key = key;
		var attrValuePairs = new Array();
		var kvp = dict[key];
		for (var name in kvp)
		{
			attrValuePairs.push(name);
			attrValuePairs.push(kvp[name]);
		}
		obj.values = attrValuePairs;
		objArray.push(obj);
	}
	return objArray;
}

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

//////////////////////////////////////////////////////////
function Enum(){}
Enum.INVALIDATEMODE_E = {NORMALINVALIDATE : 0, FLAGINVALIDATE : 1};
Enum.ATTRTYPE_E = { SINGLESELECT : 0, MULTISELECT : 1, BOOLEANSELECT : 2, EDIT : 3, STATIC : 4 };


var ATTRIBUTE_NODE = "Attribute";
var OBJECT_NODE = "Object";
var XSLT_TOP = "<?xml version=\"1.0\"?><xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"><xsl:output method=\"text\" omit-xml-declaration=\"yes\" encoding=\"UTF-8\"/>"
var XSLT_BOTTOM = "\"/></xsl:for-each></xsl:template></xsl:stylesheet>"
function CreateROMNode(id) {
    if (id === undefined)
        id = "";
		
    var retval = new ROMNode(id);
    ActiveROMObjects[retval.m_guid] = retval;
    return retval;
}

function ROMNode(id) {
    this.m_xmlDoc = null;
    this.m_KnowledgeBase = null;
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

    this.GetRoot = function () {
        try {
            var nextParent = this;
            do {
                if (nextParent != null)
                    nextParent = nextParent.GetParent();
            } while (nextParent != null);

            if (nextParent == null)
                nextParent = this;
            return nextParent;
        }
        catch (err) {
            ReportError(err);
        }
        return null;
    }

    this.GetParent = function () {
        return this.m_parent;
    }

    this.AddChildROMObject = function (child) {
        try {
            if (child.m_parent == null) {
                child.m_parent = this;
                this.m_children.push(child);
                return true;
            }
        }
        catch (err) {
            ReportError(err);
        }
        return false;
    }
	
	this.AddFriend = function (friendObj) {
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
	
	this.GetAllFriends = function() {
		return this.m_friends;
	}

    this._findObjects = function (id, recurs, resObject) {
        for (var child in this.m_children) {
            if (recurs && this.m_children[child].m_id == id && resObject != null) {
                resObject.push(this.m_children[child]);
                if (this.m_children[child].m_children.length > 0)
                    this.m_children[child]._findObjects(resObject, recurs, resObject);
            }
        }
    }

    this._findAllChildObjects = function (resObject) {
        for (var child in this.m_children) {
            resObject.push(child);
            if (this.m_children[child].m_children.length > 0)
                this.m_children[child]._findAllChildObjects(resObject);
        }
    }

    this.GetAllChildren = function (recurs) {
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

    this.FindObjects = function (xpath) {
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

    this.FindAllObjectsByID = function (id, recurs) {
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

    this._findObjectGUID = function (guid) {
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

    this.FindObjectByGUID = function (guid) {
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


    this.RemoveChildROMObject = function (child) {
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

    this.RemoveFromParent = function() {
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
	
	this.RemoveFriend = function (friendObj) {
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
	
	this.RemoveAllFriends = function () {
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

    this.DestroyROMObject = function () {
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

    this.Clone = function () {
        var newNode = null;
        try {
            newNode = new ROMNode(this.m_id);
            newNode.m_attrs = this.m_attrs.slice(0);
            newNode.m_nodeValues = this.m_nodeValues.slice(0);
            newNode.m_bChanged = false;
            for (var i = this.m_children.length - 1; i >= 0; i--) {
                if (i < this.m_children.length) {
                    var node = this.m_children[i];
                    if (node != null) {
                        var newChild = node.Clone();
                        newNode.AddChildROMObject(newChild);
                    }
                }
            }
        }
        catch (err) {
            ReportError(err);
        }
        return newNode;
    }

    this.GetAttributeValue = function (id, name, immediate) {
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

    this.GetAttribute = function (id, immediate) {
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

    this.GetAttributeExists = function (id, name) {
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

    this.SetAttribute = function (id, value) {
        try {
            return this.SetAttributeValue(id, "value", value);
        }
        catch (err) {
            ReportError(err);
        }
        return false;
    }

    this.SetAttributeValue = function (id, name, value) {
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

    this.RemoveAttribute = function (id, name) {
        var retval = false;
        if (name === undefined)
            name = "value";
        try {
            if (this.m_attrs[id] != null) {
                if (name == null || name.length == 0 || name == "value") {
                    var i = GetIndexOfItem(this.m_attrs, id);
                    this.m_attrs.splice(i, 1);
                    retval = true;
                }
                else {
                    if (this.m_attrs[id][name] != null) {
                        var j = GetIndexOfItem(this.m_attrs[id], name);
                        this.m_attrs[id].splice(j, 1);
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

    this.SetROMObjectValue = function (name, value) {
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

    this.GetROMObjectValue = function (name) {
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

    this.RemoveROMObjectValue = function (name) {
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

    this.GetROMObjectID = function () {
        return this.m_id;
    }

    this.SetROMObjectID = function (id) {
        this.m_id = id;
    }

    this.GetROMGUID = function () {
        return this.m_guid;
    }

    this.GetAllAttributes = function () {
        return this.m_attrs;
    }


    //rules
    this.LoadRules = function (knowledge_file) {
        try {
            this.m_KnowledgeBase = new KnowledgeBase(knowledge_file);
            if (this.m_KnowledgeBase != null)
                return true;
            else
                return false;
        }
        catch (err) {
            ReportError(err);
        }
    }

    this.LoadRulesFromString = function (xml) {
        try {
            this.m_KnowledgeBase = CreateKnowledgeBaseFromString(xml);
            if (this.m_KnowledgeBase != null)
                return true;
            else
                return false;
        }
        catch (err) {
            ReportError(err);
        }
		return false;
    }

    this.SetRulesDebugHandler = function (func) {
        if (this.m_KnowledgeBase != null)
            this.m_KnowledgeBase.SetDebugHandler(func);
    }

    this.GenerateTableDebugMessages = function (bGenerate) {
        try {
            if (this.m_KnowledgeBase != null)
                this.m_KnowledgeBase.GenerateDebugMessages(bGenerate);
        }
        catch (err) {
            ReportError(err);
        }
    }

    this.GetTableDebugMessages = function () {
        try {
            if (this.m_KnowledgeBase != null)
                return this.m_KnowledgeBase.GetDebugMessages();
        }
        catch (err) {
            ReportError(err);
        }
        return "";
    }

    this._getKnowledge = function () {
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
    
    this.GetKnowledgeBase = function() {
        return this._getKnowledge();
    }

    this.EvaluateTableForAttr = function (evalTable, output, bGetAll) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                if (bGetAll === undefined)
                    bGetAll = knowledge.TableIsGetAll(evalTable);
                this.LoadInputs(evalTable);
                var retval = knowledge.EvaluateTableForAttr(evalTable, output, bGetAll);
                return retval;
            }
        }
        catch (err) {
            ReportError(err);
            return null;
        }
    }

    this.EvaluateTable = function (evalTable, bGetAll) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                if (bGetAll === undefined)
                    bGetAll = knowledge.TableIsGetAll(evalTable);
                this.LoadInputs(evalTable);
                var retval = knowledge.EvaluateTable(evalTable, bGetAll);
                return retval;
            }
        }
        catch (err) {
            ReportError(err);
            return null;
        }
    }	
	
	this.EvaluateTableForAttrWithParam = function (evalTable, output, param, bGetAll) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                if (bGetAll === undefined)
                    bGetAll = knowledge.TableIsGetAll(evalTable);
                this.LoadInputs(evalTable);
                var retval = knowledge.EvaluateTableForAttrWithParam(evalTable, output, param, bGetAll);
                return retval;
            }
        }
        catch (err) {
            ReportError(err);
            return null;
        }
    }

    this.EvaluateTableWithParam = function (evalTable, param, bGetAll) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                if (bGetAll === undefined)
                    bGetAll = knowledge.TableIsGetAll(evalTable);
                this.LoadInputs(evalTable);
                var retval = knowledge.EvaluateTableWithParam(evalTable, param, bGetAll);
                return retval;
            }
        }
        catch (err) {
            ReportError(err);
            return null;
        }
    }
	
	this.GetEvalParameter = function()
	{
		try
		{
			var knowledge = this._getKnowledge();
            if (knowledge != null) {
				var retval = knowledge.GetEvalParameter();
				return retval;
			}
		}
		catch(err) {
			ReportError(err);
            return null;
		}
	}
	
	this.GetFirstTableResult = function(tableName, outputAttr)
	{
		var retval = "";
		
		var retAll = this.EvaluateTableForAttr(tableName, outputAttr);
		if (retAll != null && retAll.length > 0)
			retval = retAll[0];
		
		return retval;
	}

    this.ReverseEvaluateTable = function (evalTable, bGetAll) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                if (bGetAll === undefined)
                    bGetAll = knowledge.TableIsGetAll(evalTable);
                this.LoadOutputs(evalTable);
                var retval = knowledge.ReverseEvaluateTable(evalTable, bGetAll);
                return retval;
            }
        }
        catch (err) {
            ReportError(err);
            return null;
        }
    }

    this.ReverseEvaluateTableForAttr = function (evalTable, output, bGetAll) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                if (bGetAll === undefined)
                    bGetAll = knowledge.TableIsGetAll(evalTable);
                this.LoadOutputs(evalTable);
                var retval = knowledge.ReverseEvaluateTableForAttr(evalTable, output, bGetAll);
                return retval;
            }
        }
        catch (err) {
            ReportError(err);
            return null;
        }
    }   

    this.LoadInputs = function (evalTable) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                var inputs = knowledge.GetInputDependencies(evalTable);
                if (inputs != null) for (var i = 0; i < inputs.length; i++) {
                    var value = this.GetATableInputValue(inputs[i]);
                    knowledge.SetInputValue(inputs[i], value);
                }
            }
        }
        catch (err) {
            ReportError(err);
        }
    }

    this.LoadOutputs = function (evalTable) {
        try {
            var knowledge = this._getKnowledge();
            if (knowledge != null) {
                var outputs = knowledge.GetOutputAttrs(evalTable);
                if (outputs != null) for (var i = 0; i < outputs.length; i++) {
                    var value = this.GetATableInputValue(inputs[i]);
                    knowledge.SetInputValue(inputs[i], value);
                }
            }
        }
        catch (err) {
            ReportError(err);
        }
    }

    this.EvaluateXPATH = function (xpath, guid) {
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

    this.GetATableInputValue = function (input) {
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

    this.GetPossibleValues = function (evalTable, outputName) {
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
    this._generateXML = function (bRegen) {
        var retval = "";
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
                    for (var it in this.m_attrs) {
                        var attrObject = "<Attribute id=\"";
                        attrObject += it;
                        attrObject += "\" ";
                        for (var itValue in this.m_attrs[it]) {
                            attrObject += itValue;
                            attrObject += "=\"";
                            attrObject += this.m_attrs[it][itValue];
                            attrObject += "\" ";
                        }
                        attrObject += "/>";
                        allAttrs += attrObject;
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
                        retval += node._generateXML(node.m_bChanged);
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

    this._convertXMLDocToString = function (indented) {
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
                        if (indented && IsMoz())
                            retval = XML(serializer.serializeToString(this.m_xmlDoc)).toXMLString();
                        else
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

    this._createXMLDoc = function (bForceLoad) {
        try {
			if (bForceLoad === undefined)
				bForceLoad = false;
			
            var bChanged = (bForceLoad || this._anyHasChanged());
            if (bChanged) {
                var genXML = this._generateXML(bChanged);
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

    this.SaveXML = function (indented) {
        var retval = "";
        if (indented === undefined)
            indented = false;
        try {
            this._createXMLDoc(true);
            retval = this._convertXMLDocToString(indented);
        }
        catch (err) {
            ReportError(err);
        }
        return retval;
    }
	
    this.LoadXML = function (xmlStr) {
		var retval = false;
        try {
            var rootNode = null;
            if (IsIE()) {
                this.m_xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
                this.m_xmlDoc.async = "false";
                this.m_xmlDoc.setProperty("SelectionLanguage", "XPath");
                this.m_xmlDoc.loadXML(xmlStr);
                rootNode = this.m_xmlDoc.selectSingleNode("Object");
            }
            else {
                if (DetectAndroidWebKit())
                {
                    this.m_xmlDoc = new ExprContext(xmlParse(xmlStr));
                    var rootEval = xpathParse("Object");
                    var rootNodes = rootEval.evaluate(this.m_xmlDoc).nodeSetValue();
                    if (rootNodes != null && rootNodes.length > 0)
                        rootNode = rootNodes[0];
                }
                else
                {
                    var parser = new DOMParser();
				    this.m_xmlDoc = null;
                    this.m_xmlDoc = parser.parseFromString(xmlStr, "text/xml");
				    rootNode = this.m_xmlDoc.evaluate("Object", this.m_xmlDoc, null, FIRST_ORDERED_NODE_TYPE, null);			
				    delete parser;
				}	
			}

            if (rootNode != null) {
                var objectNode = null;
                if (IsIE())
                    objectNode = this.m_xmlDoc.selectSingleNode("Object");
                else {
                    if (DetectAndroidWebKit())
                    {
                        var objectNodeEval = xpathParse("Object");
                        var objectNodes = objectNodeEval.evaluate(this.m_xmlDoc).nodeSetValue();
                        if (objectNodes != null && objectNodes.length == 1)
                            objectNode = objectNodes[0];
                    }
                    else
                    {
                        var objectNodeSnap = this.m_xmlDoc.evaluate("Object", this.m_xmlDoc, null, ORDERED_NODE_SNAPSHOT_TYPE, null);
                        if (objectNodeSnap != null && objectNodeSnap.snapshotLength == 1)
                            objectNode = objectNodeSnap.snapshotItem(0);
                    }
                }

                if (objectNode != null && this._buildObject(objectNode, null) != null)
                    retval = true;
            }
        }
        catch (err) {
            ReportError(err);
        }
        return retval;
    }

    this._buildObject = function (objectNode, parent) {
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

            if (parent == null) {
                this.DestroyROMObject();
                this.m_id = id;
                this.m_guid = guid;
                newNode = this;
            }
            else {
                newNode = new ROMNode(id);
                newNode.m_guid = guid;
            }

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
                    var newChildObject = this._buildObject(childNode, this);
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
                        var newChildObject = this._buildObject(childNode, this);
                        if (newChildObject != null && newNode != null) {
                            newNode.AddChildROMObject(newChildObject);
                        }
                    }
                    delete ctx;
                }
                else
                {                    
                    var attrNodes = this.m_xmlDoc.evaluate("Attribute", objectNode, null, ORDERED_NODE_SNAPSHOT_TYPE, null);
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
                    var childNodes = this.m_xmlDoc.evaluate("Object", objectNode, null, ORDERED_NODE_SNAPSHOT_TYPE, null);
                    if (childNodes != null) for (var childCnt = 0; childCnt < childNodes.snapshotLength; childCnt++) {
                        var childNode = childNodes.snapshotItem(childCnt);
                        var newChildObject = this._buildObject(childNode, this);
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

    this._setAllUnchanged = function () {
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

    this._anyHasChanged = function () {
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
    
    return this;
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
    this.m_dict = new Array();
    this.m_guid = MakeGUID();

    this.GenerateTableDebugMessages = function (bGenerate) {
        try {
            if (this.m_context != null)
                this.m_context.GenerateTableDebugMessages(bGenerate);
        }
        catch (err) {
            ReportError(err);
        }
    }

    this.GetTableDebugMessages = function () {
        try {
            if (this.m_context != null)
                return this.m_context.GetDebugMessages();
        }
        catch (err) {
            ReportError(err);
        }
        return "";
    }

    this.LoadDictionary = function (dictionaryTable) {
        try {
            this.m_dict = new Array();
            var res = this.m_context.EvaluateTable(dictionaryTable, true);
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

    this.GetDictionaryAttr = function (dictAttrName) {
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

    this.GetAllDictionaryAttrs = function () {
        try {
            return this.m_dict;
        }
        catch (err) {
            ReportError(err);
        }
        return null;
    }

    this.SetRulesDebugHandler = function (func) {
        if (this.m_context != null && this.m_context.m_KnowledgeBase != null)
            this.m_context.m_KnowledgeBase.SetDebugHandler(func);
    }
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

    this.base = new ROMDictionary(context);

    this.m_EvalList = new Array();
    this.m_mapTriggers = new Array();
    this.m_CurrentRecursion = 0;
    this.m_EvalListRecursChecker = new Array();
    this.m_EvalInternal = false;
    this.m_EvalListRecursChecker = null;
    this.m_guid = this.base.m_guid;

    try {

        this.GenerateTableDebugMessages = function (bGenerate) {
            try {
                if (this.base.m_context != null)
                    this.base.m_context.GenerateTableDebugMessages(bGenerate);
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.GetTableDebugMessages = function () {
            try {
                if (this.base.m_context != null)
                    return this.base.m_context.GetDebugMessages();
            }
            catch (err) {
                ReportError(err);
            }
            return "";
        }
        
        this.GetAllDictionaryAttrs = function () { return this.base.GetAllDictionaryAttrs(); }
        this.GetDictionaryAttr = function (attrName) { return this.base.GetDictionaryAttr(attrName); }

        this.ResetValueChanged = function () {
            try {
                for (var i = 0; i < this.m_EvalList.length; i++) {
                    this.m_EvalList[i].ValueChanged = false;
                }
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.EvaluateForAttribute = function (dictAttrName, newValues, bEvalDependents) {
            //newValues could be a string or an array
            try {
                this.ResetValueChanged();
                if (!this.m_EvalInternal)
                {
                    //resets other atts when setting this one
                    this.base.m_context.SetAttribute("currentattr", dictAttrName);
                    var attrsToReset = this.base.m_context.EvaluateTableForAttr("reset", "attr");
                    for (var i = 0; i < attrsToReset.length; i++)
                    {
                        this.base.m_context.SetAttribute(attrsToReset[i], "");
                        this.RemoveTouchedByUser(attrsToReset[i]);
                        if (this.base.m_dict[attrsToReset[i]].AvailableValues.length > 0)
                            this.base.m_dict[attrsToReset[i]].Valid = false;
                    }
                    
                    //resets an attr if it has not been touched by the user                    
                    attrsToReset = this.base.m_context.EvaluateTableForAttr("reset_INCBU", "attr");
                    for (var i = 0; i < attrsToReset.length; i++)
                    {
                        if (!this.IsTouchedByUser(attrsToReset[i]))
                        {
                            this.base.m_context.SetAttribute(attrsToReset[i], "");
                            this.RemoveTouchedByUser(attrsToReset[i]);
                            if (this.base.m_dict[attrsToReset[i]].AvailableValues.length > 0)
                                this.base.m_dict[attrsToReset[i]].Valid = false;
                        }
                    }     
                }
                var newVals = new Array();
                if (typeof newValues == "string")
                    newVals.push(newValues);
                else
                    newVals = newValues;

                if (dictAttrName in this.base.m_dict) {
                    this.base.m_dict[dictAttrName].ValueChanged = true;
                    var bUserChanged = !this.m_EvalInternal;
                    if (bUserChanged)
                        this.SetTouchedByUser(dictAttrName);
                    switch (this.base.m_dict[dictAttrName].AttributeType) {
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

        this.EvaluateAll = function () {
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

        this.GetEvalList = function () {
            try {
                return this.m_EvalList;
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.GetTriggers = function () {
            try {
                return this.m_mapTriggers;
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.DictionaryIsValid = function () {
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
        this.OrderDictionary = function () {
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
        
        this.LoadTrackingAttrs = function () {
            var allAttrs = this.base.m_context.GetAllAttributes();
            for (var it in allAttrs)
            {
                if (it.indexOf(this.TBUATTR) >= 0)
                {
                    var dictAttrName = it;
	                if (dictAttrName in this.base.m_dict)	               
	                    this.base.m_dict[dictAttrName].ChangedByUser = true;	                
	            }
            }
        }

        this.EvalBoolean = function (dictAttrName, newValue) {
            try {
                var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
                var availableValues = new Array();

                var prefixes = this.ParseOutPrefixes(res, availableValues);
                this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);

                if (prefixes.length > 0 && prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
                    this.base.m_dict[dictAttrName].Visible = false;
                else
                    this.base.m_dict[dictAttrName].Visible = true;

                var currentValue = this.base.m_context.GetAttribute(dictAttrName, true);
                this.base.m_dict[dictAttrName].Valid = true;
                this.base.m_dict[dictAttrName].Enabled = true;

                //set the dictionary default on load
                var bOverrideDisabled = false;
                if (currentValue.length == 0) {
                    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0)
                        currentValue = this.base.m_dict[dictAttrName].DefaultValue;
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
                        this.base.m_context.SetAttribute(dictAttrName, "N");
                        return;
                    }
                    else if (availableValues[0] == "YN") //allow Yes or No with a default of Y
                    {
                        if (!this.IsTouchedByUser(dictAttrName)) {
                            this.base.m_context.SetAttribute(dictAttrName, "Y");
                        }
                        else {
                            this.base.m_context.SetAttribute(dictAttrName, newValue);
                        }
                    }
                    else if (availableValues[0] == "YY") //force Yes, no other choice
                    {
                        this.base.m_context.SetAttribute(dictAttrName, "Y");
                        this.RemoveTouchedByUser(dictAttrName);
                        this.base.m_dict[dictAttrName].Enabled = false;
                    }
                    else if (newValue.length == 1) //Y or N
                    {
                        this.base.m_context.SetAttribute(dictAttrName, newValue);
                    }
                    else if (currentValue.length == 0 && newValue.length == 0)
                    {
                        this.base.m_context.SetAttribute(dictAttrName, "Y");
                        this.RemoveTouchedByUser(dictAttrName);
                        this.base.m_dict[dictAttrName].Enabled = false;
                    }
                    else
                    {
                        if (currentValue == "N" && !bOverrideDisabled)
                            this.base.m_dict[dictAttrName].Enabled = false;
                        this.base.m_context.SetAttribute(dictAttrName, currentValue);
                    }
                }
                else if (newValue.length == 1) //Y or N
                {
                    this.base.m_context.SetAttribute(dictAttrName, newValue);
                }
                else if (currentValue.length == 0 && newValue.length == 0)
                {
                    this.base.m_context.SetAttribute(dictAttrName, "Y");
                    this.RemoveTouchedByUser(dictAttrName);
                    this.base.m_dict[dictAttrName].Enabled = false;
                }
                else
                {
                    if (currentValue == "N" && !bOverrideDisabled)
                        this.base.m_dict[dictAttrName].Enabled = false;
                    this.base.m_context.SetAttribute(dictAttrName, currentValue);
                }
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.EvalEdit = function (dictAttrName, newValue) {
            try {
                var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
                var availableValues = new Array();

                var prefixes = this.ParseOutPrefixes(res, availableValues);
                this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
                this.base.m_dict[dictAttrName].Enabled = true;
                this.base.m_dict[dictAttrName].Valid = true;
                var setTheValue = true;
                if (this.InvalidateMode != Enum.INVALIDATEMODE_E.NORMALINVALIDATE)
                {
                    var currentValue = this.base.m_context.GetAttribute(dictAttrName, true);
                    setTheValue = currentValue.length == 0;
                }

                var currentValue = this.base.m_context.GetAttribute(dictAttrName, false);

                //set the dictionary default on load
                if (newValue.length == 0) {
                    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0)
                        newValue = this.base.m_dict[dictAttrName].DefaultValue;
                }

                if (availableValues.length == 1) {
                    if (prefixes[0].indexOf(this.DISABLEPREFIX) >= 0) {
                        this.base.m_dict[dictAttrName].Enabled = false;
                        this.RemoveTouchedByUser(dictAttrName);
                        this.base.m_context.SetAttribute(dictAttrName, prefixes[0]);
                        return;
                    }
                    else
                        this.base.m_dict[dictAttrName].Enabled = true;

                    //check table result for range
                    if (availableValues[0].charAt(0) == '[') {
                        var dNewValue = 0, dMin = 0, dMax = 0;
                        var val = availableValues[0];
                        val = val.replace("[", "");
                        val = val.replace("]", "");

                        var vals = val.split(",");
                        dNewValue = parseFloat(newValue);
                        dMin = parseFloat(vals[0]);
                        dMax = parseFloat(vals[1]);

                        if (dNewValue <= dMax && dNewValue >= dMin) {
                            this.base.m_context.SetAttribute(dictAttrName, newValue);
                        }
                        else if (dNewValue > dMax) {
                            if (setTheValue)
                            {
                                var wstrMax = vals[1];
                                this.base.m_context.SetAttribute(dictAttrName, wstrMax);
                            }
                            else
                                this.FlagAttrInvalid(dictAttrName);
                        }
                        else if (dNewValue < dMin) {
                            if (setTheValue)
                            {
                                var wstrMin = vals[0];
                                this.base.m_context.SetAttribute(dictAttrName, wstrMin);
                            }
                            else
                                this.FlagAttrInvalid(dictAttrName);
                        }
                    }
                    else if (availableValues[0].length == 1 && availableValues[0][0] == 'Y') {
                        this.base.m_context.SetAttribute(dictAttrName, newValue);
                    }
                    else if (availableValues[0].length == 1 && availableValues[0][0] == 'N') {
                        this.base.m_context.SetAttribute(dictAttrName, "");
                        this.RemoveTouchedByUser(dictAttrName);
                        this.base.m_dict[dictAttrName].Enabled = false;
                    }
                    else if (availableValues[0].length == 2 && availableValues[0] == "YY") //user must enter something
                    {
                        this.base.m_context.SetAttribute(dictAttrName, newValue);
                        this.base.m_dict[dictAttrName].Valid = newValue.length > 0;
                    }
                    else {
                        this.base.m_context.SetAttribute(dictAttrName, availableValues[0]);
                        this.RemoveTouchedByUser(dictAttrName);
                    }
                }
                else if (availableValues.length == 0) {
                    this.base.m_context.SetAttribute(dictAttrName, "");
                    this.RemoveTouchedByUser(dictAttrName);
                    this.base.m_dict[dictAttrName].Enabled = false;
                }
                else if (availableValues.length == 1 && availableValues[0].length > 0) {
                    this.base.m_context.SetAttribute(dictAttrName, availableValues[0]);
                    this.RemoveTouchedByUser(dictAttrName);
                }
                else if (availableValues.length > 0) {
                    if (GetIndexOfItem(availableValues, newValue) >= 0) {
                        this.base.m_context.SetAttribute(dictAttrName, newValue);
                    }
                    else {
                        this.base.m_context.SetAttribute(dictAttrName, "");
                        this.RemoveTouchedByUser(dictAttrName);
                    }
                }

                if (prefixes.length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0) {
                    this.base.m_dict[dictAttrName].Visible = false;
                }
                else {
                    this.base.m_dict[dictAttrName].Visible = true;
                }
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.EvalMultiSelect = function (dictAttrName, newValues) {
            try {
                //multi-select lists, checkbox lists
                var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
                var availableValues = new Array();

                var prefixes = this.ParseOutPrefixes(res, availableValues);
                this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);
                this.base.m_dict[dictAttrName].Enabled = true;
                this.base.m_dict[dictAttrName].Valid = true;

                var currentValue = this.base.m_context.GetAttribute(dictAttrName, false);
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
                    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0) {
                        newValues = new Array();
                        newValues.push(this.base.m_dict[dictAttrName].DefaultValue);
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
                    this.base.m_dict[dictAttrName].Enabled = false;
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
                        this.base.m_context.SetAttribute(dictAttrName, finalValue);
                    else
                    {
                        if (availableValues.length > 0)
                            this.FlagAttrInvalid(dictAttrName);
                        else
                            this.base.m_context.SetAttribute(dictAttrName, finalValue);
                    }
                }
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.EvalSingleSelect = function (dictAttrName, newValue) {
            try {
                var res = this.base.m_context.EvaluateTableForAttr(this.base.m_dict[dictAttrName].RuleTable, dictAttrName);
                var availableValues = new Array();
                this.base.m_dict[dictAttrName].Enabled = true;
                this.base.m_dict[dictAttrName].Valid = true;

                //the list of results is what is available for selection in the control
                var prefixes = this.ParseOutPrefixes(res, availableValues);
                this.base.m_dict[dictAttrName].AvailableValues = availableValues.slice(0);

                var currentValue = this.base.m_context.GetAttribute(dictAttrName, false);
                var bFound = GetIndexOfItem(availableValues, newValue) >= 0;
                var setTheValue = currentValue.length == 0 || this.InvalidateMode == Enum.INVALIDATEMODE_E.NORMALINVALIDATE;

                //set the dictionary default on load
                if (newValue.length == 0) {
                    if (this.base.m_dict[dictAttrName].DefaultValue != null && this.base.m_dict[dictAttrName].DefaultValue.length > 0) {
                        newValue = this.base.m_dict[dictAttrName].DefaultValue;
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
                    this.base.m_dict[dictAttrName].Enabled = false;
                }

                if (prefixes.length > 0 && prefixes[0].length > 0 && prefixes[0].indexOf(this.INVISPREFIX) >= 0)
                    this.base.m_dict[dictAttrName].Visible = false;
                else
                    this.base.m_dict[dictAttrName].Visible = true;

                if (newValue.length > 0 && bFound) {
                    if (currentValue != newValue) {
                        if (!setTheValue)
                            setTheValue = bFound;

                        if (setTheValue)
                            this.base.m_context.SetAttribute(dictAttrName, newValue);
                        else
                            this.FlagAttrInvalid(dictAttrName);
                    }
                }
                else {
                    if (this.base.m_dict[dictAttrName].Enabled == true)
                        this.base.m_dict[dictAttrName].Valid = false;
                    if (setTheValue)
                        this.base.m_context.SetAttribute(dictAttrName, "");
                    else
                    {        
                        if (availableValues.length > 0)
                            this.FlagAttrInvalid(dictAttrName);
                        else
                            this.base.m_context.SetAttribute(dictAttrName, "");
                    }
                    
                    this.RemoveTouchedByUser(dictAttrName);
                }
            }
            catch (err) {
                ReportError(err);
            }
        }

        this.EvaluateDependencies = function (dictAttrName) {
            this.m_EvalInternal = true;
            try {
                if (dictAttrName in this.m_mapTriggers) {
                    var attrsToEval = this.m_mapTriggers[dictAttrName];
                    for (var i = 0; i < attrsToEval.length; i++) {
                        var attrName = attrsToEval[i];
                        if (attrName in this.base.m_dict) {
                            var selectedValues = this.GetSelectedValues(this.base.m_dict[attrName]);
                            var bWasChangedByUser = this.base.m_dict[attrName].ChangedByUser;
                            this.EvaluateForAttribute(attrName, selectedValues, true);
                            this.m_EvalInternal = true;
                            if (bWasChangedByUser) {
                                var bValuesRemainSame = true;
                                var newSelectedValues = this.GetSelectedValues(this.base.m_dict[attrName]);
                                if (newSelectedValues.length != selectedValues.length)
                                    bValuesRemainSame = false;
                                else for (var j = 0; j < selectedValues.length; j++) {
                                    if (GetIndexOfItem(newSelectedValues, selectedValues[j]) < 0) {
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
            catch (err) {
                ReportError(err);
            }
            this.m_EvalInternal = false;
        }
        
        this.FlagAttrInvalid = function (dictAttrName) {
            this.base.m_dict[dictAttrName].ChangedByUser = false;
            this.base.m_dict[dictAttrName].Valid = false;
        }
        
        this.IsTouchedByUser = function (dictAttrName) {
            return this.base.m_context.GetAttributeExists(this.TBUATTR + dictAttrName);
        }
        
        this.SetTouchedByUser = function (dictAttrName) {
            this.base.m_dict[dictAttrName].ChangedByUser = true;
            this.base.m_context.SetAttribute(this.TBUATTR + dictAttrName, "Y");
        }
        
        this.RemoveTouchedByUser = function (dictAttrName) {
            this.base.m_dict[dictAttrName].ChangedByUser = false;
            this.base.m_context.RemoveAttribute(this.TBUATTR + dictAttrName);
        }      

        //remove the special character flags from the values
        this.ParseOutPrefixes = function (values, valuesWithoutPrefixes) {
            var prefixes = new Array();
            try {
                var origValues = values;
                valuesWithoutPrefixes.splice(0);

                for (var i = 0; i < origValues.length; i++) {
                    var val = origValues[i];
                    var fullPrefix = "";

                    //check for leadoff ^ indicating an invisible control
                    if (val.indexOf(this.INVISPREFIX) >= 0) {
                        fullPrefix += this.INVISPREFIX;
                        val = val.replace(this.INVISPREFIX, "");
                    }

                    //check for leadoff @ indicating a default
                    if (val.indexOf(this.DEFAULTPREFIX) >= 0) {
                        fullPrefix += this.DEFAULTPREFIX;
                        val = val.replace(this.DEFAULTPREFIX, "");
                    }

                    //check for leadoff # indicating a locked edit box
                    if (val.indexOf(this.DISABLEPREFIX) >= 0) {
                        fullPrefix += this.DISABLEPREFIX;
                        val = val.replace(this.DISABLEPREFIX, "");
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

        this.GetSelectedValues = function (attr) {
            var retval = new Array();
            try {
                var currentValue = this.base.m_context.GetAttribute(attr.Name, false);
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

        this.SetRulesDebugHandler = function (func) {
            if (this.base.m_context != null && this.base.m_context.m_KnowledgeBase != null)
                this.base.m_context.m_KnowledgeBase.SetDebugHandler(func);
        }

        this.LoadDictionary = function (dictionaryTable) {
            this.base.LoadDictionary(dictionaryTable);
        }

        this.CreateLinearEngine = function (context, dictionaryTable) {
            try {
                this.base.LoadDictionary(dictionaryTable);
                //open each attr in dict, load its dependency info to create m_EvalList, m_mapTriggers
                //build an initial list that matches the dictionary order
                for (var attr in this.base.m_dict) {
                    this.m_EvalList.push(this.base.m_dict[attr]);
                    //triggers
                    var knowledge = this.base.m_context._getKnowledge();
                    var deps = knowledge.GetInputDependencies(this.base.m_dict[attr].RuleTable);
                    if (deps != null) for (var i = 0; i < deps.length; i++) {
                        var key = deps[i];
                        if (key in this.m_mapTriggers) {
                            var hasAttr = false;
                            for (var j = 0; j < this.m_mapTriggers[key].length; j++) {
                                if (this.m_mapTriggers[key][j] == this.base.m_dict[attr].Name)
                                    hasAttr = true;
                            }

                            if (!hasAttr)
                                this.m_mapTriggers[key].push(this.base.m_dict[attr].Name);
                        }
                        else if (key in this.base.m_dict) {
                            var newArray = new Array();
                            newArray.push(this.base.m_dict[attr].Name);
                            this.m_mapTriggers[key] = newArray;
                        }
                    }
                }

                this.LoadTrackingAttrs();
                //based on the triggers, re-order the dictionary
                m_CurrentRecursion = 0;
                this.OrderDictionary();
            }
            catch (err) {
                ReportError(err);
            }
        }

        if (context === undefined || dictionaryTable === undefined)
            return;
        else {            
            this.CreateLinearEngine(context, dictionaryTable);
        }
    }
    catch (err) {
        ReportError(err);
    }
    return this;
}
