/////////////////////////////////////////////////////////////////////////////
// Name:        ProjectManager.h
// Purpose:     Manages project related info for all tables, saving, loading, etc
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2012 Eric D. Schmidt, DigiRule Solutions LLC
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
#pragma once

#ifndef PROJECTMANAGER
#define PROJECTMANAGER

#include <string>
#include <set>
#include <map>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "StringTable.h"
#include "DataSet.h"


using namespace std;

class ProjectManager
{
public:
	ProjectManager(void);
	~ProjectManager(void);

	void Reset();
	bool SaveProjectFile();
	bool SaveNewProjectFileAs(wstring path);
	bool LoadProjectFile(wstring path);
	bool SaveDataSet(wstring name, wstring path, DataSet<wstring>* ds, bool GetAll);
	DataSet<wstring> LoadDataSet(wstring name);
	bool DeleteDataSet(wstring name);
	bool RenameDataSet(wstring oldName, wstring newName);
	bool RenameDataSetFolder(wstring oldFolder, wstring newFolder);
	bool AddDataSetFile(wstring name, wstring full_path);
	bool MoveFile(wstring name, wstring oldFolder, wstring newFolder);
	wstring GetProjectWorkingPath() {return m_project_working_path;}
	void WriteAllDataSetsToXMLFile(wstring savePath);
	StringTable<wstring>* GetProjectTable() {return m_project_files;}
	void SetDebugStatus(bool status, wstring con, vector<wstring> selectedDebugTables)
		{m_deubgStaus = status; m_DebugConnection = con; m_SelectedTables = selectedDebugTables;}
	void SetCompilerOptions(wstring js, wstring py)
		{m_jsCode = js; m_pyCode = py;}
	vector<wstring> GetProjectTableNames();
	vector<wstring> GetProjectTableNames(wstring path);
	wstring GetConnection() {return m_DebugConnection;}
	wstring GetJavascriptCode() {return m_jsCode;}
	wstring GetPythonCode() {return m_pyCode;}
	vector<wstring> GetSelectedDebugTables() {return m_SelectedTables;}
	wstring GetProjectFilePath() {return m_project_full_path_name;}
	wstring GetProjectName() {return m_project_name;}
	wstring GetProjectTitle();
	set<wstring>* GetProjectStrings() {return &stringCollection;}
	xmlDocPtr LoadTableRawXML(wstring name);
	bool TableIsGetAll(wstring name);

	map<wstring, vector<wstring> > GlobalORs;

private:
	xmlDocPtr GetProjectFileXML();
	StringTable<wstring>* ReadProjectFile(wstring path);
	void GetXMLValuesForCell(wstring cellText, long &operation, wstring &saveText, bool bReplaceORs);
	vector<wstring> ParseValueForGets(wstring val);
	void WriteXMLForTable(xmlNodePtr tableDataNode, StringTable<wstring>* table, string tableXMLID, bool bWriteIndexes);
	wstring ReplaceAGet(wstring &newString);
	xmlDocPtr GetDataSetXML(DataSet<wstring>* ds, bool GetAll);
	DataSet<wstring> ReadXML(wstring path);
	StringTable<wstring> CreateDataTableFromNodes(string nodeName, xmlDocPtr doc);
	wstring GetCellValueWithOperation(wstring text, long operation);
	void WriteXML(DataSet<wstring> ds, xmlDocPtr doc);
	void SaveStringDefs(xmlDocPtr doc);
	vector<wstring> CheckForOrs(wstring text);
	wstring GetFilePathForTableName(wstring name);
	void UpdateGlobalORs();

	wstring m_project_name;
	wstring m_project_working_path;
	wstring m_project_full_path_name;
	StringTable<wstring> *m_project_files;
	set<wstring> stringCollection;
	map<wstring, size_t> stringDictionary;

	bool m_deubgStaus;
	wstring m_DebugConnection;
	wstring m_CompiledName;
	vector<wstring> m_SelectedTables;

	wstring m_jsCode;
	wstring m_pyCode;
};

#endif
