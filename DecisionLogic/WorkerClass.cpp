/////////////////////////////////////////////////////////////////////////////
// Name:        WorkerClass.cpp
// Purpose:     Application event handling
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2009
// Copyright:   (c) 2009 Eric D. Schmidt
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
#include <stack>
#include <zlib.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "WorkerClass.h"
#include "utilities.h"

#ifdef POSIX
const wchar_t PATHSEP = L'/';
#else
const wchar_t PATHSEP = L'\\';
#endif

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

WorkerClass::WorkerClass(wxMDIParentFrame *parentFrame, wxTreeCtrl *tree, wxTextCtrl *log, int orient)
{
	m_tree = tree;
	m_parentFrame = parentFrame;
	m_orientation = orient;
	m_log = log;
	m_debug_status = false;
	m_server = NULL;
	config = new wxConfig(_T("DecisionLogic"));
	m_MaxRecentFiles = 5;
	wxtid_active_group = m_tree->GetRootItem();

	bIsSaved = false;
}

WorkerClass::~WorkerClass(void)
{
	SaveApplicationSettings();
	if (config) delete config;
	if (m_server) delete m_server;
}

bool WorkerClass::CheckSave()
{
	bool retval = false;
	try
	{
		wxMessageDialog saveNow(m_parentFrame, _T("Do you want to save your current changes?"),
				_T("Save Changes"), wxYES_NO);
		int res = saveNow.ShowModal();
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

bool WorkerClass::NewProject()
{
	bool retval = false;
	try
	{
		wstring savePath;
		wxFileDialog saveFileDialog (
						m_parentFrame,
						_T("Save new project file"),
						wxEmptyString,
						wxEmptyString,
						_T("Decision Logic Project files (*.dlp)|*.dlp"),
						wxSAVE
					 );

		saveFileDialog.CentreOnParent();

		if (saveFileDialog.ShowModal() == wxID_OK)
		{
			SaveApplicationSettings();
			m_pm.Reset();
			savePath = saveFileDialog.GetPath();
			if (savePath.length() > 0)
			{
				m_pm.SaveNewProjectFileAs(savePath);
				retval = true;
			}
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
		wstring openPath;
		if (fileName.length() == 0)
		{
			wxFileDialog openFileDialog (
							m_parentFrame,
							_T("Open an existing project file"),
							wxEmptyString,
							wxEmptyString,
							_T("Decision Logic Project files (*.dlp)|*.dlp"),
							wxOPEN
						 );

			openFileDialog.CentreOnParent();

			if (openFileDialog.ShowModal() == wxID_OK)
			{
				openPath = openFileDialog.GetPath();
			}
		}
		else
		{
			openPath = fileName;
		}

		if (m_pm.LoadProjectFile(openPath) == true)
		{
			bIsSaved = false;
			retval = true;
			AddAllProjectNodes();
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
	config->Read(_T("DebugMode"), &l);
	m_debugOptions.debugMode = atoi(UTILS::stringify(l).c_str());
	if (m_debugOptions.debugMode == m_debugOptions.SELECTED_TABLES)
	{
		config->Read(_T("HighlightDebug"), &l);
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
	wxString str;
	if (m_pm.GetProjectFilePath().length() > 0)
	{
		//renently opened projects
		config->Read(_T("RecentProjectList"), &str);
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
		config->Write(_T("RecentProjectList"), fileList);

		//remember debugging options
		config->Write(_T("DebugMode"), UTILS::stringify((long)m_debugOptions.debugMode).c_str());
		config->Write(_T("HighlightDebug"), UTILS::stringify((long)m_debugOptions.bOpenTable).c_str());
		vector<wstring> tables = m_debugOptions.selectedTables;
	}
}

bool WorkerClass::Save()
{
	try
	{
		bool bTablesSaved = true;

		for (vector<OpenLogicTable>::iterator it = m_opened_windows.begin(); it != m_opened_windows.end(); it++)
		{
			OpenLogicTable current_table = *it;
			MDIChild *childForm = (MDIChild*)current_table.child_window_ptr;
			current_table = childForm->Save();
			if (current_table.logic_table.HasData())
			{
				if (current_table.logic_table.Path.length() == 0)
				{
					current_table.logic_table.Path = m_pm.GetProjectWorkingPath();
					if (current_table.logic_table.Name != GLOBALORS_TABLE_NAME &&
						current_table.logic_table.Name != TRANSLATIONS_TABLE_NAME)
					{
						//set path according to its place in the tree, relative to working directory
						current_table.logic_table.Path = current_table.logic_table.Path + PATHSEP +
							GetTreeNodePath(m_tree, (wstring)m_tree->GetItemText(wxtid_active_group)) +
							current_table.logic_table.Name + L".xml";
					}
					else
					{
						current_table.logic_table.Path = current_table.logic_table.Path + PATHSEP +
							current_table.logic_table.Name + L".xml";
					}
				}

				if (!m_pm.SaveDataSet(current_table.logic_table.Name, current_table.logic_table.Path, current_table.logic_table.GetDataSet()))
					bTablesSaved = false;
			}
		}
		bool bProjectSaved = m_pm.SaveProjectFile();

		if (bProjectSaved == true && bTablesSaved == true)
			bIsSaved = true;
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
		//see if the window is already open, if so activate it
		for (vector<OpenLogicTable>::iterator it = m_opened_windows.begin(); it != m_opened_windows.end(); it++)
		{
			OpenLogicTable opened = *it;
			if (opened.logic_table.Name == name)
			{
				MDIChild *childForm = (MDIChild*)opened.child_window_ptr;
				childForm->Activate();
				retval = true;
				return retval;
			}
		}
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
				path += PATHSEP + GetTreeNodePath(m_tree, (wstring)m_tree->GetItemText(wxtid_active_group)) + name + L".xml";

				LogicTable table;
				table.LoadDataSet(ds, name, path);
				MDIChild *childForm = new MDIChild(m_parentFrame, m_orientation, table_type, &m_opened_windows, table, &m_pm, name);
				childForm->Show();
				retval = true;
			}
			else
			{
				wxtid_active_group = m_tree->GetSelection();
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
				MDIChild *childForm = new MDIChild(m_parentFrame, m_orientation, table_type, &m_opened_windows, table, &m_pm, name);
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

void WorkerClass::NewTable(wstring name)
{
	try
	{
		if (!bIsSaved)
		{
			CheckSave();
		}

		bool bUserEnteredName = name.length() == 0;
		bool bSystemTable = false;
		if (bUserEnteredName)
			name = wxGetTextFromUser(_T("Enter new table name:"), _T("Table Name"));
		if (name == GLOBALORS_TABLE_NAME || name == TRANSLATIONS_TABLE_NAME)
		{
			if (bUserEnteredName)
			{
				wxMessageBox(L"This name is a reserved system table, please try again");
				return;
			}
			bSystemTable = true;
		}

		if (name.length() > 0)
		{
			LogicTable table;
			table.CreateLogicTable(name);
			if (!bSystemTable)
			{
				wxTreeItemId newItem = AddTreeNode(wxtid_active_group, name);
				m_tree->SelectItem(newItem);
			}

			int table_type = RULES_TABLE;
			if (name == GLOBALORS_TABLE_NAME)
				table_type = GLOBAL_ORS_TABLE;
			else if (name == TRANSLATIONS_TABLE_NAME)
				table_type = TRANSLATIONS_TABLE;

			MDIChild *childForm = new MDIChild(m_parentFrame, m_orientation, table_type, &m_opened_windows, table, &m_pm, name);
			childForm->Show();
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
		MDIChild *childForm = (MDIChild*)m_parentFrame->GetActiveChild();
		childForm->RepopulateTranslationsTable(strings);
	}
	catch(...)
	{
		ReportError("WorkerClass::GenerateTranslations");
	}
}

void WorkerClass::DeleteTable()
{
	try
	{
		//get the active child
		wxMDIChildFrame *active = m_parentFrame->GetActiveChild();

		for (vector<OpenLogicTable>::iterator it = m_opened_windows.begin(); it != m_opened_windows.end(); it++)
		{
			OpenLogicTable opened = *it;
			if (opened.child_window_ptr == (void**)active)
			{
				wstring name = opened.logic_table.Name;
				wstring path = opened.logic_table.Path;
				if (m_pm.DeleteDataSet(name))
				{
					DeleteTreeNode(name);
					active->Close();
					int ans = wxMessageBox(_T("Table removed from project.\nDo you wnat to delete the file as well?"), _T("Remove File?"), wxYES_NO );
					if (ans == wxYES)
					{
					    #ifdef WIN32
						_wremove(path.c_str());
						#else
						remove(UTILS::WStrToMBCStr(path).c_str());
						#endif
					}
				}
				break;
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

		wstring name = wxGetTextFromUser(_T("Enter new group name:"), _T("Group Name")).wc_str();
		if (name.length() > 0)
		{
			AddTreeNode(wxtid_active_group, name);
			wxTreeItemId *id = FindItemNamed(m_tree, name);
			m_tree->SelectItem(id, true);
			wxtid_active_group = m_tree->GetSelection();
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::NewGroup");
	}
}

void WorkerClass::DeleteGroup()
{
	try
	{
		if (m_tree->GetSelection().IsOk())
		{
			wxTreeItemId selection = m_tree->GetSelection();
			wxTreeItemIdValue cookie;
			wxTreeItemId nextChild = m_tree->GetFirstChild(selection, cookie);
			vector<wxTreeItemId> list;
			while (nextChild.IsOk())
			{
				list.push_back(nextChild);
				nextChild = m_tree->GetNextSibling(nextChild);
			}
			wstring groupName = m_tree->GetItemText(selection).wc_str();
			wstring dirPath = GetTreeNodePath(m_tree, groupName);
			vector<wstring> paths;
			for (vector<wxTreeItemId>::iterator itChild = list.begin(); itChild != list.end(); itChild++)
			{
				wxTreeItemId id = *itChild;
				wstring name = m_tree->GetItemText(id).wc_str();
				if (name.length() > 0)
				{
					wstring path = m_pm.GetProjectWorkingPath();
					path += PATHSEP + dirPath + name + L".xml";
					paths.push_back(path);
				}
			}
			m_tree->Delete(m_tree->GetSelection());

			int ans = wxMessageBox(_T("Group removed from project.\nDo you wnat to delete the associated files as well?"), _T("Remove Files?"), wxYES_NO );
			if (ans == wxYES)
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
	for (vector<OpenLogicTable>::iterator it = m_opened_windows.begin(); it != m_opened_windows.end(); it++)
	{
		OpenLogicTable opened = *it;
		MDIChild *childForm = (MDIChild*)opened.child_window_ptr;
		childForm->RotateOrientation();
	}
}

void WorkerClass::GenerateRecentFileList(wxMenu *listMenu, int RecentFile_ID_BEGIN, int RecentFile_ID_END)
{
	wxString str;
	config->Read(_T("RecentProjectList"), &str);
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

bool WorkerClass::FindTextInAnyTable(wstring strToFind, wxPoint *startPos, wstring *last_found_name, bool bMatchCase, bool bMatchWholeWord, bool bDoReplace, wstring strReplace)
{
	bool bFoundAnything = false;
	vector<wstring> allTables = m_pm.GetProjectTableNames();
	if (allTables.size() == 0)
		return false;

	bool bNextTable = true;
	if (last_found_name->length() == 0)
	{
		*last_found_name = allTables[0].c_str();
		bNextTable = false;
	}

	bool bFoundInTable = true;
	if (startPos->x == 0 && startPos->y == 0)
	{
		vector<wstring>::iterator itFind = find(allTables.begin(), allTables.end(), last_found_name->c_str());
		for (vector<wstring>::iterator itTable = itFind; itTable != allTables.end(); itTable++)
		{
			if (bNextTable)
				itTable++;
			if (itTable == allTables.end())
			{
				last_found_name->empty();
				startPos->x = -1;
				startPos->y = -1;
				break;
			}
			//open each table, find the strings
			bFoundInTable = false;
			xmlInitParser();
			xmlDocPtr doc = m_pm.LoadTableRawXML(*itTable);
			xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
			xmlXPathObjectPtr xpathObjAttrs = xmlXPathEvalExpression((xmlChar*)"//Attr", xpathCtx);
			xmlXPathObjectPtr xpathObjValues = xmlXPathEvalExpression((xmlChar*)"//Value", xpathCtx);
			xmlNodeSetPtr attrNodes = xpathObjAttrs->nodesetval;
			xmlNodeSetPtr valueNodes = xpathObjValues->nodesetval;
			if (attrNodes != NULL)
			{
				for (int i = 0; i < attrNodes->nodeNr; i++)
				{
					wstring innerText = UTILS::MBCStrToWStr(xmlNodeGetContent(attrNodes->nodeTab[i]));
					if (TestStringTextMatch(innerText, strToFind, bMatchCase, bMatchWholeWord))
					{
						bFoundInTable = true;
						break;
					}
				}
			}

			if (bFoundInTable == false && valueNodes != NULL)
			{
				for (int i = 0; i < valueNodes->nodeNr; i++)
				{
					wstring innerText = UTILS::MBCStrToWStr(xmlNodeGetContent(valueNodes->nodeTab[i]));
					if (TestStringTextMatch(innerText, strToFind, bMatchCase, bMatchWholeWord))
					{
						bFoundInTable = true;
						break;
					}
				}
			}

			xmlXPathFreeObject(xpathObjAttrs);
			xmlXPathFreeObject(xpathObjValues);
			xmlXPathFreeContext(xpathCtx);
			xmlCleanupParser();

			if (bFoundInTable)
			{
				*last_found_name = itTable->c_str();
				if (LoadTable(*itTable))
				{
					bFoundAnything = true;
					FindTextInActiveTable(strToFind, startPos, bMatchCase, bMatchWholeWord);
					break;
				}
			}

		}
	}
	else
	{
		FindTextInActiveTable(strToFind, startPos, bMatchCase, bMatchWholeWord);
		if (startPos->x == -1 && startPos->y == -1)
		{
			startPos->x = 0;
			startPos->y = 0;
			bFoundAnything = FindTextInAnyTable(strToFind, startPos, last_found_name, bMatchCase, bMatchWholeWord, bDoReplace, strReplace);
		}
	}
	return bFoundAnything;
}

bool WorkerClass::TestStringTextMatch(wxString test, wxString find, bool bMatchCase, bool bMatchWholeWord)
{
	bool bTest = false;

	if (bMatchCase == true && bMatchWholeWord == true)
	{
		bTest = test == find;
	}
	else if (bMatchCase == true && bMatchWholeWord == false)
	{
		bTest = test.Contains(find);
	}
	else if (bMatchCase == false && bMatchWholeWord == true)
	{
		bTest = test.Lower() == find.Lower();
	}
	else if (bMatchCase == false && bMatchWholeWord == false)
	{
		bTest = test.Lower().Contains(find.Lower());
	}

	return bTest;
}

void WorkerClass::FindTextInActiveTable(wstring strToFind, wxPoint *startPos, bool bMatchCase, bool bMatchWholeWord, bool bDoReplace, wstring strReplace)
{
	wxPoint retval(-1, -1);

	MDIChild *childForm = (MDIChild*)m_parentFrame->GetActiveChild();
	if (childForm)
	{
		if (!bDoReplace)
			retval = childForm->FindText(strToFind, *startPos, bMatchCase, bMatchWholeWord);
		else
			retval = childForm->FindAndReplaceText(strToFind, *startPos, bMatchCase, bMatchWholeWord, strReplace);

		if (retval.x >= 0 && retval.y >= 0)
			retval = childForm->GetNextCellPosition(retval);
	}

	startPos->x = retval.x;
	startPos->y = retval.y;
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
	DebugOptionsDialog debugDialog(m_parentFrame, wxID_ANY, &m_debugOptions);
	debugDialog.ShowModal();
	m_pm.SetDebugStatus(m_debug_status, m_debugOptions.connection, m_debugOptions.selectedTables);
}

void WorkerClass::ShowCompileOptions()
{
	CompileOptionsDialog compileDialog(m_parentFrame, wxID_ANY, &m_compileOptions);
	compileDialog.ShowModal();
	m_pm.SetCompilerOptions(m_compileOptions.JavascriptCode, m_compileOptions.PythonCode);
}

wstring WorkerClass::CompileXML(wstring tempFilePath)
{
	wstring retval;
	try
	{
		if (!bIsSaved)
		{
			if (!CheckSave()) //have to rebuild all token ids
				return retval;
		}

		if (tempFilePath.length() == 0)
		{
			wstring savePath;
			wxFileDialog saveFileDialog (
							m_parentFrame,
							_T("Save new compiled XML ruleset"),
							wxEmptyString,
							wxEmptyString,
							_T("xml file (*.xml)|*.xml"),
							wxSAVE
						 );

			saveFileDialog.CentreOnParent();

			if (saveFileDialog.ShowModal() == wxID_OK)
			{
				savePath = saveFileDialog.GetPath();
				if (savePath.length() > 0)
				{
					m_pm.WriteAllDataSetsToXMLFile(savePath);
					retval = savePath;
				}
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
		wstring savePath;
		wxFileDialog saveFileDialog (
						m_parentFrame,
						_T("Save new compiled & compressed XML ruleset"),
						wxEmptyString,
						wxEmptyString,
						_T("gz file (*.gz)|*.gz"),
						wxSAVE
					 );

		saveFileDialog.CentreOnParent();

		if (saveFileDialog.ShowModal() == wxID_OK)
		{
			savePath = saveFileDialog.GetPath();
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
	}
	catch(...)
	{
		ReportError("WorkerClass::CompileZip");
	}
}

void WorkerClass::FillDataTable(StringTable<wstring> *table, wxGrid *grid)
{
	try
	{
		//set up table width
		table->Clear();
		for (size_t i = 0; i < table->Columns(); i++)
		{
			wstring colName = VALUE_NAME;
			if (i == 0)
				colName = ATTR_NAME;

			table->AddColumn(colName);
		}

		for (size_t j = 0; j < table->Rows(); j++)
		{
			table->AddRow();
			for (size_t i = 0; i < table->Columns(); i++)
			{
				wstring colName = table->GetColumns().at(j);
				table->SetItem(j, colName, (wstring)grid->GetCellValue(j, i));
			}
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::FillDataTable");
	}
}

wxTreeItemId WorkerClass::AddTreeNode(wxTreeItemId parent, wstring name)
{
	wxTreeItemId retval;
	try
	{
		retval = m_tree->AppendItem(parent, name, NULL, NULL, NULL);
	}
	catch(...)
	{
		ReportError("WorkerClass::AddTreeNode");
	}
	return retval;
}

void WorkerClass::DeleteTreeNode(wstring name)
{
	try
	{
		wxTreeItemId item = FindItemNamed(m_tree, name);
		m_tree->Delete(item);
	}
	catch(...)
	{
		ReportError("WorkerClass::DeleteTreeNode");
	}
}

wxTreeItemId* WorkerClass::FindItemNamed(wxTreeCtrl *tree, const wstring &name)
{
	try
	{
		stack<wxTreeItemId> items;
		if (tree->GetRootItem().IsOk())
			items.push(tree->GetRootItem());

		while (!items.empty())
		{
			wxTreeItemId next = items.top();
			items.pop();

			if (next != tree->GetRootItem() && tree->GetItemText(next) == name)
				return &next;

			wxTreeItemIdValue cookie;
			wxTreeItemId nextChild = tree->GetFirstChild(next, cookie);
			while (nextChild.IsOk())
			{
				items.push(nextChild);
				nextChild = tree->GetNextSibling(nextChild);
			}
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::FindItemNamed");
	}

	return NULL;
}

void WorkerClass::AddAllProjectNodes()
{
	try
	{
		m_tree->DeleteAllItems();
		m_tree->AddRoot(wxT("Root"), NULL, NULL, NULL);

		StringTable<wstring> *project = m_pm.GetProjectTable();
		if (project != NULL)
		{
			for (size_t j = 0; j < project->Rows(); j++)
			{
				wstring name = UTILS::FindAndReplace(project->GetItem(j, L"DataSetName"), L".xml", L"");
				wstring rel_path = project->GetItem(j,L"RelativePath");

				if (name == GLOBALORS_TABLE_NAME ||
					name == TRANSLATIONS_TABLE_NAME) continue;

				wstring parent_group, sep;
				sep += PATHSEP;
				vector<wstring> parts = UTILS::Split(rel_path, sep);
				parent_group = parts[parts.size() - 1];

				//root nodes
				if (parent_group.length() == 0)
				{
					AddTreeNode(m_tree->GetRootItem(), name);
				}
				else //recursive children
				{
					wxTreeItemId *parent_id_ptr = FindItemNamed(m_tree, parent_group);
					wxTreeItemId parentNode;
					if (parent_id_ptr == NULL) //need to create a folder node
					{
						for (size_t i = 0; i < parts.size(); i++)
						{
							if (i > 0)
								parent_id_ptr = FindItemNamed(m_tree, parts[i - 1]);
							else
								parent_id_ptr = &m_tree->GetRootItem();

							parentNode = parent_id_ptr;
							AddTreeNode(parentNode, parts[i]);
						}
						AddTreeNode(parentNode, name);
					}
					else
					{
						parentNode = parent_id_ptr;
						AddTreeNode(parentNode, name);
					}
				}
			}
		}

		m_tree->Expand(m_tree->GetRootItem());
	}
	catch(...)
	{
		ReportError("WorkerClass::AddAllProjectNodes");
	}
}

wstring WorkerClass::GetTreeNodePath(wxTreeCtrl *tree, wstring name)
{
	wstring retval;
	stack<wstring> path;

	try
	{
		wxTreeItemId next = FindItemNamed(tree, name);
		if (next.IsOk())
		{
			while (next != tree->GetRootItem())
			{
				wxTreeItemId next = tree->GetItemParent(next);

				if (!next.IsOk())
					break;

				if (next != tree->GetRootItem())
				{
					path.push((wstring)tree->GetItemText(next));
					if (tree->GetItemText(next) == name)
						break;
				}
			}
		}

		while (!path.empty())
		{
			if (retval.length() > 0)
				retval += PATHSEP;

			retval += path.top();
			path.pop();
		}
	}
	catch(...)
	{
		ReportError("WorkerClass::GetTreeNodePath");
	}

	return retval;
}

void WorkerClass::InsertCol(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->InsertCol(event);
	}
}

void WorkerClass::InsertRow(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->InsertRow(event);
	}
}

void WorkerClass::DeleteCol(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->DeleteCol(event);
	}
}

void WorkerClass::DeleteRow(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->DeleteRow(event);
	}
}

void WorkerClass::AppendRow(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->AppendRow();
	}
}

void WorkerClass::AppendColumn(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->AppendColumn();
	}
}

void WorkerClass::ClearCells(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->ClearCells(event);
	}
}

void WorkerClass::Copy(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->Copy(event);
	}
}

void WorkerClass::Cut(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->Cut(event);
	}
}

void WorkerClass::Paste(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
	if (active != NULL)
	{
		active->Paste(event);
	}
}

void WorkerClass::EditCode(wxCommandEvent& event)
{
	MDIChild *active = (MDIChild*)m_parentFrame->GetActiveChild();
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

		wxIPV4address addr;
		vector<wstring> con = UTILS::Split(m_debugOptions.connection, L":");
		unsigned short port = (unsigned short)atoi(UTILS::WStrToMBCStr(con[1]).c_str());
		addr.Service(port);

		// Create the socket
		if (m_server)
		{
			m_server->Close();
			m_server->Destroy();
		}
		m_server = new wxSocketServer(addr);

		// We use Ok() here to see if the server is really listening
		if (! m_server->Ok())
		{
			LogTextLine(_("Could not listen at the specified port !n"));
			return;
		}
		else
		{
			// Setup the event handler and subscribe to connection events
			  m_server->SetEventHandler(*m_parentFrame, server_id);
			  m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
			  m_server->Notify(true);

			LogTextLine(_("Debugger server listening.\n"));
		}
	}
	else
	{
		m_server->Close();
		m_server->Destroy();
		m_server = NULL;
		LogTextLine(_("Debugger server stopped.\n"));
	}
}

void WorkerClass::ParseSocket(wxSocketEvent& event)
{
	wxString s = _("OnServerEvent: ");
	wxSocketBase *sock = event.GetSocket();

	// First, print a message
	switch(event.GetSocketEvent())
	{
	case wxSOCKET_INPUT : s.Append(_("wxSOCKET_INPUT\n")); break;
	case wxSOCKET_LOST  : s.Append(_("wxSOCKET_LOST\n")); break;
	default             : s.Append(_("Unexpected event !\n")); break;
	}

	// Now we process the event
	switch(event.GetSocketEvent())
	{
	case wxSOCKET_INPUT:
	{
		// We disable input events, so that the test doesn't trigger
		// wxSocketEvent again.
		sock->SetNotify(wxSOCKET_LOST_FLAG);

		unsigned int len = 256;
		wchar_t *buf;
		wstring strBuff;

		sock->SetFlags(wxSOCKET_WAITALL);
		buf = new wchar_t[len];

		// Read the data in 256 byte chunks
		int bytecnt = 0;
		do
		{
			sock->Read(buf, len);
			bytecnt = sock->LastCount();
			if (bytecnt) strBuff.insert(strBuff.length(), buf, bytecnt/sizeof(wchar_t));
		} while(bytecnt > 0);

		DebugInfoReceived(strBuff);

		// Enable input events again.
		sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
		break;
	}
	case wxSOCKET_LOST:
	{
		//m_numClients--;

		// Destroy() should be used instead of delete wherever possible,
		// due to the fact that wxSocket uses 'delayed events' (see the
		// documentation for wxPostEvent) and we don't want an event to
		// arrive to the event handler (the frame, here) after the socket
		// has been deleted. Also, we might be doing some other thing with
		// the socket at the same time; for example, we might be in the
		// middle of a test or something. Destroy() takes care of all
		// this for us.
		sock->Destroy();
		break;
	}
	default: ;
	}
}

void WorkerClass::ServerEvent(wxSocketEvent& event, int socket_id)
{
  wxString s = _("OnServerEvent: ");
  wxSocketBase *sock;

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
  }

  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(false) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).

  sock = m_server->Accept(false);

  if (sock)
  {
    LogTextLine(_("Received debug info"));
  }
  else
  {
    LogTextLine(_("Error: couldn't accept a new connection\n"));
    return;
  }

  sock->SetEventHandler(*m_parentFrame, socket_id);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(true);
}

void WorkerClass::DebugInfoReceived(wstring buff)
{
	wstring logMessage;

	xmlInitParser();
	string sBuff = UTILS::WStrToMBCStr(buff);
	xmlDocPtr xmlDoc = xmlParseMemory(sBuff.c_str(), (int)sBuff.size());
	if (xmlDoc)
	{
		xmlXPathContextPtr xpathCtx = xmlXPathNewContext(xmlDoc);
		xmlXPathObjectPtr xpathInputs = xmlXPathEvalExpression((xmlChar*)"//Input", xpathCtx);
		xmlXPathObjectPtr xpathOutputs = xmlXPathEvalExpression((xmlChar*)"//Output", xpathCtx);
		wstring tableName = UTILS::MBCStrToWStr(xmlGetProp(xmlDocGetRootElement(xmlDoc), (xmlChar*)"name"));
		wstring outputName = UTILS::MBCStrToWStr(xmlGetProp(xmlDocGetRootElement(xmlDoc), (xmlChar*)"output"));
		logMessage += L"Table: ";
		logMessage += tableName;

		if (m_debugOptions.debugMode == m_debugOptions.ALL_TABLES ||
			find(m_debugOptions.selectedTables.begin(), m_debugOptions.selectedTables.end(), tableName) != m_debugOptions.selectedTables.end())
		{
			logMessage += L" Output Name: ";
			logMessage += outputName;
			logMessage += L"\n";

			xmlNodeSetPtr allInputs = xpathInputs->nodesetval;
			xmlNodeSetPtr allOutputs = xpathOutputs->nodesetval;
			if (allInputs != NULL && allOutputs != NULL)
			{
				logMessage += L"Inputs:\n";
				for (int i = 0; i < allInputs->nodeNr; i++)
				{
					xmlNodePtr input = allInputs->nodeTab[i];
					wstring inputName = UTILS::MBCStrToWStr(xmlGetProp(input, (xmlChar*)"name"));
					wstring inputValue = UTILS::MBCStrToWStr(xmlGetProp(input, (xmlChar*)"value"));
					logMessage += inputName;
					logMessage += L": ";
					logMessage += inputValue;
					logMessage += L"\n";
				}

				logMessage += L"Results:\n";
				for (int i = 0; i < allOutputs->nodeNr; i++)
				{
					xmlNodePtr output = allOutputs->nodeTab[i];
					wstring outputValue = UTILS::MBCStrToWStr(xmlGetProp(output, (xmlChar*)"value"));
					string strSolutionIdx = UTILS::ToASCIIString(UTILS::MBCStrToWStr(xmlGetProp(output, (xmlChar*)"index")));
					int iSolnIdx = atoi(strSolutionIdx.c_str());
					logMessage += outputValue;
					logMessage += L"\n";

					if (m_debugOptions.debugMode == m_debugOptions.SELECTED_TABLES && m_debugOptions.bOpenTable == true)
						HighlightTableAndRule(tableName, iSolnIdx);
				}
			}

			LogTextLine(logMessage);
		}

		xmlXPathFreeObject(xpathInputs);
		xmlXPathFreeObject(xpathOutputs);
		xmlXPathFreeContext(xpathCtx);
	}

	xmlCleanupParser();
}

void WorkerClass::HighlightTableAndRule(wstring tableName, size_t iSolnIdx)
{
	if (LoadTable(tableName))
	{
		MDIChild *childForm = (MDIChild*)m_parentFrame->GetActiveChild();
		if (childForm)
		{
			size_t actualRowIndex = iSolnIdx; //the index should account for disabled rows (skip them)
			vector<size_t> disabledIndexes = childForm->GetDisabledRules();
			//how many disabled rules between us and where we want to highlight?
			for (size_t i = 0; i < disabledIndexes.size(); i++)
			{
				if (disabledIndexes[i] <= iSolnIdx)
					actualRowIndex++;
				else
					break;
			}

			childForm->HighlightRule(actualRowIndex);
		}
	}
}
