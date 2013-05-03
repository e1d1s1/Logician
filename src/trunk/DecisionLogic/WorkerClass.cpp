/////////////////////////////////////////////////////////////////////////////
// Name:        WorkerClass.cpp
// Purpose:     Application event handling
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2013 Eric D. Schmidt, DigiRule Solutions LLC
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
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/filesystem.hpp>
#include <cctype>

#include "WorkerClass.h"
#include "utilities.h"

const wstring INPUT_TABLE = L"Inputs";
const wstring OUTPUT_TABLE = L"Outputs";
const wstring VALUE_NAME = L"Value";
const wstring ATTR_NAME = L"Attr";

enum
{
    RULES_TABLE = 0,
    GLOBAL_ORS_TABLE,
    TRANSLATIONS_TABLE
};

void* pt2WorkerObject;
void SignalTableClosed(wstring tableName)
{
	WorkerClass *mySelf = (WorkerClass*)pt2WorkerObject;
	mySelf->ChildWindowHasClosed(tableName);
}

bool OpenTableCallback(wstring tableName)
{
	WorkerClass *mySelf = (WorkerClass*)pt2WorkerObject;
	return mySelf->LoadTable(tableName);
}

bool SaveTableCallback(OpenLogicTable *table)
{
	WorkerClass *mySelf = (WorkerClass*)pt2WorkerObject;
	return mySelf->Save(table);
}

WorkerClass::WorkerClass(GUIClass *gui, int orient)
{
	m_gui = gui;
	m_orientation = orient;
	m_debug_status = false;
	pt2WorkerObject = (void*)this;

	bIsSaved = true;
}

WorkerClass::~WorkerClass(void)
{
	SaveApplicationSettings();
}

bool WorkerClass::CheckSave()
{
	bool retval = false;
	try
	{
		int res = m_gui->PromptQuestion(L"Do you want to save your current changes?", L"Save Changes");
		if (res == ID_YES)
		{
			retval = Save();
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::CheckSave");
	}
	return retval;
}

wstring WorkerClass::GetProjectName()
{
	if (m_pm.GetProjectFilePath().length() > 0)
		return m_pm.GetProjectName();
	else return L"";
}

bool WorkerClass::NewProject()
{
	bool retval = false;
	try
	{
		if (m_gui->GetOpenWindows()->size() > 0 || bIsSaved == false)
			CheckSave();
		m_gui->CloseAllWindows();

		wstring savePath = m_gui->SaveDialog("dlp");
		if (savePath.length() > 0)
		{
			SaveApplicationSettings();
			m_pm.Reset();
			m_pm.SaveNewProjectFileAs(savePath);
			retval = true;
			bIsSaved = true;
			m_gui->AddAllProjectNodes(m_pm.GetProjectTable());
			GetSettings();
			SaveApplicationSettings();
		}

	}
	catch(...)
	{
		ReportError("WorkerClass::NewProject");
	}
	return retval;
}

bool WorkerClass::OpenProject(wstring fileName)
{
	bool retval = false;
	try
	{
		if (m_gui->GetOpenWindows()->size() > 0 || bIsSaved == false)
			CheckSave();
		m_gui->CloseAllWindows();

		wstring openPath;
		if (fileName.length() == 0)
		{
			openPath = m_gui->OpenDialog(L"Decision Logic Project files (*.dlp)|*.dlp");
		}
		else
		{
			openPath = fileName;
		}

		if (openPath.length() > 0 && m_pm.LoadProjectFile(openPath) == true)
		{
			bIsSaved = true;
			retval = true;
			m_gui->AddAllProjectNodes(m_pm.GetProjectTable());
			GetSettings();
			SaveApplicationSettings();
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::OpenProject");
	}
	return retval;
}

void WorkerClass::GetSettings()
{
	long l;

	m_debugOptions.connection = m_pm.GetConnection();
	m_gui->GetConfig(L"DebugMode", &l);
	m_debugOptions.debugMode = atoi(UTILS::stringify(l).c_str());
	if (m_debugOptions.debugMode == m_debugOptions.SELECTED_TABLES)
	{
		m_gui->GetConfig(L"HighlightDebug", &l);
		m_debugOptions.bOpenTable = l > 0;
	}
	else
	{
		m_debugOptions.bOpenTable = false;
	}

	m_debugOptions.selectedTables = m_pm.GetSelectedDebugTables();
	m_compileOptions.PythonCode = m_pm.GetPythonCode();
	m_compileOptions.JavascriptCode = m_pm.GetJavascriptCode();
}

void WorkerClass::SaveApplicationSettings()
{
	wstring str;
	if (m_pm.GetProjectFilePath().length() > 0)
	{
		//renently opened projects
		m_gui->GetConfig(L"RecentProjectList", &str);
		vector<wstring> files = UTILS::Split((wstring)str, L";");
		if (find(files.begin(), files.end(), m_pm.GetProjectFilePath()) == files.end())
			files.insert(files.begin(), m_pm.GetProjectFilePath());
		wstring fileList;
		int cnt = MAX_RECENT_FILES;
		for (vector<wstring>::iterator it = files.begin(); it != files.end(); it++)
		{
			if (cnt <= 1) break;
			if (fileList.length() > 0)
				fileList += L";";
			fileList += *it;
			cnt--;
		}
		m_gui->WriteConfig(L"RecentProjectList", fileList);

		//remember debugging options
		m_gui->WriteConfig(L"DebugMode", m_debugOptions.debugMode);
		m_gui->WriteConfig(L"HighlightDebug", (int)m_debugOptions.bOpenTable);
		vector<wstring> tables = m_debugOptions.selectedTables;
	}
}

bool WorkerClass::Save(OpenLogicTable *targetTable)
{
	try
	{
		bool bTablesSaved = true;

		m_gui->EnableAllChildWindows(false);

		for (vector<OpenLogicTable>::iterator it = m_gui->GetOpenWindows()->begin(); it != m_gui->GetOpenWindows()->end(); it++)
		{
			OpenLogicTable current_table;
			if (targetTable)
				current_table = *targetTable;
			else
				current_table = *it;

			MDIChild *childForm = (MDIChild*)current_table.child_window_ptr;

			//update with any table changes
			wstring oldPath = current_table.logic_table.Path;
			current_table = childForm->Save();
			current_table.logic_table.Path = oldPath;
			if (current_table.logic_table.HasData())
			{
				if (current_table.logic_table.Path.length() == 0)
				{
					current_table.logic_table.Path = m_pm.GetProjectWorkingPath();

					if (current_table.logic_table.Path[current_table.logic_table.Path.length() - 1] != PATHSEP)
							current_table.logic_table.Path = current_table.logic_table.Path + PATHSEP;

					//append the subdir, if needed

					if (current_table.logic_table.Name != GLOBALORS_TABLE_NAME &&
						current_table.logic_table.Name != TRANSLATIONS_TABLE_NAME)
					{
						//set path according to its place in the tree, relative to working directory
						wstring subdir = m_gui->GetTreeNodePath(m_gui->GetActiveGroupName());
						if (subdir.length() > 1)
							current_table.logic_table.Path = current_table.logic_table.Path + subdir;
						current_table.logic_table.Path = current_table.logic_table.Path + current_table.logic_table.Name + L".xml";
					}
					else
					{
						current_table.logic_table.Path = current_table.logic_table.Path +
							current_table.logic_table.Name + L".xml";
					}
				}

				if (!m_pm.SaveDataSet(current_table.logic_table.Name, current_table.logic_table.Path, current_table.logic_table.GetDataSet(), current_table.logic_table.bGetAll))
					bTablesSaved = false;
			}
			if (targetTable)
				break;
		}

		if (!targetTable)
		{
			bool bProjectSaved = m_pm.SaveProjectFile();

			if (bProjectSaved == true && bTablesSaved == true)
				bIsSaved = true;

			m_gui->EnableAllChildWindows(true);
		}
		else
		{
			m_gui->EnableAllChildWindows(true);
			return bTablesSaved;
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::Save");
	}

	return bIsSaved;
}

bool WorkerClass::LoadTable(wstring name)
{
	bool retval = false;
	try
	{
		if (name == L"Root")
		{
			m_gui->SelectAnItem(name);
			return false;
		}

		//see if the tree node is already selected
		m_gui->SelectAnItem(name);

		//see if the window is already open, if so activate it
		if (m_gui->BringWindowToFront(name))
			return true;

		int table_type = RULES_TABLE;
		if (name == GLOBALORS_TABLE_NAME)
			table_type = GLOBAL_ORS_TABLE;
		else if (name == TRANSLATIONS_TABLE_NAME)
			table_type = TRANSLATIONS_TABLE;

		wstring path = m_pm.GetProjectWorkingPath();
		if (table_type == RULES_TABLE)
		{
			DataSet<wstring> ds = m_pm.LoadDataSet(name);
			if (ds.TableCount() == 3)
			{
				path += PATHSEP + m_gui->GetTreeNodePath(m_gui->GetActiveGroupName()) + name + L".xml";
				LogicTable table;
				table.LoadDataSet(ds, name, path);
				table.bGetAll = m_pm.TableIsGetAll(name);
				MDIChild *childForm = new MDIChild(m_gui->GetParentFrame(), SignalTableClosed, OpenTableCallback, SaveTableCallback, m_orientation, table_type, m_gui->GetOpenWindows(), table, &m_pm, name);
				childForm->Show(true);
				retval = true;
			}
		}
		else
		{
			path += PATHSEP + name + L".xml";;
			if (UTILS::FileExists(path))
			{
				DataSet<wstring> ds = m_pm.LoadDataSet(name);
				LogicTable table;
				table.LoadDataSet(ds, name, path);
				MDIChild *childForm = new MDIChild(m_gui->GetParentFrame(), SignalTableClosed, OpenTableCallback, SaveTableCallback, m_orientation, table_type, m_gui->GetOpenWindows(), table, &m_pm, name);
				childForm->Show(true);
				retval = true;
			}
			else
			{
				NewTable(name);
			}


		}
	}
	catch(...)
	{
		ReportError("WorkerClass::LoadTable");
	}
	return retval;
}

void WorkerClass::CloseTable()
{
    wstring nameToDelete;
	OpenLogicTable opened;

    opened = m_gui->GetActiveChild();

    if (!opened.child_window_ptr)
        return;
    else
        nameToDelete = opened.logic_table.Name;

    if (nameToDelete.length() == 0)
        return;

    m_gui->CloseWindow(nameToDelete);
}

bool WorkerClass::MoveTable(wstring table, wstring oldLoc, wstring newLoc)
{
	bool retval = false;
	try
	{
		if (!bIsSaved)
		{
			if (!CheckSave())
			{
				m_gui->PromptMessage(L"You must save any existing changes before moving an existing table.");
				return retval;
			}
		}

		wstring pathFrom = m_pm.GetProjectWorkingPath() + PATHSEP;
		wstring pathTo = m_pm.GetProjectWorkingPath() + PATHSEP;

		wstring folderFrom = oldLoc;
		if (folderFrom.length() > 0)
			if (!(folderFrom.length() == 1 && folderFrom[0] == PATHSEP))
				pathFrom += folderFrom;
		pathFrom += table + L".xml";

		wstring folderTo = newLoc;
		if (folderTo.length() > 0)
			if (!(folderTo.length() == 1 && folderTo[0] == PATHSEP))
				pathTo += folderTo;
		pathTo += table + L".xml";

		//close the window if its open
		bool bWasClosed = m_gui->CloseWindow(table);

		//make sure any subdirs exist
		if (pathTo.length() > 0)
		{
			wstring fileName = PATHSEP + table + L".xml";
			wstring pathToFile = UTILS::FindAndReplace(pathTo, fileName, L"");
			#ifdef WIN32
			_wmkdir(pathToFile.c_str());
			#else
			mkdir(UTILS::WStrToMBCStr(pathToFile).c_str(), 0777);
			#endif
		}

		//move the file on disk
		#ifdef WIN32
		FILE *oldFile = _wfopen(pathFrom.c_str(), L"rb");
		FILE *newFile = _wfopen(pathTo.c_str(), L"wb");
		#else
		FILE *oldFile = fopen(UTILS::WStrToMBCStr(pathFrom).c_str(), "rb");
		FILE *newFile = fopen(UTILS::WStrToMBCStr(pathTo).c_str(), "wb");
		#endif

		if (oldFile == NULL || newFile == NULL)
		{
			ReportError("Could not move the file");
			return retval;
		}
		else
		{
			char *buffer = NULL;
			fseek(oldFile, 0, SEEK_END);
			size_t len = ftell(oldFile);
			buffer = new char[len];
			rewind(oldFile);
			size_t result = fread(buffer, 1, len, oldFile);
			if (result > 0)
				fwrite(buffer, 1, len, newFile);
			delete[] buffer;
			fclose(oldFile);
			fclose(newFile);
			#ifdef WIN32
			_wremove(pathFrom.c_str());
			#else
			remove(UTILS::WStrToMBCStr(pathFrom).c_str());
			#endif
		}

		m_pm.MoveFile(table + L".xml", oldLoc, newLoc);

		//rearrange the nodes
		vector<wstring> existing_names = m_gui->GetChildrenOfGroup(newLoc);
		existing_names.push_back(table);
		sort(existing_names.begin(), existing_names.end());
		size_t pos = find(existing_names.begin(), existing_names.end(), table) - existing_names.begin();
		wstring prevValue;
		if (existing_names.size() > 1)
		{
			if (pos > 0)
			{
				wstring prevValue = existing_names[pos - 1];
			}
		}

		m_gui->SetActiveGroup(oldLoc);
		m_gui->DeleteTreeNode(table);
		m_gui->SetActiveGroup(newLoc);
		m_gui->AddTreeNodeToActiveGroup(prevValue, table);

		//if the windows was closed, reopen it
		if (bWasClosed)
			LoadTable(table);

		Save();

		retval = true;
	}
	catch(...)
	{
		ReportError("WorkerClass::MoveTable");
	}
	return retval;
}

void WorkerClass::RenameTable(wstring oldTableName, wstring newTableName)
{
	wstring existingName, newName;
	OpenLogicTable opened;

	if (oldTableName.length() == 0 && newTableName.length() == 0)
	{
		opened = m_gui->GetActiveChild();

		if (!opened.child_window_ptr)
			return;
		else
			existingName = opened.logic_table.Name;

		newName = m_gui->PromptUserForText(L"Rename Table:", L"Rename Table", existingName);
	}
	else
	{
		existingName = oldTableName;
		newName = newTableName;
	}

	if (newName == existingName && existingName.length() > 0)
		return;

	if (newName.length() == 0)
		return;

	if (!ValidateFolderName(newName))
	{
		m_gui->PromptMessage(L"Invalid file name.");
		return;
	}

	if (newName == GLOBALORS_TABLE_NAME || newName == TRANSLATIONS_TABLE_NAME)
	{
		m_gui->PromptMessage(L"This name is a reserved system table, please try again");
		return;
	}

	if (newName.length() > 0 && existingName.length() > 0)
	{
		//now add the table as a new item
		vector<wstring> existing_names = m_pm.GetProjectTableNames();
		vector<wstring>::iterator itFind = find(existing_names.begin(), existing_names.end(), newName);
		if (itFind == existing_names.end())
		{
			wstring path, oldPath;
			if (opened.child_window_ptr)
			{
				opened.logic_table.Name = newName;
				oldPath = opened.logic_table.Path;
				opened.logic_table.Path = opened.logic_table.Path.replace(opened.logic_table.Path.find(existingName), existingName.length(), newName);
				path = opened.logic_table.Path;
			}
			else
			{
				path = m_pm.GetProjectWorkingPath();
				wstring folder = m_gui->GetTreeNodePath(existingName);
				if (folder.length() > 0)
				{
					path += PATHSEP + folder;
					oldPath = path;
				}
				oldPath += PATHSEP + existingName + L".xml";
				path += PATHSEP + newName + L".xml";
			}
			if (m_pm.RenameDataSet(existingName + L".xml", newName + L".xml"))
			{
				existing_names.erase(find(existing_names.begin(), existing_names.end(), existingName));
				existing_names.push_back(newName);
				sort(existing_names.begin(), existing_names.end());

				m_gui->CloseWindow(existingName);
				m_gui->AddTreeNodeToActiveGroup(existingName, newName);
				m_gui->DeleteTreeNode(existingName);

				//rename the file on disk
				if (ChangeXMLTableName(oldPath, path, existingName, newName))
				{
                    if (opened.child_window_ptr)
                        LoadTable(newName);
                    Save();
				}
			}
		}
		else
		{
			m_gui->PromptMessage(L"This table name already exists.");
		}
	}
}

bool WorkerClass::ChangeXMLTableName(wstring oldpath, wstring newpath, wstring oldName, wstring newName)
{
    bool retval = false;
    xmlDocPtr doc = xmlParseFile(UTILS::WStrToMBCStr(oldpath).c_str());

    wstring srcStr = L"//Tables/Table[@name='";
    srcStr += oldName;
    srcStr += L"']";

    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    string xpathExpression = UTILS::WStrToMBCStr(srcStr);
    xmlChar* tableXPath = (xmlChar*)xpathExpression.c_str();
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(tableXPath, xpathCtx);
    xmlNodeSetPtr allRes= xpathObj->nodesetval;

    if (allRes != NULL && allRes->nodeNr == 1)
    {
        xmlNodePtr TableNode = allRes->nodeTab[0];
        xmlSetProp(TableNode, (xmlChar*)"name", (xmlChar*)UTILS::WStrToMBCStr(newName).c_str());
        if (doc != NULL)
		{
			xmlSaveFormatFileEnc(UTILS::WStrToMBCStr(newpath).c_str(), doc, "UTF-8", 1);
		}
		xmlFreeDoc(doc);
		#ifdef WIN32
        _wremove(oldpath.c_str());
        #else
        remove(UTILS::WStrToMBCStr(oldpath).c_str());
        #endif
        retval = true;
    }
    return retval;
}

void WorkerClass::InsertTable()
{
	try
	{
		bool bInsertedOK = false;
		if (!bIsSaved && m_gui->GetNodeCount() > 1)
		{
			if (!CheckSave())
			{
				m_gui->PromptMessage(L"You must save any existing changes before creating a new table.");
				return;
			}
		}

		//prompt user for a table from disk, copy to the working dir unless it is the same dir
		wstring path = m_gui->OpenDialog(L"Decision Logic Table files (*.xml)|*.xml");
		if (path.length() == 0)
			return;
		if (UTILS::StringContains(path, GLOBALORS_TABLE_NAME) || UTILS::StringContains(path, TRANSLATIONS_TABLE_NAME))
		{
			m_gui->PromptMessage(L"This file is a reserved system table, please try again");
			return;
		}

		wstring wstrSep;
		wstrSep += PATHSEP;
		vector<wstring> pathParts = UTILS::Split(path, wstrSep);
		wstring name = UTILS::FindAndReplace(pathParts[pathParts.size() - 1], L".xml", L"");

		wstring target = m_pm.GetProjectWorkingPath();
        target += m_gui->GetTreeNodePath(m_gui->GetActiveGroupName()) + name + L".xml";
		wstring sourcePath = path;
		wstring targetPath = target;
		if (targetPath != sourcePath)
		{
			//copy from source to target
			//move the file on disk
			#ifdef WIN32
			FILE *oldFile = _wfopen(path.c_str(), L"rb");
			FILE *newFile = _wfopen(target.c_str(), L"wb");
			#else
			FILE *oldFile = fopen(UTILS::WStrToMBCStr(path).c_str(), "rb");
			FILE *newFile = fopen(UTILS::WStrToMBCStr(targetPath).c_str(), "wb");
			#endif

			if (oldFile != NULL)
			{
				if (newFile == NULL)
				{
					size_t pos = targetPath.find_last_of(PATHSEP);
					wstring newPath = targetPath.substr(0, pos);
					boost::filesystem::create_directories(newPath);
                    #ifdef WIN32
                        newFile = _wfopen(target.c_str(), L"wb");
                    #else
                        newFile = fopen(UTILS::WStrToMBCStr(target).c_str(), "wb");
                    #endif
				}

				if (newFile != NULL)
				{
					char *buffer = NULL;
					fseek(oldFile, 0, SEEK_END);
					size_t len = ftell(oldFile);
					buffer = new char[len];
					rewind(oldFile);
					size_t result = fread(buffer, 1, len, oldFile);
					if (result > 0)
						fwrite(buffer, 1, len, newFile);
					delete[] buffer;
					fclose(newFile);
					bInsertedOK = true;
				}
				fclose(oldFile);
			}
		}
		else
		{
            bInsertedOK = true;
		}

		if (bInsertedOK)
		{
			m_pm.AddDataSetFile(name, targetPath);
			AddTableToProject(name, false, false);
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::InsertTable");
	}
}

void WorkerClass::NewTable(wstring name)
{
	try
	{
		if (!bIsSaved && m_gui->GetNodeCount() > 1)
		{
			if (!CheckSave())
			{
				m_gui->PromptMessage(L"You must save any existing changes before creating a new table.");
				return;
			}
		}

		bool bUserEnteredName = name.length() == 0;
		bool bSystemTable = false;
		if (bUserEnteredName)
			name = m_gui->PromptUserForText(L"Enter new table name:", L"Table Name");
		if (name == GLOBALORS_TABLE_NAME || name == TRANSLATIONS_TABLE_NAME)
		{
			if (bUserEnteredName)
			{
				m_gui->PromptMessage(L"This name is a reserved system table, please try again");
				return;
			}
			bSystemTable = true;
		}
		if (name.length() == 0)
			return;
		if (!ValidateFolderName(name))
		{
			m_gui->PromptMessage(L"Invalid file name.");
			return;
		}

		AddTableToProject(name, true, bSystemTable);
	}
	catch(...)
	{
		ReportError("WorkerClass::NewTable");
	}
}

void WorkerClass::AddTableToProject(wstring name, bool bCreateNew, bool bSystemTable)
{
	if (name.length() > 0)
	{
		vector<wstring> existing_names = m_pm.GetProjectTableNames(m_gui->GetTreeNodePath(m_gui->GetActiveGroupName()));
		vector<wstring>::iterator itFind = find(existing_names.begin(), existing_names.end(), name);
		if (!bCreateNew || itFind == existing_names.end())
		{
			existing_names.push_back(name);
			sort(existing_names.begin(), existing_names.end());

			if (!bSystemTable)
			{
				size_t pos = find(existing_names.begin(), existing_names.end(), name) - existing_names.begin();

				wstring preValue;
				if (existing_names.size() > 1)
				{
					if (pos > 0)
					{
						wstring preValue = existing_names[pos - 1];
					}
				}
				m_gui->AddTreeNodeToActiveGroup(preValue, name);
			}

			int table_type = RULES_TABLE;
			if (name == GLOBALORS_TABLE_NAME)
				table_type = GLOBAL_ORS_TABLE;
			else if (name == TRANSLATIONS_TABLE_NAME)
				table_type = TRANSLATIONS_TABLE;

			if (bCreateNew)
			{
				LogicTable table;
				table.CreateLogicTable(name);
				table.Path = m_pm.GetProjectWorkingPath();
				table.bGetAll = true;
				if (table.Path[table.Path.length() - 1] != PATHSEP)
					table.Path = table.Path + PATHSEP;
				wstring subdir = m_gui->GetTreeNodePath(m_gui->GetActiveGroupName());
				if (subdir.length() > 1)
					table.Path = table.Path + subdir;
				table.Path = table.Path + table.Name + L".xml";
				MDIChild *childForm = new MDIChild(m_gui->GetParentFrame(), SignalTableClosed, OpenTableCallback, SaveTableCallback, m_orientation, table_type, m_gui->GetOpenWindows(), table, &m_pm, name);
				childForm->Show(true);
			}

			bIsSaved = false;
		}
		else
		{
			m_gui->PromptMessage(L"This table name already exists.");
		}
	}
}

void WorkerClass::GenerateTranslations()
{
	try
	{
		//save the project file so the string dictionary is generated
		Save();
		set<wstring> *strings = m_pm.GetProjectStrings();

		//load the table
		LoadTable(TRANSLATIONS_TABLE_NAME);
		MDIChild *childForm = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
		childForm->RepopulateTranslationsTable(strings);
	}
	catch(...)
	{
		ReportError("WorkerClass::GenerateTranslations");
	}
}

void WorkerClass::DeleteTable(wstring nameDelete)
{
	try
	{
		wstring nameToDelete;
		wstring path = m_pm.GetProjectWorkingPath();
		OpenLogicTable opened;

		if (nameDelete.length() > 0)
		{
			nameToDelete = nameDelete;
			wstring folder = m_gui->GetTreeNodePath(nameDelete);
			if (folder.length() > 0)
				path += PATHSEP + folder;
		}
		else
		{
			//get the active child
			opened = m_gui->GetActiveChild();

			if (!opened.child_window_ptr)
				return;
			else
				nameToDelete = opened.logic_table.Name;
		}

		if (nameToDelete.length() == 0)
		{
			nameToDelete = m_gui->GetSelectedNodeName();
		}

		path += PATHSEP + nameToDelete + L".xml";

		if (nameToDelete.length() > 0 && path.length() > 0)
		{
			wstring question = L"Do you want to remove the " + nameToDelete + L" table?";
			int ans = m_gui->PromptQuestion(question, L"Remove Table?");

			if (ans == ID_YES)
			{
				m_gui->DeleteTreeNode(nameToDelete);
				if (opened.child_window_ptr)
					m_gui->CloseWindow(nameToDelete);
				m_pm.DeleteDataSet(nameToDelete + L".xml");
				if (UTILS::FileExists(path))
				{
					ans = m_gui->PromptQuestion(L"Table removed from project.\nDo you want to delete the file as well?", L"Delete File?");
					if (ans == ID_YES)
					{
						#ifdef WIN32
						_wremove(path.c_str());
						#else
						remove(UTILS::WStrToMBCStr(path).c_str());
						#endif
						//if file is gone, force a save of the project
						Save();
					}
				}
			}
		}

	}
	catch(...)
	{
		ReportError("WorkerClass::DeleteTable");
	}
}

void WorkerClass::NewGroup()
{
	try
	{
		if (!bIsSaved)
		{
			CheckSave();
		}

		wstring name = m_gui->PromptUserForText(L"Enter new group name:", L"Group Name");
		if (name.length() > 0 && ValidateFolderName(name))
		{
			m_gui->AddTreeNodeToActiveGroup(L"", name, "Group");
			Save();
		}
		else
			m_gui->PromptMessage(L"Invalid file name");

	}
	catch(...)
	{
		ReportError("WorkerClass::NewGroup");
	}
}

void WorkerClass::RenameGroup(wstring oldGroupName, wstring newGroupName)
{
	try
	{
		if (!bIsSaved)
		{
			if (!CheckSave())
			{
				m_gui->PromptMessage(L"You must save any existing changes before renaming an existing group.");
				return;
			}
		}

		vector<wstring> tables = m_gui->GetChildTablesOfActiveGroup(), tableThatClosed;
		wstring activeTable = m_gui->GetActiveChild().logic_table.Name;
		for (vector<wstring>::iterator it = tables.begin(); it != tables.end(); it++)
			if (m_gui->CloseWindow(*it))
				tableThatClosed.push_back(*it);

		wstring existingName, newName;
		if (oldGroupName.length() == 0 && newGroupName.length() == 0)
		{
			existingName = m_gui->GetActiveGroupName();
			newName = m_gui->PromptUserForText(L"Rename Group:", L"Rename Group", existingName);
		}
		else
		{
			existingName = oldGroupName;
			newName = newGroupName;
		}

		if (newName == existingName && existingName.length() > 0)
			return;

		if (!ValidateFolderName(newName))
		{
			m_gui->PromptMessage(L"Invalid folder name.");
			return;
		}

		wstring path, oldPath;
		path = m_pm.GetProjectWorkingPath();
		oldPath = m_pm.GetProjectWorkingPath();
		wstring wsPathSep; wsPathSep += PATHSEP;
		wstring oldFolder = m_gui->GetTreeNodePath(existingName);
		if (oldFolder.length() > 0)
		{
			path += PATHSEP + UTILS::FindAndReplace(oldFolder, existingName, newName);
			oldPath += PATHSEP + oldFolder;
		}

		if (path.length() > 0 && oldPath.length() > 0)
		{
			if (m_pm.RenameDataSetFolder(existingName + PATHSEP, newName + PATHSEP))
			{
				m_gui->DeleteTreeNode(existingName); //the new active node will become the parent node of whats being deleted
				m_gui->AddTreeNodeToActiveGroup(existingName, newName, "Group");
				Save();

				//rename the folder on disk
				#ifdef WIN32
				_wrename(oldPath.c_str(), path.c_str());
				#else
				rename(UTILS::WStrToMBCStr(oldPath).c_str(), UTILS::WStrToMBCStr(path).c_str());
				#endif

				m_gui->AddAllProjectNodes(m_pm.GetProjectTable());
				m_gui->SelectAnItem(newName);
			}
			else if (m_gui->GetChildTablesOfActiveGroup().size() == 0)
			{
				m_gui->AddAllProjectNodes(m_pm.GetProjectTable());
				m_gui->SelectAnItem(newName);
			}

			for (vector<wstring>::iterator it = tableThatClosed.begin(); it != tableThatClosed.end(); it++)
				LoadTable(*it);
			if (activeTable.length() > 0)
				LoadTable(activeTable);
		}

	}
	catch(...)
	{
		ReportError("WorkerClass::RenameGroup");
	}
}

bool WorkerClass::ValidateFolderName(wstring name)
{
	char invalidCharacters[9] = {'<', '>',':', '\"', '/', '\\', '|', '\?', '*'};
	for (int i = 0; i < name.length(); i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (name[i] == invalidCharacters[j])
				return false;
		}
	}

	if (name.length() > 0)
		return true;
	else
		return false;
}

void WorkerClass::DeleteGroup()
{
	try
	{
		wstring groupName = m_gui->GetActiveGroupName();
		vector<wstring> list = m_gui->GetChildrenOfActiveGroup();
		vector<wstring> paths;
		wstring dirPath;
		if (list.size() > 0)
		{
			dirPath = m_gui->GetTreeNodePath(groupName);
			for (vector<wstring>::iterator itChild = list.begin(); itChild != list.end(); itChild++)
			{
				wstring name = *itChild;
				if (name.length() > 0)
				{
					wstring path = m_pm.GetProjectWorkingPath();
					path += PATHSEP + dirPath + name + L".xml";
					paths.push_back(path);
				}
			}
		}

		if (groupName != L"Root")
			m_gui->DeleteTreeNode(groupName);

		if (paths.size() > 0)
		{	int ans = m_gui->PromptQuestion(L"Group removed from project.\nDo you want to delete the associated files as well?", L"Delete Files?");
			if (ans == ID_YES)
			{
				for (vector<wstring>::iterator it = paths.begin(); it != paths.end(); it++)
				{
					#ifdef WIN32
					_wremove(it->c_str());
					#else
					remove(UTILS::WStrToMBCStr(*it).c_str());
					#endif
				}
				if (dirPath.length() > 0 && !(dirPath.length() == 1 && dirPath[0] == PATHSEP))
				{
					#ifdef WIN32
					_wrmdir(dirPath.c_str());
					#else
					rmdir(UTILS::WStrToMBCStr(dirPath).c_str());
					#endif
				}
			}
		}
		else if (dirPath.length() > 1 && !(dirPath.length() == 1 && dirPath[0] == PATHSEP))
		{
			#ifdef WIN32
			_wrmdir(dirPath.c_str());
			#else
			rmdir(UTILS::WStrToMBCStr(dirPath).c_str());
			#endif
		}


	}
	catch(...)
	{
		ReportError("WorkerClass::DeleteGroup");
	}
}

void WorkerClass::SetOrientation(int orient)
{
	m_orientation = orient;

	//reorient all open tables
	for (vector<OpenLogicTable>::iterator it = m_gui->GetOpenWindows()->begin(); it != m_gui->GetOpenWindows()->end(); it++)
	{
		OpenLogicTable opened = *it;
		MDIChild *childForm = (MDIChild*)opened.child_window_ptr;
		childForm->RotateOrientation();
	}
}

void WorkerClass::ShowDebugOptions()
{
	vector<wstring> allTables = m_pm.GetProjectTableNames();
	//erase system tables from list
	vector<wstring>::iterator itFind = find(allTables.begin(), allTables.end(), GLOBALORS_TABLE_NAME);
	if (itFind != allTables.end()) allTables.erase(itFind);
	itFind = find(allTables.begin(), allTables.end(), TRANSLATIONS_TABLE_NAME);
	if (itFind != allTables.end()) allTables.erase(itFind);

	m_debugOptions.allTables = allTables;
	DebugOptionsDialog debugDialog(m_gui->GetParentFrame(), -1, &m_debugOptions);
	debugDialog.ShowModal();
	m_pm.SetDebugStatus(m_debug_status, m_debugOptions.connection, m_debugOptions.selectedTables);
}

void WorkerClass::ShowCompileOptions()
{
	CompileOptionsDialog compileDialog(m_gui->GetParentFrame(), -1, &m_compileOptions);
	compileDialog.ShowModal();
	bIsSaved = false;
	m_pm.SetCompilerOptions(m_compileOptions.JavascriptCode, m_compileOptions.PythonCode);
}

wstring WorkerClass::CompileXML(wstring tempFilePath)
{
	wstring retval;
	try
	{
		Save();

		bool bOverWriteTable = false;
		if (tempFilePath.length() == 0)
		{
			if (lastCompiledFileName.length() == 0)
				lastCompiledFileName = m_pm.GetProjectTitle();
			wstring savePath = m_gui->SaveDialog("xml", lastCompiledFileName);
			bOverWriteTable = CheckOKCompilePath(savePath);
			if (!bOverWriteTable && savePath.length() > 0)
			{
				m_pm.WriteAllDataSetsToXMLFile(savePath);
				lastCompiledFileName = UTILS::StripExtension(savePath);
				retval = savePath;
			}
			else if (bOverWriteTable)
			{
				m_gui->PromptMessage(L"You cannot overwrite an existing project table file with compiled output, you would lose your data.\nChoose a new output filename.");
			}
		}
		else
		{
			bOverWriteTable = CheckOKCompilePath(tempFilePath);
			if (!bOverWriteTable)
			{
				m_pm.WriteAllDataSetsToXMLFile(UTILS::FindAndReplace(tempFilePath, L".gz", L".xml"));
				retval = tempFilePath;
			}
			else
				m_gui->PromptMessage(L"You cannot overwrite an existing project table file with compiled output, you would lose your data.\nChoose a new output filename.");
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::CompileXML");
		retval.empty();
	}
	return retval;
}

bool WorkerClass::CheckOKCompilePath(wstring savePath)
{
	bool bExists = UTILS::FileExists(savePath);
	//make sure not overwriting one of our own project files
	vector<wstring> existingFiles = m_pm.GetProjectFilePaths();
	bool bOverWriteTable = false;
	if (bExists)
		bOverWriteTable = find(existingFiles.begin(), existingFiles.end(), savePath) != existingFiles.end();
	return bOverWriteTable;
}

void WorkerClass::CompileZip()
{
	try
	{
		if (lastCompiledFileName.length() == 0)
			lastCompiledFileName = m_pm.GetProjectTitle();
		wstring savePath = m_gui->SaveDialog("gz", lastCompiledFileName);
		if (savePath.length() > 0)
		{
			lastCompiledFileName = UTILS::StripExtension(savePath);
			CompileXML(savePath);
			//compress file
			string sPath = UTILS::WStrToMBCStr(savePath);
			ifstream file(UTILS::WStrToMBCStr(UTILS::FindAndReplace(savePath, L".gz", L".xml")).c_str(), std::ifstream::in);
			ofstream outfile(UTILS::WStrToMBCStr(savePath).c_str(), std::ofstream::out | std::ofstream::binary);
			if (file.is_open() && outfile.is_open())
			{
				boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
				in.push(boost::iostreams::gzip_compressor());
				in.push(file);
				boost::iostreams::copy(in, outfile);
				file.close();
				outfile.close();

				wstring fileToRemove = UTILS::FindAndReplace(savePath, L".gz", L".xml");
				#ifdef WIN32
				_wremove(fileToRemove.c_str());
				#else
				remove(UTILS::WStrToMBCStr(fileToRemove).c_str());
				#endif
			}
		}

	}
	catch (std::exception ex)
	{
		ReportError(ex.what());
	}
	catch(...)
	{
		ReportError("WorkerClass::CompileZip");
	}
}

void WorkerClass::InsertCol()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->InsertCol();
	}
}

void WorkerClass::InsertRow()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->InsertRow();
	}
}

void WorkerClass::DeleteCol()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->DeleteCol();
	}
}

void WorkerClass::DeleteRow()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->DeleteRow();
	}
}

void WorkerClass::AppendRow()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->AppendRow();
	}
}

void WorkerClass::AppendColumn()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->AppendColumn();
	}
}

void WorkerClass::ClearCells()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->ClearCells();
	}
}

void WorkerClass::Redo()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Redo();
	}
}

void WorkerClass::Undo()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Undo();
	}
}

void WorkerClass::Copy()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Copy();
	}
}

void WorkerClass::Cut()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Cut();
	}
}

void WorkerClass::Paste()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Paste();
	}
}

void WorkerClass::EditCode()
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->EditCode();
	}
}

void WorkerClass::SetProjectDebugging(bool enabled, int server_id)
{
	m_debug_status = enabled;
	m_pm.SetDebugStatus(enabled, m_debugOptions.connection, m_debugOptions.selectedTables);
	if (enabled) //start socket server, listen for incoming connections
	{
		vector<wstring> con = UTILS::Split(m_debugOptions.connection, L":");
		unsigned short port = (unsigned short)atoi(UTILS::WStrToMBCStr(con[1]).c_str());
		// Create the socket
		if (m_gui)
		{
			m_gui->CreateServer(port);
		}

		//see if the server is really listening
		if (!m_gui->ServerReady())
		{
			LogTextLine(L"Could not listen at the specified port !n");
			return;
		}
		else
		{
			// Setup the event handler and subscribe to connection events
			m_gui->StartServer(server_id, &m_debugOptions);
			LogTextLine(L"Debugger server listening.\n");
		}
	}
	else
	{
		m_gui->ShutdownServer();

		LogTextLine(L"Debugger server stopped.\n");
	}
}


void WorkerClass::ChildWindowHasClosed(wstring tableName)
{
	//deselect all items of tree
	m_gui->ChildWindowsHasClosed(tableName);
}
