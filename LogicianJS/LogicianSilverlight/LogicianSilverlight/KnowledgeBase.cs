using System;
using System.Net;
using System.Windows;
using System.Collections.Generic;
using System.Windows.Browser;

namespace LogicianSilverlight
{
    public delegate void DebugHandlerDelegate(string msg);

    public class KnowledgeBase
    {
        public KnowledgeBase() { DebugDelegate = null; m_KnowledgeBase = null; }
		public KnowledgeBase(string knowledge_file) {CreateKnowledgeBase(knowledge_file);}
        public bool CreateKnowledgeBase(string knowledge_file)
        {
            object o = HtmlPage.Window.Invoke("CreateKnowledgeBase", knowledge_file);
            m_KnowledgeBase = (ScriptObject)o;
            DebugDelegate = null;
            if (m_KnowledgeBase != null)
                return true;
            else
                return false;
        }
		~KnowledgeBase() {if (m_KnowledgeBase != null) m_KnowledgeBase = null;}
        static public implicit operator ScriptObject(KnowledgeBase knowledge) { return knowledge.m_KnowledgeBase; }

        public DebugHandlerDelegate             DebugDelegate;
        public void                             SetDebugging(bool set) { if (m_KnowledgeBase != null) m_KnowledgeBase.Invoke("GenerateDebugMessages", set); }

		public int								TableCount(){ return (int)(double)m_KnowledgeBase.Invoke("TableCount"); }
		public bool								IsOpen(){ return (bool)m_KnowledgeBase.Invoke("IsOpen"); }	
		public bool								TableHasScript(string tableName){ return (bool)m_KnowledgeBase.Invoke("TableHasScript", tableName); }
		public bool								TableIsGetAll(string tableName){ return (bool)m_KnowledgeBase.Invoke("TableIsGetAll", tableName); }

        public string[] EvaluateTableWithParam(string tableName, string outputAttr, string param, bool bGetAll) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("EvaluateTableForAttrWithParam", new object[] { tableName, outputAttr, param, bGetAll }); Debug(); return ScriptMarshal.ScriptObjectToStringArray(o); }
        public string[] EvaluateTableWithParam(string tableName, string outputAttr, string param) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("EvaluateTableForAttrWithParam", new object[] { tableName, outputAttr, param }); Debug(); return ScriptMarshal.ScriptObjectToStringArray(o); }
        public Dictionary<string, string[]> EvaluateTableWithParam(string tableName, string param) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("EvaluateTableWithParam", new object[] { tableName, param }); Debug(); return ScriptMarshal.ScriptObjectToDictionaryOfStringArray(o); }
        public Dictionary<string, string[]> EvaluateTableWithParam(string tableName, string param, bool bGetAll) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("EvaluateTableWithParam", new object[] { tableName, param, bGetAll }); Debug(); return ScriptMarshal.ScriptObjectToDictionaryOfStringArray(o); }
        public string[] EvaluateTable(string tableName, string outputAttr) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("EvaluateTableForAttr", new object[] { tableName, outputAttr }); Debug(); return ScriptMarshal.ScriptObjectToStringArray(o); }
        public string[] EvaluateTable(string tableName, string outputAttr, bool bGetAll) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("EvaluateTableForAttr", new object[] { tableName, outputAttr, bGetAll }); Debug(); return ScriptMarshal.ScriptObjectToStringArray(o); }
		public Dictionary<string, string[]>	    EvaluateTable(string tableName) {return EvaluateTable(tableName, TableIsGetAll(tableName));}
        public Dictionary<string, string[]> EvaluateTable(string tableName, bool bGetAll) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("EvaluateTable", new object[] { tableName, bGetAll }); Debug(); return ScriptMarshal.ScriptObjectToDictionaryOfStringArray(o); }
        public string[] ReverseEvaluateTable(string tableName, string inputAttr) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("ReverseEvaluateTableForAttr", new object[] { tableName, inputAttr }); Debug(); return ScriptMarshal.ScriptObjectToStringArray(o); }
        public string[]                         ReverseEvaluateTable(string tableName, string inputAttr, bool bGetAll) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("ReverseEvaluateTableForAttr", new object[] { tableName, inputAttr, bGetAll }); Debug(); return ScriptMarshal.ScriptObjectToStringArray(o); }
        public Dictionary<string, string[]> ReverseEvaluateTable(string tableName) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("ReverseEvaluateTable", tableName); Debug(); return ScriptMarshal.ScriptObjectToDictionaryOfStringArray(o); }
        public Dictionary<string, string[]> ReverseEvaluateTable(string tableName, bool bGetAll) { ScriptObject o = (ScriptObject)m_KnowledgeBase.Invoke("ReverseEvaluateTable", new object[] { tableName, bGetAll }); Debug(); return ScriptMarshal.ScriptObjectToDictionaryOfStringArray(o); }
        public string                           GetEvalParameter() { return (string)m_KnowledgeBase.Invoke("GetEvalParameter"); }	
		public void							    SetInputValues(Dictionary<string, int> values) {m_KnowledgeBase.Invoke("SetInputValues", values);}
		public void								SetInputValue(string name, string value) {m_KnowledgeBase.Invoke("SetInputValue", new object[]{name, value});}
		public void								ResetTable(string tableName) {m_KnowledgeBase.Invoke("ResetTable", tableName);}

        public string[] GetInputAttrs(string tableName) { return ScriptMarshal.ScriptObjectToStringArray(m_KnowledgeBase.Invoke("GetInputAttrs", tableName)); }
        public string[] GetInputDependencies(string tableName) { return ScriptMarshal.ScriptObjectToStringArray(m_KnowledgeBase.Invoke("GetInputDependencies", tableName)); }
        public string[] GetOutputAttrs(string tableName) { return ScriptMarshal.ScriptObjectToStringArray(m_KnowledgeBase.Invoke("GetOutputAttrs", tableName)); }
        public string[] GetAllPossibleOutputs(string tableName, string outputName) { return ScriptMarshal.ScriptObjectToStringArray(m_KnowledgeBase.Invoke("GetAllPossibleOutputs", new object[] { tableName, outputName })); }

		public string							Localize(string baseValue, string locale) {return Translate(baseValue, "", locale);}
		public string							DeLocalize(string localeValue) {return (string)m_KnowledgeBase.Invoke("DeLocalize", localeValue);}
		public string							Translate(string source, string sourceLocale, string destLocale) {return (string)m_KnowledgeBase.Invoke("Translate", new object[]{source, sourceLocale, destLocale});}

        private void Debug()
        {
            if (m_KnowledgeBase != null && DebugDelegate != null)
            {
                object o = m_KnowledgeBase.Invoke("GetDebugMessages");
                if (o != null)
                {
                    string msg = (string)o;
                    if (!string.IsNullOrEmpty(msg))
                        DebugDelegate(msg);
                }
            }
        }        

		private ScriptObject					m_KnowledgeBase;
    }

    
}
