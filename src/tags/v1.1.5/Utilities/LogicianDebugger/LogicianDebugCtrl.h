#ifndef _WX_LOGICIANDEBUG_H_BASE_
#define _WX_LOGICIANDEBUG_H_BASE_

#include "StringTable.h"
#include "ROMViewer.h"
#include "EDSDebug.h"
#include "KnowledgeBase.h"
#include "ROMNode.h"

using namespace std;
using namespace EDS;
using namespace ROM;

class wxLogicianDebugCtrl : public wxPanel
{
	DECLARE_DYNAMIC_CLASS(wxLogicianDebugCtrl)		
    DECLARE_EVENT_TABLE()

public:
// Constructors
    wxLogicianDebugCtrl() { Init(); }
	virtual ~wxLogicianDebugCtrl() {}

	wxLogicianDebugCtrl(ROMNode* node, wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize):
	wxPanel(parent, id, pos, size)
    {
        Init();
        Create(node, parent, id, pos, size);
		defaultSize = size;
		Update();
    }

    // Creation
	bool Create(ROMNode* node, wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize)
	{
		bLoading = true;
		wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

		m_splitter = new wxSplitterWindow(this, wxID_ANY, pos, size);
		m_splitter->SetSashGravity(0.0);
		
		wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
		wxPanel *top = new wxPanel(m_splitter, wxID_ANY);		
		m_viewer = new wxROMViewerCtrl(node, top, wxID_ANY);
		topSizer->Add(m_viewer, 1, wxEXPAND|wxSHRINK);
		top->SetSizer(topSizer);

		wxBoxSizer *bottomSizer = new wxBoxSizer(wxVERTICAL);
		wxPanel *bottom = new wxPanel(m_splitter, wxID_ANY);
		m_edsDebug = new wxEDSDebugCtrl(node->GetKnowledgeBase(), bottom, wxID_ANY);
		bottomSizer->Add(m_edsDebug, 1, wxEXPAND|wxSHRINK);
		bottom->SetSizer(bottomSizer);

		m_splitter->SplitHorizontally(top, bottom, 400);

		sizerTop->Add(m_splitter, 1, wxEXPAND|wxSHRINK);

		this->SetSizer(sizerTop);
		sizerTop->SetSizeHints(parent);
		sizerTop->Fit(parent);

		bLoading = false;
		return true;
	}

	// Common initialization
    void Init() { bLoading = false;}

    // Overrides
    wxSize DoGetBestSize() const { return defaultSize; }
	void OnSize(wxSizeEvent& event)
	{
		wxSize size = GetClientSize();
		m_splitter->SetSize(size.GetWidth(), size.GetHeight());
		event.Skip();
	}

	void Update()
	{
		if (m_viewer)
			m_viewer->Update();
	}

private:
	wxSplitterWindow*	m_splitter;
	wxROMViewerCtrl*	m_viewer;
	wxEDSDebugCtrl*		m_edsDebug;
	bool				bLoading;
	wxSize				defaultSize;
};

BEGIN_EVENT_TABLE(wxLogicianDebugCtrl, wxControl)
	EVT_SIZE (wxLogicianDebugCtrl::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxLogicianDebugCtrl, wxControl)
#endif