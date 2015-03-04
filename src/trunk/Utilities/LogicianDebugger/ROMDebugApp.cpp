/////////////////////////////////////////////////////////////////////////////
// Name:        HydraulicCylinderC++.cpp
// Purpose:     Sample C++ Logician app
// Author:      Eric D. Schmidt
// Created:     10/1/2010
/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

#include "ROMDebugApp.h"
#ifdef WIN32
#include <windows.h>
#endif

#include <memory>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame(_T("Logician C++ Test App"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("ROMViewer C++ Test App"));
#endif // wxUSE_STATUSBAR

	auto rawptr = ROMNode::LoadXML("config.xml", nullptr);
	node = std::unique_ptr<ROMNode>(rawptr);
		

	ROMNode *childNode = new ROMNode("Child1");
	childNode->SetAttribute("attr1", "A child attr");
	node->AddChildROMObject(childNode);
	ROMNode *childNode2 = new ROMNode("Child2");
	childNode2->SetAttribute("attr2", "A child attr");
	node->AddChildROMObject(childNode2);
	ROMNode *childNode3 = new ROMNode("Child3");
	childNode3->SetAttribute("attr3", "A child attr");
	childNode3->SetAttribute("attr3", "attrSub1", "A sub child attr");
	childNode3->SetAttribute("attr3", "attrSub2", "A 2nd sub child attr");
	childNode2->AddChildROMObject(childNode3);

	panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

	debug = new wxLogicianDebugCtrl(node.get(), panel, VIEWER, wxDefaultPosition,
		wxSize(640, 480));

	sizerTop->Add(debug, 1, wxEXPAND|wxSHRINK);
	panel->SetSizer(sizerTop);

	bLoadingItems = false;
	SetupApplication();

	sizerTop->SetSizeHints(this);
	sizerTop->Fit(this);
}

MyFrame::~MyFrame()
{
}

void MyFrame::SetupApplication()
{
	UpdateControls();
}

void MyFrame::UpdateControls()
{
	bLoadingItems = true;

	bLoadingItems = false;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T("Welcome to %s!\n")
                    _T("\n")
                    _T("This is the ROMViewer Demo\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About ROMViewer Demo"),
                 wxOK | wxICON_INFORMATION,
                 this);
}
