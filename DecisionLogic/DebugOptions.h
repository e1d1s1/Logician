/////////////////////////////////////////////////////////////////////////////
// Name:        DebugOptions.h
// Purpose:     DebugOptions class header and DebugOptionsDialog box
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2011 Eric D. Schmidt
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

#ifndef DEBUGOPTIONS
#define DEBUGOPTIONS

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

class DebugOptions
{
public:
	DebugOptions()
	{
		debugMode = ALL_TABLES;
		bOpenTable = false;
		connection = _T("localhost:11000");
	}

	vector<wstring> allTables;
	vector<wstring> selectedTables;
	int debugMode;
	bool bOpenTable;
	wstring connection;

	enum
	{
		ALL_TABLES = 0,
		SELECTED_TABLES = 1
	};
};

const wxString debugOutputChoices[2] = { _T("&All Tables"), _T("&Selected Tables Only") };

enum
{
	DEBUG_SCOPE = 1100,
	DEBUG_ADD_TABLE,
	DEBUG_REMOVE_TABLE,
	TABLELIST,
	DEBUGLIST
};

class DebugOptionsDialog : public wxPropertySheetDialog
{
public:
	DebugOptionsDialog(wxWindow* parent, wxWindowID id, DebugOptions* options):
	wxPropertySheetDialog(parent, id, _T("Debugging Options"), 
	wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxRESIZE_BORDER)
	, _T("Debugging Options"))
	{
		m_options = options;
		CreateButtons(wxOK|wxCANCEL|wxHELP);

		wxBookCtrlBase* notebook = GetBookCtrl();

		wxPanel* tableDebugging = CreateTableDebuggingPage(notebook);
		notebook->AddPage(tableDebugging, _T("Table Debugging"), true);

		LayoutDialog();
	}

	inline void OnOK(wxCommandEvent& WXUNUSED(event))
	{
		m_options->selectedTables.clear();

		if (debugChoice->GetSelection() != 0) for (unsigned int i = 0; i < selectedList->GetCount(); i++)
		{
			m_options->selectedTables.push_back((wstring)selectedList->GetString(i));
		}

		m_options->debugMode = debugChoice->GetSelection();
		m_options->bOpenTable = chkOpenTableOnDebug->IsChecked();
		m_options->connection = (wstring)txtCon->GetValue();

		Close();
	}

private:
	inline wxPanel* CreateTableDebuggingPage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);

		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
		
		debugChoice = new wxRadioBox(panel, DEBUG_SCOPE, _T("Debugging &scope"),
			wxDefaultPosition, wxDefaultSize, 2, debugOutputChoices);
		txtCon = new wxTextCtrl(panel, wxID_ANY, m_options->connection);
		
		wxBoxSizer *tableSizer = new wxBoxSizer( wxHORIZONTAL );
		wxStaticBox *box1 = new wxStaticBox(panel, wxID_ANY, _T("&Project Tables"));
		wxSizer *staticBox1 = new wxStaticBoxSizer(box1, wxVERTICAL);
		tableList = new wxListBox(panel, TABLELIST, wxDefaultPosition, wxSize(150, 200), 0, NULL, wxLB_SORT);		
		staticBox1->Add(tableList, 1, wxGROW);

		wxBoxSizer *arrowSizer = new wxBoxSizer( wxVERTICAL );
		wxButton *btnAdd = new wxButton(panel, DEBUG_ADD_TABLE, _T(">"), wxDefaultPosition, wxSize(15,15));
		wxButton *btnRemove = new wxButton(panel, DEBUG_REMOVE_TABLE, _T("<"), wxDefaultPosition, wxSize(15,15));
		arrowSizer->Add(btnAdd, 0);
		arrowSizer->AddSpacer(10);
		arrowSizer->Add(btnRemove, 0);

		wxStaticBox *box2 = new wxStaticBox(panel, wxID_ANY, _T("&Debugging Tables"));
		wxSizer *staticBox2 = new wxStaticBoxSizer(box2, wxVERTICAL);
		selectedList = new wxListBox(panel, DEBUGLIST, wxDefaultPosition, wxSize(150, 200), 0, NULL, wxLB_SORT);	
		staticBox2->Add(selectedList, 1, wxGROW);
		chkOpenTableOnDebug = new wxCheckBox(panel, wxID_ANY, _T("&Open table and highlight on debug"));

		tableSizer->Add(staticBox1, 1, wxGROW|wxALL);
		tableSizer->Add(arrowSizer, 0, wxCENTER);
		tableSizer->Add(staticBox2, 1, wxGROW|wxALL);
		topSizer->Add(debugChoice, 0, wxGROW|wxALL);
		topSizer->Add(txtCon, 0, wxGROW|wxALL);
		topSizer->Add(tableSizer, 1, wxGROW|wxALL);
		topSizer->Add(chkOpenTableOnDebug, 0, wxGROW|wxALL);

		//initialize items
		debugChoice->Select(m_options->debugMode);


		//populate the two lists
		for (vector<wstring>::iterator it = m_options->selectedTables.begin(); it != m_options->selectedTables.end(); it++)
		{
			selectedList->Append(*it);
		}

		for (vector<wstring>::iterator it = m_options->allTables.begin(); it != m_options->allTables.end(); it++)
		{
			if (find(m_options->selectedTables.begin(), m_options->selectedTables.end(), *it) == m_options->selectedTables.end())
				tableList->Append(*it);
		}		

		chkOpenTableOnDebug->SetValue(m_options->bOpenTable);

		if (debugChoice->GetSelection() == 0)
		{
			tableList->Enable(false);
			selectedList->Enable(false);
			chkOpenTableOnDebug->SetValue(false);
			chkOpenTableOnDebug->Enable(false);
		}

		panel->SetSizer(topSizer);

		return panel;
	}

	inline void OnScopeChanage(wxCommandEvent &event)
	{
		if (debugChoice->GetSelection() == 0)
		{
			tableList->Enable(false);
			selectedList->Enable(false);
			chkOpenTableOnDebug->SetValue(false);
			chkOpenTableOnDebug->Enable(false);
		}
		else
		{
			tableList->Enable(true);
			selectedList->Enable(true);
			chkOpenTableOnDebug->Enable(true);
		}
	}

	inline void AddRemoveClick(wxCommandEvent &event)
	{
		if (event.GetId() == DEBUG_ADD_TABLE || event.GetId() == TABLELIST)
		{
			int n_all = tableList->GetSelection();
			if (n_all != wxNOT_FOUND && selectedList->FindString(tableList->GetString(n_all)) == wxNOT_FOUND)
			{
				selectedList->Append(tableList->GetString(n_all));
				tableList->Delete(n_all);
			}
		}
		else
		{
			int n_selected = selectedList->GetSelection();
			if (n_selected != wxNOT_FOUND && tableList->FindString(selectedList->GetString(n_selected)) == wxNOT_FOUND)
			{
				tableList->Append(selectedList->GetString(n_selected));
				selectedList->Delete(n_selected);
			}
		}
	}

	DebugOptions *m_options;
	wxListBox *selectedList, *tableList;
	wxRadioBox *debugChoice;
	wxCheckBox *chkOpenTableOnDebug;
	wxTextCtrl *txtCon;

	DECLARE_EVENT_TABLE()
};

#endif