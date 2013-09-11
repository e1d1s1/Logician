/////////////////////////////////////////////////////////////////////////////
// Name:        GUIClass.cpp
// Purpose:     UI functions
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

#include <list>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "GUIClass.h"
#include "MDIChild.h"
#include "utilities.h"


wstring GUIClass::SaveDialog(string type, wstring defaultName)
{
	wstring savePath, msg, filetype;
	if (type == "dlp")
	{
		msg = L"Save new project file";
		filetype = L"Decision Logic Project files (*.dlp)|*.dlp";
	}
	else if (type == "xml")
	{
		msg = L"Save new compiled XML ruleset";
		filetype = L"xml file (*.xml)|*.xml";
	}
	else
	{
		msg = L"Save new compiled & compressed XML ruleset";
		filetype = L"gz file (*.gz)|*.gz";
	}
	wxFileDialog saveFileDialog (
					m_parent,
					msg,
					wxEmptyString,
					defaultName,
					filetype,
					wxFD_SAVE
				 );

	saveFileDialog.CentreOnParent();

	if (saveFileDialog.ShowModal() == wxID_OK)
	{
		savePath = saveFileDialog.GetPath();
	}
	return savePath;
}

wstring GUIClass::GetTreeNodePath(void* nodePtr)
{
	wstring retval;
	wxTreeItemId next(nodePtr);
	list<wstring> path;
	try
	{
		if (next.IsOk())
		{
			wxTreeItemId targetItem = next;
			wstring name = m_tree->GetItemText(next).wc_str();
			while (next != m_tree->GetRootItem())
			{
				next = m_tree->GetItemParent(next);

				if (!next.IsOk())
					break;

				if (next != m_tree->GetRootItem() && m_tree->GetItemImage(next) != TreeCtrlIcon_File)
				{
					wstring pathPart = m_tree->GetItemText(next).wc_str();
					if (pathPart != L"Root")
						path.push_front(pathPart);
					if (m_tree->GetItemText(next) == name)
						break;
				}
			}
			if (name != L"Root" && m_tree->GetItemImage(targetItem) != TreeCtrlIcon_File)
				path.push_back(name);
		}

		while (!path.empty())
		{
			retval += path.front();
			retval += PATHSEP;
			path.pop_front();
		}

		if (retval.length() == 0)
			retval += PATHSEP;
	}
	catch(...)
	{
		ReportError("GUIClass::GetTreeNodePath");
	}
	return retval;
}

wstring GUIClass::GetTreeNodePath(wstring name)
{
	wstring retval;

	wxTreeItemId next = FindItemNamed(m_tree->GetRootItem(), name), targetItem = next;
	retval = GetTreeNodePath((void*)next.m_pItem);

	return retval;
}

wxTreeItemId GUIClass::AddTreeNode(wxTreeItemId parent, wxTreeItemId previous, wstring name, string type)
{
	wxTreeItemId retval;
	try
	{
		int image = -1;
		if (type == "Table")
		{
			image = TreeCtrlIcon_File;
		}
		else if (type == "Group")
		{
			image = TreeCtrlIcon_Folder;
		}

		if (previous.IsOk() == false)
			retval = m_tree->AppendItem(parent, name, image, image, NULL);
		else
			retval = m_tree->InsertItem(parent, previous, name, image, image, NULL);
	}
	catch(...)
	{
		ReportError("GUIClass::AddTreeNode");
	}
	return retval;
}

void GUIClass::DeleteTreeNode(wstring name)
{
	try
	{
		if (name != L"Root")
		{
			wxTreeItemId item = FindItemNamed(m_tree->GetRootItem(), name);
			wxTreeItemId parent = m_tree->GetItemParent(item);
			m_tree->Delete(item);
			m_tree->Unselect();
			if (parent.IsOk())
				wxtid_active_group = parent;
			else
				wxtid_active_group = m_tree->GetRootItem();
			m_tree->SelectItem(wxtid_active_group);
		}
	}
	catch(...)
	{
		ReportError("GUIClass::DeleteTreeNode");
	}
}

wxTreeItemId GUIClass::FindItemNamed(wxTreeItemId root, const wxString& sSearchFor, bool bCaseSensitive, bool bExactMatch)
{
	try
	{
		wxTreeItemId item=root, child;
		wxTreeItemIdValue cookie;
		wxString findtext(sSearchFor), itemtext;
		bool bFound;
		if(!bCaseSensitive) findtext.MakeLower();

		while(item.IsOk())
		{
			itemtext = m_tree->GetItemText(item);
			if(!bCaseSensitive) itemtext.MakeLower();
			bFound = bExactMatch ? (itemtext == findtext) : itemtext.Contains(findtext);
			if(bFound) return item;
			child = m_tree->GetFirstChild(item, cookie);
			if(child.IsOk()) child = FindItemNamed(child, sSearchFor, bCaseSensitive, bExactMatch);
			if(child.IsOk()) return child;
			item = m_tree->GetNextSibling(item);
		}

		return item;


	}
	catch(...)
	{
		ReportError("GUIClass::FindItemNamed");
	}

	return NULL;
}

void GUIClass::AddTreeNodeToActiveGroup(wstring preValue, wstring name, string type, bool bSelect)
{
	wstring active = m_tree->GetItemText(wxtid_active_group).wc_str();
	wxTreeItemId locItem;
	if (preValue.length() > 0)
		locItem = FindItemNamed(wxtid_active_group, preValue);
	wxTreeItemId newItem = AddTreeNode(wxtid_active_group, locItem, name, type);
	if (bSelect)
		m_tree->SelectItem(newItem);
}

wstring GUIClass::GetSelectedNodeName()
{
	wstring retval;
	wxTreeItemId sel_item = m_tree->GetSelection(), initial_sel = sel_item;
	if (sel_item.IsOk())
	{
		retval = m_tree->GetItemText(sel_item);
	}
	return retval;
}

void GUIClass::SelectAnItem(wstring name)
{
	wxTreeItemId sel_item = m_tree->GetSelection(), initial_sel = sel_item;
	if (!sel_item.IsOk())
	{
		sel_item = FindItemNamed(m_tree->GetRootItem(), name);
	}

	if (sel_item.IsOk())
	{
		if (m_tree->GetItemText(sel_item).c_str() != name)
		{
			m_tree->Unselect();
			wxTreeItemId locItem = FindItemNamed(m_tree->GetRootItem(), name);
			if (locItem.IsOk())
			{
				sel_item = locItem;
			}
		}

		if (m_tree->GetItemImage(sel_item) != TreeCtrlIcon_File)
		{
			wxtid_active_group = sel_item;
		}
		else
		{
			wxTreeItemId parentId = m_tree->GetItemParent(sel_item);
			if (parentId.IsOk())
				wxtid_active_group = parentId;
		}
	}
	if (initial_sel != sel_item)
		m_tree->SelectItem(sel_item);
	wstring active = m_tree->GetItemText(wxtid_active_group).wc_str();
}

vector<wstring> GUIClass::GetChildrenOfGroup(wstring groupPath)
{
	vector<wstring> names;
	wstring strPathSep; strPathSep += PATHSEP;
	vector<wstring> pathParts = UTILS::Split(groupPath, strPathSep);
	wxTreeItemId grpItem;
	if (pathParts.size() > 0)
	{
		grpItem = FindItemNamed(m_tree->GetRootItem(), pathParts[0]);
		for (vector<wstring>::iterator it = pathParts.begin() + 1; it != pathParts.end(); it++)
		{
			grpItem = FindItemNamed(grpItem, *it);
		}
	}

	if (grpItem.IsOk())
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId nextChild = m_tree->GetFirstChild(grpItem, cookie);
		while (nextChild.IsOk())
		{
			wstring name = m_tree->GetItemText(nextChild).wc_str();
			names.push_back(name);
			nextChild = m_tree->GetNextSibling(nextChild);
		}
	}

	return names;
}

wstring GUIClass::GetParentGroupName(wstring tableName)
{
	wstring name = L"";
	wxTreeItemId grpItem = FindItemNamed(m_tree->GetRootItem(), tableName);
	if (grpItem.IsOk())
	{
		 wxTreeItemId parent = m_tree->GetItemParent(grpItem);
		 if (parent.IsOk())
			name = m_tree->GetItemText(parent).wc_str();
	}
	return name;
}

void GUIClass::SetActiveGroup(wstring groupPath)
{
	wstring strPathSep; strPathSep += PATHSEP;
	vector<wstring> pathParts = UTILS::Split(groupPath, strPathSep);
	wxTreeItemId grpItem;
	if (pathParts.size() > 0)
	{
		if (pathParts.size() == 1)
		{
			if (pathParts[0].length() == 1 && pathParts[0][0] == PATHSEP)
				grpItem = m_tree->GetRootItem();
			else
				grpItem = FindItemNamed(m_tree->GetRootItem(), pathParts[0]);
		}
		else if (pathParts.size() > 1)
		{
			grpItem = FindItemNamed(m_tree->GetRootItem(), pathParts[pathParts.size() - 1]);
		}
	}
	else
		grpItem = m_tree->GetRootItem();

	if (grpItem.IsOk())
	{
		wxtid_active_group = grpItem;
	}
}

vector<wstring> GUIClass::GetChildrenOfActiveGroup()
{
	vector<wstring> names;
	if (wxtid_active_group.IsOk())
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId nextChild = m_tree->GetFirstChild(wxtid_active_group, cookie);
		while (nextChild.IsOk())
		{
			wstring name = m_tree->GetItemText(nextChild).wc_str();
			names.push_back(name);
			nextChild = m_tree->GetNextSibling(nextChild);
		}
	}
	return names;
}

vector<wstring> GUIClass::GetChildTablesOfActiveGroup()
{
	vector<wstring> names;
	if (wxtid_active_group.IsOk())
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId nextChild = m_tree->GetFirstChild(wxtid_active_group, cookie);
		while (nextChild.IsOk() && m_tree->GetItemImage(nextChild) == TreeCtrlIcon_File)
		{
			wstring name = m_tree->GetItemText(nextChild).wc_str();
			names.push_back(name);
			nextChild = m_tree->GetNextSibling(nextChild);
		}
	}
	return names;
}

void GUIClass::AddAllProjectNodes(StringTable<wstring> *project)
{
	try
	{
		m_tree->DeleteAllItems();
		m_tree->AddRoot(wxT("Root"), TreeCtrlIcon_Folder, TreeCtrlIcon_Folder);
		wxtid_active_group = m_tree->GetRootItem();

		project->SortByCol(L"DataSetName");
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
					AddTreeNode(m_tree->GetRootItem(), NULL, name);
				}
				else //children
				{
					wxTreeItemId tableParent = m_tree->GetRootItem();
					for (size_t i = 0; i < parts.size(); i++)
					{
						wxTreeItemId newParent = FindItemNamed(tableParent, parts[i]);
						if (!newParent.IsOk())
						{
							newParent = AddTreeNode(tableParent, NULL, parts[i], "Group");
							tableParent = newParent;
						}
						else
							tableParent = newParent;
					}
					if (tableParent.IsOk())
					{
						AddTreeNode(tableParent, NULL, name);
					}
				}
			}
		}

		m_tree->Expand(m_tree->GetRootItem());
	}
	catch(...)
	{
		ReportError("GUIClass::AddAllProjectNodes");
	}
}

bool GUIClass::BringWindowToFront(wstring name)
{
	bool retval = false;
	for (vector<OpenLogicTable>::iterator it = m_opened_windows->begin(); it != m_opened_windows->end(); it++)
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
	return retval;
}

void GUIClass::CloseAllWindows()
{
	while (m_opened_windows->size() > 0)
	{
		OpenLogicTable current_table = m_opened_windows->at(m_opened_windows->size() - 1);
		MDIChild *childForm = (MDIChild*)current_table.child_window_ptr;
		childForm->Close();
	}
	m_opened_windows->clear();
}

bool GUIClass::CloseWindow(wstring name)
{
	bool retval = false;
	for (int i = 0; i < m_opened_windows->size(); i++)
	{
		OpenLogicTable current_table = m_opened_windows->at(i);
		if (current_table.logic_table.Name == name)
		{
			MDIChild *childForm = (MDIChild*)current_table.child_window_ptr;
			childForm->Close();
			retval = true;
			break;
		}
	}
	return retval;
}

void GUIClass::EnableAllChildWindows(bool enable)
{
	for (vector<OpenLogicTable>::iterator it = m_opened_windows->begin(); it != m_opened_windows->end(); it++)
	{
		OpenLogicTable current_table = *it;
		MDIChild *childForm = (MDIChild*)current_table.child_window_ptr;
		childForm->Enable(enable);
	}
}

OpenLogicTable GUIClass::GetActiveChild()
{
	OpenLogicTable retval;
	wxMDIChildFrame *active = NULL;
	active = m_parent->GetActiveChild();
	if (!active)
		return retval;

	for (vector<OpenLogicTable>::iterator it = m_opened_windows->begin(); it != m_opened_windows->end(); it++)
	{
		retval = *it;
		if (retval.child_window_ptr == (void**)active)
		{
			break;
		}
	}

	return retval;
}

void GUIClass::ChildWindowsHasClosed(wstring tableName)
{
	wxTreeItemId parentId = m_tree->GetRootItem();
	wxTreeItemId selId = FindItemNamed(m_tree->GetRootItem(), tableName);
	if (selId.IsOk())
	{
		parentId = m_tree->GetItemParent(selId);
		if (!parentId.IsOk())
			parentId = m_tree->GetRootItem();
	}
	m_tree->Unselect();
	wxtid_active_group = parentId;
	m_tree->SelectItem(parentId);
	wstring active = m_tree->GetItemText(wxtid_active_group).wc_str();
}

void GUIClass::GenerateRecentFileList(wxMenu *listMenu, int RecentFile_ID_BEGIN, int RecentFile_ID_END)
{
	wstring str;
	ReadConfig(L"RecentProjectList", &str);
	vector<wstring> files = UTILS::Split((wstring)str, L";");
	int cnt = RecentFile_ID_END - RecentFile_ID_BEGIN, index = 0;
	for (vector<wstring>::iterator it = files.begin(); it != files.end(); it++)
	{
		if (cnt <= 1 || (*it).length() == 0) break;
		listMenu->Append(RecentFile_ID_BEGIN + index, *it);
		cnt--;
		index++;
	}
}

bool GUIClass::FindTextInAnyTable(wstring strToFind, wxPoint *startPos, wstring *last_found_name, ProjectManager *pm, bool bMatchCase, bool bMatchWholeWord, bool bDoReplace, wstring strReplace)
{
	bool bFoundAnything = false;
	vector<wstring> allTables = pm->GetProjectTableNames();
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
			xmlDocPtr doc = pm->LoadTableRawXML(*itTable);
			xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
			xmlXPathObjectPtr xpathObjAttrs = xmlXPathEvalExpression((xmlChar*)"//Attr", xpathCtx);
			xmlXPathObjectPtr xpathObjValues = xmlXPathEvalExpression((xmlChar*)"//Value", xpathCtx);
			xmlNodeSetPtr attrNodes = xpathObjAttrs->nodesetval;
			xmlNodeSetPtr valueNodes = xpathObjValues->nodesetval;
			if (attrNodes != NULL)
			{
				for (int i = 0; i < attrNodes->nodeNr; i++)
				{
					wstring innerText = UTILS::XMLStrToWStr(xmlNodeGetContent(attrNodes->nodeTab[i]));
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
					wstring innerText = UTILS::XMLStrToWStr(xmlNodeGetContent(valueNodes->nodeTab[i]));
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
				if (m_OpenTableCallback(*itTable))
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
			bFoundAnything = FindTextInAnyTable(strToFind, startPos, last_found_name, pm, bMatchCase, bMatchWholeWord, bDoReplace, strReplace);
		}
	}
	return bFoundAnything;
}

bool GUIClass::TestStringTextMatch(wxString test, wxString find, bool bMatchCase, bool bMatchWholeWord)
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

void GUIClass::FindTextInActiveTable(wstring strToFind, wxPoint *startPos, bool bMatchCase, bool bMatchWholeWord, bool bDoReplace, wstring strReplace)
{
	wxPoint retval(-1, -1);

	MDIChild *childForm = (MDIChild*)m_parent->GetActiveChild();
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

void GUIClass::CreateServer(unsigned short port)
{
	if (m_server)
	{
		m_server->Close();
		m_server->Destroy();
	}
	wxIPV4address addr;
	addr.Service(port);
	m_server = new wxSocketServer(addr);
}

bool GUIClass::ServerReady()
{
	if (m_server)
		return m_server->IsOk();
	else
		return false;
}
void GUIClass::ParseSocket(wxSocketEvent& event)
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

void GUIClass::ServerEvent(wxSocketEvent& event, int socket_id)
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

  wstring msg;
  bool bSkipNotify = false;
  if (sock)
  {
    msg = L"Received debug info\n";
  }
  else
  {
    msg = L"Error: couldn't accept a new connection\n";
	bSkipNotify = true;
  }
  if (m_lastDebugLine != msg)
	  LogText(msg);
  m_lastDebugLine = msg;

  if (bSkipNotify)
	  return;

  sock->SetEventHandler(*m_parent, socket_id);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(true);
}

void GUIClass::StartServer(int server_id, DebugOptions *debug)
{
	if (m_server)
	{
		m_server->SetEventHandler(*m_parent, server_id);
		m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
		m_server->Notify(true);
	}
	m_debugOptions = debug;
}

void GUIClass::ShutdownServer()
{
	if (m_server)
	{
		m_server->Close();
		m_server->Destroy();
	}
	m_server = NULL;
}

void GUIClass::DebugInfoReceived(wstring buff)
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
		wstring tableName = UTILS::XMLStrToWStr(xmlGetProp(xmlDocGetRootElement(xmlDoc), (xmlChar*)"name"));
		wstring outputName = UTILS::XMLStrToWStr(xmlGetProp(xmlDocGetRootElement(xmlDoc), (xmlChar*)"output"));
		logMessage += L"Table: ";
		logMessage += tableName;

		if (m_debugOptions->debugMode == m_debugOptions->ALL_TABLES ||
			find(m_debugOptions->selectedTables.begin(), m_debugOptions->selectedTables.end(), tableName) != m_debugOptions->selectedTables.end())
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
					wstring inputName = UTILS::XMLStrToWStr(xmlGetProp(input, (xmlChar*)"name"));
					wstring inputValue = UTILS::XMLStrToWStr(xmlGetProp(input, (xmlChar*)"value"));
					logMessage += inputName;
					logMessage += L": ";
					logMessage += inputValue;
					logMessage += L"\n";
				}

				logMessage += L"Results:\n";
				for (int i = 0; i < allOutputs->nodeNr; i++)
				{
					xmlNodePtr output = allOutputs->nodeTab[i];
					wstring outputValue = UTILS::XMLStrToWStr(xmlGetProp(output, (xmlChar*)"value"));
					string strSolutionIdx = UTILS::ToASCIIString(UTILS::XMLStrToWStr(xmlGetProp(output, (xmlChar*)"index")));
					int iSolnIdx = atoi(strSolutionIdx.c_str());
					if (outputValue.length() > 0)
					{
						logMessage += outputValue;
						logMessage += L"\n";
					}

					if (m_debugOptions->debugMode == m_debugOptions->SELECTED_TABLES && m_debugOptions->bOpenTable == true)
						HighlightTableAndRule(tableName, iSolnIdx);
				}
			}

			if (logMessage.length() > 0)
				LogText(logMessage + L"\n");
		}

		xmlXPathFreeObject(xpathInputs);
		xmlXPathFreeObject(xpathOutputs);
		xmlXPathFreeContext(xpathCtx);
	}

	xmlCleanupParser();
}

void GUIClass::HighlightTableAndRule(wstring tableName, size_t iSolnIdx)
{
	if (m_OpenTableCallback(tableName))
	{
		MDIChild *childForm = (MDIChild*)m_parent->GetActiveChild();
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
