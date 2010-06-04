/////////////////////////////////////////////////////////////////////////////
// Name:        MDIChild.cpp
// Purpose:     Manages project related info for all tables, saving, loading, etc
// Author:      Eric D. Schmidt
// Modified by:
// Created:     12/04/2010
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
void* pt2Object;

#include "stdafx.h"
#include "MDIChild.h"

void SignalTableChanged()
{
	MDIChild *mySelf = (MDIChild*)pt2Object;
	mySelf->SignalTableChangedCallback();
}

MDIChild::MDIChild(wxMDIParentFrame *parent, void(*ChildCloseCallback)(wstring), bool(*ChildOpenTableCallback)(wstring), 
		bool(*ChildSaveTableCallback)(OpenLogicTable*), int orient, int type, vector<OpenLogicTable> *open_tables, 
		LogicTable logic, ProjectManager *pm, const wxString& title, const wxPoint& pos,
		const wxSize& size, const long style):
		  wxMDIChildFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
                         wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
	m_pm = pm;
	m_type = type;
	m_orientation = orient;
	if (m_orientation == wxVERTICAL)
		m_orientation_opposite = wxHORIZONTAL;
	else
		m_orientation_opposite = wxVERTICAL;

	m_opened_window_tracker = open_tables;
	OpenLogicTable table;
	table.child_window_ptr = (void**)this;
	table.logic_table = logic;
	m_opened_window_tracker->push_back(table);
	m_ChildClosedCallback = ChildCloseCallback;
	m_OpenTableCallback = ChildOpenTableCallback;
	m_ChildSaveCallback = ChildSaveTableCallback;
	m_table = NULL;
	m_table = new LogicGrid(this, m_orientation, -1, type, SignalTableChanged, m_OpenTableCallback, &pm->GlobalORs, wxDefaultPosition, wxDefaultSize);
	if (!m_table)
		throw exception("Could not create LogicGrid object");

	sizer = new wxBoxSizer(wxHORIZONTAL);	
	sizer->Add(m_table, wxEXPAND);

	DisplayTableData(logic);
	StringTable<wstring> rawData = m_table->GetRawTableData();
	stUndo.push(rawData);

	//be consistent with window maximizing or smaller windows
	if (m_opened_window_tracker->size() > 1)
	{
		MDIChild *sibling = (MDIChild*)m_opened_window_tracker->at(0).child_window_ptr;
		if (sibling->IsMaximized())
			this->Maximize(true);
	}
	else
		this->Maximize(true);

	pt2Object = (void*)this;
}
		  
MDIChild::~MDIChild(void)
{
}

void MDIChild::RotateOrientation()
{
	StringTable<wstring> tempInput = m_table->GatherValues(INPUTS);
	StringTable<wstring> tempOutput = m_table->GatherValues(OUTPUTS);
	StringTable<wstring> tempStatus = m_table->GatherValues(STATUS);

	int temp = m_orientation_opposite;
	m_orientation_opposite = m_orientation;
	m_orientation = temp;
	m_table->SetOrientation(m_orientation);
	bool bGetAll = false;
	if (m_table->GetCellValue(0, 0).length() > 0)
		bGetAll = true;

	this->Show(false);
	m_table->FillGrid(&tempInput, &tempOutput, &tempStatus, bGetAll);
	//reset the undo (we could rotate the data in the future and avoid this)
	StringTable<wstring> rawData = m_table->GetRawTableData();
	stUndo.empty();
	stRedo.empty();
	stUndo.push(rawData);

	this->Show(true);
	this->Refresh();
}

void MDIChild::DisplayTableData(LogicTable table)
{
	try
	{
		m_table->ClearGrid();

		DataSet<wstring> *ds = table.GetDataSet();

		//this fills grids with the xml table data
		StringTable<wstring> *inputTable = ds->GetTable(_INPUT_TABLE);
		StringTable<wstring> *outputTable = ds->GetTable(_OUTPUT_TABLE);
		StringTable<wstring> *statusTable = ds->GetTable(_STATUS_TABLE);

		//should have equal number of columns
		if (m_type == 0 && (inputTable->Columns() > 0 && inputTable->Columns() != outputTable->Columns()))
			throw "Tables do not have the same number of columns";

		m_table->FillGrid(inputTable, outputTable, statusTable, table.bGetAll);
	}
	catch(...)
	{
		ReportError("MDIChild::DisplayTableData");
	}
}

void MDIChild::DisplayRawTableData(StringTable<wstring> table)
{
	m_table->FillGrid(table);
}

void MDIChild::RepopulateTranslationsTable(set<wstring> *strings)
{
	//the strings go in the base language column (0).
	//we want to add new strings, but leave the existing rows untouched
	set<wstring> *tableStrings = new set<wstring>(strings->begin(), strings->end());
	StringTable<wstring> existingData = m_table->GatherValues();
	for (size_t i = 1; i < existingData.Columns(); i++)
	{
		wstring val = existingData.GetItem(0, i);
		if (val.length() > 0)
			tableStrings->insert(val);
	}

	//remove any special commands/operations
	vector<wstring> removeItems;
	for (set<wstring>::iterator it = tableStrings->begin(); it != tableStrings->end(); it++)
	{
		if (UTILS::StringContains(*it, L"get(") == true ||
			UTILS::StringContains(*it, L"eval(") == true ||
			UTILS::StringIsNumeric(*it) == true)
		{
			removeItems.push_back(*it);
		}
	}

	for (vector<wstring>::iterator it = removeItems.begin(); it != removeItems.end(); it++)
		tableStrings->erase(*it);

	size_t last_found_location = 1;
	for (set<wstring>::iterator it = tableStrings->begin(); it != tableStrings->end(); it++)
	{
		bool bFoundExistingEntry = false;
		for (size_t i = 1; i < existingData.Columns(); i++)
		{
			if (existingData.GetItem(0, i) == *it)
			{
				bFoundExistingEntry = true;
				last_found_location = i + 1;
				break;
			}
		}

		if (!bFoundExistingEntry)
		{
			existingData.InsertColumn(*it, last_found_location);
			existingData.SetItem(0, last_found_location, *it);
			last_found_location++;
		}
	}

	m_table->FillGrid(&existingData, NULL, NULL, false);

	//highlight dead values
	tableStrings = new set<wstring>(strings->begin(), strings->end());
	for (size_t i = 1; i < existingData.Columns(); i++)
	{
		wstring val = existingData.GetItem(0, i);
		if (val.length() > 0)
		{
			set<wstring>::iterator itFind = tableStrings->find(val);
			if (itFind == tableStrings->end())
			{
				m_table->HighlightRule(i);
			}
		}
	}
	delete tableStrings;
}

void MDIChild::InsertCol(wxCommandEvent& event) {m_table->OnInsertCol(event);}
void MDIChild::InsertRow(wxCommandEvent& event) {m_table->OnInsertRow(event);}
void MDIChild::AppendRow() {m_table->OnAppendRow(true);}
void MDIChild::AppendColumn() {m_table->OnAppendColumn(true);}
void MDIChild::DeleteCol(wxCommandEvent& event) {m_table->OnDeleteCol(event);}
void MDIChild::DeleteRow(wxCommandEvent& event) {m_table->OnDeleteRow(event);}
void MDIChild::ClearCells(wxCommandEvent& event) {m_table->OnClearCells(event);}
void MDIChild::Undo(wxCommandEvent& event) 
{
	if (stUndo.size() > 0)
	{
		StringTable<wstring> tableData = stUndo.top();
		if (stUndo.size() > 1) //don't lose the original
		{
			stUndo.pop();
			stRedo.push(tableData);
		}
		tableData = stUndo.top();
		
		DisplayRawTableData(tableData);
	}
}

void MDIChild::Redo(wxCommandEvent& event) 
{
	if (stRedo.size() > 0)
	{
		StringTable<wstring> tableData = stRedo.top();
		stRedo.pop();
		stUndo.push(tableData);
		DisplayRawTableData(tableData);
	}
}

void MDIChild::Cut(wxCommandEvent& event) {return m_table->OnCut(event);}
void MDIChild::Copy(wxCommandEvent& event) {return m_table->OnCopy(event);}
void MDIChild::Paste(wxCommandEvent& event) {return m_table->OnPaste(event);}
void MDIChild::EditCode(wxCommandEvent& event) {return m_table->OnEditCode(event);}

void MDIChild::HighlightRule(int rule) {m_table->HighlightRule(rule);}
bool MDIChild::HasChanged() {return m_table->HasChanged;}
OpenLogicTable MDIChild::Save()
{
	OpenLogicTable table = GetCurrentTable();
	m_table->HasChanged = false;
	return table;
}

wxPoint MDIChild::FindText(wstring textToFind, wxPoint startPos, bool bMatchCase, bool bMatchWholeWord)
{
	wxPoint retval(-1, -1);
	if (startPos.x < 0 || startPos.y < 0)
		return retval;

	for (int j = startPos.y; j < m_table->GetNumberRows(); j++)
	{
		for (int i = startPos.x; i < m_table->GetNumberCols(); i++)
		{
			if (TestCellTextMatch(j, i, textToFind, bMatchCase, bMatchWholeWord))
			{
				retval.x = i;
				retval.y = j;
				m_table->SelectBlock(j, i, j, i);
				return retval;
			}
		}
		startPos.x = 0;
	}
	return retval;
}

wxPoint MDIChild::FindAndReplaceText(wstring textToFind, wxPoint startPos, bool bMatchCase, bool bMatchWholeWord, wstring textToReplace)
{
	wxPoint curLocCheck = GetPreviousCellPosition(startPos);
	wxString find = textToFind;
	wxString replace = textToReplace;
	if (TestCellTextMatch(curLocCheck.y, curLocCheck.x, textToFind, bMatchCase, bMatchWholeWord))
	{
		m_table->SelectBlock(curLocCheck.y, curLocCheck.x, curLocCheck.y, curLocCheck.x);
		if (bMatchWholeWord)
			m_table->SetCellValue(curLocCheck.y, curLocCheck.x, textToReplace);
		else
		{
			wxString cellvalue = m_table->GetCellValue(curLocCheck.y, curLocCheck.x);
			int pos = cellvalue.Lower().Find(find.Lower());
			cellvalue.Remove(pos, find.Length());
			cellvalue = cellvalue.insert(pos, textToReplace);
			m_table->SetCellValue(curLocCheck.y, curLocCheck.x, cellvalue);
		}
		SignalTableChanged();			
		return curLocCheck;
	}

	wxPoint retval = FindText(textToFind, startPos, bMatchCase, bMatchWholeWord);
	if (retval.x != -1 && retval.y != -1)
	{
		if (bMatchWholeWord)
			m_table->SetCellValue(retval.y, retval.x, textToReplace);
		else
		{
			wxString cellvalue = m_table->GetCellValue(retval.y, retval.x);
			int pos = cellvalue.Lower().Find(find.Lower());
			cellvalue.Remove(pos, find.Length());
			cellvalue = cellvalue.insert(pos, textToReplace);
			m_table->SetCellValue(retval.y, retval.x, cellvalue);
		}
		SignalTableChanged();
	}

	return retval;
}

wxPoint MDIChild::GetNextCellPosition(wxPoint pos)
{
	wxPoint retval = pos;
	if (pos.x < m_table->GetNumberCols())
		retval.x++;
	else if (pos.y < m_table->GetNumberRows())
		retval.y++;
	else
	{
		retval.x = 0;
		retval.y = 0;
	}
	return retval;
}

wxPoint MDIChild::GetPreviousCellPosition(wxPoint pos)
{
	wxPoint retval = pos;
	if (pos.x > 0)
		retval.x--;
	else if (pos.y > 0)
		retval.y--;
	else
	{
		retval.x = 0;
		retval.y = 0;
	}
	return retval;
}

vector<size_t> MDIChild::GetDisabledRules()
{
	vector<size_t> retval;
	wstring value;
	size_t cnt = 0;


	if (m_orientation == wxHORIZONTAL)
	{
		for (int i = 2; i < m_table->GetNumberCols(); i++)
		{
			value = m_table->GetCellValue(0, i);

			if (value == L"Disabled")
				retval.push_back(cnt);
			cnt++;
		}
	}
	else
	{
		for (int j = 2; j < m_table->GetNumberRows(); j++)
		{
			value = m_table->GetCellValue(j, 0);
			if (value == L"Disabled")
				retval.push_back(cnt);
			cnt++;
		}
	}
	return retval;
}

void MDIChild::OnActivate(wxActivateEvent& event) 
{
	pt2Object = (void*)this;
}

void MDIChild::OnChildClose(wxCloseEvent& event)
{
	vector<OpenLogicTable>::iterator it;
	for (it = m_opened_window_tracker->begin(); it != m_opened_window_tracker->end(); it++)
	{
		if (it->child_window_ptr == (void*)this)
		{
			break;
		}
	}
	if (HasChanged())
	{
		wxMessageDialog saveNow(this, _T("Do you want to save the current table changes?"),
			_T("Save Changes"), wxYES_NO | wxCANCEL);
		int res = saveNow.ShowModal();
		if (res == wxID_YES)
		{
			OpenLogicTable newData = Save();
			if (!m_ChildSaveCallback(&newData))
			{
				wxMessageBox(_T("Save failed"));
				return;
			}
			else
			{
				m_table->HasChanged = false;
				m_opened_window_tracker->erase(it);
			}
		}
		else if (res == wxID_CANCEL)
			return;
		else if (res == wxID_NO)
		{
			m_opened_window_tracker->erase(it);
		}
	}
	else
		m_opened_window_tracker->erase(it);

	m_ChildClosedCallback((wstring)this->GetTitle());
	event.Skip();
}

bool MDIChild::TestCellTextMatch(int row, int col, wxString find, bool bMatchCase, bool bMatchWholeWord)
{
	bool bTest = false;

	if (bMatchCase == true && bMatchWholeWord == true)
	{
		bTest = m_table->GetCellValue(row, col) == find;
	}
	else if (bMatchCase == true && bMatchWholeWord == false)
	{
		bTest = m_table->GetCellValue(row, col).Contains(find);
	}
	else if (bMatchCase == false && bMatchWholeWord == true)
	{
		bTest = m_table->GetCellValue(row, col).Lower() == find.Lower();
	}
	else if (bMatchCase == false && bMatchWholeWord == false)
	{
		bTest = m_table->GetCellValue(row, col).Lower().Contains(find.Lower());
	}

	return bTest;
}

OpenLogicTable MDIChild::GetCurrentTable()
{
	StringTable<wstring> inputTable = m_table->GatherValues(INPUTS);
	StringTable<wstring> outputTable = m_table->GatherValues(OUTPUTS);
	StringTable<wstring> statusTable = m_table->GatherValues(STATUS);
	OpenLogicTable table;

	for (vector<OpenLogicTable>::iterator it = m_opened_window_tracker->begin(); it != m_opened_window_tracker->end(); it++)
	{
		if (it->child_window_ptr == (void*)this)
		{
			table = (*it);
			break;
		}
	}

	if (m_table->GetCellValue(0, 0).length() > 0)
		table.logic_table.bGetAll = true;
	else
		table.logic_table.bGetAll = false;

	DataSet<wstring> *ds = table.logic_table.GetDataSet();
	ds->Clear();
	ds->AddTable(inputTable);
	ds->AddTable(outputTable);
	ds->AddTable(statusTable);		

	return table;
}

void MDIChild::SignalTableChangedCallback()
{
	m_table->HasChanged = true;
	StringTable<wstring> tableData = m_table->GetRawTableData();
	stUndo.push(tableData);
	while (!stRedo.empty())
		stRedo.pop();
}