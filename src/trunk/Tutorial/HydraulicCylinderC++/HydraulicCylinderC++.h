
// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
//#define USEDEBUGGER 1 //use this to demonstrate integrated application debugging
#include "RelationalObjectModel/ROMNode.h"
#include "RelationalObjectModel/LinearEngine.h"

#ifdef USEDEBUGGER
#include "LogicianDebugCtrl.h"
#endif

using namespace std;
using namespace ROM;

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
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);
	~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
	void ComboEvaluate(wxCommandEvent& event);
	void TextChanged(wxCommandEvent& event);

private:
	void SetupApplication();
	void UpdateControls();
	void UpdateCatalog();
	void SetControlUI(IROMDictionaryAttribute* attr);

#ifdef USEDEBUGGER
	wxLogicianDebugCtrl *m_debugger;
#endif
	EDS::IKnowledgeBase *m_rules;
	ROMNode *m_rootNode;
	LinearEngine *m_engine;
	bool bLoadingItems;
	wxPanel *panel;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	CylinderSeries = 0,
	ComboCtrl,
	EditCtrl,
	LabelCtrl,
	lbOptionHead,
	Catalog,
    // menu items
    Minimal_Quit = wxID_EXIT,
	Minimal_Open = wxID_OPEN,
	Minimal_Save = wxID_SAVE,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};
