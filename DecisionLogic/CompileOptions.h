/////////////////////////////////////////////////////////////////////////////
// Name:        CompileOptions.h
// Purpose:     CompileOptions class header and CompileOptionsDialog box
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2011 Eric D. Schmidt, DigiRule Solutions LLC
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
#include "CodeControl.h"
#include <string>
#include <vector>
#include "utilities.h"

using namespace std;

class CompileOptions
{
public:
	CompileOptions()
	{
		PythonCode = L"";
		JavascriptCode = L"";
	}

	wstring JavascriptCode;
	wstring PythonCode;
};

class CompileOptionsDialog : public wxPropertySheetDialog
{
public:
	CompileOptionsDialog(wxWindow* parent, wxWindowID id, CompileOptions* options):
		wxPropertySheetDialog(parent, id, _T("Global Code"), 
		wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxRESIZE_BORDER)
		, _T("Global Code"))
	{
		m_options = options;
		CreateButtons(wxOK|wxCANCEL);

		wxBookCtrlBase* notebook = GetBookCtrl();

		wxPanel* jsOptions = CreateJavascriptPage(notebook);
		wxPanel* pyOptions = CreatePythonPage(notebook);
		notebook->AddPage(jsOptions, _T("Javascript Global Code"), true);
		notebook->AddPage(pyOptions, _T("Python Global Code"));

		LayoutDialog();
	}

	inline void OnOK(wxCommandEvent& WXUNUSED(event))
	{
		m_options->JavascriptCode = javaCodeBox->GetText();
		m_options->PythonCode = pythonCodeBox->GetText();

		Close();
	}

private:
	inline wxPanel* CreateJavascriptPage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

		javaCodeBox = new Edit(panel, wxID_ANY, wxDefaultPosition, wxSize(600, 480), wxTE_MULTILINE);
		javaCodeBox->LoadText(m_options->JavascriptCode, L"C++");

		topSizer->Add(javaCodeBox, 1, wxGROW);

		panel->SetSizer(topSizer);

		return panel;
	}

	inline wxPanel* CreatePythonPage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

		pythonCodeBox = new Edit(panel, wxID_ANY, wxDefaultPosition, wxSize(600, 480), wxTE_MULTILINE);
		pythonCodeBox->LoadText(m_options->PythonCode, L"Python");

		topSizer->Add(pythonCodeBox, 1, wxGROW);

		panel->SetSizer(topSizer);

		return panel;
	}	

	Edit *javaCodeBox, *pythonCodeBox;
	CompileOptions *m_options;

	DECLARE_EVENT_TABLE()
};

#endif