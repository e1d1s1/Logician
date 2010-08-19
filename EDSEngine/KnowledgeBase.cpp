/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 Eric D. Schmidt

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "KnowledgeBase.h"
#include "utilities.h"
#include <vector>
#include <algorithm>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <string>
#include <boost/asio.hpp>
#ifdef USE_PYTHON
#include <boost/python.hpp>
#endif

#include "zlib.h"
#ifdef WIN32
#include <direct.h>
#endif
#ifdef USE_JAVASCRIPT
    #ifdef USE_WINDOWS_SCRIPTING
    #import "msscript.ocx"
    using namespace MSScriptControl;
    #else
    #include <jsapi.h>
    #endif
#endif

#if defined(WIN32) && defined(_MSC_VER)
// ascii to unsigned long long
unsigned long long atoull(const char *str){
        return _strtoui64(str, NULL, 10);
}
#else
#define atoull(x) strtoull(x, 0, 10);
#endif

using namespace std;
using namespace EDSUTIL;
using boost::asio::ip::tcp;

EDS::CKnowledgeBase::~CKnowledgeBase(void)
{
#ifdef USE_MSXML
	CoUninitialize();
#endif
}

EDS::CKnowledgeBase::CKnowledgeBase()
{
	m_DEBUGGING_MSGS = false;
	m_DEBUGGING_CON = L"localhost:11000";
	m_IsOpen = false;
	mapBaseIDtoTranslations.clear();

#ifdef USE_MSXML
	HRESULT hr = CoInitialize(NULL);
#endif
}

wstring EDS::CKnowledgeBase::DeLocalize(wstring localeValue)
{
	wstring retval = localeValue;
	for (hash_map<size_t, hash_map<wstring, wstring> >::iterator itAllIndexes = mapBaseIDtoTranslations.begin(); itAllIndexes != mapBaseIDtoTranslations.end(); itAllIndexes++)
	{
		for (hash_map<wstring, wstring>::iterator itTranslations = itAllIndexes->second.begin(); itTranslations != itAllIndexes->second.end(); itTranslations++)
		{
			if (itTranslations->second == localeValue)
			{
				retval = m_stringsMap.GetStringByID(itAllIndexes->first);
				return retval;
			}
		}
	}
	return retval;
}

wstring EDS::CKnowledgeBase::Translate(wstring source, wstring sourceLocale, wstring destLocale)
{
	wstring retval = source;
	size_t id = INVALID_STRING;
	if (sourceLocale.length() == 0)
		id = m_stringsMap.GetIDByString(source);
	else
	{
		for (hash_map<size_t, hash_map<wstring, wstring> >::iterator itAllIndexes = mapBaseIDtoTranslations.begin(); itAllIndexes!= mapBaseIDtoTranslations.end(); itAllIndexes++)
		{
			for (hash_map<wstring, wstring>::iterator itTranslations = itAllIndexes->second.begin(); itTranslations != itAllIndexes->second.end(); itTranslations++)
			{
				if (itTranslations->first == sourceLocale && itTranslations->second == source)
				{
					id = itAllIndexes->first;
					break;
				}
			}
			if (id != INVALID_STRING)
				break;
		}
	}

	if (id != INVALID_STRING)
	{
		hash_map<size_t, hash_map<wstring, wstring> >::iterator itAllIndexes = mapBaseIDtoTranslations.find(id);
		if (itAllIndexes != mapBaseIDtoTranslations.end())
		{
			for (hash_map<wstring, wstring>::iterator itTranslations = itAllIndexes->second.begin(); itTranslations != itAllIndexes->second.end(); itTranslations++)
			{
				if (itTranslations->first == destLocale)
				{
					retval = itAllIndexes->second[destLocale];
					return retval;
				}
			}
		}
	}
	return retval;
}

void EDS::CKnowledgeBase::ResetTable(wstring tableName)
{
	m_stringsMap.ClearUserStrings();
	iRecursingDepth = 0;

	CRuleTable *table = m_TableSet.GetTable(tableName);
	table->ResetTable();
}

bool EDS::CKnowledgeBase::TableHasScript(wstring tableName)
{
	CRuleTable *table = m_TableSet.GetTable(tableName);
	return (table->HasJS() || table->HasPython());
}

bool EDS::CKnowledgeBase::TableIsGetAll(wstring tableName)
{
	CRuleTable *table = m_TableSet.GetTable(tableName);
	return table->IsGetAll();
}

//public functions
vector<wstring> EDS::CKnowledgeBase::EvaluateTable(wstring tableName, wstring outputAttr, bool bGetAll)
{
	return EvaluateTableWithParam(tableName, outputAttr, L"", bGetAll);
}

map<wstring, vector<wstring> > EDS::CKnowledgeBase::EvaluateTable(wstring tableName, bool bGetAll)
{
	return EvaluateTableWithParam(tableName, L"", bGetAll);
}

vector<wstring> EDS::CKnowledgeBase::EvaluateTableWithParam(std::wstring tableName, std::wstring outputAttr, std::wstring param, bool bGetAll)
{
	vector<wstring> retval;
	try
	{
		CRuleTable *table = m_TableSet.GetTable(tableName);
		if (iRecursingDepth == 0)
		{
			m_StateParameter = param;
		}
		iRecursingDepth++;		
		
		table->EnbleDebugging(m_DEBUGGING_MSGS);

		table->SetInputValues(m_GlobalInputAttrsValues);

		vector<wstring> results = table->EvaluateTable(outputAttr, bGetAll);
		//check for existance of table chain
		if (table->HasChain() == true)
		{
			vector<wstring> eraseResults;
			vector<wstring> newResults;
			for (vector<wstring>::iterator it = results.begin(); it != results.end(); it++)
			{
				if (StringContains(*it, L"eval("))
				{
					wstring cmd((*it).begin() + 5, (*it).end() - 1);
					vector<wstring> args = Split(cmd, L",");
					vector<wstring> chainedResults;
					wstring debugVals;
					if (args.size() == 2)
					{
						wstring chainTableName = TrimString(args[0]);
						wstring chainAttrName = TrimString(args[1]);

						chainedResults = EvaluateTableWithParam(chainTableName, chainAttrName, param, TableIsGetAll(chainTableName));
						for (vector<wstring>::iterator itRes = chainedResults.begin(); itRes != chainedResults.end(); itRes++)
						{
							newResults.push_back((*itRes));
							if (m_DEBUGGING_MSGS)
							{
								if (debugVals.size() > 0)
									debugVals+=L"|";
								else
									debugVals = L":";

								debugVals+=XMLSafe(*itRes);
							}
						}
					}
					if (m_DEBUGGING_MSGS && chainedResults.size() > 0)
					{ //replace the eval( string with the actual value
						table->DebugMessage = EDSUTIL::FindAndReplace(table->DebugMessage, *it, *it + debugVals);
					}
				}
				else
				{
					newResults.push_back(*it);
				}
			}

			results.clear();
			for (vector<wstring>::iterator it = newResults.begin(); it != newResults.end(); it++)
			{
				results.push_back((*it));
			}
		}

		//check for existance of runtime scripting, JavaScript or Python
	#ifdef USE_JAVASCRIPT

		if (table->HasJS() == true)
		{
			vector<wstring> eraseResults;
			vector<wstring> newResults;

			for (vector<wstring>::iterator it = results.begin(); it != results.end(); it++)
			{
				if (StringContains(*it, L"js("))
				{
					wstring val = L"ERROR";
					#ifdef USE_WINDOWS_SCRIPTING
					IScriptControlPtr pScriptControl(__uuidof(ScriptControl));

					LPSAFEARRAY psa;
					SAFEARRAYBOUND rgsabound[]  = { 1, 0 }; // 1 element, 0-based

					psa = SafeArrayCreate(VT_VARIANT, 1, rgsabound);
					if (psa)
					{
						VARIANT vParam[1];
						VariantInit(&vParam[0]);
						V_VT(&vParam[0]) = VT_BSTR;

						V_BSTR(&vParam[0]) = SysAllocString(m_StateParameter.c_str());
						long lZero = 0;
						HRESULT hr = SafeArrayPutElement(psa, &lZero, &vParam[0]);
						SysFreeString(vParam[0].bstrVal);
					}
					#endif
					try
					{
						//everything must return as a string
						wstring customCode((*it).begin() + 3, (*it).end() - 1);
						vector<wstring> lines = EDSUTIL::Split(customCode, L"\n");
						if (lines.size() == 1) //do this for covienience and table brevity
						{
							if (!EDSUTIL::StringContains(lines[0], L"return"))
							{
								lines[0] = L"return (" + lines[0] + L").toString();";
							}
						}
						wstring codeBody;
						for (vector<wstring>::iterator it = lines.begin(); it != lines.end(); it++)
						{
							codeBody += *it;
							codeBody += L"\n";
						}
						string JSCode = "function myfunc(param){\n" + WStrToMBCStr(codeBody) + "}\n";
						#ifdef USE_WINDOWS_SCRIPTING
						if (psa)
						{
							pScriptControl->Language = "JScript";
							pScriptControl->AddCode(JSCode.c_str());

							val = VariantToWStr(pScriptControl->Run("myfunc", &psa));

							SafeArrayDestroy(psa);
						}
						#else
						/* JS variables. */
						JSRuntime *rt;
						JSContext *cx;
						JSObject  *global;

						/* Create a JS runtime. */
						rt = JS_NewRuntime(8L * 1024L * 1024L);
						if (rt == NULL)
							throw;

						/* Create a context. */
						cx = JS_NewContext(rt, 8192);
						if (cx == NULL)
							throw;
						JS_SetOptions(cx, JSOPTION_VAROBJFIX);
						JS_SetVersion(cx, JSVERSION_DEFAULT);
						//JS_SetErrorReporter(cx, reportError);

						/* Create the global object. */
						global = JS_NewObject(cx, NULL, NULL, NULL);
						if (global == NULL)
							throw;

						/* Populate the global object with the standard globals,
						   like Object and Array. */
						if (!JS_InitStandardClasses(cx, global))
							throw;

						jsval rval, funval;
						JS_EvaluateScript(cx, global, JSCode.c_str(), JSCode.length(), "EDS_JScript", 1, &funval);

						jsval argv;
						string cStr = WStrToMBCStr(m_StateParameter).c_str();
						char* tempStr = (char*)JS_malloc(cx, cStr.length() + 1);
						strcpy(tempStr, cStr.c_str());
						JSString *jsStr = JS_NewString(cx, tempStr, strlen(tempStr));
						argv = STRING_TO_JSVAL(jsStr);
						JSBool ok = JS_CallFunctionName(cx, global, "myfunc", 1, &argv, &rval);


						if (rval != NULL && ok)
						{
							char* s = JS_GetStringBytes(JSVAL_TO_STRING(rval));
							val = MBCStrToWStr(s);
						}

						//free(tempStr);
						// Cleanup.
						JS_DestroyContext(cx);
						JS_DestroyRuntime(rt);
						JS_ShutDown();

						#endif
					}
					#ifdef USE_WINDOWS_SCRIPTING
					catch(_com_error e)
					{
						wstring message = L"Failed to evaluate javascript\n";
						message += e.Error();
						message += L":";
						message += e.Source();
						message += L"\n";
						message += e.Description();
						OutputDebugString(message.c_str());
						val = L"ERROR";
					}
					#else
					catch(...)
					{
						val = L"ERROR";
					}
					#endif
					newResults.push_back(val);

					if (m_DEBUGGING_MSGS)
					{ //replace the js( string with the actual value
						table->DebugMessage = EDSUTIL::FindAndReplace(table->DebugMessage, *it, *it + L":" + XMLSafe(val));
					}
				}
				else
				{
					newResults.push_back(*it);
				}
			}

			results.clear();
			for (vector<wstring>::iterator it = newResults.begin(); it != newResults.end(); it++)
			{
				results.push_back((*it));
			}
		}
	#endif

	#ifdef USE_PYTHON
		if (table->HasPython() == true)
		{
			vector<wstring> eraseResults;
			vector<wstring> newResults;
			for (vector<wstring>::iterator it = results.begin(); it != results.end(); it++)
			{
				if (StringContains(*it, L"py("))
				{
					wstring val = L"ERROR";
					try
					{
						wstring customCode((*it).begin() + 3, (*it).end() - 1), indentedCode;

						//everything must return as a string
						vector<wstring> lines = EDSUTIL::Split(customCode, L"\n");
						if (lines.size() == 1) //do this for covienience and table brevity
						{
							if (!EDSUTIL::StringContains(lines[0], L"return"))
							{
								lines[0] = L"return str(" + lines[0] + L")";
							}
						}

						//function code must be indented
						for (vector<wstring>::iterator it = lines.begin(); it != lines.end(); it++)
						{
							indentedCode += L"   ";
							indentedCode += *it;
							indentedCode += L"\n";
						}
						string PythonCode = "def myfunc():\n" + WStrToMBCStr(indentedCode) + "\n";
						Py_Initialize();
						PyRun_SimpleString(WStrToMBCStr(m_pyCode).c_str());
						PyRun_SimpleString(PythonCode.c_str());

						boost::python::object module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__"))));
						boost::python::object function = module.attr("myfunc");
						boost::python::object dictionary = module.attr("__dict__");
						dictionary["param"] = WStrToMBCStr(m_StateParameter);

						boost::python::object result = function();
						val = boost::python::extract<wstring>(result);
						m_StateParameter = boost::python::extract<wstring>(dictionary["param"]);

						Py_Finalize();
					}
					catch (boost::python::error_already_set)
					{
						//PyErr_Print();
						val = L"ERROR";
					}
					newResults.push_back(val);

					if (m_DEBUGGING_MSGS)
					{	//replace the py( string with the actual value
						table->DebugMessage = EDSUTIL::FindAndReplace(table->DebugMessage, *it, *it + L":" + XMLSafe(val));
					}
				}
				else
				{
					newResults.push_back(*it);
				}
			}

			results.clear();
			for (vector<wstring>::iterator it = newResults.begin(); it != newResults.end(); it++)
			{
				results.push_back((*it));
			}
		}
	#endif

		iRecursingDepth--;

		if (m_DEBUGGING_MSGS == true)
		{
			SendToDebugServer(table->DebugMessage);
		}

		retval = results;
	}
	catch (...)
	{
		ReportError("CKnowledgeBase::EvaluateTableWithParam\nCheck your table name.");
	}
	return retval;
}

vector<wstring> EDS::CKnowledgeBase::ReverseEvaluateTable(wstring tableName, wstring inputAttr, bool bGetAll)
{
	vector<wstring> retval;
	//no chaining or scripting in reverse
	try
	{
		CRuleTable *table = m_TableSet.GetTable(tableName);
		table->EnbleDebugging(m_DEBUGGING_MSGS);
		table->SetInputValues(m_GlobalInputAttrsValues);
		retval = table->EvaluateTable(inputAttr, bGetAll, false);
	}
	catch (...)
	{
		ReportError("CKnowledgeBase::ReverseEvaluateTable\nCheck your table name.");
	}
	return retval;
}

map<wstring, vector<wstring> > EDS::CKnowledgeBase::ReverseEvaluateTable(wstring tableName, bool bGetAll)
{
	map<wstring, vector<wstring> > retval;

	try
	{
		CRuleTable *table = m_TableSet.GetTable(tableName);
		table->EnbleDebugging(m_DEBUGGING_MSGS);
		table->SetInputValues(m_GlobalInputAttrsValues);
		vector<pair<wstring, vector<CRuleCell> > > outputCollection = table->GetInputAttrsTests();
		//for all the outputs get the results
		for (vector<pair<wstring, vector<CRuleCell> > >::iterator itOut = outputCollection.begin(); itOut != outputCollection.end(); itOut++)
		{
			vector<wstring> result = table->EvaluateTable((*itOut).first, bGetAll, false);
			retval[(*itOut).first] = result;
		}
	}
	catch (...)
	{
		ReportError("CKnowledgeBase::ReverseEvaluateTable\nCheck your table name.");
	}

	return retval;
}

wstring EDS::CKnowledgeBase::XMLSafe(wstring str)
{
	//replace any illegal characters with escapes
	wstring retval = EDSUTIL::FindAndReplace(str, L"\"", L"&quot;");
	retval = EDSUTIL::FindAndReplace(retval, L"'", L"&apos;");
	retval = EDSUTIL::FindAndReplace(retval, L"<", L"&lt;");
	retval = EDSUTIL::FindAndReplace(retval, L">", L"&gt;");
	retval = EDSUTIL::FindAndReplace(retval, L"&", L"&amp;");
	return retval;
}

void EDS::CKnowledgeBase::SendToDebugServer(wstring msg)
{
	try
	{
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		vector<string> con = EDSUTIL::Split(EDSUTIL::ToASCIIString(m_DEBUGGING_CON), ":");
		tcp::resolver::query query(tcp::v4(), con[0], con[1]);
		tcp::resolver::iterator iterator = resolver.resolve(query);
		tcp::socket sock(io_service);
		sock.connect(*iterator);
		boost::asio::write(sock, boost::asio::buffer(msg.c_str(), msg.length()*sizeof(wchar_t)));
	}
	catch (std::exception& e)
	{
		string msg = "CKnowledgeBase::ConnectToDebugServer()\n";
		msg += e.what();
		ReportError(msg);
	}
}


map<wstring, vector<wstring> > EDS::CKnowledgeBase::EvaluateTableWithParam(std::wstring tableName, std::wstring param, bool bGetAll)
{
	map<wstring, vector<wstring> > retval;

	CRuleTable *table = m_TableSet.GetTable(tableName);
	vector<pair<wstring, vector<CRuleCell> > > outputAttrsValues = table->GetOutputAttrsValues();
	//for all the outputs get the results
	for (vector<pair<wstring, vector<CRuleCell> > >::iterator itOut = outputAttrsValues.begin(); itOut != outputAttrsValues.end(); itOut++)
	{
		vector<wstring> result = EvaluateTableWithParam(tableName, (*itOut).first, param, bGetAll);
		retval[(*itOut).first] = result;
	}

	return retval;
}

void EDS::CKnowledgeBase::SetInputValue(wstring name, wstring value)
{
	size_t id = m_stringsMap.GetIDByString(value);
	if (id == INVALID_STRING) //wasnt in our existing list
	{
		//add a new tokenized string
		id = m_stringsMap.AddUserString(value);
	}
	m_GlobalInputAttrsValues[name] = id;
}

//loading
EDS::CKnowledgeBase::CKnowledgeBase(wstring knowledge_file)
{
	CreateKnowledgeBase(knowledge_file);
}

void EDS::CKnowledgeBase::CreateKnowledgeBase(string knowledge_file)
{
	CreateKnowledgeBase(MBCStrToWStr(knowledge_file.c_str()));
}

bool EDS::CKnowledgeBase::CreateKnowledgeBase(wstring knowledge_file)
{
	bool retval = false;
	m_IsOpen = false;
	iRecursingDepth = 0;
	mapBaseIDtoTranslations.clear();
	m_GlobalInputAttrsValues.clear();
	m_GlobalInputAttrsValues[L""] = EMPTY_STRING;
	m_GlobalInputAttrsValues[L"NULL"] = EXPLICIT_NULL_STRING;
#ifdef USE_MSXML
	HRESULT hr = CoInitialize(NULL);
#endif

	try
	{
	    wstring file_name, pathSep;
		#ifdef POSIX
		pathSep = L"/";
		#else
		pathSep = L"\\";
		#endif

		wstring wsFileName = knowledge_file, wsPath, wsExtension, unzippedFileName;
		size_t pos = wsFileName.find_last_of(pathSep);
		wsPath = wsFileName.substr(0, pos);
		pos = wsFileName.find_last_of(L".");
		wsExtension = wsFileName.substr(pos + 1);

		if (wsExtension == L"gz")
		{
			//get the directory to extract files to
			char * pcPath;
			pcPath = getenv("TEMP");
			unzippedFileName = MBCStrToWStr(pcPath);

			file_name = knowledge_file.substr(knowledge_file.find_last_of(pathSep) + 1);
			file_name = FindAndReplace(file_name, L".gz", L".xml");

//#if	_DEBUG
//	char temp[_MAX_PATH];
//	getcwd(temp, _MAX_PATH);
//#endif
			//unzip
			gzFile infile = NULL;
			string s = WStrToMBCStr(knowledge_file);
			infile = gzopen(s.c_str(), "rb");
			unzippedFileName += pathSep;
			unzippedFileName += file_name;
			FILE *outfile = fopen(WStrToMBCStr(unzippedFileName).c_str(), "wb");

			unsigned char buffer[128];
			int num_read = 0;
			gzrewind(infile);
			while ((num_read = gzread(infile, buffer, sizeof(buffer))) > 0)
			{
				fwrite(buffer, 1, num_read, outfile);
			}
			gzclose(infile);
			fclose(outfile);
		}
		else
		{
			unzippedFileName = wsFileName;
		}

		//parse the table data and create tables in the tableset

		vector<wstring> FormulaInputs;
		//parse the table from xml
		#ifdef USE_MSXML
			Document	xmlDocument;
			xmlDocument = NULL;
			xmlDocument.CreateInstance(L"MSXML2.DOMDocument.6.0");
			xmlDocument->async = VARIANT_FALSE;
			xmlDocument->resolveExternals = VARIANT_FALSE;
			xmlDocument->setProperty("SelectionLanguage", "XPath");
			xmlDocument->setProperty("SelectionNamespaces", "");
			//// Turn on the new parser in MSXML6 for better standards compliance (leadding whitespaces in attr values);
			////this must be done prior to loading the document
			xmlDocument->setProperty("NewParser", VARIANT_TRUE);

			try
			{
				VARIANT_BOOL ok = xmlDocument->load(unzippedFileName.c_str());

				if (ok)
				{
					m_IsOpen = true;
					Node TablesNode = xmlDocument->selectSingleNode("//Tables");
					wstring debug = VariantToWStr(TablesNode->attributes->getNamedItem("debug")->nodeValue);
					if (debug == L"true")
					{
						m_DEBUGGING_MSGS = true;
						wstring con = VariantToWStr(TablesNode->attributes->getNamedItem("connection")->nodeValue);
						if (con.length() > 0)
						{
							m_DEBUGGING_CON = con;
						}
					}
					else
						m_DEBUGGING_MSGS = false;

					NodeList allTables = xmlDocument->selectNodes("Tables/Table");
					vector<pair<wstring, vector<CRuleCell> > > InputAttrsTests;
					vector<pair<wstring, vector<CRuleCell> > > OutputAttrsValues;

					for (int i = 0; i < allTables->length; i++)
					{
						Node TableNode = allTables->item[i];

						NodeList inputList = TableNode->selectNodes("Inputs");
						InputAttrsTests = GetTableRowFromXML(inputList, xmlDocument);
						NodeList outputList = TableNode->selectNodes("Outputs");
						OutputAttrsValues = GetTableRowFromXML(outputList, xmlDocument);

						wstring name = VariantToWStr(TableNode->attributes->getNamedItem("name")->nodeValue);
						wstring sGetAll = VariantToWStr(TableNode->attributes->getNamedItem("getall")->nodeValue);
						bool bGetAll = false;
						if (sGetAll.length() > 0 && sGetAll[0] == L't')
							bGetAll = true;

						NodeList formulaInputNodes = TableNode->selectNodes("FormulaInput");
						if (formulaInputNodes != NULL)
						{
							for (int j = 0; j < formulaInputNodes->length; j++)
							{
								Node formulaInputNode = formulaInputNodes->item[j];
								FormulaInputs.push_back((wstring)(formulaInputNode->Gettext()));
							}
						}

						m_TableSet.AddTable(InputAttrsTests, OutputAttrsValues, FormulaInputs, &m_stringsMap, name, bGetAll);
						FormulaInputs.clear();
						retval = true;
					}

					NodeList allTranslations = xmlDocument->selectNodes("//Translations/String");
					for (int i = 0; i < allTranslations->length; i++)
					{
						Node StringNode = allTranslations->item[i];
						NamedNodeMap attrs = StringNode->attributes;
						if (attrs)
						{
							size_t id = atoull(EDSUTIL::ToASCIIString(VariantToWStr(StringNode->attributes->getNamedItem("id")->nodeValue)).c_str());
							for (int childAttr = 0; childAttr < attrs->length; childAttr++)
							{
								Node childNode = attrs->item[childAttr];
								wstring name = VariantToWStr(childNode->nodeName);
								if (name != L"id")
								{
									wstring langType = name;
									wstring langValue = VariantToWStr(StringNode->attributes->getNamedItem(name.c_str())->nodeValue);
									pair<wstring, wstring> kvp;
									kvp.first = langType;
									kvp.second = langValue;
									stdext::hash_map<size_t, stdext::hash_map<wstring, wstring> >::iterator itFind = mapBaseIDtoTranslations.find(id);
									if (itFind != mapBaseIDtoTranslations.end())
									{
										stdext::hash_map<wstring, wstring> *newTranlation = &itFind->second;
										newTranlation->insert(kvp);
									}
									else
									{
										stdext::hash_map<wstring, wstring> newTranslation;
										newTranslation.insert(kvp);
										pair<size_t, stdext::hash_map<wstring, wstring> > idTrans_kvp;
										idTrans_kvp.first = id;
										idTrans_kvp.second = newTranslation;
										mapBaseIDtoTranslations.insert(idTrans_kvp);
									}
								}
							}
						}
					}

					Node nodeJS = xmlDocument->selectSingleNode("//Javascript");
					Node nodePY = xmlDocument->selectSingleNode("//Python");
					if (nodeJS != NULL)
						m_jsCode = nodeJS->Gettext() + L"\n";
					if (nodePY != NULL)
						m_pyCode = nodePY->Gettext() + L"\n";

				}
			}
			catch(const _com_error& e)
			{
				ReportError(ToASCIIString((wstring)(e.Description())));
			}
			xmlDocument.Release();
		#endif

		#ifdef USE_LIBXML
			xmlInitParser();
			Document xmlDocument = NULL;
			std::string strBuff(unzippedFileName.begin(), unzippedFileName.end());
			xmlDocument = xmlParseFile(strBuff.c_str());
			if (xmlDocument != NULL)
			{
				m_IsOpen = true;
				xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDocument);

				xmlChar* tablesXPath = (xmlChar*)"//Tables";
				xmlXPathObjectPtr xpathTables = xmlXPathEvalExpression(tablesXPath, xpathCtx);
				Node tablesNode = xpathTables->nodesetval->nodeTab[0];
				wstring debug = MBCStrToWStr(xmlGetProp(tablesNode, (xmlChar*)"debug"));
				if (debug == L"true")
				{
					m_DEBUGGING_MSGS = true;
					wstring con = MBCStrToWStr(xmlGetProp(tablesNode, (xmlChar*)"connection"));
					if (con.length() > 0)
					{
						m_DEBUGGING_CON = con;
					}
				}
				else
					m_DEBUGGING_MSGS = false;

				xmlChar* tableXPath = (xmlChar*)"//Tables/Table";
				xmlXPathObjectPtr xpathTable = xmlXPathEvalExpression(tableXPath, xpathCtx);
				NodeList allTables = xpathTable->nodesetval;

				if (allTables != NULL)
				{
					for (int i = 0; i < allTables->nodeNr; i++)
					{
						Node TableNode = allTables->nodeTab[i];
						xpathCtx->node = TableNode;

						xmlXPathObjectPtr xpathObjInputs = xmlXPathEvalExpression((xmlChar*)"Inputs", xpathCtx);
						xmlXPathObjectPtr xpathObjOutputs = xmlXPathEvalExpression((xmlChar*)"Outputs", xpathCtx);

						NodeList inputList = xpathObjInputs->nodesetval;
						NodeList outputList = xpathObjOutputs->nodesetval;

						wstring name = MBCStrToWStr(xmlGetProp(TableNode, (xmlChar*)"name"));
						wstring sGetAll = MBCStrToWStr(xmlGetProp(TableNode, (xmlChar*)"getall"));
						bool bGetAll = false;
						if (sGetAll.length() > 0 && sGetAll[0] == L't')
							bGetAll = true;

						xmlXPathObjectPtr xpathObjFormulas = xmlXPathEvalExpression((xmlChar*)"FormulaInput", xpathCtx);
						NodeList formulaInputNodes = xpathObjFormulas->nodesetval;
						for (int j = 0; j < formulaInputNodes->nodeNr; j++)
						{
							Node formulaInputNode = formulaInputNodes->nodeTab[j];
							FormulaInputs.push_back(MBCStrToWStr(xmlNodeGetContent(formulaInputNode)));
						}

						vector<pair<wstring, vector<CRuleCell> > > InputAttrsTests = GetTableRowFromXML(inputList, xmlDocument);
						vector<pair<wstring, vector<CRuleCell> > > OutputAttrsValues = GetTableRowFromXML(outputList, xmlDocument);

						m_TableSet.AddTable(InputAttrsTests, OutputAttrsValues, FormulaInputs, &m_stringsMap, name, bGetAll);
						FormulaInputs.clear();
						retval = true;

						xmlXPathFreeObject(xpathObjInputs);
						xmlXPathFreeObject(xpathObjOutputs);
						xmlXPathFreeObject(xpathObjFormulas);
					}
				}



				xpathCtx->node = xmlDocGetRootElement(xmlDocument);
				xmlChar* stringXPath = (xmlChar*)"//Translations/String";
				xmlXPathObjectPtr xpathStrings = xmlXPathEvalExpression(stringXPath, xpathCtx);
				if (xpathStrings != NULL)
				{
					NodeList allTranslations = xpathStrings->nodesetval;
					if (allTranslations != NULL)
					{
						for (int i = 0; i < allTranslations->nodeNr; i++)
						{
							Node StringNode = allTranslations->nodeTab[i];
							size_t id = atoull(EDSUTIL::ToASCIIString(MBCStrToWStr(xmlGetProp(StringNode, (xmlChar*)"id"))).c_str());
							for (Attribute childAttr = StringNode->properties; childAttr != NULL; childAttr = childAttr->next)
							{
                                wstring name = MBCStrToWStr(childAttr->name);
                                if (name != L"id")
                                {
                                    wstring langType = name;
                                    wstring langValue = MBCStrToWStr(xmlGetProp(StringNode, (xmlChar*)WStrToMBCStr(name).c_str()));
                                    pair<wstring, wstring> kvp;
                                    kvp.first = langType;
                                    kvp.second = langValue;
                                    #ifdef _MSC_VER
                                    stdext::hash_map<size_t, stdext::hash_map<wstring, wstring> >::iterator itFind = mapBaseIDtoTranslations.find(id);
                                    #else
                                    __gnu_cxx::hash_map<size_t, __gnu_cxx::hash_map<wstring, wstring> >::iterator itFind = mapBaseIDtoTranslations.find(id);
                                    #endif
                                    if (itFind != mapBaseIDtoTranslations.end())
                                    {
                                        #ifdef _MSC_VER
                                        stdext::hash_map<wstring, wstring> *newTranlation = &itFind->second;
                                        #else
                                        __gnu_cxx::hash_map<wstring, wstring> *newTranlation = &itFind->second;
                                        #endif
                                        newTranlation->insert(kvp);
                                    }
                                    else
                                    {
                                        #ifdef _MSC_VER
                                        stdext::hash_map<wstring, wstring> newTranslation;
                                        #else
                                        __gnu_cxx::hash_map<wstring, wstring> newTranslation;
                                        #endif
                                        newTranslation.insert(kvp);
                                        #ifdef _MSC_VER
                                        pair<size_t, stdext::hash_map<wstring, wstring> > idTrans_kvp;
                                        #else
                                        pair<size_t, __gnu_cxx::hash_map<wstring, wstring> > idTrans_kvp;
                                        #endif
                                        idTrans_kvp.first = id;
                                        idTrans_kvp.second = newTranslation;
                                        mapBaseIDtoTranslations.insert(idTrans_kvp);
                                    }
                                }
							}
						}
					}
				}

				xmlXPathObjectPtr xpathJS = xmlXPathEvalExpression((xmlChar*)"//Javascript", xpathCtx);
				xmlXPathObjectPtr xpathPY = xmlXPathEvalExpression((xmlChar*)"//Python", xpathCtx);
				if (xpathJS != NULL && xpathJS->nodesetval != NULL && xpathJS->nodesetval->nodeNr == 1)
					m_jsCode = MBCStrToWStr(xmlNodeGetContent(xpathJS->nodesetval->nodeTab[0])) + L"\n";
				if (xpathJS != NULL && xpathJS->nodesetval != NULL && xpathPY->nodesetval->nodeNr == 1)
					m_pyCode = MBCStrToWStr(xmlNodeGetContent(xpathPY->nodesetval->nodeTab[0])) + L"\n";
				xmlXPathFreeObject(xpathJS);
				xmlXPathFreeObject(xpathPY);
				xmlXPathFreeObject(xpathTables);
				xmlXPathFreeObject(xpathTable);
				xmlXPathFreeContext(xpathCtx);

				if (xmlDocument)
					xmlFreeDoc(xmlDocument);
			}
			xmlCleanupParser();
		#endif


		//delete extracted file
		if (wsExtension == L"gz")
			remove(WStrToMBCStr(unzippedFileName).c_str());

		m_TableSet.Initialize();
	}
	catch (...)
	{
		ReportError("CKnowledgeBase::CreateKnowledgeBase");
		retval = false;
	}
	return retval;
}

//private
vector<pair<wstring, vector<CRuleCell> > > EDS::CKnowledgeBase::GetTableRowFromXML(NodeList nodes, Document xmlDocument)
{
	vector<pair<wstring, vector<CRuleCell> > > retval;
	try
	{
	#ifdef USE_MSXML
		long nodeCnt = nodes->length;
		for (long i = 0; i < nodeCnt; i++)
		{
			pair<wstring, vector<CRuleCell> > currentAttrRow;

			Node currentInputAttr = nodes->item[i];
			NodeList values = currentInputAttr->selectNodes("Value");
			Node attrNode = currentInputAttr->selectSingleNode("Attr");
			wstring attrName = (wstring)(attrNode->Gettext());
			currentAttrRow.first = attrName;
			for (long j = 0; j < values->length; j++)
			{
				Node currentValue = values->item[j];
				Node idNode = currentValue->attributes->getNamedItem("id");
				wstring wsIDs;
				CRuleCell cell;

				if (idNode != NULL)
				{
					wsIDs = VariantToWStr(idNode->nodeValue);
					vector<wstring> cellValues = Split((wstring)(currentValue->Gettext()), L"|");
					string sIDs(wsIDs.begin(), wsIDs.end()); //contains numerical so this ok
					vector<string> ids = Split(sIDs, ",");
					if (ids.size() != cellValues.size())
						throw "Bad OR";

					for (long idCnt = 0; idCnt < ids.size(); idCnt++)
					{
						long id = atoull(ids[idCnt].c_str());
						wstring value = cellValues[idCnt];
						m_stringsMap.AddString(id, value);
						cell.Values.push_back(id);
					}
				}

				Attribute operNode = currentValue->attributes->getNamedItem("operation");
				wstring wsOper = L"";
				long lOper = 0;
				if (operNode != NULL)
				{
					wsOper = VariantToWStr(operNode->nodeValue);
					string sOper(wsOper.begin(), wsOper.end());
					lOper = atoull(sOper.c_str());
				}
				cell.Operation = lOper;
				currentAttrRow.second.push_back(cell);

			}
			if (attrName.length() > 0)
				retval.push_back(currentAttrRow);
		}
	#endif

	#ifdef USE_LIBXML
		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDocument);
		size_t nodeCnt = nodes->nodeNr;
		for (size_t i = 0; i < nodeCnt; i++)
		{
			pair<wstring, vector<CRuleCell> > currentAttrRow;

			Node currentInputAttr = nodes->nodeTab[i];
			xpathCtx->node = currentInputAttr;

			xmlXPathObjectPtr xmlXPathObjValues = xmlXPathEvalExpression((xmlChar*)"Value", xpathCtx);
			xmlXPathObjectPtr xmlXPathObjAttr = xmlXPathEvalExpression((xmlChar*)"Attr", xpathCtx);
			NodeList values = xmlXPathObjValues->nodesetval;
			Node attrNode = xmlXPathObjAttr->nodesetval->nodeTab[0];

			wstring attrName = MBCStrToWStr(xmlNodeGetContent(attrNode));
			currentAttrRow.first = attrName;
			if (values != NULL)
			{
				for (int j = 0; j < values->nodeNr; j++)
				{
					Node currentValue = values->nodeTab[j];
					wstring wsIDs = MBCStrToWStr(xmlGetProp(currentValue, (xmlChar*)"id"));

					CRuleCell cell;
					if (wsIDs.length() > 0)
					{
						vector<wstring> cellValues = Split(MBCStrToWStr(xmlNodeGetContent(currentValue)), L"|");
						string sIDs(wsIDs.begin(), wsIDs.end()); //contains numerical so this ok
						vector<string> ids = Split(sIDs, ",");
						if (ids.size() != cellValues.size())
							throw "Bad OR";

						for (size_t i = 0; i < ids.size(); i++)
						{
							size_t id = atoull(ids[i].c_str());
							wstring value = cellValues[i];
							m_stringsMap.AddString(id, value);
							cell.Values.push_back(id);
						}
					}

					wstring wsOper = MBCStrToWStr(xmlGetProp(currentValue, (xmlChar*)"operation"));
					long lOper = 0;
					if (wsOper.length() > 0)
					{
						string sOper(wsOper.begin(), wsOper.end());
						lOper = atoull(sOper.c_str());
					}
					cell.Operation = lOper;
					currentAttrRow.second.push_back(cell);
				}

			}
			if (attrName.length() > 0)
				retval.push_back(currentAttrRow);

			xmlXPathFreeObject(xmlXPathObjValues);
			xmlXPathFreeObject(xmlXPathObjAttr);
		}
		xmlXPathFreeContext(xpathCtx);
	#endif

	}
	catch(...)
	{
		ReportError("CKnowledgeBase::GetTableRowFromXML");
	}

	return retval;
}

//ASCII Overloads
bool EDS::CKnowledgeBase::TableHasScript(std::string tableName)
{
	return TableHasScript(MBCStrToWStr(tableName));
}

bool EDS::CKnowledgeBase::TableIsGetAll(std::string tableName)
{
	return TableIsGetAll(MBCStrToWStr(tableName));
}

vector<string> EDS::CKnowledgeBase::EvaluateTable(string tableName, string outputAttr, bool bGetAll)
{
	return EvaluateTableWithParam(tableName, outputAttr, "", bGetAll);
}

map<string, vector<string> > EDS::CKnowledgeBase::EvaluateTable(string tableName, bool bGetAll)
{
	return EvaluateTableWithParam(tableName, "", bGetAll);
}

vector<string> EDS::CKnowledgeBase::EvaluateTableWithParam(std::string tableName, std::string outputAttr, std::string param, bool bGetAll)
{
	return ToASCIIStringVector(EvaluateTableWithParam(MBCStrToWStr(tableName), MBCStrToWStr(outputAttr), MBCStrToWStr(param), bGetAll));
}

std::map<string, vector<string> > EDS::CKnowledgeBase::EvaluateTableWithParam(std::string tableName, std::string param, bool bGetAll)
{
	std::map<wstring, vector<wstring> > res = EvaluateTableWithParam(MBCStrToWStr(tableName), MBCStrToWStr(param), bGetAll);
	std::map<string, vector<string> > retval;
	for (std::map<wstring, vector<wstring> >::iterator it = res.begin(); it != res.end(); it++)
	{
		retval[ToASCIIString((*it).first)] = ToASCIIStringVector((*it).second);
	}
	return retval;
}

vector<string> EDS::CKnowledgeBase::ReverseEvaluateTable(string tableName, string inputAttr, bool bGetAll)
{
	//no chaining or scripting in reverse
	CRuleTable *table = m_TableSet.GetTable(MBCStrToWStr(tableName));
	table->EnbleDebugging(m_DEBUGGING_MSGS);
	table->SetInputValues(m_GlobalInputAttrsValues);
	return ToASCIIStringVector(table->EvaluateTable(MBCStrToWStr(inputAttr), bGetAll, false));
}

map<string, vector<string> > EDS::CKnowledgeBase::ReverseEvaluateTable(string tableName, bool bGetAll)
{
	map<string, vector<string> > retval;

	CRuleTable *table = m_TableSet.GetTable(MBCStrToWStr(tableName));
	vector<pair<wstring, vector<CRuleCell> > > outputCollection = table->GetInputAttrsTests();
	//for all the outputs get the results
	for (vector<pair<wstring, vector<CRuleCell> > >::iterator itOut = outputCollection.begin(); itOut != outputCollection.end(); itOut++)
	{
		vector<wstring> result = ReverseEvaluateTable(MBCStrToWStr(tableName), (*itOut).first, bGetAll);
		retval[ToASCIIString((*itOut).first)] = ToASCIIStringVector(result);
	}

	return retval;
}


void EDS::CKnowledgeBase::SetInputValues(hash_map<string, size_t> values)
{
	hash_map<wstring, size_t> wValues;
	for (hash_map<string, size_t>::iterator it = values.begin(); it != values.end(); it++)
	{
		wValues[MBCStrToWStr((*it).first)] = (*it).second;
	}
	SetInputValues(wValues);
}


void EDS::CKnowledgeBase::SetInputValue(std::string name, std::string value)
{
	SetInputValue(MBCStrToWStr(name), MBCStrToWStr(value));
}

void EDS::CKnowledgeBase::ResetTable(std::string tableName)
{
	ResetTable(MBCStrToWStr(tableName));
}

vector<string> EDS::CKnowledgeBase::GetInputAttrs(std::string tableName)
{
	return ToASCIIStringVector(GetInputAttrs(MBCStrToWStr(tableName)));
}

vector<string> EDS::CKnowledgeBase::GetInputDependencies(std::string tableName)
{
	return ToASCIIStringVector(GetInputDependencies(MBCStrToWStr(tableName)));
}

vector<string> EDS::CKnowledgeBase::GetOutputAttrs(std::string tableName)
{
	return ToASCIIStringVector(GetOutputAttrs(MBCStrToWStr(tableName)));
}

vector<string> EDS::CKnowledgeBase::GetAllPossibleOutputs(std::string tableName, std::string outputName)
{
	return ToASCIIStringVector(GetAllPossibleOutputs(MBCStrToWStr(tableName), MBCStrToWStr(outputName)));
}

EDS::CKnowledgeBase::CKnowledgeBase(std::string knowledge_file)
{
	CKnowledgeBase(MBCStrToWStr(knowledge_file));
}
