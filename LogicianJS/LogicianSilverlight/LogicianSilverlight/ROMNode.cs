/*
This file is part of the LogicianSilverlight Library.
Copyright (C) 2009 - 2011 Eric D. Schmidt

    LogicianSilverlight is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LogicianSilverlight is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LogicianSilverlight.  If not, see <http://www.gnu.org/licenses/>.
*/
using System;
using System.Net;
using System.Windows;
using System.Collections.Generic;
using System.Windows.Browser;

namespace LogicianSilverlight
{
    public class ROMNode
    {
        public ROMNode() { m_ROMNode = null; /*m_KnowledgeBase = null*/; CreateROMNode(""); }
        public ROMNode(ScriptObject o) { m_ROMNode = o; /*m_KnowledgeBase = (ScriptObject)m_ROMNode.Invoke("GetKnowledgeBase");*/ }
		public ROMNode(string id) {CreateROMNode(id);}
		public bool CreateROMNode(string id)
        {
            object o = HtmlPage.Window.Invoke("CreateROMNode", id);
            m_ROMNode = (ScriptObject)o;
            DebugDelegate = null;
            if (m_ROMNode != null)
                return true;
            else
                return false;
        }
        public bool DestroyROMObject() { return (bool)m_ROMNode.Invoke("DestroyROMObject"); }
        static public implicit operator ScriptObject(ROMNode node) { return node.m_ROMNode; }

        //debugger
		public DebugHandlerDelegate	DebugDelegate;
		public void					SetDebugging(bool set) {if (m_ROMNode != null) m_ROMNode.Invoke("GenerateTableDebugMessages", set);}
		public void					PumpDebugMessages()
        {
            if (m_ROMNode != null && DebugDelegate != null)
            {
                object o = (object)m_ROMNode.Invoke("GetTableDebugMessages");
                if (o != null)
                {
                    string msg = (string)o;
                    if (!string.IsNullOrEmpty(msg))
                        DebugDelegate(msg);
                }
            }
        }

        //relational functions
		public ROMNode			GetRoot() {return (ROMNode)m_ROMNode.Invoke("GetRoot");}
		public ROMNode			Parent() {return (ROMNode)m_ROMNode.Invoke("Parent");}
        public ROMNode[]        GetAllChildren(bool recurs) { return ScriptMarshal.ScriptObjectToROMArray(m_ROMNode.Invoke("GetAllChildren", recurs)); }
        public ROMNode[]        FindObjects(string xpath) { return ScriptMarshal.ScriptObjectToROMArray(m_ROMNode.Invoke("FindObjects", xpath)); }
        public ROMNode[]        FindAllObjectsByID(string id, bool recurs) { return ScriptMarshal.ScriptObjectToROMArray(m_ROMNode.Invoke("FindAllObjectsByID", new object[] { id, recurs })); }
		public ROMNode			FindObjectByGUID(string guid) {return (ROMNode)m_ROMNode.Invoke("FindObjectByGUID");}
        public bool             AddChildROMObject(ROMNode child) { return (bool)m_ROMNode.Invoke("AddChildROMObject", child.m_ROMNode); }
        public bool             RemoveChildROMObject(ROMNode child) { return (bool)m_ROMNode.Invoke("RemoveChildROMObject", child.m_ROMNode); }
        public ROMNode          Clone() {return (ROMNode)m_ROMNode.Invoke("Clone");}

		//attribute functions
		public string			GetAttribute(string id, string name, bool immediate) {return (string)m_ROMNode.Invoke("GetAttributeValue", new object[]{id, name, immediate});}
		public string			GetAttribute(string id, bool immediate) {return GetAttribute(id, "value", immediate);}
		public string			GetAttribute(string id) {return GetAttribute(id, "value", false);}
		public bool				GetAttributeExists(string id, string name) {return (bool)m_ROMNode.Invoke("GetAttributeExists", new object[]{id, name});}
		public bool				GetAttributeExists(string id) {return GetAttributeExists(id, "value");}
		public bool				SetAttribute(string id, string name, string value) {return (bool)m_ROMNode.Invoke("SetAttributeValue", new object[]{id, name, value});}
		public bool				SetAttribute(string id, string value) {return SetAttribute(id, "value", value);}
		public bool				RemoveAttribute(string id, string name) {return (bool)m_ROMNode.Invoke("RemoveAttribute", new object[]{id, name});}
		public bool				RemoveAttribute(string id) {return RemoveAttribute(id, "value");} 	
		public bool				SetROMObjectValue(string name, string value) {return (bool)m_ROMNode.Invoke("SetROMObjectValue", new object[]{name, value});}
		public string			GetROMObjectValue(string name) {return (string)m_ROMNode.Invoke("GetROMObjectValue", name);}
		public bool				RemoveROMObjectValue(string name) {return (bool)m_ROMNode.Invoke("RemoveROMObjectValue", name);}
		public string			GetROMObjectID() {return (string)m_ROMNode.Invoke("GetROMObjectID");}
		public void				SetROMObjectID(string id) {m_ROMNode.Invoke("SetROMObjectID", id);}
		public string			GetROMGUID() {return (string)m_ROMNode.Invoke("GetROMGUID");}
        public Dictionary<string, Dictionary<string, string>> GetAllAttributes() { return ScriptMarshal.ScriptObjectToDictionaryOfStringDictionary(m_ROMNode.Invoke("GetAllAttributes")); }

		//rules
        public bool             LoadRules(string knowledge_file) { bool retval = (bool)m_ROMNode.Invoke("LoadRules", knowledge_file); /*m_KnowledgeBase = (ScriptObject)m_ROMNode.Invoke("GetKnowledgeBase");*/ return retval; }
        public bool             LoadRulesFromString(string xml) { bool retval = (bool)m_ROMNode.Invoke("LoadRulesFromString", xml); /*m_KnowledgeBase = (ScriptObject)m_ROMNode.Invoke("GetKnowledgeBase");*/ return retval; }
        public string[]         EvaluateTable(string evalTable, string output, bool bGetAll) { string[] retval = ScriptMarshal.ScriptObjectToStringArray(m_ROMNode.Invoke("EvaluateTableForAttr", new object[] { evalTable, output, bGetAll })); PumpDebugMessages(); return retval; }
        public string[]         EvaluateTable(string evalTable, string output) { string[] retval = ScriptMarshal.ScriptObjectToStringArray(m_ROMNode.Invoke("EvaluateTableForAttr", new object[] { evalTable, output })); PumpDebugMessages(); return retval; }
        public Dictionary<string, string[]> EvaluateTable(string evalTable, bool bGetAll) { Dictionary<string, string[]> retval = ScriptMarshal.ScriptObjectToDictionaryOfStringArray(m_ROMNode.Invoke("EvaluateTable", new object[] { evalTable, bGetAll })); PumpDebugMessages(); return retval; }
        public Dictionary<string, string[]> EvaluateTable(string evalTable) { Dictionary<string, string[]> retval = ScriptMarshal.ScriptObjectToDictionaryOfStringArray(m_ROMNode.Invoke("EvaluateTable", evalTable)); PumpDebugMessages(); return retval; }
        public string[] EvaluateTableWithParam(string evalTable, string output, string param, bool bGetAll) { string[] retval = ScriptMarshal.ScriptObjectToStringArray(m_ROMNode.Invoke("EvaluateTableForAttrWithParam", new object[] { evalTable, output, param, bGetAll })); PumpDebugMessages(); return retval; }
        public string[] EvaluateTableWithParam(string evalTable, string output, string param) { string[] retval = ScriptMarshal.ScriptObjectToStringArray(m_ROMNode.Invoke("EvaluateTableForAttWithParamr", new object[] { evalTable, output, param })); PumpDebugMessages(); return retval; }
        public Dictionary<string, string[]> EvaluateTableWithParam(string evalTable, string param, bool bGetAll) { Dictionary<string, string[]> retval = ScriptMarshal.ScriptObjectToDictionaryOfStringArray(m_ROMNode.Invoke("EvaluateTableWithParam", new object[] { evalTable, param, bGetAll })); PumpDebugMessages(); return retval; }
        public Dictionary<string, string[]> EvaluateTableWithParam(string evalTable, string param) { Dictionary<string, string[]> retval = ScriptMarshal.ScriptObjectToDictionaryOfStringArray(m_ROMNode.Invoke("EvaluateTableWithParam", new object[] {evalTable, param})); PumpDebugMessages(); return retval; }
        public string           GetEvalParameter() { return (string)m_ROMNode.Invoke("GetEvalParameter");}
        public string           GetFirstTableResult(string evalTable, string output) { string retval = (string)m_ROMNode.Invoke("GetFirstTableResult", new object[] { evalTable, output }); PumpDebugMessages(); return retval; }

		//IO
        public string           SaveXML(bool indented) { return (string)m_ROMNode.Invoke("SaveXML", indented); }
		public bool				LoadXML(string xmlStr) { return (bool)m_ROMNode.Invoke("LoadXML", xmlStr); }

		//XPATH
        public string           EvaluateXPATH(string xpath, string guid) { return (string)m_ROMNode.Invoke("EvaluateXPATH", new object[]{xpath, guid}); }
		public string			EvaluateXPATH(string xpath) { return (string)m_ROMNode.Invoke("EvaluateXPATH", xpath); }

		//public ScriptObject		GetScriptObject() {return m_ROMNode;}

        //private members
		private ScriptObject	m_ROMNode;
        //private ScriptObject    m_KnowledgeBase;
    }
}
