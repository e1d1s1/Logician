/////////////////////////////////////////////////////////////////////////////
// Name:        CodeEditor.h
// Purpose:     Simple dialog for edit script code
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2010 Eric D. Schmidt
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
#pragma once

#ifndef CODEEDITOR
#define CODEEDITOR

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dialog.h>
#include <wx/bookctrl.h>
#include <wx/propdlg.h>
#include <string>
#include <vector>
#include "utilities.h"

using namespace std;

class CodeEditorDialog : public wxPropertySheetDialog
{
public:
	CodeEditorDialog(wxWindow* parent, wxWindowID id, wstring *text):wxPropertySheetDialog(parent, id, _T("Code Editor"), 
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxRESIZE_BORDER)
		, _T("Code Editor"))
	{
		m_code = text;
		CreateButtons(wxOK|wxCANCEL|wxHELP);

		wxBookCtrlBase* notebook = GetBookCtrl();
		wxPanel* codePanel = CreateCodePage(notebook);
		notebook->AddPage(codePanel, _T("Source"), true);

		LayoutDialog();		
	}

	void OnOK(wxCommandEvent& WXUNUSED(event))
	{
		*m_code = codeBox->GetValue();
		Close();
	}

private:

	wxPanel* CreateCodePage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

		codeBox = new wxTextCtrl(panel, wxID_ANY, m_code->c_str(), wxDefaultPosition, wxSize(320, 480), wxTE_MULTILINE);
		
		topSizer->Add(codeBox, 1, wxGROW|wxALL);
		panel->SetSizer(topSizer);

		return panel;
	}

	wxTextCtrl *codeBox;
	wstring *m_code;
	DECLARE_EVENT_TABLE()
};

#endif