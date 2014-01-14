/////////////////////////////////////////////////////////////////////////////
// Name:        FindReplaceDialog.h
// Purpose:     Custom reimplemtation of wxWidgets Find/Replace dialog
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2014 Eric D. Schmidt, DigiRule Solutions LLC
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

#ifndef _MY_FDREPDLG_H_
#define _MY_FDREPDLG_H_

#include <wx/fdrepdlg.h>

class WXDLLEXPORT wxCheckBox;
class WXDLLEXPORT wxRadioBox;
class WXDLLEXPORT wxTextCtrl;

// ----------------------------------------------------------------------------
// MyFindReplaceDialog: dialog for searching / replacing text
// ----------------------------------------------------------------------------

class WXDLLEXPORT MyFindReplaceDialog : public wxFindReplaceDialogBase
{
public:
    MyFindReplaceDialog() { Init(); }

    MyFindReplaceDialog(wxWindow *parent,
                               wxFindReplaceData *data,
                               const wxString& title,
                               int style = 0)
    {
        Init();

        (void)Create(parent, data, title, style);
    }

    bool Create(wxWindow *parent,
                wxFindReplaceData *data,
                const wxString& title,
                int style = 0);

protected:
    void Init();

    void SendEvent(const wxEventType& evtType);

    void OnFind(wxCommandEvent& event);
    void OnReplace(wxCommandEvent& event);
    void OnReplaceAll(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    void OnUpdateFindUI(wxUpdateUIEvent& event);

    void OnCloseWindow(wxCloseEvent& event);

	wxRadioBox *m_radioScope;

    wxCheckBox *m_chkCase,
               *m_chkWord;

    wxRadioBox *m_radioDir;

    wxTextCtrl *m_textFind,
               *m_textRepl;

private:
    DECLARE_DYNAMIC_CLASS(MyFindReplaceDialog)

    DECLARE_EVENT_TABLE()
};

enum
{
	wxFR_PROJECT = 16
};

#endif // _WX_GENERIC_FDREPDLG_H_
