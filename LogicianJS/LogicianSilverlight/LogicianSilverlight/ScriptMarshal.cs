using System;
using System.Net;
using System.Windows;
using System.Collections.Generic;
using System.Windows.Browser;

namespace LogicianSilverlight
{
    public class ScriptMarshal
    {
        public static string[] ScriptObjectToStringArray(object o)
        {
            List<string> retval = new List<string>();
            if (o != null)
            {
                ScriptObject scrObj = (ScriptObject)o;
                int i = 0;
                string str = null;

                do
                {
                    str = (string)scrObj.GetProperty(i);
                    if (str != null)
                        retval.Add(str);
                    i++;
                } while (str != null);
            }

            return retval.ToArray();
        }

        public static Dictionary<string, string[]> ScriptObjectToDictionaryOfStringArray(object o)
        {
            Dictionary<string, string[]> retval = new Dictionary<string, string[]>();
            if (o != null)
            {
                string[] keys = ScriptObjectToStringArray(HtmlPage.Window.Invoke("getKeysOfObject", o));
                ScriptObject scrObj = (ScriptObject)o;

                foreach (string key in keys)
                {
                    List<string> values = new List<string>();
                    int i = 0;
                    string str = null;
                    ScriptObject scrObjValues = (ScriptObject)scrObj.GetProperty(key);

                    do
                    {
                        str = (string)scrObjValues.GetProperty(i);
                        if (str != null)
                            values.Add(str);
                        i++;
                    } while (str != null);

                    retval.Add(key, values.ToArray());
                }
            }

            return retval;
        }

        public static Dictionary<string, Dictionary<string, string>> ScriptObjectToDictionaryOfStringDictionary(object o)
        {
            Dictionary<string, Dictionary<string, string>> retval = new Dictionary<string, Dictionary<string, string>>();
            if (o != null)
            {
                string[] keys = ScriptObjectToStringArray(HtmlPage.Window.Invoke("getKeysOfObject", o));
                ScriptObject objMap = (ScriptObject)o;

                foreach (string key in keys)
                {
                    ScriptObject oSubMap = (ScriptObject)objMap.GetProperty(key);
                    if (oSubMap != null)
                    {
                        Dictionary<string, string> subDict = new Dictionary<string,string>();
                        string[] subkeys = ScriptObjectToStringArray(HtmlPage.Window.Invoke("getKeysOfObject", oSubMap));
                        foreach (string subkey in subkeys)
                        {
                            object objString = (object)oSubMap.GetProperty(subkey);
                            if (objString != null)
                            {
                                subDict[subkey] = (string)objString;
                            }
                        }
                        retval[key] = subDict;
                    }
                }
            }
            return retval;
        }

        public static Dictionary<string, ROMDictionaryAttribute> ScriptObjectToDictionaryOfROMAttributes(object o)
        {
            Dictionary<string, ROMDictionaryAttribute> retval = new Dictionary<string, ROMDictionaryAttribute>();
            if (o != null)
            {
                string[] keys = ScriptObjectToStringArray(HtmlPage.Window.Invoke("getKeysOfObject", o));
                ScriptObject scrObj = (ScriptObject)o;

                foreach (string key in keys)
                {
                    int i = 0;
                    string str = null;
                    ScriptObject scrObjValue = (ScriptObject)scrObj.GetProperty(key);
                    if (scrObjValue != null)
                    {
                        ROMDictionaryAttribute attr = new ROMDictionaryAttribute(scrObjValue);
                        retval.Add(key, attr);                        
                    }                    
                }
            }
            return retval;
        }

        public static ROMNode[] ScriptObjectToROMArray(object o)
        {
            List<ROMNode> retval = new List<ROMNode>();
            if (o != null)
            {
                ScriptObject scrObj = (ScriptObject)o;
                int i = 0;
                object arrayItem = null;
                do
                {
                    arrayItem = scrObj.GetProperty(i);
                    if (arrayItem != null)
                    {
                        ROMNode romObj = new ROMNode((ScriptObject)arrayItem);
                        retval.Add(romObj);
                    }
                    i++;
                } while (arrayItem != null);
            }
            return retval.ToArray();
        }

        public static ROMDictionaryAttribute[] ScriptObjectToAttributeArray(object o)
        {
            List<ROMDictionaryAttribute> retval = new List<ROMDictionaryAttribute>();
            if (o != null)
            {
                ScriptObject scrObj = (ScriptObject)o;
                int i = 0;
                ROMDictionaryAttribute attr = null;
                object attrObj = null;
                do
                {
                    attrObj = scrObj.GetProperty(i);
                    if (attrObj != null)
                    {
                        attr = new ROMDictionaryAttribute((ScriptObject)attrObj);
                        if (attr != null)
                            retval.Add(attr);
                        i++;
                    }
                } while (attrObj != null && attr != null);
            }

            return retval.ToArray();
        }
    }
}

