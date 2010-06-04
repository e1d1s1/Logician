/////////////////////////////////////////////////////////////////////////////
// Name:        WorkerClass.cpp
// Purpose:     Application event handling
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2010 Eric D. Schmidt
// Licence:     GNU GPLv3
/*
	DecisionLogic is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
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
#include <zlib.h>

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
	m_MaxRecentFiles = 5;	
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
		if (res == wxID_YES)
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
			openPath = m_gui->OpenDialog();
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
	wxString str;
	long l;

	m_debugOptions.connection = m_pm.GetConnection();
	m_gui->GetConfig(L"DebugMode", &l);
	m_debugOptions.debugMode = atoi(UTILS::stringify(l).c_str());
	if (m_debugOptions.debugMode == m_debugOptions.SELECTED_TABLES)
	{
		m_gui->GetConfig(L"HighlightDebug", &l);		
		m_debugOptions.bOpenTable = (bool)l;
	}
	else
	{
		m_debugOptions.bOpenTable = false;
	}

	m_debugOptions.selectedTables = m_pm.GetSelectedDebugTables();

	m_compileOptions.PythonCode == m_pm.GetPythonCode();
	m_compileOptions.JavascriptCode == m_pm.GetJavascriptCode();
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
		int cnt = m_MaxRecentFiles;
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
		m_gui->WriteConfig(L"DebugMode", UTILS::stringify((long)m_debugOptions.debugMode).c_str());
		m_gui->WriteConfig(L"HighlightDebug", UTILS::stringify((long)m_debugOptions.bOpenTable).c_str());
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
					//append the subdir, if needed

					if (current_table.logic_table.Name != GLOBALORS_TABLE_NAME &&
						current_table.logic_table.Name != TRANSLATIONS_TABLE_NAME)
					{
						//set path according to its place in the tree, relative to working directory
						current_table.logic_table.Path = current_table.logic_table.Path + PATHSEP +
							m_gui->GetTreeNodePath(m_gui->GetActiveGroupName()) +
							current_table.logic_table.Name + L".xml";
					}
					else
					{
						current_table.logic_table.Path = current_table.logic_table.Path + PATHSEP +
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
			return false;
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
				childForm->Show();
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
				childForm->Show();
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

void WorkerClass::MoveTable(wstring table, wstring oldLoc, wstring newLoc)
{
	try
	{

	}
	catch(...)
	{
		ReportError("WorkerClass::MoveTable");
	}
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

		newName = wxGetTextFromUser(_T("Rename Table:"), _T("Rename Table"));
	}
	else
	{
		existingName = oldTableName;
		newName = newTableName;
	}

	if (!ValidateFolderName(newName))
	{
		m_gui->PromptMessage(L"Invalid file name.");
		return;
	}

	bool bSystemTable = false;	
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
				#ifdef WIN32
				_wrename(oldPath.c_str(), path.c_str());
				#else
				rename(UTILS::WStrToMBCStr(oldPath).c_str(), UTILS::WStrToMBCStr(path).c_str());
				#endif

				if (opened.child_window_ptr)
					LoadTable(newName);					
				Save();
			}
		}
		else
		{
			m_gui->PromptMessage(L"This table name already exists.");
		}		
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
			name = wxGetTextFromUser(_T("Enter new table name:"), _T("Table Name"));
		if (name == GLOBALORS_TABLE_NAME || name == TRANSLATIONS_TABLE_NAME)
		{
			if (bUserEnteredName)
			{
				m_gui->PromptMessage(L"This name is a reserved system table, please try again");
				return;
			}
			bSystemTable = true;
		}
		if (!ValidateFolderName(name))
		{
			m_gui->PromptMessage(L"Invalid file name.");
			return;
		}

		if (name.length() > 0)
		{
			vector<wstring> existing_names = m_pm.GetProjectTableNames();			
			vector<wstring>::iterator itFind = find(existing_names.begin(), existing_names.end(), name);
			if (itFind == existing_names.end())
			{
				existing_names.push_back(name);
				sort(existing_names.begin(), existing_names.end());
				LogicTable table;
				table.CreateLogicTable(name);
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

				MDIChild *childForm = new MDIChild(m_gui->GetParentFrame(), SignalTableClosed, OpenTableCallback, SaveTableCallback, m_orientation, table_type, m_gui->GetOpenWindows(), table, &m_pm, name);
				childForm->Show();
				bIsSaved = false;
			}
			else
			{
				m_gui->PromptMessage(L"This table name already exists.");
			}
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::NewTable");
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
			path += PATHSEP + nameToDelete + L".xml";
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

		if (nameToDelete.length() > 0 && path.length() > 0)
		{
			wstring question = L"Do you wnat to delete the " + nameToDelete + L" table";
			int ans = m_gui->PromptQuestion(question, L"Remove Table?");

			if (ans == ID_YES && m_pm.DeleteDataSet(nameToDelete + L".xml"))
			{
				m_gui->DeleteTreeNode(nameToDelete);
				if (opened.child_window_ptr)
					m_gui->CloseWindow(nameToDelete);

				ans = m_gui->PromptQuestion(L"Table removed from project.\nDo you wnat to delete the file as well?", L"Remove File?");
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
	catch(...)
	{
		ReportError("WorkerClass::DeleteTable");
	}
}

void WorkerClass::NewGroup()
{
	try
	{
		if (bIsSaved)
		{
			CheckSave();
		}

		wstring name = m_gui->PromptUserForText(L"Enter new group name:", L"Group Name");
		if (name.length() > 0 && ValidateFolderName(name))
		{
			m_gui->AddTreeNodeToActiveGroup(NULL, name, "Group");			
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
		if (!CheckSave())
		{
			m_gui->PromptMessage(L"You must save any existing changes before renaming an existing group.");
			return;			
		}
		else
			m_gui->CloseAllWindows();
		
		wstring existingName, newName;
		if (oldGroupName.length() == 0 && newGroupName.length() == 0)
		{
			existingName = m_gui->GetActiveGroupName();
			newName = m_gui->PromptUserForText(L"Rename Group:", L"Rename Group");
		}
		else
		{
			existingName = oldGroupName;
			newName = newGroupName;
		}
		if (!ValidateFolderName(newName))
		{
			m_gui->PromptMessage(L"Invalid folder name.");
			return;
		}

		wstring path, oldPath;
		path = m_pm.GetProjectWorkingPath();
		oldPath = m_pm.GetProjectWorkingPath();
		wstring oldFolder = m_gui->GetTreeNodePath(existingName);
		if (oldFolder.length() > 0)
		{
			path += PATHSEP + newName;
			oldPath += PATHSEP + oldFolder;
		}

		if (path.length() > 0 && oldPath.length() > 0)
		{
			if (m_pm.RenameDataSetFolder(existingName, newName))
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
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::RenameGroup");
	}
}

bool WorkerClass::ValidateFolderName(wstring name)
{
	char invalidCharacters[9] = {'<', '>',':', '"', '/', '\\', '|', '?', '*'};
	for (int i = 0; i < name.length(); i++)
	{
		for (int j = 0; j < strlen(invalidCharacters); j++)
		{
			if (name[i] == invalidCharacters[j])
				return false;
		}
	}

	if (name.length() > 0)
		return true;
}

void WorkerClass::DeleteGroup()
{
	try
	{
		vector<wstring> list = m_gui->GetChildrenOfSelection();
		if (list.size() > 0)
		{			
			wstring groupName = m_gui->GetActiveGroupName();
			wstring dirPath = m_gui->GetTreeNodePath(groupName);
			vector<wstring> paths;
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

			m_gui->DeleteTreeNode(groupName);

			int ans = m_gui->PromptQuestion(L"Group removed from project.\nDo you wnat to delete the associated files as well?", L"Remove Files?");
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
				#ifdef WIN32
				_wrmdir(dirPath.c_str());
				#else
				rmdir(UTILS::WStrToMBCStr(dirPath).c_str());
				#endif
			}
			
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

void WorkerClass::GenerateRecentFileList(wxMenu *listMenu, int RecentFile_ID_BEGIN, int RecentFile_ID_END)
{
	wstring str;
	m_gui->ReadConfig(L"RecentProjectList", &str);	
	vector<wstring> files = UTILS::Split((wstring)str, L";");
	int cnt = RecentFile_ID_END - RecentFile_ID_BEGIN, index = 0;
	m_MaxRecentFiles = cnt;
	for (vector<wstring>::iterator it = files.begin(); it != files.end(); it++)
	{
		if (cnt <= 1 || (*it).length() == 0) break;
		listMenu->Append(RecentFile_ID_BEGIN + index, *it);
		cnt--;
		index++;
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
	DebugOptionsDialog debugDialog(m_gui->GetParentFrame(), wxID_ANY, &m_debugOptions);
	debugDialog.ShowModal();
	m_pm.SetDebugStatus(m_debug_status, m_debugOptions.connection, m_debugOptions.selectedTables);
}

void WorkerClass::ShowCompileOptions()
{
	CompileOptionsDialog compileDialog(m_gui->GetParentFrame(), wxID_ANY, &m_compileOptions);
	compileDialog.ShowModal();
	m_pm.SetCompilerOptions(m_compileOptions.JavascriptCode, m_compileOptions.PythonCode);
}

wstring WorkerClass::CompileXML(wstring tempFilePath)
{
	wstring retval;
	try
	{
		Save();

		if (tempFilePath.length() == 0)
		{
			wstring savePath = m_gui->SaveDialog("xml");
			if (savePath.length() > 0)
			{
				m_pm.WriteAllDataSetsToXMLFile(savePath);
				retval = savePath;
			}			
		}
		else
		{
			m_pm.WriteAllDataSetsToXMLFile(UTILS::FindAndReplace(tempFilePath, L".gz", L".xml"));
			retval = tempFilePath;
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::CompileXML");
		retval.empty();
	}
	return retval;
}

void WorkerClass::CompileZip()
{
	try
	{
		wstring savePath = m_gui->SaveDialog("gz");
		if (savePath.length() > 0)
		{
			CompileXML(savePath);
			//compress file
			string sPath = UTILS::WStrToMBCStr(savePath);
			FILE *infile = fopen(UTILS::WStrToMBCStr(UTILS::FindAndReplace(savePath, L".gz", L".xml")).c_str(), "rb");
			gzFile outfile = gzopen(UTILS::WStrToMBCStr(savePath).c_str(), "wb");
			if (!infile || !outfile) return;

			char inbuffer[128];
			int num_read = 0;
			unsigned long total_read = 0;
			while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0)
			{
				total_read += num_read;
				gzwrite(outfile, inbuffer, num_read);
			}
			fclose(infile);
			gzclose(outfile);
			wstring fileToRemove = UTILS::FindAndReplace(savePath, L".gz", L".xml");
			#ifdef WIN32
			_wremove(fileToRemove.c_str());
			#else
			remove(UTILS::WStrToMBCStr(fileToRemove).c_str());
			#endif
		}
		
	}
	catch(...)
	{
		ReportError("WorkerClass::CompileZip");
	}
}

void WorkerClass::InsertCol(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->InsertCol(event);
	}
}

void WorkerClass::InsertRow(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->InsertRow(event);
	}
}

void WorkerClass::DeleteCol(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->DeleteCol(event);
	}
}

void WorkerClass::DeleteRow(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->DeleteRow(event);
	}
}

void WorkerClass::AppendRow(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->AppendRow();
	}
}

void WorkerClass::AppendColumn(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->AppendColumn();
	}
}

void WorkerClass::ClearCells(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->ClearCells(event);
	}
}

void WorkerClass::Redo(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Redo(event);
	}
}

void WorkerClass::Undo(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Undo(event);
	}
}

void WorkerClass::Copy(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Copy(event);
	}
}

void WorkerClass::Cut(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Cut(event);
	}
}

void WorkerClass::Paste(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->Paste(event);
	}
}

void WorkerClass::EditCode(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_gui->GetActiveChild().child_window_ptr;
	if (active != NULL)
	{
		active->EditCode(event);
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
			LogTextLine(_("Could not listen at the specified port !n"));
			return;
		}
		else
		{
			// Setup the event handler and subscribe to connection events
			m_gui->StartServer(server_id, &m_debugOptions);
			LogTextLine(_("Debugger server listening.\n"));
		}
	}
	else
	{
		m_gui->ShutdownServer();
		
		LogTextLine(_("Debugger server stopped.\n"));
	}
}


void WorkerClass::ChildWindowHasClosed(wstring tableName)
{
	//deselect all items of tree
	m_gui->ChildWindowsHasClosed(tableName);	
}