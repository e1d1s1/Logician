/////////////////////////////////////////////////////////////////////////////
// Name:        ProjectManager.h
// Purpose:     Manages project related info for all tables, saving, loading, etc
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2011 Eric D. Schmidt, DigiRule LLC
// Licence:     GNU GPLv2
/*
	DecisionLogic is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    DecisionLogic is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DecisionLogic.  If not, see <http://www.gnu.org/licenses/>.
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <set>
#include <assert.h>
#include <wx/ffile.h>
#include <wx/textdlg.h>

#include "ProjectManager.h"
#include "utilities.h"

const long EQUALS = 1;
const long NOT_EQUAL  = 2;
const long LESS_THAN = 4;
const long LESS_THAN_EQUAL = 8;
const long GREATER_THAN = 16;
const long GREATER_THAN_EQUAL = 32;
const long RANGE_INCLUSIVE = 64;
const long RANGE_END_INCLUSIVE = 128;
const long RANGE_START_INCLUSIVE = 256;
const long RANGE_NOT_INCLUSIVE = 512;
const long PYTHON = 1024;
const long GETS = 2048;
const long CHAIN = 4096;
const long JAVASCRIPT = 8192;

const string STATUS_TABLE = "Status";
const string INPUT_TABLE = "Inputs";
const string OUTPUT_TABLE = "Outputs";
const string VALUE_NAME = "Value";
const string ATTR_NAME = "Attr";
const string FORMULA_INPUT_NAME = "FormulaInput";

ProjectManager::ProjectManager(void)
{
	xmlInitParser();
	m_project_files = new StringTable<wstring>(L"DocumentElement");
	m_deubgStaus = false;
	m_DebugConnection = L"localhost:11000";
	Reset();
}

ProjectManager::~ProjectManager(void)
{
	xmlCleanupParser();
	if (m_project_files)
		delete m_project_files;
}

void ProjectManager::Reset()
{
	try
	{
		m_project_files->Clear();
		m_project_files->AddColumn(L"DataSetName");
		m_project_files->AddColumn(L"RelativePath");

		m_project_working_path.clear();
		m_project_name.clear();
		m_project_full_path_name.clear();
		m_SelectedTables.clear();
	}
	catch (...)
    {
        ReportError("ProjectManager::Reset");
    }

}

bool ProjectManager::SaveProjectFile()
{
	bool retval = false;
    if (m_project_full_path_name.length() == 0)
        return retval;

    try
    {
		xmlDocPtr doc = GetProjectFileXML();
		if (doc != NULL)
		{
			retval = xmlSaveFormatFileEnc(UTILS::WStrToMBCStr(m_project_full_path_name).c_str(), doc, "UTF-8", 1) > 0;
			xmlFreeDoc(doc);
		}

        retval = true;
    }
    catch (...)
    {
        retval = false;
        ReportError("ProjectManager::SaveProjectFile");
    }

    return retval;
}

bool ProjectManager::SaveNewProjectFileAs(wstring path)
{
	bool retVal = false;
    if (path.length() == 0 || m_project_files->Rows() > 0)
        return retVal;

    try
    {
        ////convert the file list to XML data
		wstring file_name;
		size_t pos = path.find_last_of(PATHSEP);
        m_project_working_path = path.substr(0, pos);
		m_project_name = path.substr(pos + 1);
		m_project_full_path_name = path;

		retVal = SaveProjectFile();
    }
    catch (...)
    {
		retVal = false;
        ReportError("ProjectManager::SaveNewProjectFileAs");
    }

    return retVal;
}

void ProjectManager::SaveStringDefs(xmlDocPtr doc)
{
	//open every file in project and build a unique string dictionary and token ids
	stringCollection.clear();
	stringDictionary.clear();

	//reserved ids always
	stringCollection.insert(L"");
	stringDictionary[L""] = EMPTY_STRING;
	stringCollection.insert(L"NULL");
	stringDictionary[L"NULL"] = EXPLICIT_NULL_STRING;

	//start by building the OR list
	GlobalORs.clear();
	wstring ors_path = m_project_working_path + PATHSEP + GLOBALORS_TABLE_NAME + L".xml";
	if (UTILS::FileExists(ors_path))
	{
		xmlDocPtr doc = xmlParseFile(UTILS::WStrToMBCStr(ors_path).c_str());
		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
		xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)"//Inputs/Attr", xpathCtx);
		xmlNodeSetPtr ORs = xpathObj->nodesetval;
		if (ORs != NULL && ORs->nodeNr > 0)
		{
			for (int i = 0; i < ORs->nodeNr; i++)
			{
				xmlNodePtr currentOR = ORs->nodeTab[i];
				wstring ORName = UTILS::XMLStrToWStr(xmlNodeGetContent(currentOR));
				xpathCtx->node = currentOR->parent;
				xpathObj = xmlXPathEvalExpression((xmlChar*)"Value", xpathCtx);
				xmlNodeSetPtr OR_Values = xpathObj->nodesetval;
				vector<wstring> values;
				if (OR_Values != NULL && OR_Values->nodeNr > 0)
				{
					for (int j = 0; j < OR_Values->nodeNr; j++)
					{
						xmlNodePtr currentValue = OR_Values->nodeTab[j];
						wstring ORValue = UTILS::XMLStrToWStr(xmlNodeGetContent(currentValue));
						if (ORValue.length() > 0)
							values.push_back(ORValue);
					}
				}
				GlobalORs[ORName] = values;
			}
		}

		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
		xmlFreeDoc(doc);
	}

	for (size_t j = 0; j < m_project_files->Rows(); j++)
	{
		wstring file_name = m_project_files->GetItem(j, L"DataSetName");
		if (file_name == GLOBALORS_TABLE_NAME + L".xml" ||
			file_name == TRANSLATIONS_TABLE_NAME + L".xml") continue;
		wstring rel_path = m_project_files->GetItem(j,L"RelativePath");
		wstring full_path = m_project_working_path + PATHSEP + rel_path;
		if (rel_path.length() > 0)
			full_path += PATHSEP;
		full_path += file_name;

		//open the table and find all its strings
		xmlDocPtr doc = xmlParseFile(UTILS::WStrToMBCStr(full_path).c_str());

		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
		xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)"//Value", xpathCtx);
		xmlNodeSetPtr values = xpathObj->nodesetval;
		if (values != NULL)
		{
			for (int nodeCnt = 0; nodeCnt < values->nodeNr; nodeCnt++)
			{
				xmlNodePtr valueNode = values->nodeTab[nodeCnt];
				wstring value = UTILS::XMLStrToWStr(xmlNodeGetContent(valueNode));
				//check for ors
				vector<wstring> values = CheckForOrs(value);
				for (vector<wstring>::iterator it = values.begin(); it != values.end(); it++)
				{
					if ((*it).length() > 0 && stringCollection.insert((*it)).second)
					{
						stringDictionary[(*it)] = stringDictionary.size() + 3; //0, 1, 2 are reserved
					}
				}
			}
		}

		xmlXPathFreeContext(xpathCtx);
		xmlXPathFreeObject(xpathObj);
		xmlFreeDoc(doc);
	}

	xmlNodePtr rootNode = xmlDocGetRootElement(doc);
	xmlNodePtr tokens = xmlNewNode(NULL, (xmlChar*)"Tokens");

	for (map<wstring, size_t>::iterator it = stringDictionary.begin(); it != stringDictionary.end(); it++)
	{
		wstring tokenValue = (*it).first;
		size_t id = (*it).second;
		string str_id = UTILS::stringify(id);

		xmlNodePtr tokenNode = xmlNewNode(NULL, (xmlChar*)"Token");
		xmlNodePtr innerText = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(tokenValue).c_str());
		xmlSetProp(tokenNode, (xmlChar*)"id", (xmlChar*)str_id.c_str());
		xmlAddChild(tokenNode, innerText);
		xmlAddChild(tokens, tokenNode);
	}
	xmlAddChild(rootNode, tokens);
}

vector<wstring> ProjectManager::CheckForOrs(wstring text)
{
	vector<wstring> retval = UTILS::Split(text,L"|");
	if (retval.size() == 1)
	{
		//check for "OR "definition
		map<wstring, vector<wstring> >::iterator itFind = GlobalORs.find(text);
		if (itFind != GlobalORs.end())
		{
			retval = (*itFind).second;
		}
	}
	return retval;
}

xmlDocPtr ProjectManager::GetProjectFileXML()
{
	xmlDocPtr xmlDoc = xmlNewDoc((xmlChar*)"1.0");
	try
	{
		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
		xmlNodePtr rootNode = xmlNewNode(NULL, (xmlChar*)"DecisionLogicProject");
		xmlDocSetRootElement(xmlDoc, rootNode);
		xmlSetProp(rootNode, (xmlChar*)"connection", (xmlChar*)UTILS::WStrToMBCStr(m_DebugConnection).c_str());

		for (size_t iRow = 0; iRow < m_project_files->Rows(); iRow++)
		{
			xmlNodePtr tableDataNode = xmlNewNode(NULL, (xmlChar*)"DataSet");
			xmlAddChild(rootNode, tableDataNode);
			for (size_t iCol = 0; iCol < m_project_files->Columns(); iCol++)
			{
				wstring name = m_project_files->GetColumns().at(iCol);
				wstring text = m_project_files->GetItem(iRow, iCol);
				xmlNodePtr nameNode = xmlNewNode(NULL, (xmlChar*)UTILS::WStrToMBCStr(name).c_str());
				if (name == L"DataSetName")
				{
					vector<wstring>::iterator itFind = find(m_SelectedTables.begin(), m_SelectedTables.end(), UTILS::FindAndReplace(text, L".xml", L""));
					if (itFind != m_SelectedTables.end())
						xmlSetProp(tableDataNode, (xmlChar*)"debug", (xmlChar*)"1");
					else
						xmlSetProp(tableDataNode, (xmlChar*)"debug", (xmlChar*)"0");
				}
				else if (name == L"RelativePath")
				{
					wstring pathSep; pathSep+=PATHSEP;
					text = UTILS::FindAndReplace(text, pathSep, L"/");
				}
				xmlNodePtr textNode = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(text).c_str());
				xmlAddChild(tableDataNode, nameNode);
				xmlAddChild(nameNode, textNode);
			}
		}

		//compiler settings
		xmlNodePtr compileDataNode = xmlNewNode(NULL, (xmlChar*)"Compiler");
		xmlNodePtr jsDataNode = xmlNewNode(NULL, (xmlChar*)"Javascript");
		xmlNodePtr innerTextJS = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(m_jsCode).c_str());
		xmlNodePtr pyDataNode = xmlNewNode(NULL, (xmlChar*)"Python");
		xmlNodePtr innerTextPY = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(m_pyCode).c_str());
		xmlAddChild(jsDataNode, innerTextJS);
		xmlAddChild(pyDataNode, innerTextPY);
		xmlAddChild(compileDataNode, jsDataNode);
		xmlAddChild(compileDataNode, pyDataNode);
		xmlAddChild(rootNode, compileDataNode);

		xmlXPathFreeContext(xpathCtx);

		SaveStringDefs(xmlDoc);
	}
	catch(...)
	{
		ReportError("ProjectManager::GetProjectFileXML");
		xmlFreeDoc(xmlDoc);
		xmlDoc = NULL;
	}

	return xmlDoc;
}

xmlDocPtr ProjectManager::GetDataSetXML(DataSet<wstring>* ds, bool GetAll)
{
	xmlDocPtr xmlDoc = xmlNewDoc((xmlChar*)"1.0");

	try
	{
		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
		xmlNodePtr tablesRootNode = xmlNewNode(NULL, (xmlChar*)"Tables");
		xmlDocSetRootElement(xmlDoc, tablesRootNode);
		xmlNodePtr tableDataNode = xmlNewNode(NULL, (xmlChar*)"Table");
		xmlSetProp(tableDataNode, (xmlChar*)"name", (xmlChar*)UTILS::WStrToMBCStr(ds->DataSetName).c_str());
		if (GetAll)
			xmlSetProp(tableDataNode, (xmlChar*)"getall", (xmlChar*)"true");
		else
			xmlSetProp(tableDataNode, (xmlChar*)"getall", (xmlChar*)"false");
		xmlAddChild(tablesRootNode, tableDataNode);

		if (ds->DataSetName == GLOBALORS_TABLE_NAME)
		{
			StringTable<wstring>* inputs = ds->GetTable(GLOBALORS_TABLE_NAME);
			WriteXMLForTable(tableDataNode, inputs, INPUT_TABLE, false);
		}
		else if (ds->DataSetName == TRANSLATIONS_TABLE_NAME)
		{
			StringTable<wstring>* inputs = ds->GetTable(TRANSLATIONS_TABLE_NAME);
			WriteXMLForTable(tableDataNode, inputs, INPUT_TABLE, false);
		}
		else
		{
			//input table
			StringTable<wstring>* inputs = ds->GetTable(UTILS::ToWString(INPUT_TABLE));
			WriteXMLForTable(tableDataNode, inputs, INPUT_TABLE, false);

			//output table
			StringTable<wstring>* outputs = ds->GetTable(UTILS::ToWString(OUTPUT_TABLE));
			WriteXMLForTable(tableDataNode, outputs, OUTPUT_TABLE, false);

			//status table
			StringTable<wstring>* status = ds->GetTable(UTILS::ToWString(STATUS_TABLE));
			WriteXMLForTable(tableDataNode, status, STATUS_TABLE, false);
		}

		xmlXPathFreeContext(xpathCtx);
	}
	catch(...)
	{
		ReportError("ProjectManager::GetDataSetXML");
		xmlFreeDoc(xmlDoc);
		xmlDoc = NULL;
	}

	return xmlDoc;
}

void ProjectManager::WriteXMLForTable(xmlNodePtr tableDataNode, StringTable<wstring>* table, string tableXMLID, bool bWriteIndexes)
{
	try
	{
		if (tableXMLID == STATUS_TABLE)
		{
			xmlNodePtr rowNode = xmlNewNode(NULL, (xmlChar*)"Status");
			vector<wstring> currentRow = table->GetRow(0);

			for (size_t colIndex = 0; colIndex < currentRow.size(); colIndex++)
			{
				xmlNodePtr statusElement = xmlNewNode(NULL, (xmlChar*)"RuleStatus");
				if (currentRow[colIndex] == L"Disabled")
				{
					xmlSetProp(statusElement, (xmlChar*)"enabled", (xmlChar*)"false");
				}
				else
				{
					xmlSetProp(statusElement, (xmlChar*)"enabled", (xmlChar*)"true");
				}
				xmlAddChild(rowNode, statusElement);
			}
			xmlAddChild(tableDataNode, rowNode);
		}
		else
		{
			set<wstring> formulaInputs;

			for(size_t rowIndex = 0; rowIndex <= table->Rows(); rowIndex++)
			{
				xmlNodePtr rowNode = xmlNewNode(NULL, (xmlChar*)tableXMLID.c_str());

				vector<wstring> currentRow;
				if (rowIndex < table->Rows() && table->Rows() > 0)
					currentRow = table->GetRow(rowIndex);
				else if (table->Rows() == 0)
					currentRow = table->NewRow(); //we need to write at least 1 row to the xml to hold the column data
				else break;

				for (size_t colIndex = 0; colIndex < currentRow.size(); colIndex++)
				{
					string colName = VALUE_NAME;

					if (colIndex == 0)
					{
						colName = ATTR_NAME;
					}

					xmlNodePtr attrElement = xmlNewNode(NULL, (xmlChar*)colName.c_str());

					if (currentRow[colIndex].length() > 0)
					{
						wstring val;
						long operation = EQUALS;
						if (colIndex == 0)
						{
							val = currentRow[colIndex];
						}
						else
						{
							GetXMLValuesForCell(currentRow[colIndex], operation, val, bWriteIndexes);
							if (operation & PYTHON || operation & GETS || operation & JAVASCRIPT)
							{
								//add to formula inputs
								vector<wstring> attrs = ParseValueForGets(val);
								for(vector<wstring>::iterator itFormula = attrs.begin(); itFormula != attrs.end(); itFormula++)
									formulaInputs.insert(*itFormula);
							}
							else if (tableXMLID == OUTPUT_TABLE && !(operation & CHAIN))
							{
								val = currentRow[colIndex];
								operation = EQUALS;
							}
						}
						xmlNodePtr innerText = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(val).c_str());
						xmlAddChild(attrElement, innerText);

						if (val.length() > 0 && colIndex != 0)
						{
							if (bWriteIndexes)
							{
								string valueID;
								vector<wstring> vals = CheckForOrs(val);
								for (vector<wstring>::iterator valIter = vals.begin(); valIter != vals.end(); valIter++)
								{
									if (valueID.length() > 0)
										valueID += ",";

									size_t id = stringDictionary[*valIter];
									valueID += UTILS::stringify(id);
								}
								xmlSetProp(attrElement, (xmlChar*)"id", (xmlChar*)valueID.c_str());
							}
							string operationID = UTILS::stringify(operation);
							xmlSetProp(attrElement, (xmlChar*)"operation", (xmlChar*)operationID.c_str());
						}
					}
					xmlAddChild(rowNode, attrElement);
				}
				xmlAddChild(tableDataNode, rowNode);
			}

			if (formulaInputs.size() > 0)
			{
				for (set<wstring>::iterator itForm = formulaInputs.begin(); itForm != formulaInputs.end(); itForm++)
				{
					xmlNodePtr formulaNode = xmlNewNode(NULL, (xmlChar*)FORMULA_INPUT_NAME.c_str());
					xmlNodePtr innerText = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(*itForm).c_str());
					xmlAddChild(formulaNode, innerText);
					xmlAddChild(tableDataNode, formulaNode);
				}
			}
		}
	}
	catch (...)
    {
        ReportError("ProjectManager::WriteXMLForTable");
    }
}

void ProjectManager::GetXMLValuesForCell(wstring cellText, long &operation, wstring &saveText, bool bReplaceORs = false)
{
	try
	{
		if (cellText.length() == 0)
		{
			operation = EQUALS;
			saveText.empty();
			return;
		}

		switch (cellText[0])
		{
			case L'<':
				if (cellText.length() >= 2 && cellText[1] == L'=')
				{
					operation = LESS_THAN_EQUAL;
					saveText = cellText.substr(2);
				}
				else if (cellText.length() >= 2 && cellText[1] == L'>')
				{
					operation = NOT_EQUAL;
					saveText = cellText.substr(2);
				}
				else
				{
					operation = LESS_THAN;
					saveText = cellText.substr(1);
				}
				break;
			case L'>':
				if (cellText.length() >= 2 && cellText[1] == L'=')
				{
					operation = GREATER_THAN_EQUAL;
					saveText = cellText.substr(2);
				}
				else
				{
					operation = GREATER_THAN;
					saveText = cellText.substr(1);
				}
				break;
			case L'(':
				if (cellText.length() >= 2 && cellText[cellText.length() - 1] == L')')
				{
					operation = RANGE_NOT_INCLUSIVE;
				}
				else
				{
					operation = RANGE_END_INCLUSIVE;
				}
				saveText = cellText.substr(1, cellText.length() - 2);
				break;
			case L'[':
				if (cellText.length() >= 2 && cellText[cellText.length() - 1] == L')')
				{
					operation = RANGE_START_INCLUSIVE;
				}
				else
				{
					operation = RANGE_INCLUSIVE;
				}
				saveText = cellText.substr(1, cellText.length() - 2);
				break;
			case L'!':
				if (cellText.length() >= 2 && cellText[1] == L'=')
				{
					operation = NOT_EQUAL;
					saveText = cellText.substr(2);
				}
				break;
			default:
				if (UTILS::StringContains(cellText, L"eval("))
					operation = CHAIN;
				else
					operation = EQUALS;
				saveText = cellText;
				break;
		}

		if (UTILS::StringContains(cellText, L"get("))
			operation += GETS;
		if (UTILS::StringContains(cellText, L"py("))
			operation += PYTHON;
		else if (UTILS::StringContains(cellText, L"js("))
			operation += JAVASCRIPT;

		//check for ORs
		if (bReplaceORs)
		{
			vector<wstring> ors = CheckForOrs(saveText);
			if (ors.size() > 1)
			{
				wstring newText;
				for (vector<wstring>::iterator it = ors.begin(); it != ors.end(); it++)
				{
					if (newText.length() > 0)
						newText+=L"|";
					newText+=*it;
				}
				saveText = newText;
			}
		}
	}
	catch (...)
    {
        ReportError("ProjectManager::GetXMLValuesForCell");
    }
}

vector<wstring> ProjectManager::ParseValueForGets(wstring val)
{
    wstring fullString = val;
    vector<wstring> foundAttrs;

	try
	{
		do
		{
			wstring attrName = ReplaceAGet(fullString);
			if (attrName.length() > 0)
				foundAttrs.push_back(attrName);
		} while (UTILS::StringContains(fullString, L"get("));
	}
	catch (...)
    {
        ReportError("ProjectManager::ParseValueForGets");
    }

    return foundAttrs;
}

wstring ProjectManager::ReplaceAGet(wstring &newString)
{
    wstring retval;

	try
	{
		int iStartPos = newString.find(L"get(", 0);
		int iEndPos = newString.find(L")", iStartPos);
		if (iStartPos >= 0 && iEndPos > iStartPos)
		{
			wstring attrName(newString.begin() + iStartPos + 4, newString.begin() + iEndPos);
			wstring getText = L"get(" + attrName + L")";
			//get the value of the input attr
			newString = UTILS::FindAndReplace(newString, getText, L"");
			retval = attrName;
		}
	}
	catch (...)
    {
        ReportError("ProjectManager::ReplaceAGet");
    }

    return retval;
}

bool ProjectManager::LoadProjectFile(wstring path)
{
	bool retval = true;

	try
	{
		if (m_project_files)
			delete m_project_files;
		m_SelectedTables.clear();
		m_project_files = ReadProjectFile(path);

		size_t pos = path.find_last_of(PATHSEP);
		m_project_working_path = path.substr(0, pos);
		m_project_name = path.substr(pos + 1);
		m_project_full_path_name = path;

		UpdateGlobalORs();
	}
	catch(...)
	{
		retval = false;
		ReportError("ProjectManager::LoadProjectFile");
	}
	return retval;
}

StringTable<wstring>* ProjectManager::ReadProjectFile(wstring path)
{
	StringTable<wstring> *retval = new StringTable<wstring>(L"DocumentElement");
	try
	{
		retval->AddColumn(L"DataSetName");
		retval->AddColumn(L"RelativePath");

		wxFFile myfile(path.c_str(), _T("r"));
		wstring xmlBuff;
		if (myfile.IsOpened())
		{
			wxString s;
			myfile.ReadAll(&s, wxConvUTF8);
			xmlBuff = s;
			myfile.Close();
		}

		if (xmlBuff.size() > 0)
		{
			string buff = UTILS::WStrToMBCStr(xmlBuff);
			xmlDocPtr xmlDoc = xmlParseMemory(buff.c_str(), buff.length());

			xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
			xmlNodePtr rootNode = xmlDocGetRootElement(xmlDoc);
			m_DebugConnection = UTILS::XMLStrToWStr(xmlGetProp(rootNode, (xmlChar*)"connection"));
			xmlChar* itemsXPath = (xmlChar*)"//DataSet";
			xmlXPathObjectPtr xpathItems = xmlXPathEvalExpression(itemsXPath, xpathCtx);
			xmlNodeSetPtr allItems = xpathItems->nodesetval;

			if (allItems != NULL)
			{
				for (int iRow = 0; iRow < allItems->nodeNr; iRow++)
				{
					xmlNodePtr datasetNode = allItems->nodeTab[iRow];
					xpathCtx->node = datasetNode;
					xmlXPathObjectPtr xpathObjDataSet = xmlXPathEvalExpression((xmlChar*)"DataSetName", xpathCtx);
					xmlXPathObjectPtr xpathObjPath = xmlXPathEvalExpression((xmlChar*)"RelativePath", xpathCtx);

					xmlNodePtr nodeName = xpathObjDataSet->nodesetval->nodeTab[0];
					xmlNodePtr nodePath = xpathObjPath->nodesetval->nodeTab[0];

					wstring name = UTILS::XMLStrToWStr(xmlNodeGetContent(nodeName));
					char cDebugStatus = '0';
					if (xmlHasProp(datasetNode, (xmlChar*)"debug"))
						cDebugStatus = UTILS::XMLStrToWStr(xmlGetProp(datasetNode, (xmlChar*)"debug")).at(0);
					if (cDebugStatus == '1')
						m_SelectedTables.push_back(UTILS::FindAndReplace(name, L".xml", L""));
					wstring path = UTILS::XMLStrToWStr(xmlNodeGetContent(nodePath));
					//saved unix style so well formed
					wstring pathSep; pathSep+=PATHSEP;
					path = UTILS::FindAndReplace(path, L"/", pathSep);
					retval->AddRow();
					retval->SetItem(iRow, L"DataSetName", name);
					retval->SetItem(iRow, L"RelativePath", path);

					xmlXPathFreeObject(xpathObjDataSet);
					xmlXPathFreeObject(xpathObjPath);
				}
			}
			xmlXPathObjectPtr xpathJS = xmlXPathEvalExpression((xmlChar*)"//Javascript", xpathCtx);
			xmlXPathObjectPtr xpathPY = xmlXPathEvalExpression((xmlChar*)"//Python", xpathCtx);
			if (xpathJS != NULL && xpathJS->nodesetval != NULL && xpathJS->nodesetval->nodeNr == 1)
				m_jsCode = UTILS::XMLStrToWStr(xmlNodeGetContent(xpathJS->nodesetval->nodeTab[0]));
			if (xpathPY != NULL && xpathPY->nodesetval != NULL && xpathPY->nodesetval->nodeNr == 1)
				m_pyCode = UTILS::XMLStrToWStr(xmlNodeGetContent(xpathPY->nodesetval->nodeTab[0]));

			xmlXPathFreeObject(xpathJS);
			xmlXPathFreeObject(xpathPY);
			xmlXPathFreeObject(xpathItems);
			xmlXPathFreeContext(xpathCtx);
			xmlFreeDoc(xmlDoc);
		}
	}
	catch (...)
    {
        ReportError("ProjectManager::ReadProjectFile");
    }

	return retval;
}

bool ProjectManager::AddDataSetFile(wstring name, wstring full_path)
{
	bool retval = true;
    if (name.length() == 0 || full_path.length() == 0)
        return false;

	try
	{
		//get the relative path from the current path
		wstring working = m_project_working_path + PATHSEP;
		wstring rel_path = UTILS::FindAndReplace(full_path, working, L"");
		rel_path = UTILS::FindAndReplace(rel_path, name + L".xml", L"");
		wstring pathSep; pathSep+=PATHSEP;
		rel_path = UTILS::FindAndReplace(rel_path, pathSep, L"/"); //save unix style

		m_project_files->AddRow();
		m_project_files->SetItem(m_project_files->Rows() - 1, L"DataSetName", name + L".xml");
		m_project_files->SetItem(m_project_files->Rows() - 1, L"RelativePath", rel_path);
	}
	catch(...)
	{
		throw;
	}

	return retval;
}

bool ProjectManager::SaveDataSet(wstring name, wstring full_path, DataSet<wstring>* ds, bool GetAll)
{
    bool retval = false;
    if (name.length() == 0)
        return retval;

    try
    {
        //set the path in the project
        bool isNewTable = true;

        //get the relative path from the current path
		wstring working = m_project_working_path + PATHSEP;
		wstring rel_path = UTILS::FindAndReplace(full_path, working, L"");
		rel_path = UTILS::FindAndReplace(rel_path, name + L".xml", L"");
		wstring pathSep; pathSep+=PATHSEP;
		rel_path = UTILS::FindAndReplace(rel_path, pathSep, L"/"); //save unix style
        for (size_t iRow = 0; iRow < m_project_files->Rows(); iRow++)
        {
			wstring saved = m_project_files->GetItem(iRow, L"DataSetName");
            if (saved == name + L".xml")
            {
                isNewTable = false;
                break;
            }
        }

        //add new DataSet to the project
        if (isNewTable)
        {
			m_project_files->AddRow();
			m_project_files->SetItem(m_project_files->Rows() - 1, L"DataSetName", name + L".xml");
            m_project_files->SetItem(m_project_files->Rows() - 1, L"RelativePath", rel_path);
        }

        ds->DataSetName = name;

		xmlDocPtr doc = GetDataSetXML(ds, GetAll);
		if (doc != NULL)
		{
			//make sure any subdirs exist
			if (rel_path.length() > 0)
			{
				wstring remove = PATHSEP + name + L".xml";
				wstring pathToFile = UTILS::FindAndReplace(full_path, remove, L"");
				#ifdef WIN32
				_wmkdir(pathToFile.c_str());
				#else
				mkdir(UTILS::WStrToMBCStr(pathToFile).c_str(), 0777);
				#endif
			}
			retval = xmlSaveFormatFileEnc(UTILS::WStrToMBCStr(full_path).c_str(), doc, "UTF-8", 1) > 0;
			xmlFreeDoc(doc);
		}

		if (name == GLOBALORS_TABLE_NAME)
		{
			UpdateGlobalORs();
		}
    }
    catch (...)
    {
        throw;
    }

    return retval;
}

void ProjectManager::UpdateGlobalORs()
{
	GlobalORs.clear();

	wstring path = GetProjectWorkingPath();
	path += PATHSEP + GLOBALORS_TABLE_NAME + L".xml";;
	if (UTILS::FileExists(path))
	{
		DataSet<wstring> ds = LoadDataSet(GLOBALORS_TABLE_NAME);
		StringTable<wstring> *table = ds.GetTable(UTILS::ToWString(INPUT_TABLE));

		for (size_t j = 0; j < table->Rows(); j++)
		{
			wstring ORName = table->GetItem(j, 0);
			if (ORName.length() > 0)
			{
				vector<wstring> vValues;
				for (size_t i = 1; i < table->Columns(); i++)
				{
					wstring ORValue = table->GetItem(j, i);
					if (ORValue.length() > 0)
						vValues.push_back(ORValue);
				}
				GlobalORs[ORName] = vValues;
			}
		}
	}
}

xmlDocPtr ProjectManager::LoadTableRawXML(wstring name)
{
	xmlDocPtr retval = NULL;

	try
    {
        wstring filename = GetFilePathForTableName(name);

        if (filename.length() > 0)
        {
			retval = xmlParseFile(UTILS::WStrToMBCStr(filename).c_str());
        }
    }
    catch (...)
    {
        ReportError("ProjectManager::LoadTableRawXML");
    }

	return retval;
}

DataSet<wstring> ProjectManager::LoadDataSet(wstring name)
{
    DataSet<wstring> retval;

    try
    {
        wstring filename = GetFilePathForTableName(name);

        if (filename.length() > 0)
        {
            retval = ReadXML(filename);
			retval.DataSetName = name;
        }
    }
    catch (...)
    {
        ReportError("ProjectManager::LoadDataSet");
    }

    return retval;
}

bool ProjectManager::TableIsGetAll(wstring name)
{
	bool retval = false;
	try
	{
		wstring filename = GetFilePathForTableName(name);
		wfstream fin;
		#ifdef WIN32
		fin.open(filename.c_str(),ios::in);
		#else
		fin.open(UTILS::WStrToMBCStr(filename).c_str(), ios::in);
		#endif
		if(fin.is_open())
		{
			xmlDocPtr doc = xmlParseFile(UTILS::WStrToMBCStr(filename).c_str());
			wstring srcStr = L"//Tables/Table[@name='";
			srcStr += name;
			srcStr += L"']";

			xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
			string xpathExpression = UTILS::WStrToMBCStr(srcStr);
			xmlChar* tableXPath = (xmlChar*)xpathExpression.c_str();
			xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(tableXPath, xpathCtx);
			xmlNodeSetPtr allRes= xpathObj->nodesetval;

			if (allRes != NULL && allRes->nodeNr == 1)
			{
				xmlNodePtr TableNode = allRes->nodeTab[0];
				wstring sGetAll = UTILS::XMLStrToWStr(xmlGetProp(TableNode, (xmlChar*)"getall"));
				if (sGetAll.length() > 0 && sGetAll[0] == 't')
					retval = true;
			}

			xmlXPathFreeObject(xpathObj);
			xmlXPathFreeContext(xpathCtx);
			if (doc)
				xmlFreeDoc(doc);
		}
	}
	catch (...)
	{
		 ReportError("ProjectManager::TableIsGetAll");
	}
	return retval;
}

wstring ProjectManager::GetFilePathForTableName(wstring name)
{
	wstring path, filename;
    for (size_t rowIndex = 0; rowIndex < m_project_files->Rows(); rowIndex++)
    {
        if (m_project_files->GetItem(rowIndex, L"DataSetName") == name + L".xml")
        {
            path = m_project_files->GetItem(rowIndex, L"RelativePath");
            filename = m_project_working_path + PATHSEP + path;
			if (path.length() > 0)
				filename += PATHSEP;
			filename += name + L".xml";
            break;
        }
	}
	return filename;
}

vector<wstring> ProjectManager::GetProjectTableNames()
{
	vector<wstring> retval;

	for (size_t rowIndex = 0; rowIndex < m_project_files->Rows(); rowIndex++)
    {
        wstring name = m_project_files->GetItem(rowIndex, L"DataSetName");
		name = UTILS::FindAndReplace(name, L".xml", L"");
        retval.push_back(name);
    }

	return retval;
}

vector<wstring> ProjectManager::GetProjectTableNames(wstring path)
{
	vector<wstring> retval;
	if (path.length() == 1 && path[0] == PATHSEP)
		path.clear();

	for (size_t rowIndex = 0; rowIndex < m_project_files->Rows(); rowIndex++)
    {
		if (m_project_files->GetItem(rowIndex, L"RelativePath") == path)
		{
			wstring name = m_project_files->GetItem(rowIndex, L"DataSetName");
			name = UTILS::FindAndReplace(name, L".xml", L"");
			retval.push_back(name);
		}
    }

	return retval;
}

wstring ProjectManager::GetProjectTitle()
{
	return UTILS::FindAndReplace(m_project_name, L".dlp", L"");
}

bool ProjectManager::RenameDataSet(wstring oldName, wstring newName)
{
	bool retval = false;

    for (size_t rowIndex = 0; rowIndex < m_project_files->Rows(); rowIndex++)
    {
        if (m_project_files->GetItem(rowIndex, L"DataSetName") == oldName)
        {
			m_project_files->SetItem(rowIndex, L"DataSetName", newName);
			retval = true;
            break;
        }
    }

	return retval;
}

bool ProjectManager::DeleteDataSet(wstring name)
{
	bool retval = false;

    for (size_t rowIndex = 0; rowIndex < m_project_files->Rows(); rowIndex++)
    {
        if (m_project_files->GetItem(rowIndex, L"DataSetName") == name)
        {
			m_project_files->DeleteRow(rowIndex);
			retval = true;
            break;
        }
    }


	return retval;
}

bool ProjectManager::RenameDataSetFolder(wstring oldFolder, wstring newFolder)
{
	bool retval = false;
	if (oldFolder.length() == 1 && oldFolder[0] == PATHSEP)
		oldFolder.clear();
	if (newFolder.length() == 1 && newFolder[0] == PATHSEP)
		newFolder.clear();

    for (size_t rowIndex = 0; rowIndex < m_project_files->Rows(); rowIndex++)
    {
		wstring pathFind = oldFolder, pathReplace = newFolder;
		if (UTILS::StringContains(m_project_files->GetItem(rowIndex, L"RelativePath"), pathFind))
        {
			wstring newPath = UTILS::FindAndReplace(m_project_files->GetItem(rowIndex, L"RelativePath"), pathFind, pathReplace);
			m_project_files->SetItem(rowIndex, L"RelativePath", newPath);
			retval = true;
        }
    }

	return retval;
}

bool ProjectManager::MoveFile(wstring name, wstring oldFolderPath, wstring newFolderPath)
{
	bool retval = false;

	if (oldFolderPath.length() == 1 && oldFolderPath[0] == PATHSEP)
		oldFolderPath.clear();
	if (newFolderPath.length() == 1 && newFolderPath[0] == PATHSEP)
		newFolderPath.clear();

    for (size_t rowIndex = 0; rowIndex < m_project_files->Rows(); rowIndex++)
    {
		wstring pathFind = oldFolderPath, pathReplace = newFolderPath;
        if (m_project_files->GetItem(rowIndex, L"DataSetName") == name && m_project_files->GetItem(rowIndex, L"RelativePath") == pathFind)
        {
			m_project_files->SetItem(rowIndex, L"RelativePath", pathReplace);
			retval = true;
            break;
        }
    }

	return retval;
}

DataSet<wstring> ProjectManager::ReadXML(wstring path)
{
	DataSet<wstring> retval;

	try
	{
		wfstream fin;
		#ifdef WIN32
		fin.open(path.c_str(),ios::in);
		#else
		fin.open(UTILS::WStrToMBCStr(path).c_str(), ios::in);
		#endif
		if(fin.is_open() )
		{
			xmlDocPtr doc = xmlParseFile(UTILS::WStrToMBCStr(path).c_str());
			StringTable<wstring> dtStatus = CreateDataTableFromNodes(STATUS_TABLE, doc);
			StringTable<wstring> dtInputs = CreateDataTableFromNodes(INPUT_TABLE, doc);
			StringTable<wstring> dtOutputs = CreateDataTableFromNodes(OUTPUT_TABLE, doc);

			retval.AddTable(dtInputs);
			retval.AddTable(dtOutputs);
			retval.AddTable(dtStatus);

			if (doc)
				xmlFreeDoc(doc);
		}
		fin.close();
	}
	catch(...)
	{
		ReportError("ProjectManager::ReadXML");
	}


	return retval;
}

StringTable<wstring> ProjectManager::CreateDataTableFromNodes(string nodeName, xmlDocPtr doc)
{
	wstring wName(nodeName.begin(), nodeName.end());
	StringTable<wstring> retval(wName);

	try
	{
		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
		if (nodeName == STATUS_TABLE)
		{
			xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)"//RuleStatus", xpathCtx);
			xmlNodeSetPtr nodes = xpathObj->nodesetval;
			if (nodes != NULL)
			{
				for (int i = 0; i < nodes->nodeNr; i++)
				{
					retval.AddColumn(L"RuleStatus");
				}
				vector<wstring> newRow = retval.NewRow();
				for (int i = 0; i < nodes->nodeNr; i++)
				{
					wstring status = UTILS::XMLStrToWStr(xmlGetProp(nodes->nodeTab[i], (xmlChar*)"enabled"));

					if (status == L"true")
					{
						newRow[i] = L"Enabled";
					}
					else
					{
						newRow[i] = L"Disabled";
					}
				}
				retval.AddRow(newRow);
			}
		}
		else
		{
			string src = "//" + nodeName;
			xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)src.c_str(), xpathCtx);
			xmlNodeSetPtr nodes = xpathObj->nodesetval;
			if (nodes != NULL)
			{
				if (nodes->nodeNr != 0) //blank table was saved
				{
					//assign columns to the table
					xmlNodePtr nodeRow = nodes->nodeTab[0];
					retval.AddColumn(UTILS::ToWString(ATTR_NAME));
					src = "Value";
					xmlXPathContextPtr xpathCtx2 = xmlXPathNewContext(doc);
					xpathCtx2->node = nodeRow;
					xmlXPathObjectPtr xpathObj2 = xmlXPathEvalExpression((xmlChar*)src.c_str(), xpathCtx2);
					xmlNodeSetPtr child_nodes = xpathObj2->nodesetval;

					int iChildCnt = child_nodes->nodeNr;
					for (int i = 0; i < iChildCnt; i++)
						retval.AddColumn(UTILS::ToWString(VALUE_NAME));

					xmlXPathFreeObject(xpathObj2);
					xmlXPathFreeContext(xpathCtx2);
					//put data in the table
					for (int i = 0; i < nodes->nodeNr; i++)
					{
						nodeRow = nodes->nodeTab[i];
						vector<wstring> newRow = retval.NewRow();
						int colIndex = 0;
						for (xmlNodePtr childNode = nodeRow->children; childNode != NULL; childNode = childNode->next)
						{
							if (childNode->type == XML_ELEMENT_NODE)
							{
								wstring innerText = UTILS::XMLStrToWStr(xmlNodeGetContent(childNode));
								if (colIndex == 0)
									newRow[colIndex] = innerText;
								else
								{
									wstring wsOper = UTILS::XMLStrToWStr(xmlGetProp(childNode, (xmlChar*)"operation"));
									long operation = EQUALS;
									if (wsOper.length() > 0)
									{
										operation = atol(UTILS::ToASCIIString(wsOper).c_str());
									}
									wstring cellText = GetCellValueWithOperation(innerText, operation);
									newRow[colIndex] = cellText;
								}
								colIndex++;
							}
						}
						retval.AddRow(newRow);
					}
				}
			}

			xmlXPathFreeObject(xpathObj);
		}
		xmlXPathFreeContext(xpathCtx);
	}
	catch (...)
    {
        ReportError("ProjectManager::CreateDataTableFromNodes");
    }

	return retval;
}

wstring ProjectManager::GetCellValueWithOperation(wstring value, long operation)
{
	wstring retval;
	try
	{
		if (operation & NOT_EQUAL)
			retval = L"<>" + value;
		else if (operation & LESS_THAN)
			retval = L"<" + value;
		else if (operation & LESS_THAN_EQUAL)
			retval = L"<=" + value;
		else if (operation & GREATER_THAN)
			retval = L">" + value;
		else if (operation & GREATER_THAN_EQUAL)
			retval = L">=" + value;
		else if (operation & RANGE_END_INCLUSIVE)
			retval = L"(" + value + L"]";
		else if (operation & RANGE_INCLUSIVE)
			retval = L"[" + value + L"]";
		else if (operation & RANGE_START_INCLUSIVE)
			retval = L"[" + value + L")";
		else if (operation & RANGE_NOT_INCLUSIVE)
			retval = L"(" + value + L")";
		else
			retval = value;
	}
	catch (...)
    {
        ReportError("GetCellValueWithOperation");
    }
    return retval;
}

//compiler
void ProjectManager::WriteAllDataSetsToXMLFile(wstring savePath)
{
	try
	{
		xmlDocPtr xmlDoc = xmlNewDoc((xmlChar*)"1.0");

		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
		xmlNodePtr tablesRootNode = xmlNewNode(NULL, (xmlChar*)"Tables");

		string strDebug = "false";
		if (m_deubgStaus) strDebug = "true";
		wstring tables;
		if (m_SelectedTables.size() > 0)
		{			
			for (vector<wstring>::iterator it = m_SelectedTables.begin(); it != m_SelectedTables.end(); it++)
				tables += *it + L",";
			if (tables.length() > 0)
				tables = tables.substr(0, tables.length() - 1);			
		}
		xmlSetProp(tablesRootNode, (xmlChar*)"debugtables", (xmlChar*)UTILS::WStrToMBCStr(tables).c_str());
		xmlSetProp(tablesRootNode, (xmlChar*)"debug", (xmlChar*)strDebug.c_str());
		xmlSetProp(tablesRootNode, (xmlChar*)"connection", (xmlChar*)UTILS::WStrToMBCStr(m_DebugConnection).c_str());
		xmlDocSetRootElement(xmlDoc, tablesRootNode);

		//just like saving individual table, but we add indexing and combine them all
		for (size_t j = 0; j < m_project_files->Rows(); j++)
		{
			wstring name = UTILS::FindAndReplace(m_project_files->GetItem(j, L"DataSetName"), L".xml", L"");
			if (name.length() > 0 && name != GLOBALORS_TABLE_NAME && name != TRANSLATIONS_TABLE_NAME)
			{
				DataSet<wstring> ds = LoadDataSet(name);
				bool GetAll = TableIsGetAll(name);

				xmlNodePtr tableDataNode = xmlNewNode(NULL, (xmlChar*)"Table");
				xmlSetProp(tableDataNode, (xmlChar*)"name", (xmlChar*)UTILS::WStrToMBCStr(ds.DataSetName).c_str());
				if (GetAll)
					xmlSetProp(tableDataNode, (xmlChar*)"getall", (xmlChar*)"true");
				else
					xmlSetProp(tableDataNode, (xmlChar*)"getall", (xmlChar*)"false");
				xmlAddChild(tablesRootNode, tableDataNode);

				//input table
				StringTable<wstring> inputs = ds.GetTableCopy(UTILS::ToWString(INPUT_TABLE));
				//output table
				StringTable<wstring> outputs = ds.GetTableCopy(UTILS::ToWString(OUTPUT_TABLE));
				//status, we will skip these rules
				StringTable<wstring> status = ds.GetTableCopy(UTILS::ToWString(STATUS_TABLE));

				for (long ruleCnt = status.Columns() - 1; ruleCnt > 0 ; ruleCnt--)
				{
					if (status.GetItem(0, ruleCnt) == L"Disabled")
					{
						inputs.RemoveColumn(ruleCnt + 1);
						outputs.RemoveColumn(ruleCnt + 1);
					}
				}

				//optimize the output by removing blank rules
				for (long ruleCnt = inputs.Columns() - 1; ruleCnt > 1 ; ruleCnt--)
				{
					bool bNoInputs = true;
					bool bNoOutputs = true;
					for (long cellCnt = outputs.Rows() - 1; cellCnt >= 0; cellCnt--)
					{
						wstring val = outputs.GetItem(cellCnt, ruleCnt);
						if (val.length() != 0)
						{
							bNoOutputs = false;
							break;
						}
					}

					if (bNoOutputs == true) for (long cellCnt = inputs.Rows() - 1; cellCnt >= 0; cellCnt--)
					{
						wstring val = inputs.GetItem(cellCnt, ruleCnt);
						if (val.length() != 0)
						{
							bNoInputs = false;
							break;
						}
					}

					if (bNoOutputs == true && bNoInputs == true)
					{
						inputs.RemoveColumn(ruleCnt);
						outputs.RemoveColumn(ruleCnt);
					}
				}

				WriteXMLForTable(tableDataNode, &inputs, INPUT_TABLE, true);
				WriteXMLForTable(tableDataNode, &outputs, OUTPUT_TABLE, true);
			}
		}

		//translation info
		if (UTILS::FileExists(m_project_working_path + PATHSEP + L"Translations.xml"))
		{
			DataSet<wstring> ds = LoadDataSet(L"Translations");
			StringTable<wstring> *transTable = ds.GetTable(UTILS::ToWString(INPUT_TABLE));
			xmlNodePtr translationsNode = xmlNewNode(NULL, (xmlChar*)"Translations");

			map<size_t, map<wstring, wstring> > mapBaseIDtoTranslations; //mapBaseIDtoTranslations[0]["en_US"] = "Hello"
			for (size_t j = 1; j < transTable->Rows(); j++) //skip base lang data, we identify by index
			{
				for (size_t i = 1; i < transTable->Columns(); i++)
				{
					wstring langType = transTable->GetItem(j, 0);
					wstring baseLangValue = transTable->GetItem(0, i);
					wstring langValue = transTable->GetItem(j, i);
					if (langValue.length() > 0 && baseLangValue.length() > 0)
					{
						map<wstring, size_t>::iterator itIndexFinder = stringDictionary.find(baseLangValue);
						if (itIndexFinder != stringDictionary.end())
						{
							size_t id = itIndexFinder->second;
							pair<wstring, wstring> kvp;
							kvp.first = langType;
							kvp.second = langValue;
							map<size_t, map<wstring, wstring> >::iterator itFind = mapBaseIDtoTranslations.find(id);
							if (itFind != mapBaseIDtoTranslations.end())
							{
								map<wstring, wstring > *newTranslation = &itFind->second;
								newTranslation->insert(kvp);
							}
							else
							{
								map<wstring, wstring> newTranslation;
								newTranslation.insert(kvp);
								pair<size_t, map<wstring, wstring> > idTrans_kvp;
								idTrans_kvp.first = id;
								idTrans_kvp.second = newTranslation;
								mapBaseIDtoTranslations.insert(idTrans_kvp);
							}
						}
					}
				}
			}

			for (map<size_t, map<wstring, wstring> >::iterator it = mapBaseIDtoTranslations.begin(); it != mapBaseIDtoTranslations.end(); it++)
			{
				xmlNodePtr stringNode = xmlNewNode(NULL, (xmlChar*)"String");
				string s_id = UTILS::stringify(it->first);
				xmlSetProp(stringNode, (xmlChar*)"id", (xmlChar*)s_id.c_str());
				map<wstring, wstring> *trasnlations = &(it->second);
				for (map<wstring, wstring>::iterator it_trans = trasnlations->begin(); it_trans != trasnlations->end(); it_trans++)
				{
					xmlSetProp(stringNode, (xmlChar*)UTILS::WStrToMBCStr(it_trans->first).c_str(), (xmlChar*)UTILS::WStrToMBCStr(it_trans->second).c_str());
				}
				xmlAddChild(translationsNode, stringNode);
			}

			xmlAddChild(tablesRootNode, translationsNode);
		}

		xmlNodePtr scriptNode = xmlNewNode(NULL, (xmlChar*)"Scripting");
		xmlNodePtr jsNode = xmlNewNode(NULL, (xmlChar*)"Javascript");
		xmlNodePtr pyNode = xmlNewNode(NULL, (xmlChar*)"Python");
		xmlNodePtr jsText = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(m_jsCode).c_str());
		xmlNodePtr pyText = xmlNewText((xmlChar*)UTILS::WStrToMBCStr(m_pyCode).c_str());
		xmlAddChild(pyNode, pyText);
		xmlAddChild(jsNode, jsText);
		xmlAddChild(scriptNode, pyNode);
		xmlAddChild(scriptNode, jsNode);
		xmlAddChild(tablesRootNode, scriptNode);

		xmlXPathFreeContext(xpathCtx);

		if (xmlDoc != NULL)
		{
			xmlSaveFormatFileEnc(UTILS::WStrToMBCStr(savePath).c_str(), xmlDoc, "UTF-8", 1);
		}
		xmlFreeDoc(xmlDoc);
	}
	catch (...)
    {
        ReportError("ProjectManager::WriteAllDataSetsToXMLDoc");
    }
}
