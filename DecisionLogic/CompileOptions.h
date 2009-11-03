/////////////////////////////////////////////////////////////////////////////
// Name:        CompileOptions.h
// Purpose:     CompileOptions class header and CompileOptionsDialog box
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
#pragma once

#ifndef COMPILEOPTIONS
#define COMPILEOPTIONS

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

class CompileOptions
{
public:
	CompileOptions()
	{
		PythonCode = L"from math import *\nfrom xml import *";
		JavascriptCode = L"";
	}

	wstring JavascriptCode;
	wstring PythonCode;
};

class CompileOptionsDialog : public wxPropertySheetDialog
{
public:
	CompileOptionsDialog(wxWindow* parent, wxWindowID id, CompileOptions* options):
		wxPropertySheetDialog(parent, id, _T("Compiler Options"), 
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxRESIZE_BORDER)
		, _T("Compiler Options"))
	{
		m_options = options;
		CreateButtons(wxOK|wxCANCEL|wxHELP);

		wxBookCtrlBase* notebook = GetBookCtrl();

		wxPanel* jsOptions = CreateJavascriptPage(notebook);
		wxPanel* pyOptions = CreatePythonPage(notebook);
		notebook->AddPage(jsOptions, _T("Javascript Options"), true);
		notebook->AddPage(pyOptions, _T("Python Options"));

		LayoutDialog();
	}

	inline void OnOK(wxCommandEvent& WXUNUSED(event))
	{
		m_options->JavascriptCode = javaCodeBox->GetValue();
		m_options->PythonCode = pythonCodeBox->GetValue();

		Close();
	}

private:
	inline wxPanel* CreateJavascriptPage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

		javaCodeBox = new wxTextCtrl(panel, wxID_ANY, m_options->JavascriptCode, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		
		topSizer->Add(javaCodeBox, 1, wxGROW);

		panel->SetSizer(topSizer);

		return panel;
	}

	inline wxPanel* CreatePythonPage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

		pythonCodeBox = new wxTextCtrl(panel, wxID_ANY, m_options->PythonCode, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		
		topSizer->Add(pythonCodeBox, 1, wxGROW);

		panel->SetSizer(topSizer);

		return panel;
	}

	wxTextCtrl *javaCodeBox, *pythonCodeBox;
	CompileOptions *m_options;

	DECLARE_EVENT_TABLE()
};

#endif