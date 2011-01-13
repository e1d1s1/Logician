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
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Collections.Generic;
using System.Windows.Browser;

namespace LogicianSilverlight
{
    public class ROMDictionaryAttribute
    {        
		public ROMDictionaryAttribute() {m_ROMDictionaryAttribute = null;}
        public ROMDictionaryAttribute(ScriptObject o) { m_ROMDictionaryAttribute = o; }
		public bool CreateROMDictionaryAttribute() 
        {
            object o = HtmlPage.Window.Invoke("CreateROMDictionaryAttribute");
            m_ROMDictionaryAttribute = (ScriptObject)o;
            if (m_ROMDictionaryAttribute != null)
                return true;
            else
                return false;
        }
		~ROMDictionaryAttribute() {if (m_ROMDictionaryAttribute != null) m_ROMDictionaryAttribute = null;}
        static public implicit operator ScriptObject(ROMDictionaryAttribute attr) { return attr.m_ROMDictionaryAttribute; }

		public string Name
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (string)m_ROMDictionaryAttribute.GetProperty("Name");
                else
                    return null;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("Name", value);
			}
		}
		public string Description
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (string)m_ROMDictionaryAttribute.GetProperty("Description");
                else
                    return null;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("Description", value);
			}
		}
		public string DefaultValue
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (string)m_ROMDictionaryAttribute.GetProperty("DefaultValue");
                else
                    return null;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("DefaultValue", value);
			}
		}
		public string RuleTable
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (string)m_ROMDictionaryAttribute.GetProperty("RuleTable");
                else
                    return null;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("RuleTable", value);
			}
		}
		public int AttributeType
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (int)m_ROMDictionaryAttribute.GetProperty("AttributeType");
                else
                    return 0;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("AttributeType", value);
			}
		}
		public bool ValueChanged
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (bool)m_ROMDictionaryAttribute.GetProperty("ValueChanged");
                else
                    return false;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("ValueChanged", value);
			}
		}
		public bool ChangedByUser
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (bool)m_ROMDictionaryAttribute.GetProperty("ChangedByUser");
                else
                    return false;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("ChangedByUser", value);
			}
		}
		public bool Valid
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (bool)m_ROMDictionaryAttribute.GetProperty("Valid");
                else
                    return false;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("Valid", value);
			}
		}
		public bool Visible
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (bool)m_ROMDictionaryAttribute.GetProperty("Visible");
                else
                    return false;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("Visible", value);
			}
		}
		public bool Enabled
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return (bool)m_ROMDictionaryAttribute.GetProperty("Enabled");
                else
                    return false;
			}
			set
			{
                if (m_ROMDictionaryAttribute != null)
                    m_ROMDictionaryAttribute.SetProperty("Enabled", value);
			}
		}
		public string[] PossibleValues
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return ScriptMarshal.ScriptObjectToStringArray(m_ROMDictionaryAttribute.GetProperty("PossibleValues"));
                else
                    return null;				
			}
			set
			{
                m_ROMDictionaryAttribute.SetProperty("PossibleValues", value);			
			}
		}
		public string[] AvailableValues
		{
			get
			{
                if (m_ROMDictionaryAttribute != null)
                    return ScriptMarshal.ScriptObjectToStringArray(m_ROMDictionaryAttribute.GetProperty("AvailableValues"));
                else
                    return null;	
			}
			set
			{
                m_ROMDictionaryAttribute.SetProperty("AvailableValues", value);	
			}
		}
	
	    private ScriptObject m_ROMDictionaryAttribute;
    }

    public class ROMDictionary
    {
        public ROMDictionary() { m_ROMDictionary = null; DebugDelegate = null; }
        public ROMDictionary(ROMNode node) { CreateROMDictionary(node); }
		private ROMDictionary(ScriptObject o) {m_ROMDictionary = o;}
        public bool CreateROMDictionary(ROMNode node) 
        {
            object o = HtmlPage.Window.Invoke("CreateROMDictionary", node);
            m_ROMDictionary = (ScriptObject)o;
            DebugDelegate = null;
            if (m_ROMDictionary != null)
                return true;
            else
                return false;
        }
        ~ROMDictionary() { if (m_ROMDictionary != null) m_ROMDictionary = null; }
        static public implicit operator ScriptObject(ROMDictionary dict) { return dict.m_ROMDictionary; }

        //debugger
        public DebugHandlerDelegate DebugDelegate;
        public void SetDebugging(bool set) { if (m_ROMDictionary != null) m_ROMDictionary.Invoke("GenerateTableDebugMessages", set); }
        public void PumpDebugMessages()
        {
            if (m_ROMDictionary != null && DebugDelegate != null)
            {
                object o = (object)m_ROMDictionary.Invoke("GetTableDebugMessages");
                if (o != null)
                {
                    string msg = (string)o;
                    if (!string.IsNullOrEmpty(msg))
                        DebugDelegate(msg);
                }
            }
        }

        public void LoadDictionary(string dictionaryTable)
        {
            if (m_ROMDictionary != null)
            {
                m_ROMDictionary.Invoke("LoadDictionary");
                PumpDebugMessages();
            }
        }

        public ROMDictionaryAttribute GetDictionaryAttr(string dictAttrName)
        {
            ROMDictionaryAttribute retval = null;
            if (m_ROMDictionary != null)
            {
                object o = m_ROMDictionary.Invoke("GetDictionaryAttr", dictAttrName);
                if (o != null)
                    retval = new ROMDictionaryAttribute((ScriptObject)o);
            }
            return retval;
        }

        public Dictionary<string, ROMDictionaryAttribute> GetAllDictionaryAttrs()
        {
            Dictionary<string, ROMDictionaryAttribute> retval = new Dictionary<string, ROMDictionaryAttribute>();
            if (m_ROMDictionary != null)
            {
                object o = m_ROMDictionary.Invoke("GetAllDictionaryAttrs");
                if (o != null)
                    retval = ScriptMarshal.ScriptObjectToDictionaryOfROMAttributes(o);
            }

            return retval;
        }

        private ScriptObject m_ROMDictionary;
    }

    public class LinearEngine
    {
        public LinearEngine() { m_LinearEngine = null; DebugDelegate = null; }
        public LinearEngine(ROMNode node, string dictionaryTable) { CreateLinearEngine(node, dictionaryTable); }
		private LinearEngine(ScriptObject o) {m_LinearEngine = o;}
        public bool CreateLinearEngine(ROMNode node, string dictionaryTable) 
        {
            object o = HtmlPage.Window.Invoke("CreateLinearEngine", new object[] {(ScriptObject)node, dictionaryTable});
            m_LinearEngine = (ScriptObject)o;
            DebugDelegate = null;
            if (m_LinearEngine != null)
                return true;
            else
                return false;
        }
        ~LinearEngine() { if (m_LinearEngine != null) m_LinearEngine = null; }
        static public implicit operator ScriptObject(LinearEngine engine) { return engine.m_LinearEngine; }

        //debugger
        public DebugHandlerDelegate DebugDelegate;
        public void SetDebugging(bool set) { if (m_LinearEngine != null) m_LinearEngine.Invoke("GenerateTableDebugMessages", set); }
        public void PumpDebugMessages()
        {
            if (m_LinearEngine != null && DebugDelegate != null)
            {
                object o = (object)m_LinearEngine.Invoke("GetTableDebugMessages");
                if (o != null)
                {
                    string msg = (string)o;
                    if (!string.IsNullOrEmpty(msg))
                        DebugDelegate(msg);
                }
            }
        }

        public void LoadDictionary(string dictionaryTable)
        {
            if (m_LinearEngine != null)
            {
                m_LinearEngine.Invoke("LoadDictionary");
                PumpDebugMessages();
            }
        }

        public ROMDictionaryAttribute GetDictionaryAttr(string dictAttrName)
        {
            ROMDictionaryAttribute retval = null;
            if (m_LinearEngine != null)
            {
                object o = m_LinearEngine.Invoke("GetDictionaryAttr", dictAttrName);
                if (o != null)
                    retval = new ROMDictionaryAttribute((ScriptObject)o);
            }
            return retval;
        }

        public Dictionary<string, ROMDictionaryAttribute> GetAllDictionaryAttrs()
        {
            Dictionary<string, ROMDictionaryAttribute> retval = new Dictionary<string, ROMDictionaryAttribute>();
            if (m_LinearEngine != null)
            {
                object o = m_LinearEngine.Invoke("GetAllDictionaryAttrs");
                if (o != null)
                    retval = ScriptMarshal.ScriptObjectToDictionaryOfROMAttributes(o);
            }

            return retval;
        }

        public void EvaluateForAttribute(string dictAttrName, string[] newValues) { EvaluateForAttribute(dictAttrName, newValues, true); }
        public void EvaluateForAttribute(string dictAttrName, string[] newValues, bool bEvalDependents)
        {
            if (m_LinearEngine != null)
            {
                m_LinearEngine.Invoke("EvaluateForAttribute", new object[] { dictAttrName, newValues, bEvalDependents });
                PumpDebugMessages();
            }
        }

        public void EvaluateForAttribute(string dictAttrName, string newValue) { EvaluateForAttribute(dictAttrName, newValue, true); }
        public void EvaluateForAttribute(string dictAttrName, string newValue, bool bEvalDependents)
        {
            if (m_LinearEngine != null)
            {
                m_LinearEngine.Invoke("EvaluateForAttribute", new object[] { dictAttrName, newValue, bEvalDependents });
                PumpDebugMessages();
            }
        }

        public void EvaluateAll() 
        {
            if (m_LinearEngine != null) 
                m_LinearEngine.Invoke("EvaluateAll");
        }

        public ROMDictionaryAttribute[] GetEvalList()
        {
            if (m_LinearEngine != null)
            {
                return ScriptMarshal.ScriptObjectToAttributeArray(m_LinearEngine.Invoke("GetEvalList"));
            }
            return null;
        }

        public Dictionary<string, string[]> GetTriggers()
        {
            if (m_LinearEngine != null)
            {
                return ScriptMarshal.ScriptObjectToDictionaryOfStringArray(m_LinearEngine.Invoke("GetTriggers"));
            }
            return null;
        }

        public bool DictionaryIsValid
		{
			get
			{
                if (m_LinearEngine != null)
                    return (bool)m_LinearEngine.Invoke("DictionaryIsValid");
                else
                    return false;
			}
		}

        private ScriptObject m_LinearEngine;
    }
}
