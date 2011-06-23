#pragma once

#include <string>
#include <vector>
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <string>
#include <wx/grid.h>
#include <wx/treectrl.h>
#include <wx/laywin.h>
#if defined(WIN32)
	#include <winsock.h>
#endif
#include <wx/socket.h>
#include <wx/config.h>

#include "LogicTable.h"
#include "OpenLogicTable.h"
#include "ProjectManager.h"
#include "DebugOptions.h"

using namespace std;

class GUIClass
{
public:
	GUIClass(wxMDIParentFrame* parent, wxTreeCtrl* tree, wxTextCtrl* log, ProjectManager* pm, bool(*ChildOpenTableCallback)(wstring))
	{
		m_parent = parent; 
		m_tree = tree; m_server = NULL; 
		m_pm = pm;
		m_log = log;
		m_config = new wxConfig(_T("DecisionLogic")); 
		wxtid_active_group = m_tree->GetRootItem();
		m_OpenTableCallback = ChildOpenTableCallback;
		m_opened_windows = new vector<OpenLogicTable>();
	}
	~GUIClass() 
	{
		if (m_server) 
			delete m_server; 
		if (m_config) 
			delete m_config;
		if (m_opened_windows)
			delete m_opened_windows;
	}

	//tree
	wstring GetTreeNodePath(wstring nodeName);
	wstring GetTreeNodePath(void* nodePtr);
	wstring GetSelectedNodeName();
	void AddTreeNodeToActiveGroup(wstring preValue, wstring name, string type = "Table", bool bSelect = true);
	void DeleteTreeNode(wstring name);		
	void SelectAnItem(wstring name);
	wstring GetActiveGroupName(){return (wstring)m_tree->GetItemText(wxtid_active_group);}
	unsigned int GetNodeCount() {return m_tree->GetCount();}
	vector<wstring> GetChildrenOfActiveGroup();
	vector<wstring> GetChildTablesOfActiveGroup();
	vector<wstring> GetChildrenOfGroup(wstring groupPath);
	void AddAllProjectNodes(StringTable<wstring> *project);
	void SetActiveGroup(wstring groupPath);

	void LogText(wstring text) {m_log->AppendText(text);}
	int PromptQuestion(wstring question, wstring title)
	{
		wxMessageDialog saveNow(m_parent, question, title, wxYES_NO);
		return saveNow.ShowModal();
	}
	void PromptOK(wstring question, wstring title)
	{
		wxMessageDialog okBox(m_parent, question, title, wxOK);
		okBox.ShowModal();
	}
	void PromptMessage(wstring msg) {wxMessageBox(msg);}
	wstring PromptUserForText(wstring msg, wstring title, wstring defaultValue = L"") {return wxGetTextFromUser(msg, title, defaultValue).wc_str();}
	wstring SaveDialog(string type, wstring defaultName = L"");
	wstring OpenDialog(wstring filterString)
	{
		wstring openPath;
		wxFileDialog openFileDialog (
							m_parent,
							_T("Open an existing file"),
							wxEmptyString,
							wxEmptyString,
							filterString,
							wxOPEN
						 );

		openFileDialog.CentreOnParent();

		if (openFileDialog.ShowModal() == wxID_OK)
		{
			openPath = openFileDialog.GetPath();
		}
		return openPath;
	}
	void GetConfig(wstring configItem, long *l)
	{
		long lValue;
		wxString key = configItem;
		m_config->Read(key, &lValue);
		*l = lValue;
	}
	void GetConfig(wstring configItem, wstring *str)
	{
		wxString strValue, key = configItem;
		m_config->Read(key, &strValue);
		*str = (wstring)strValue;
	}
	void WriteConfig(wstring configItem, wstring value)
	{
		wxString key = configItem;
		m_config->Write(key, value);
	}
	void WriteConfig(wstring configItem, string value)
	{
		wxString key = configItem;
		wstring wval;
		wval.assign(value.begin(), value.end());
		wxString val = wval;
		m_config->Write(key, val);
	}	
	void ReadConfig(wstring configItem, wstring *value)
	{
		wxString str, key = configItem;
		m_config->Read(key, &str);
		*value = (wstring)str;
	}

	//window mgmt
	vector<OpenLogicTable>* GetOpenWindows() {return m_opened_windows;};
	bool BringWindowToFront(wstring name);
	void CloseAllWindows();
	void EnableAllChildWindows(bool enable);
	wxMDIParentFrame* GetParentFrame() {return m_parent;}
	OpenLogicTable GetActiveChild();
	bool CloseWindow(wstring name);
	void ChildWindowsHasClosed(wstring tableName);
	void GenerateRecentFileList(wxMenu *listMenu, int RecentFile_ID_BEGIN, int RecentFile_ID_END);

	//debugging server
	void CreateServer(unsigned short port);
	void ParseSocket(wxSocketEvent& event);
	void ServerEvent(wxSocketEvent& event, int socket_id);
	bool ServerReady();
	void StartServer(int server_id, DebugOptions *debug);
	void ShutdownServer();
	

	//find
	void FindTextInActiveTable(wstring strToFind, wxPoint *startPos,
		bool bMatchCase, bool bMatchWholeWord, bool bDoReplace = false, wstring strReplace = L"");
	bool FindTextInAnyTable(wstring strToFind, wxPoint *startPos, wstring *last_found_name,
		bool bMatchCase, bool bMatchWholeWord, bool bDoReplace = false, wstring strReplace = L"");


	
private:
	wxTreeItemId FindItemNamed(wxTreeItemId root, const wxString& sSearchFor, bool bCaseSensitive = true, bool bExactMatch = true);
	wxTreeItemId AddTreeNode(wxTreeItemId parent, wxTreeItemId previous, wstring name, string type = "Table");	
	vector<OpenLogicTable> *m_opened_windows;
	bool TestStringTextMatch(wxString test, wxString find, bool bMatchCase, bool bMatchWholeWord);
	void DebugInfoReceived(wstring buff);
	void HighlightTableAndRule(wstring tableName, size_t iSolnIdx);

	wxConfig *m_config;
	wxTreeCtrl *m_tree;
	wxTreeItemId *root_node;
	wxTreeItemId wxtid_active_group;
	wxTextCtrl *m_log;
	wxSocketServer *m_server;
	wxMDIParentFrame *m_parent;
	ProjectManager *m_pm;
	DebugOptions *m_debugOptions;
	wstring m_lastDebugLine;

	bool (*m_OpenTableCallback)(wstring tableName);
};

enum
{
	TreeCtrlIcon_File = 0,
	TreeCtrlIcon_Folder,
	TreeCtrlIcon_Folder_Open
};