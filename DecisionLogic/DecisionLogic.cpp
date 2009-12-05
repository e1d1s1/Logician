/////////////////////////////////////////////////////////////////////////////
// Name:        DecisionLogic.cpp
// Purpose:     DecisionLogic GUI implementation
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

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "stdafx.h"
#include "DecisionLogic.h"
#include <wx/fdrepdlg.h>

//Icon
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "DecisionLogicIcon.xpm"
#endif

const int DEFAULT_ORIENTATION = wxVERTICAL;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(DecisionLogicFrame, wxFrame)
	//MENUS/////////////////////////////////////////////////////////////////////////
    EVT_MENU(DecisionLogic_Quit,  DecisionLogicFrame::OnQuit)
	EVT_MENU(DecisionLogic_NewProject,  DecisionLogicFrame::OnNewProject)
	EVT_MENU(DecisionLogic_OpenProject,  DecisionLogicFrame::OnOpenProject)
	EVT_MENU(DecisionLogic_SaveProject,  DecisionLogicFrame::OnSaveProject)
	EVT_MENU(DecisionLogic_Undo, DecisionLogicFrame::OnUndo)
	EVT_MENU(DecisionLogic_Redo, DecisionLogicFrame::OnRedo)
	EVT_MENU(DecisionLogic_Quit,  DecisionLogicFrame::OnQuit)
	EVT_MENU(DecisionLogic_Cut,  DecisionLogicFrame::OnCut)
	EVT_MENU(DecisionLogic_Copy,  DecisionLogicFrame::OnCopy)
	EVT_MENU(DecisionLogic_Paste,  DecisionLogicFrame::OnPaste)
	EVT_MENU(DecisionLogic_Find,  DecisionLogicFrame::ShowFindReplaceDialog)
	EVT_MENU(DecisionLogic_NewTable,  DecisionLogicFrame::OnNewTable)
	EVT_MENU(DecisionLogic_DeleteTable,  DecisionLogicFrame::OnDeleteTable)
	EVT_MENU(DecisionLogic_NewGroup,  DecisionLogicFrame::OnNewGroup)
	EVT_MENU(DecisionLogic_DeleteGroup, DecisionLogicFrame::OnDeleteGroup)
	EVT_MENU(DecisionLogic_OrientationHorizontal, DecisionLogicFrame::OnOrientationChange)
    EVT_MENU(DecisionLogic_OrientationVertical, DecisionLogicFrame::OnOrientationChange)
	EVT_MENU(DecisionLogic_GlobalORs, DecisionLogicFrame::OnGlobalORs)
	EVT_MENU(DecisionLogic_Translations, DecisionLogicFrame::OnTranslations)
	EVT_MENU(DecisionLogic_GenerateTranslations, DecisionLogicFrame::OnGenerateTranslations)
	EVT_MENU(DecisionLogic_Debug, DecisionLogicFrame::OnDebugOptions)
	EVT_MENU(DecisionLogic_DebugON, DecisionLogicFrame::OnDebugChange)
	EVT_MENU(DecisionLogic_DebugOFF, DecisionLogicFrame::OnDebugChange)
	EVT_MENU(DecisionLogic_ClearLog, DecisionLogicFrame::OnClearLog)
	EVT_MENU(DecisionLogic_RecentFile1, DecisionLogicFrame::OnOpenRecentFile)
	EVT_MENU(DecisionLogic_RecentFile2, DecisionLogicFrame::OnOpenRecentFile)
	EVT_MENU(DecisionLogic_RecentFile3, DecisionLogicFrame::OnOpenRecentFile)
	EVT_MENU(DecisionLogic_RecentFile4, DecisionLogicFrame::OnOpenRecentFile)
	EVT_MENU(DecisionLogic_RecentFile5, DecisionLogicFrame::OnOpenRecentFile)
	EVT_MENU(INSERT_COL, DecisionLogicFrame::OnInsertCol)
	EVT_MENU(INSERT_ROW, DecisionLogicFrame::OnInsertRow)
	EVT_MENU(DELETE_COL, DecisionLogicFrame::OnDeleteCol)
	EVT_MENU(DELETE_ROW, DecisionLogicFrame::OnDeleteRow)
	EVT_MENU(APPEND_COL, DecisionLogicFrame::OnAppendColumn)
	EVT_MENU(APPEND_ROW, DecisionLogicFrame::OnAppendRow)
	EVT_MENU(CLEAR_CELLS, DecisionLogicFrame::OnClearCells)
	EVT_MENU(EDIT_CODE, DecisionLogicFrame::OnEditCode)

	EVT_MENU(DecisionLogic_CompileCompressed,  DecisionLogicFrame::OnCompileCompressed)
	EVT_MENU(DecisionLogic_CompileXML,  DecisionLogicFrame::OnCompileXML)
	EVT_MENU(DecisionLogic_CompileOptions,  DecisionLogicFrame::OnCompileOptions)

    EVT_MENU(DecisionLogic_About, DecisionLogicFrame::OnAbout)

	//FRAME EVENTS//////////////////////////////////////////////////////////////////
	EVT_SIZE(DecisionLogicFrame::OnSize)
	EVT_SASH_DRAGGED_RANGE(DecisionLogic_TreeWindow, DecisionLogic_LogWindow, DecisionLogicFrame::OnSashDrag)

	//TREE EVENTS///////////////////////////////////////////////////////////////////
	EVT_TREE_SEL_CHANGED(DecisionLogic_TreeConrol, DecisionLogicFrame::TreeItemSelected)

	//DEBUGGER//////////////////////////////////////////////////////////////////////
	EVT_SOCKET(DecisionLogic_Server,  DecisionLogicFrame::OnServerEvent)
	EVT_SOCKET(DecisionLogic_Socket,  DecisionLogicFrame::OnSocketEvent)

	//FIND-REPLACE//////////////////////////////////////////////////////////////////
    EVT_FIND(wxID_ANY, DecisionLogicFrame::OnFindDialog)
    EVT_FIND_NEXT(wxID_ANY, DecisionLogicFrame::OnFindDialog)
    EVT_FIND_REPLACE(wxID_ANY, DecisionLogicFrame::OnFindDialog)
    EVT_FIND_REPLACE_ALL(wxID_ANY, DecisionLogicFrame::OnFindDialog)
    EVT_FIND_CLOSE(wxID_ANY, DecisionLogicFrame::OnFindDialog)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MDIChild, wxMDIChildFrame)
	EVT_CLOSE(MDIChild::OnChildClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(LogicGrid, wxGrid)
	EVT_GRID_CELL_RIGHT_CLICK(LogicGrid::OnContextMenu)
	EVT_GRID_RANGE_SELECT(LogicGrid::OnRangeSelected)
	EVT_GRID_SELECT_CELL(LogicGrid::OnSelectCell)
	EVT_MENU(INSERT_COL, LogicGrid::OnInsertCol)
	EVT_MENU(INSERT_ROW, LogicGrid::OnInsertRow)
	EVT_MENU(DELETE_COL, LogicGrid::OnDeleteCol)
	EVT_MENU(DELETE_ROW, LogicGrid::OnDeleteRow)
	EVT_MENU(CLEAR_CELLS, LogicGrid::OnClearCells)
	EVT_MENU(CUT, LogicGrid::OnCut)
	EVT_MENU(COPY, LogicGrid::OnCopy)
	EVT_MENU(PASTE, LogicGrid::OnPaste)
	EVT_MENU(EDIT_CODE, LogicGrid::OnEditCode)
	EVT_GRID_CELL_CHANGE(LogicGrid::OnCellChange)

	EVT_GRID_EDITOR_SHOWN(LogicGrid::OnEditorShown)
    EVT_GRID_EDITOR_HIDDEN(LogicGrid::OnEditorHidden)
	EVT_GRID_CELL_LEFT_CLICK(LogicGrid::OnLeftClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(DebugOptionsDialog, wxPropertySheetDialog)
    EVT_BUTTON(DEBUG_ADD_TABLE, DebugOptionsDialog::AddRemoveClick)
	EVT_BUTTON(DEBUG_REMOVE_TABLE, DebugOptionsDialog::AddRemoveClick)
	EVT_BUTTON(wxID_OK, DebugOptionsDialog::OnOK)
	EVT_RADIOBOX(DEBUG_SCOPE, DebugOptionsDialog::OnScopeChanage)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CompileOptionsDialog, wxPropertySheetDialog)
	EVT_BUTTON(wxID_OK, CompileOptionsDialog::OnOK)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CodeEditorDialog, wxPropertySheetDialog)
	EVT_BUTTON(wxID_OK, CodeEditorDialog::OnOK)
END_EVENT_TABLE()
// ============================================================================
// implementation
// ============================================================================
// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool DecisionLogicApp::OnInit(void)
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    DecisionLogicFrame *frame = new DecisionLogicFrame(_T("DecisionLogic"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

	SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
DecisionLogicFrame::DecisionLogicFrame(const wxString& title)
       : wxMDIParentFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxHSCROLL | wxVSCROLL)
{
	SetIcon(wxICON(DecisionLogicIcon));

	SetSize(800, 600);

    // create a menu bar
    fileMenu = new wxMenu();
	fileMenu->Append(DecisionLogic_NewProject, _T("&New Project\tCtrl-N"), _T("Start a new rules project"));
	fileMenu->Append(DecisionLogic_OpenProject, _T("&Open Project\tCtrl-O"), _T("Open an existing rules project"));
	fileMenu->Append(DecisionLogic_SaveProject, _T("&Save Project\tCtrl-S"), _T("Save project"));

	fileMenu->AppendSeparator();
	recentFileMenu = new wxMenu();
	fileMenu->AppendSubMenu(recentFileMenu, _T("&Recent Projects"));
	fileMenu->AppendSeparator();

	fileMenu->Append(DecisionLogic_Quit, _T("E&xit"), _T("Quit this program"));
	fileMenu->Enable(DecisionLogic_SaveProject, false);

	wxMenu *editMenu = new wxMenu();
	editMenu->Append(DecisionLogic_Undo, _T("&Undo\tCtrl-Z"), _T("Undo"));
	editMenu->Append(DecisionLogic_Redo, _T("&Redo\tCtrl-Y"), _T("Redo"));
	editMenu->AppendSeparator();
	editMenu->Append(DecisionLogic_Cut, _T("Cu&t\tCtrl-X"), _T("Cut"));
	editMenu->Append(DecisionLogic_Copy, _T("&Copy\tCtrl-C"), _T("Copy"));
	editMenu->Append(DecisionLogic_Paste, _T("&Paste\tCtrl-V"), _T("Paste"));
	editMenu->Append(DecisionLogic_Find, _T("&Find\tCtrl-F"), _T("Find"));

	tableMenu = new wxMenu();
	tableMenu->Append(DecisionLogic_NewTable, _T("New &Table"), _T("Create a new logic table"));
	tableMenu->Append(DecisionLogic_DeleteTable, _T("&Delete Table"), _T("Delete logic table"));
	tableMenu->Append(DecisionLogic_NewGroup, _T("New &Group"), _T("Create a new table group"));
	tableMenu->Append(DecisionLogic_DeleteGroup, _T("D&elete Group"), _T("Delete a table group"));
	wxMenu *orientationMenu = new wxMenu();
	tableMenu->AppendSubMenu(orientationMenu, _T("&Orientation"));
	orientationMenu->AppendRadioItem(DecisionLogic_OrientationHorizontal, _T("&Horizontal"), _T("Horizontal orientaion of tables"));
	orientationMenu->AppendRadioItem(DecisionLogic_OrientationVertical, _T("&Vertical"), _T("Vertical orientaion of tables"));
	if (DEFAULT_ORIENTATION == wxHORIZONTAL)
		orientationMenu->Check(DecisionLogic_OrientationHorizontal, true);
	else
		orientationMenu->Check(DecisionLogic_OrientationVertical, true);
	tableMenu->AppendSeparator();
	tableMenu->Append(DecisionLogic_GlobalORs, _T("&Global ORs"));
	wxMenu *translationsMenu = new wxMenu();
	translationsMenu->Append(DecisionLogic_Translations, _T("&Show Translations"));
	translationsMenu->Append(DecisionLogic_GenerateTranslations, _T("&Regenerate Translations"));
	tableMenu->AppendSubMenu(translationsMenu, _T("&Translations"));
	tableMenu->AppendSeparator();
	tableMenu->Append(CLEAR_CELLS, _T("C&lear Cell(s)"));
	tableMenu->AppendSeparator();
	tableMenu->Append(INSERT_COL, _T("Insert &Column(s)"));
	tableMenu->Append(INSERT_ROW, _T("Insert &Row(s)"));
	tableMenu->Append(APPEND_COL, _T("Append C&olumn"));
	tableMenu->Append(APPEND_ROW, _T("Append Ro&w"));
	tableMenu->AppendSeparator();
	tableMenu->Append(DELETE_COL, _T("&Delete Column(s)"));
	tableMenu->Append(DELETE_ROW, _T("D&elete Row(s)"));
	tableMenu->AppendSeparator();
	tableMenu->Append(EDIT_CODE, _T("Edit &Code"));
	tableMenu->Enable(DecisionLogic_NewTable, false);
	tableMenu->Enable(DecisionLogic_DeleteTable, false);
	tableMenu->Enable(DecisionLogic_NewGroup, false);
	tableMenu->Enable(DecisionLogic_DeleteGroup, false);

	compilerMenu = new wxMenu();
	wxMenu *debugMenu = new wxMenu();
	debugMenu->AppendRadioItem(DecisionLogic_DebugON, _T("&Enabled"));
	debugMenu->AppendRadioItem(DecisionLogic_DebugOFF, _("D&isabled"));
	debugMenu->Check(DecisionLogic_DebugOFF, true);
	compilerMenu->AppendSubMenu(debugMenu, _T("&Debug"));
	compilerMenu->Append(DecisionLogic_Debug, _T("Debugging &Options"));
	compilerMenu->AppendSeparator();
	compilerMenu->Append(DecisionLogic_ClearLog, _T("&Clear Log Window"));
	compilerMenu->AppendSeparator();
	compilerMenu->Append(DecisionLogic_CompileOptions, _T("Compiler Options"), _T("Python and Javascript compile options"));
	compilerMenu->Append(DecisionLogic_CompileXML, _T("Compile to &XML"), _T("Compile rules as a single xml file"));
	compilerMenu->Append(DecisionLogic_CompileCompressed, _T("Compile to Compressed &Archive"), _T("Compile rules as a gzip file"));
	compilerMenu->Enable(DecisionLogic_Debug, false);
	compilerMenu->Enable(DecisionLogic_CompileOptions, false);
	compilerMenu->Enable(DecisionLogic_CompileXML, false);
	compilerMenu->Enable(DecisionLogic_CompileCompressed, false);

	// the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu();
    helpMenu->Append(DecisionLogic_About, _T("&About...\tF1"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
	menuBar->Append(editMenu, _T("&Edit"));
	menuBar->Append(tableMenu, _T("&Table"));
	menuBar->Append(compilerMenu, _T("&Compile"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    CreateStatusBar(2);
    SetStatusText(_T("Ready"));

	//contents
	winTree = new wxSashLayoutWindow(this, DecisionLogic_TreeWindow,
                             wxDefaultPosition, wxSize(120, 600),
                             wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
	winTree->SetDefaultSize(wxSize(120, 600));
	winTree->SetOrientation(wxLAYOUT_VERTICAL);
	winTree->SetAlignment(wxLAYOUT_LEFT);
	winTree->SetSashVisible(wxSASH_RIGHT, true);

	//output and debug
	winLog = new wxSashLayoutWindow(this, DecisionLogic_LogWindow,
                               wxDefaultPosition, wxSize(800, 100),
                               wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);
	winLog->SetDefaultSize(wxSize(800, 100));
	winLog->SetOrientation(wxLAYOUT_HORIZONTAL);
	winLog->SetAlignment(wxLAYOUT_BOTTOM);
	winLog->SetSashVisible(wxSASH_TOP, true);
	txtLog = new wxTextCtrl(winLog, DecisionLogic_Log, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);

	m_tree = new wxTreeCtrl(winTree, DecisionLogic_TreeConrol, wxDefaultPosition, wxSize(200, 600));
	wxTreeItemId rootId = m_tree->AddRoot(wxT("Root"),NULL, NULL, NULL);

	winTree->Show(true);
	winLog->Show(true);
	m_worker = new WorkerClass(this, m_tree, txtLog, DEFAULT_ORIENTATION);

	m_worker->GenerateRecentFileList(recentFileMenu, DecisionLogic_RecentFile1, DecisionLogic_RecentFile1 + 5);
	m_dlgReplace = NULL;
	m_last_find_pos = NULL;
}


// event handlers
void DecisionLogicFrame::OnNewProject(wxCommandEvent& WXUNUSED(event))
{
	if (m_worker->NewProject())
		EnableAllMenus();
}

void DecisionLogicFrame::OnOpenProject(wxCommandEvent& WXUNUSED(event))
{
	if (m_worker->OpenProject(L""))
		EnableAllMenus();
}

void DecisionLogicFrame::OnSaveProject(wxCommandEvent& WXUNUSED(event))
{
	if (m_worker->Save())
		EnableAllMenus();
}

void DecisionLogicFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	if (m_worker) delete m_worker;
    // true is to force the frame to close
    Close(true);
}

void DecisionLogicFrame::OnUndo(wxCommandEvent& event)
{
	m_worker->Undo(event);
}

void DecisionLogicFrame::OnRedo(wxCommandEvent& event)
{
	m_worker->Redo(event);
}

void DecisionLogicFrame::OnCut(wxCommandEvent& event)
{
	m_worker->Cut(event);
}

void DecisionLogicFrame::OnCopy(wxCommandEvent& event)
{
	m_worker->Copy(event);
}

void DecisionLogicFrame::OnPaste(wxCommandEvent& event)
{
	m_worker->Paste(event);
}

void DecisionLogicFrame::OnNewTable(wxCommandEvent& WXUNUSED(event))
{
	m_worker->NewTable();
}

void DecisionLogicFrame::OnDeleteTable(wxCommandEvent& WXUNUSED(event))
{
	m_worker->DeleteTable();
}

void DecisionLogicFrame::OnNewGroup(wxCommandEvent& WXUNUSED(event))
{
	m_worker->NewGroup();
}

void DecisionLogicFrame::OnDeleteGroup(wxCommandEvent& WXUNUSED(event))
{
	m_worker->DeleteGroup();
}

void DecisionLogicFrame::OnOrientationChange(wxCommandEvent& event)
{
	if (event.GetId() == DecisionLogic_OrientationHorizontal)
	{
		m_worker->SetOrientation(wxHORIZONTAL);
	}
	else
	{
		m_worker->SetOrientation(wxVERTICAL);
	}
}

void DecisionLogicFrame::OnGlobalORs(wxCommandEvent& WXUNUSED(event))
{
	m_worker->LoadTable(GLOBALORS_TABLE_NAME);
}

void DecisionLogicFrame::OnTranslations(wxCommandEvent& WXUNUSED(event))
{
	m_worker->LoadTable(TRANSLATIONS_TABLE_NAME);
}

void DecisionLogicFrame::OnGenerateTranslations(wxCommandEvent& WXUNUSED(event))
{
	m_worker->GenerateTranslations();
}

void DecisionLogicFrame::OnDebugChange(wxCommandEvent& event)
{
	if (event.GetId() == DecisionLogic_DebugON)
	{
		m_worker->SetProjectDebugging(true, DecisionLogic_Server);
	}
	else
	{
		m_worker->SetProjectDebugging(false, DecisionLogic_Server);
	}
}

void DecisionLogicFrame::OnDebugOptions(wxCommandEvent& WXUNUSED(event))
{
	m_worker->ShowDebugOptions();
}

void DecisionLogicFrame::OnCompileCompressed(wxCommandEvent& WXUNUSED(event))
{
	m_worker->CompileZip();
}

void DecisionLogicFrame::OnCompileXML(wxCommandEvent& WXUNUSED(event))
{
	m_worker->CompileXML();
}

void DecisionLogicFrame::OnCompileOptions(wxCommandEvent& WXUNUSED(event))
{
	m_worker->ShowCompileOptions();
}

void DecisionLogicFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
        _T("This is DecisionLogic ")
        _T("running under %s."),
        wxGetOsDescription().c_str()
     ),
     _T("About DecisionLogic"),
     wxOK | wxICON_INFORMATION,
     this);
}

void DecisionLogicFrame::OnOpenRecentFile (wxCommandEvent& event)
{
	wstring filename = recentFileMenu->GetLabelText(event.GetId()).wc_str();
	if (m_worker->OpenProject(filename))
		EnableAllMenus();
}

//Tree Events
void DecisionLogicFrame::TreeItemSelected(wxTreeEvent& event)
{
	m_worker->LoadTable((wstring)m_tree->GetItemText(m_tree->GetSelection()));
}

//Frame Events
void DecisionLogicFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE
}

void DecisionLogicFrame::OnSashDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE ||
		winTree == NULL)
        return;

	if (event.GetId() == DecisionLogic_TreeWindow)
		winTree->SetDefaultSize(wxSize(event.GetDragRect().width, this->GetSize().GetHeight()));
	else if (event.GetId() == DecisionLogic_LogWindow)
		winLog->SetDefaultSize(wxSize(800, event.GetDragRect().height));

#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE

    // Leaves bits of itself behind sometimes
    GetClientWindow()->Refresh();
}

void DecisionLogicFrame::EnableAllMenus()
{
	fileMenu->Enable(DecisionLogic_SaveProject, true);
	compilerMenu->Enable(DecisionLogic_Debug, true);
	compilerMenu->Enable(DecisionLogic_CompileOptions, true);
	compilerMenu->Enable(DecisionLogic_CompileXML, true);
	compilerMenu->Enable(DecisionLogic_CompileCompressed, true);
	tableMenu->Enable(DecisionLogic_NewTable, true);
	tableMenu->Enable(DecisionLogic_DeleteTable, true);
	tableMenu->Enable(DecisionLogic_NewGroup, true);
	tableMenu->Enable(DecisionLogic_DeleteGroup, true);
}

void DecisionLogicFrame::OnInsertCol(wxCommandEvent& event)
{
	m_worker->InsertCol(event);
}

void DecisionLogicFrame::OnInsertRow(wxCommandEvent& event)
{
	m_worker->InsertRow(event);
}

void DecisionLogicFrame::OnDeleteCol(wxCommandEvent& event)
{
	m_worker->DeleteCol(event);
}

void DecisionLogicFrame::OnDeleteRow(wxCommandEvent& event)
{
	m_worker->DeleteRow(event);
}

void DecisionLogicFrame::OnAppendRow(wxCommandEvent& event)
{
	m_worker->AppendRow(event);
}

void DecisionLogicFrame::OnAppendColumn(wxCommandEvent& event)
{
	m_worker->AppendColumn(event);
}

void DecisionLogicFrame::OnClearCells(wxCommandEvent& event)
{
	m_worker->ClearCells(event);
}

void DecisionLogicFrame::OnEditCode(wxCommandEvent& event)
{
	m_worker->EditCode(event);
}

void DecisionLogicFrame::OnSocketEvent(wxSocketEvent& event)
{
	m_worker->ParseSocket(event);
}

void DecisionLogicFrame::OnServerEvent(wxSocketEvent& event)
{
	m_worker->ServerEvent(event, DecisionLogic_Socket);
}

void DecisionLogicFrame::ShowFindReplaceDialog( wxCommandEvent& WXUNUSED(event) )
{
    if ( m_dlgReplace )
    {
        delete m_dlgReplace;
        m_dlgReplace = NULL;
    }
    else
    {
        m_dlgReplace = new MyFindReplaceDialog
		(
			this,
			&m_findData,
			_T("Find/Replace"),
			wxFR_REPLACEDIALOG | wxFR_NOUPDOWN
		);

        m_dlgReplace->Show(true);
    }
}

void DecisionLogicFrame::OnFindDialog(wxFindDialogEvent& event)
{
    wxEventType type = event.GetEventType();
	bool bMatchCase = event.GetFlags() & wxFR_MATCHCASE;
	bool bMatchWholeWord = event.GetFlags() & wxFR_WHOLEWORD;

    if ( type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT )
    {
		if (event.GetFlags() & wxFR_PROJECT)
		{
			if (!m_last_find_pos)
				m_last_find_pos = new wxPoint(0,0);
			bool res = m_worker->FindTextInAnyTable((wstring)event.GetFindString(), m_last_find_pos, &m_found_name, bMatchCase, bMatchWholeWord);
			if (res == false)
			{
				wxMessageBox(_T("No more matches"));
				m_found_name.clear();
			}
			if (m_last_find_pos->x == -1 && m_last_find_pos->y == -1)
			{
				delete m_last_find_pos; //next find will start a new table
				m_last_find_pos = NULL;
			}
		}
		else
		{
			if (type == wxEVT_COMMAND_FIND || (m_last_find_pos->x == -1 && m_last_find_pos->y == -1))
			{
				if (m_last_find_pos)
				{
					delete m_last_find_pos;
					m_last_find_pos = NULL;
				}
				m_last_find_pos = new wxPoint(0,0);
			}

			//highlight on current open table
			m_worker->FindTextInActiveTable((wstring)event.GetFindString(), m_last_find_pos, bMatchCase, bMatchWholeWord);
			if (m_last_find_pos->x == -1 && m_last_find_pos->y == -1)
			{
				wxMessageBox(_T("Reached the end of the table, no matches"));
				if (m_last_find_pos)
				{
					delete m_last_find_pos;
					m_last_find_pos = NULL;
				}
			}
		}
    }
    else if ( type == wxEVT_COMMAND_FIND_REPLACE ||
                type == wxEVT_COMMAND_FIND_REPLACE_ALL )
    {
		if (type == wxEVT_COMMAND_FIND_REPLACE_ALL)
		{
			if (m_last_find_pos)
			{
				delete m_last_find_pos;
				m_last_find_pos = NULL;
			}
			m_last_find_pos = new wxPoint(0,0);
			do
			{
				m_worker->FindTextInActiveTable((wstring)event.GetFindString(), m_last_find_pos, bMatchCase, bMatchWholeWord, true, (wstring)event.GetReplaceString());
			} while (m_last_find_pos->x != -1 && m_last_find_pos->y != -1);
		}
		else
		{
			if (!m_last_find_pos)
				m_last_find_pos = new wxPoint(0,0);
			m_worker->FindTextInActiveTable((wstring)event.GetFindString(), m_last_find_pos, bMatchCase, bMatchWholeWord, true, (wstring)event.GetReplaceString());
			if (m_last_find_pos->x == -1 && m_last_find_pos->y == -1)
				wxMessageBox(_T("Reached the end of the table, no matches"));
		}
    }
    else if ( type == wxEVT_COMMAND_FIND_CLOSE )
    {
		if (m_last_find_pos)
		{
			delete m_last_find_pos;
			m_last_find_pos = NULL;
		}

		m_dlgReplace->Destroy();
		m_dlgReplace = NULL;
    }
    else
    {
        wxLogError(wxT("Unknown find dialog event!"));
    }
}
