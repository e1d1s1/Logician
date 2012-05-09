/////////////////////////////////////////////////////////////////////////////
// Name:        DecisionLogic.h
// Purpose:     EDSEngine rules generator
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

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)

#include <wx/treectrl.h>
#include <wx/splitter.h>
#include <wx/process.h>
#include "WorkerClass.h"
#include "FindReplaceDialog.h"
// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------
// Define a new application type, each program should derive a class from wxApp
class DecisionLogicApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit(void);
};

// Define a new frame type: this is going to be our main frame
class DecisionLogicFrame : public wxMDIParentFrame
{
public:
    // ctor(s)
    DecisionLogicFrame(const wxString& title);

    // event handlers
	//file menu
    void OnQuit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnNewProject(wxCommandEvent& event);
	void OnOpenProject(wxCommandEvent& event);
	void OnSaveProject(wxCommandEvent& event);
	void OnOpenRecentFile (wxCommandEvent& event);

	//edit menu
	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnCut(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	void ShowFindReplaceDialog(wxCommandEvent& event);

	//table menu
	void OnNewTable(wxCommandEvent& event);
	void OnCloseTable(wxCommandEvent& event);
	void OnInsertTable(wxCommandEvent& event);
	void OnDeleteTable(wxCommandEvent& event);
	void OnRenameTable(wxCommandEvent& event);
	void OnNewGroup(wxCommandEvent& event);
	void OnRenameGroup(wxCommandEvent& event);
	void OnDeleteGroup(wxCommandEvent& event);
	void OnOrientationChange(wxCommandEvent& event);
	void OnInsertCol(wxCommandEvent& event);
	void OnInsertRow(wxCommandEvent& event);
	void OnDeleteCol(wxCommandEvent& event);
	void OnDeleteRow(wxCommandEvent& event);
	void OnAppendColumn(wxCommandEvent& event);
	void OnAppendColumns(wxCommandEvent& event);
	void OnAppendRow(wxCommandEvent& event);
	void OnAppendRows(wxCommandEvent& event);
	void OnClearCells(wxCommandEvent& event);
	void OnEditCode(wxCommandEvent& event);
	void OnGlobalORs(wxCommandEvent& event);
	void OnTranslations(wxCommandEvent& event);
	void OnGenerateTranslations(wxCommandEvent& event);

	//compiler menu
	void OnDebugChange(wxCommandEvent& event);
	void OnDebugOptions(wxCommandEvent& event);
	void OnClearLog(wxCommandEvent& event) {txtLog->Clear();}
	void OnCompileCompressed(wxCommandEvent& event);
	void OnCompileXML(wxCommandEvent& event);
	void OnCompileOptions(wxCommandEvent& event);

	//help menu
	void OnHelp(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

	//tree events
	void TreeItemSelected(wxTreeEvent& event);
	void TreeOnBeginDrag(wxTreeEvent& event);
	void TreeOnEndDrag(wxTreeEvent& event);
	void TreeOnBeginLabelEdit(wxTreeEvent& event);
	void TreeOnEndLabelEdit(wxTreeEvent& event);
	void TreeOnKeyDownItem(wxTreeEvent& event);

	//debugger
	void OnSocketEvent(wxSocketEvent& event);
	void OnServerEvent(wxSocketEvent& event);

	WorkerClass* GetWorker(){return m_worker;}

private:
	void OnSize(wxSizeEvent& event);
	void OnSashDrag(wxSashEvent& event);
	void EnableAllMenus();
	void ShowReplaceDialog(wxCommandEvent& WXUNUSED(event));
	void OnFindDialog(wxFindDialogEvent& event);
	void SaveAndQuit();
	bool EditNotAllowed(const wxTreeItemId& item);
	bool ItemIsDraggable(const wxTreeItemId& item);
	wxImageList* CreateImageList(int size = 16);
	void OnTreeContextMenu(wxTreeEvent& event);


	WorkerClass *m_worker;
	GUIClass *m_gui;
	wxTreeCtrl *m_tree;
	wxSashLayoutWindow *winTree, *winLog;
	wxTextCtrl *txtLog;
	wxTreeItemId m_draggedItem;
	wxMenu *compilerMenu, *fileMenu, *tableMenu, *recentFileMenu;
	wstring m_lastName;
	wxPoint *m_last_find_pos;
	wstring m_found_name;
	wstring m_draiggingName;
	wxFindReplaceData m_findData;
    MyFindReplaceDialog *m_dlgReplace;
	bool m_bBypassLoad;
	wxString m_working_directory;
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE();

};
// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------
// IDs for the controls and the menu commands
enum
{
    // menu items
    DecisionLogic_Quit = wxID_EXIT,
	DecisionLogic_NewProject = wxID_NEW,
	DecisionLogic_OpenProject = wxID_OPEN,
	DecisionLogic_SaveProject = wxID_SAVE,
	DecisionLogic_Cut = wxID_CUT,
	DecisionLogic_Copy = wxID_COPY,
	DecisionLogic_Paste = wxID_PASTE,
	DecisionLogic_Find = wxID_FIND,
	DecisionLogic_Undo = wxID_UNDO,
	DecisionLogic_Redo = wxID_REDO,
	DecisionLogic_NewTable = 0,
	DecisionLogic_CloseTable,
	DecisionLogic_InsertTable,
	DecisionLogic_DeleteTable,
	DecisionLogic_RenameTable,
	DecisionLogic_NewGroup,
	DecisionLogic_RenameGroup,
	DecisionLogic_CompileCompressed,
	DecisionLogic_CompileXML,
	DecisionLogic_CompileOptions,
	DecisionLogic_TreeConrol,
	DecisionLogic_TreeWindow,
	DecisionLogic_LogWindow,
	DecisionLogic_DeleteGroup,
	DecisionLogic_OrientationHorizontal,
	DecisionLogic_OrientationVertical,
	DecisionLogic_Debug,
	DecisionLogic_DebugON,
	DecisionLogic_DebugOFF,
	DecisionLogic_RecentFile1,
	DecisionLogic_RecentFile2,
	DecisionLogic_RecentFile3,
	DecisionLogic_RecentFile4,
	DecisionLogic_RecentFile5,
	DecisionLogic_GlobalORs,
	DecisionLogic_Translations,
	DecisionLogic_GenerateTranslations,
	DecisionLogic_Log,
	DecisionLogic_ClearLog,
	DecisionLogic_Socket,
	DecisionLogic_Server,
	APPEND_COL,
	APPEND_ROW,
	APPEND_COLS,
	APPEND_ROWS,
	// it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    DecisionLogic_About = wxID_ABOUT,
	DecisionLogic_Help = wxID_HELP
};

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. DecisionLogicApp and
// not wxApp)
IMPLEMENT_APP(DecisionLogicApp)


// This is the handler for process termination events
class MyProcess : public wxProcess
{
public:
    MyProcess(DecisionLogicFrame *parent, const wxString& cmd)
        : wxProcess(parent), m_cmd(cmd)
    {
        m_parent = parent;
    }

    // instead of overriding this virtual function we might as well process the
    // event from it in the frame class - this might be more convenient in some
    // cases
    virtual void MyProcess::OnTerminate(int pid, int status)
	{
		// we're not needed any more
		delete this;
	}

protected:
    DecisionLogicFrame *m_parent;
    wxString m_cmd;
};