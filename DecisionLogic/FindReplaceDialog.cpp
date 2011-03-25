/////////////////////////////////////////////////////////////////////////////
// Name:        DebugOptions.cpp
// Purpose:     DebugOptions class header and DebugOptionsDialog box
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2011 Eric D. Schmidt
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"

    #include "wx/sizer.h"

    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/settings.h"
#endif

#include "FindReplaceDialog.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(MyFindReplaceDialog, wxDialog)

BEGIN_EVENT_TABLE(MyFindReplaceDialog, wxDialog)
    EVT_BUTTON(wxID_FIND, MyFindReplaceDialog::OnFind)
    EVT_BUTTON(wxID_REPLACE, MyFindReplaceDialog::OnReplace)
    EVT_BUTTON(wxID_REPLACE_ALL, MyFindReplaceDialog::OnReplaceAll)
    EVT_BUTTON(wxID_CANCEL, MyFindReplaceDialog::OnCancel)

    EVT_UPDATE_UI(wxID_FIND, MyFindReplaceDialog::OnUpdateFindUI)
    EVT_UPDATE_UI(wxID_REPLACE, MyFindReplaceDialog::OnUpdateFindUI)
    EVT_UPDATE_UI(wxID_REPLACE_ALL, MyFindReplaceDialog::OnUpdateFindUI)

    EVT_CLOSE(MyFindReplaceDialog::OnCloseWindow)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// MyFindReplaceDialog
// ----------------------------------------------------------------------------

void MyFindReplaceDialog::Init()
{
    m_FindReplaceData = NULL;

	m_radioScope = NULL;

    m_chkWord =
    m_chkCase = NULL;

    m_radioDir = NULL;

    m_textFind =
    m_textRepl = NULL;
}

bool MyFindReplaceDialog::Create(wxWindow *parent,
                                        wxFindReplaceData *data,
                                        const wxString& title,
                                        int style)
{
    if ( !wxDialog::Create(parent, wxID_ANY, title,
                           wxDefaultPosition, wxDefaultSize,
                           wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
                           | style) )
    {
        return false;
    }

    SetData(data);

    wxCHECK_MSG( m_FindReplaceData, false,
                 _T("can't create dialog without data") );

    bool isPda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

    wxBoxSizer *leftsizer = new wxBoxSizer( wxVERTICAL );

    // 3 columns because there is a spacer in the middle
    wxBoxSizer *typesizer = new wxBoxSizer( isPda ? wxVERTICAL : wxHORIZONTAL );

	int majorDimension = 0;
	int rbStyle;
    if (isPda)
        rbStyle = wxRA_SPECIFY_ROWS;
    else
        rbStyle = wxRA_SPECIFY_COLS;
	static const wxString searchTypes[] = {_T("Current Table"), _T("Project")};
	m_radioScope = new wxRadioBox(this, wxID_ANY, _("Scope"),
                                wxDefaultPosition, wxDefaultSize,
                                WXSIZEOF(searchTypes), searchTypes,
                                majorDimension, rbStyle);
	typesizer->Add(m_radioScope);
	leftsizer->Add(typesizer);

	wxFlexGridSizer *sizer2Col = new wxFlexGridSizer(3);
    sizer2Col->AddGrowableCol(2);

    sizer2Col->Add(new wxStaticText(this, wxID_ANY, _("Search for:"),
                                    wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                                    0,
                                    wxALIGN_CENTRE_VERTICAL | wxALIGN_RIGHT);

    sizer2Col->Add(10, 0);

    m_textFind = new wxTextCtrl(this, wxID_ANY, m_FindReplaceData->GetFindString());
    sizer2Col->Add(m_textFind, 1, wxALIGN_CENTRE_VERTICAL | wxEXPAND);

    if ( style & wxFR_REPLACEDIALOG )
    {
        sizer2Col->Add(new wxStaticText(this, wxID_ANY, _("Replace with:"),
                                        wxDefaultPosition, wxSize(80, wxDefaultCoord)),
                                        0,
                                        wxALIGN_CENTRE_VERTICAL |
                                        wxALIGN_RIGHT | wxTOP, 5);

        sizer2Col->Add(isPda ? 2 : 10, 0);

        m_textRepl = new wxTextCtrl(this, wxID_ANY,
                                    m_FindReplaceData->GetReplaceString());
        sizer2Col->Add(m_textRepl, 1,
                       wxALIGN_CENTRE_VERTICAL | wxEXPAND | wxTOP, 5);
    }

    leftsizer->Add(sizer2Col, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer *optsizer = new wxBoxSizer( isPda ? wxVERTICAL : wxHORIZONTAL );

    wxBoxSizer *chksizer = new wxBoxSizer( wxVERTICAL);

    m_chkWord = new wxCheckBox(this, wxID_ANY, _("Whole word"));
    chksizer->Add(m_chkWord, 0, wxALL, 3);

    m_chkCase = new wxCheckBox(this, wxID_ANY, _("Match case"));
    chksizer->Add(m_chkCase, 0, wxALL, 3);

    optsizer->Add(chksizer, 0, wxALL, 10);

    static const wxString searchDirections[] = {_("Up"), _("Down")};

    m_radioDir = new wxRadioBox(this, wxID_ANY, _("Search direction"),
                                wxDefaultPosition, wxDefaultSize,
                                WXSIZEOF(searchDirections), searchDirections,
                                majorDimension, rbStyle);

    optsizer->Add(m_radioDir, 0, wxALL, isPda ? 5 : 10);

    leftsizer->Add(optsizer);

    wxBoxSizer *bttnsizer = new wxBoxSizer(wxVERTICAL);

    wxButton* btn = new wxButton(this, wxID_FIND);
    btn->SetDefault();
    bttnsizer->Add(btn, 0, wxALL, 3);

    bttnsizer->Add(new wxButton(this, wxID_CANCEL), 0, wxALL, 3);

    if ( style & wxFR_REPLACEDIALOG )
    {
        bttnsizer->Add(new wxButton(this, wxID_REPLACE, _("&Replace")),
                                    0, wxALL, 3);

        bttnsizer->Add(new wxButton(this, wxID_REPLACE_ALL, _("Replace &all")),
                                    0, wxALL, 3);
    }

    wxBoxSizer *topsizer = new wxBoxSizer( wxHORIZONTAL );

    topsizer->Add(leftsizer, 1, wxALL, isPda ? 0 : 5);
    topsizer->Add(bttnsizer, 0, wxALL, isPda ? 0 : 5);

    int flags = m_FindReplaceData->GetFlags();

    if ( flags & wxFR_MATCHCASE )
        m_chkCase->SetValue(true);

    if ( flags & wxFR_WHOLEWORD )
        m_chkWord->SetValue(true);

    m_radioDir->SetSelection( flags & wxFR_DOWN );

    if ( style & wxFR_NOMATCHCASE )
        m_chkCase->Enable(false);

    if ( style & wxFR_NOWHOLEWORD )
        m_chkWord->Enable(false);

    if ( style & wxFR_NOUPDOWN)
        m_radioDir->Enable(false);

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );

    m_textFind->SetFocus();

    return true;
}

// ----------------------------------------------------------------------------
// send the notification event
// ----------------------------------------------------------------------------

void MyFindReplaceDialog::SendEvent(const wxEventType& evtType)
{
    wxFindDialogEvent event(evtType, GetId());
    event.SetEventObject(this);
    event.SetFindString(m_textFind->GetValue());
    if ( HasFlag(wxFR_REPLACEDIALOG) )
    {
        event.SetReplaceString(m_textRepl->GetValue());
    }

    int flags = 0;

    if ( m_chkCase->GetValue() )
        flags |= wxFR_MATCHCASE;

    if ( m_chkWord->GetValue() )
        flags |= wxFR_WHOLEWORD;

    if ( !m_radioDir || m_radioDir->GetSelection() == 1 )
    {
        flags |= wxFR_DOWN;
    }

	if (!m_radioScope || m_radioScope->GetSelection() == 1 )
	{
		flags |= wxFR_PROJECT;
	}

    event.SetFlags(flags);

    wxFindReplaceDialogBase::Send(event);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void MyFindReplaceDialog::OnFind(wxCommandEvent& WXUNUSED(event))
{
    SendEvent(wxEVT_COMMAND_FIND_NEXT);
}

void MyFindReplaceDialog::OnReplace(wxCommandEvent& WXUNUSED(event))
{
    SendEvent(wxEVT_COMMAND_FIND_REPLACE);
}

void MyFindReplaceDialog::OnReplaceAll(wxCommandEvent& WXUNUSED(event))
{
    SendEvent(wxEVT_COMMAND_FIND_REPLACE_ALL);
}

void MyFindReplaceDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    SendEvent(wxEVT_COMMAND_FIND_CLOSE);

    Show(false);
}

void MyFindReplaceDialog::OnUpdateFindUI(wxUpdateUIEvent &event)
{
    // we can't search for empty strings
    event.Enable( !m_textFind->GetValue().empty() );
}

void MyFindReplaceDialog::OnCloseWindow(wxCloseEvent &)
{
    SendEvent(wxEVT_COMMAND_FIND_CLOSE);
}