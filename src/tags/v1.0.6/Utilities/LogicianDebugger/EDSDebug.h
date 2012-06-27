/*!
 * A control for debugging EDSEngine table evaluation
 */

#ifndef _WX_EDSDEBUG_H_BASE_
#define _WX_EDSDEBUG_H_BASE_

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


#include "KnowledgeBase.h"
using namespace EDS;

enum
{
	CLEAR = 200
};

void UpdateDebug(wstring msg);
void* ptEDSDebugCtrlObject;

class wxEDSDebugCtrl : public wxPanel
{
	DECLARE_DYNAMIC_CLASS(wxEDSDebugCtrl)		
    DECLARE_EVENT_TABLE()

public:
    // Constructors
    wxEDSDebugCtrl() { Init(); }
	virtual ~wxEDSDebugCtrl() {}

	wxEDSDebugCtrl(EDS::CKnowledgeBase* knowledge, wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize):
	wxPanel(parent, id, pos, size)
    {
        Init();
        Create(knowledge, parent, id, pos, size);
		defaultSize = size;
    }

    // Creation
	bool Create(EDS::CKnowledgeBase* knowledge, wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize)
	{
		bLoading = true;
		if (knowledge)
		{
			knowledge->SetDebugHandler(UpdateDebug);
		}

		wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

		m_panel = new wxPanel(this, wxID_ANY, pos, size);

		wxBoxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
		wxString choicesView[2] = {_("All Tables"), _("Selected Table")};
		m_txtTable = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, 25));
		m_rbDebugMode = new wxRadioBox(m_panel, wxID_ANY, _("Debug Mode"), wxDefaultPosition, wxSize(200, 50),
			2, choicesView);
		wxButton *btnClear = new wxButton(m_panel, CLEAR, L"Clear");
		sizerRow->Add(m_rbDebugMode);
		sizerRow->Add(m_txtTable, 2, wxCENTER);
		sizerRow->Add(btnClear, 0, wxCENTER);
		sizerTop->Add(sizerRow);

		m_txtLog = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(size.GetWidth(), 150), wxTE_MULTILINE | wxTE_READONLY);
		sizerTop->Add(m_txtLog, 1, wxGROW);

		m_panel->SetSizer(sizerTop);
		sizerTop->SetSizeHints(parent);
		sizerTop->Fit(parent);

		bLoading = false;
		return true;
	}

	// Common initialization
    void Init() { bLoading = false; ptEDSDebugCtrlObject = (void*)this;}

    // Overrides
    wxSize DoGetBestSize() const { return defaultSize; }
	void OnSize(wxSizeEvent& event)
	{
		wxSize size = GetClientSize();
		m_panel->SetSize(size.GetWidth(), size.GetHeight());
		event.Skip();
	}

	void Update(wstring msg)
	{
		if (m_rbDebugMode->GetSelection() == 0)			
		{
			m_txtLog->AppendText(msg);
			m_txtLog->AppendText(L"\n");
		}
		else
		{
			size_t start = 17; //sizeof("<TableEval name=\"");
			size_t end = msg.find(L"\"", start);
			wstring tableName = msg.substr(start, end - start);
			wstring filter = m_txtTable->GetValue();
			if (filter.find(tableName) != string::npos)
			{
				m_txtLog->AppendText(msg);
				m_txtLog->AppendText(L"\n");
			}
		}
	}

	void OnClear(wxCommandEvent& event)
	{
		m_txtLog->Clear();
	}

private:
	wxPanel		*m_panel;
	wxRadioBox	*m_rbDebugMode;
	wxTextCtrl	*m_txtTable, *m_txtLog;
	bool		bLoading;
	wxSize		defaultSize;
};

void UpdateDebug(wstring msg)
{
	wxEDSDebugCtrl *mySelf = (wxEDSDebugCtrl*)ptEDSDebugCtrlObject;
	if (mySelf)
		mySelf->Update(msg);
}

BEGIN_EVENT_TABLE(wxEDSDebugCtrl, wxControl)
	EVT_SIZE (wxEDSDebugCtrl::OnSize)
	EVT_BUTTON (CLEAR, wxEDSDebugCtrl::OnClear)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxEDSDebugCtrl, wxControl)
#endif