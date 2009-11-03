/////////////////////////////////////////////////////////////////////////////
// Name:        MDIChild.h
// Purpose:     GUI impelmentation of rules table
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

#ifndef MDICHILD
#define MDICHILD

#include <wx/mdi.h>
#include <wx/splitter.h>
#include <wx/grid.h>
#include <wx/generic/gridctrl.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/clipbrd.h>
#include <set>
#include "ProjectManager.h"
#include "LogicTable.h"
#include "OpenLogicTable.h"
#include "CodeEditor.h"
#include "utilities.h"

using namespace std;

const wstring _INPUT_TABLE = L"Inputs";
const wstring _OUTPUT_TABLE = L"Outputs";
const wstring _STATUS_TABLE = L"Status";
const int INPUTS = 0;
const int OUTPUTS = 1;
const int STATUS = 2;
const wstring INPUT_COL = L"Input";
const wstring OUTPUT_COL = L"Output";
const wstring _VALUE_NAME = L"Value";
const wstring _ATTR_NAME = L"Attr";
const wstring _RULESTATUS_NAME = L"RuleStatus";
const long PINK = 12173055;
const long LIGHTPINK = 15264511;
const long LIGHTGREEN = 12779440;
const long VERYLIGHTGREEN = 15204319;
const long LIGHTBLUE = 16766906;
const long LIGHTYELLOW = 11337470;

const wstring GLOBALORS_TABLE_NAME = L"GlobalORs";
const wstring TRANSLATIONS_TABLE_NAME = L"Translations";

const wxString strChoices[2] = {_T("Input"), _T("Output")};
const wxString strChoicesRule[2] = {_T("Enabled"), _T("Disabled")};

enum
{
	CUT = wxID_CUT,
	COPY = wxID_COPY,
	PASTE = wxID_PASTE,
	INSERT_COL = 1000,
	INSERT_ROW,
	DELETE_COL,
	DELETE_ROW,
	CLEAR_CELLS,
	EDIT_CODE
};

class LogicGrid: public wxGrid
{
public:
	LogicGrid(wxWindow *parent, int orient, wxWindowID id, int type, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS,
		const wxString& name = wxPanelNameStr):
	wxGrid(parent, id, pos, size, style, name)
	{
		m_orientation = orient;
		HasChanged = false;
		m_type = type;
	}

	enum
	{
		RULES_TABLE = 0,
		GLOBAL_ORS_TABLE,
		TRANSLATIONS_TABLE
	};

	inline void FormatCell(int row, int col)
	{
		bool formatIO = false;
		bool formatAttr = false;

		this->SetReadOnly(row, col, false);

		if (m_type == RULES_TABLE)
		{
			if (m_orientation == wxHORIZONTAL)
			{
				if (col == 0)
				{
					formatIO = true;
				}
				else if (col == 1)
				{
					formatAttr = true;
				}
			}
			else
			{
				if (row == 0)
				{
					formatIO = true;
				}
				else if (row == 1)
				{
					formatAttr = true;
				}
			}

			if (formatIO)
			{
				wstring value = this->GetCellValue(row, col).wc_str();
				if (value == INPUT_COL)
				{
					this->SetCellBackgroundColour(row, col, wxColour(PINK));
				}
				else if (value.length() > 0)
				{
					this->SetCellBackgroundColour(row, col, wxColour(LIGHTGREEN));
				}
				else
				{
					this->SetReadOnly(row, col, true);
				}
			}
			else if (formatAttr)
			{
				wstring value;
				if (m_orientation == wxHORIZONTAL)
				{
					value = this->GetCellValue(row, 0);
				}
				else
				{
					value = this->GetCellValue(0, col);
				}

				if (value == INPUT_COL)
				{
					this->SetCellBackgroundColour(row, col, wxColour(LIGHTPINK));
				}
				else if (value.length() > 0)
				{
					this->SetCellBackgroundColour(row, col, wxColour(VERYLIGHTGREEN));
				}
				else
				{
					this->SetReadOnly(row, col, true);
				}
			}
			else
			{

			}
		}
		else
		{
			if (m_orientation == wxVERTICAL)
			{
				this->SetCellBackgroundColour(0, col, wxColour(LIGHTPINK));
			}
			else
			{
				this->SetCellBackgroundColour(row, 0, wxColour(LIGHTPINK));
			}
		}
	}

	inline void HighlightRule(int index)
	{
		int offset = 0;
		if (m_type == RULES_TABLE)
			offset = 2;

		if (m_orientation == wxHORIZONTAL)
		{
			this->SelectCol(index + offset, true);
		}
		else
		{
			this->SelectRow(index + offset, true);
		}
	}

	inline void FillGrid(StringTable<wstring> *inputtable, StringTable<wstring> *outputtable, StringTable<wstring> *statustable)
	{
		int io_item_offset = 0;
		int status_offset = 0;
		if (m_type == RULES_TABLE)
		{
			io_item_offset = 1;
			status_offset = 1;
		}

		int rows = 0;
		if (outputtable != NULL && statustable != NULL)
		{
			rows = inputtable->Rows() + outputtable->Rows() + statustable->Rows();
		}
		else
		{
			rows = inputtable->Rows();
		}
		int cols = inputtable->Columns();

		if (this->GetNumberCols() > 0 || this->GetNumberRows() > 0)
		{
			this->DeleteRows(0, this->GetNumberRows());
			this->DeleteCols(0, this->GetNumberCols());

			if (m_orientation == wxHORIZONTAL)
			{
				this->AppendCols(cols + io_item_offset);
				this->AppendRows(rows);
			}
			else
			{
				this->AppendCols(rows);
				this->AppendRows(cols + io_item_offset);
			}
		}
		else
		{
			if (m_orientation == wxHORIZONTAL)
				this->CreateGrid(rows, cols + io_item_offset);
			else
				this->CreateGrid(cols + io_item_offset, rows);
		}

		for (size_t j = 0; j < inputtable->Rows(); j++)
		{
			for (size_t i = 0; i < inputtable->Columns(); i++)
			{
				if (m_orientation == wxHORIZONTAL)
					this->SetCellValue(j + status_offset, i + io_item_offset, inputtable->GetItem(j, i));
				else
					this->SetCellValue(i + io_item_offset, j + status_offset, inputtable->GetItem(j, i));
			}
		}

		int startIndex = inputtable->Rows() + status_offset;

		if (outputtable != NULL)
		{
			for (size_t j = 0; j < outputtable->Rows(); j++)
			{
				for (size_t i = 0; i < outputtable->Columns(); i++)
				{
					if (m_orientation == wxHORIZONTAL)
						this->SetCellValue(j + startIndex, i + io_item_offset, outputtable->GetItem(j, i));
					else
						this->SetCellValue(i + io_item_offset, j + startIndex, outputtable->GetItem(j, i));
				}
			}
		}


		if (m_type == RULES_TABLE)
		{	//IO lables
			wxString strChoices[2] = {_T("Input"), _T("Output")};
			if (m_orientation == wxHORIZONTAL)
			{
				for (size_t labelIndex = status_offset; labelIndex < inputtable->Rows() + status_offset; labelIndex++)
				{
					this->SetCellEditor(labelIndex, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
					this->SetCellValue(labelIndex, 0, strChoices[0]);
				}
				for (size_t labelIndex = inputtable->Rows() + status_offset; labelIndex < (size_t)this->GetNumberRows(); labelIndex++)
				{
					this->SetCellEditor(labelIndex, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
					this->SetCellValue(labelIndex, 0, strChoices[1]);
				}
			}
			else
			{
				for (size_t labelIndex = status_offset; labelIndex < inputtable->Rows() + status_offset; labelIndex++)
				{
					this->SetCellEditor(0, labelIndex, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
					this->SetCellValue(0, labelIndex, strChoices[0]);
				}
				for (size_t labelIndex = inputtable->Rows() + status_offset; labelIndex < (size_t)this->GetNumberCols(); labelIndex++)
				{
					this->SetCellEditor(0, labelIndex, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
					this->SetCellValue(0, labelIndex, strChoices[1]);
				}
			}

			//rule status
			wxString strChoicesRule[2] = {_T("Enabled"), _T("Disabled")};
			if (m_orientation == wxVERTICAL)
			{
				for (int labelIndex = io_item_offset + 1; labelIndex < this->GetNumberRows(); labelIndex++)
				{
					this->SetCellEditor(labelIndex, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoicesRule), strChoicesRule));
				}
			}
			else
			{
				for (int labelIndex = io_item_offset + 1; labelIndex < this->GetNumberCols(); labelIndex++)
				{
					this->SetCellEditor(0, labelIndex, new wxGridCellChoiceEditor(WXSIZEOF(strChoicesRule), strChoicesRule));
				}
			}

			for (size_t ruleCnt = 0; ruleCnt < statustable->Columns(); ruleCnt++)
			{
				wstring status = statustable->GetItem(0, ruleCnt);
				if (m_orientation == wxVERTICAL)
				{
					this->SetCellValue(ruleCnt + io_item_offset + 1, 0, status);
				}
				else
				{
					this->SetCellValue(0, ruleCnt + io_item_offset + 1, status);
				}
			}
		}
		else if (m_type == GLOBAL_ORS_TABLE || m_type == TRANSLATIONS_TABLE)
		{
			wstring label = L"OR";
			if (m_type == TRANSLATIONS_TABLE)
				label = L"Language";
			if (m_orientation == wxVERTICAL)
			{
				for (int j = 1; j < this->GetNumberRows(); j++)
					this->SetRowLabelValue(j, UTILS::ToWString(UTILS::stringify((long)j)));
				for (int i = 0; i < this->GetNumberCols(); i++)
					this->SetColMinimalWidth(i, 100);
				this->SetRowLabelValue(0, label);
			}
			else
			{
				for (int i = 1; i < this->GetNumberCols(); i++)
					this->SetColLabelValue(i, UTILS::ToWString(UTILS::stringify((long)i)));
				this->SetColLabelValue(0, label);
				this->SetColMinimalWidth(0, 100);
			}
		}

		this->AutoSizeColumns(false);

		for (int j = 0; j < this->GetNumberRows(); j++)
		{
			for (int i = 0; i < this->GetNumberCols(); i++)
			{
				this->FormatCell(j, i);
			}
		}
	}

	inline StringTable<wstring> GatherValues(int io_type = -1)
	{
		StringTable<wstring> retval;
		int i = 0, j = 0, rule_offset = 0, name_idx = 0;
		try
		{
			wstring io;
			if (m_type == RULES_TABLE)
			{
				rule_offset = 2;
				name_idx = 1;

				if (io_type == INPUTS)
				{
					io = L"Input";
					retval.TableName = _INPUT_TABLE;
				}
				else if (io_type == OUTPUTS)
				{
					io = L"Output";
					retval.TableName = _OUTPUT_TABLE;
				}
				else if (io_type == STATUS)
				{
					retval.TableName = _STATUS_TABLE;
					rule_offset = 0;
				}

				if (io_type != STATUS) retval.AddColumn(L"IO");
			}
			else if (m_type == GLOBAL_ORS_TABLE)
			{
				retval.TableName = GLOBALORS_TABLE_NAME;
			}
			else if (m_type == TRANSLATIONS_TABLE)
			{
				retval.TableName = TRANSLATIONS_TABLE_NAME;
			}

			if (io_type != STATUS) retval.AddColumn(_ATTR_NAME);
			if (m_orientation == wxHORIZONTAL)
			{
				for (int col_cnt = 0; col_cnt < this->GetNumberCols() - rule_offset; col_cnt++)
				{
					if (io_type != STATUS)
						retval.AddColumn(_VALUE_NAME);
					else
						retval.AddColumn(_RULESTATUS_NAME);
				}
				for (int row_cnt = 0; row_cnt < this->GetNumberRows(); row_cnt++)
					retval.AddRow();
			}
			else
			{
				for (int col_cnt = 0; col_cnt < this->GetNumberRows() - rule_offset; col_cnt++)
				{
					if (io_type != STATUS)
						retval.AddColumn(_VALUE_NAME);
					else
						retval.AddColumn(_RULESTATUS_NAME);
				}
				for (int row_cnt = 0; row_cnt < this->GetNumberCols(); row_cnt++)
					retval.AddRow();
			}

			if (io_type == STATUS)
			{
				int max_row = 1; int max_col = 1;
				if (m_orientation == wxHORIZONTAL)
				{
					max_col = this->GetNumberCols();
				}
				else
				{
					max_row = this->GetNumberRows();
				}

				for (j = 0; j < max_row; j++)
				{
					for (i = 0; i < max_col; i++)
					{
						wstring value = this->GetCellValue(j, i).wc_str();
						if (m_orientation == wxHORIZONTAL)
						{
							retval.SetItem(j, i, value);
						}
						else
						{
							retval.SetItem(i, j, value);
						}
					}
				}
			}
			else
			{
				for (j = 0; j < this->GetNumberRows(); j++)
				{
					wstring test_io;
					for (i = 0; i < this->GetNumberCols(); i++)
					{
						if (m_orientation == wxHORIZONTAL)
						{
							test_io = this->GetCellValue(j, 0);
						}
						else
						{
							test_io = this->GetCellValue(0, i);
						}

						if (test_io == io || m_type != RULES_TABLE)
						{
							wstring value = this->GetCellValue(j, i).wc_str();
							if (m_orientation == wxHORIZONTAL)
							{
								retval.SetItem(j, i, value);
							}
							else
							{
								retval.SetItem(i, j, value);
							}
						}
					}
				}
			}

			//clean up the data
			//remove empty rules
			set<size_t> rowIndexesToRemove;
			for (j = 0; j < this->GetNumberRows(); j++)
			{
				for (i = 0; i < this->GetNumberCols(); i++)
				{
					wstring attrName, test_io;
					if (m_orientation == wxHORIZONTAL)
						attrName = this->GetCellValue(j, name_idx);
					else
						attrName = this->GetCellValue(name_idx, i);

					if (m_type == RULES_TABLE)
					{
						if (m_orientation == wxHORIZONTAL)
						{
							test_io = this->GetCellValue(j, 0);
						}
						else
						{
							test_io = this->GetCellValue(0, i);
						}
					}

					if ( (attrName.length() == 0 && io_type != STATUS) || (test_io != io && m_type == RULES_TABLE))
					{
						if (m_orientation == wxHORIZONTAL)
							rowIndexesToRemove.insert(j);
						else
							rowIndexesToRemove.insert(i);
					}
				}
			}

			for (set<size_t>::reverse_iterator it = rowIndexesToRemove.rbegin(); it != rowIndexesToRemove.rend(); it++)
			{
				retval.DeleteRow(*it);
			}
			if (m_type == RULES_TABLE)
			{
				retval.RemoveColumn(0);
				if (io_type == STATUS)
					retval.RemoveColumn(0);
			}
		}
		catch(...)
		{
			ReportError("LogicGrid::GatherValues", "i=" + UTILS::stringify((long)i) + " j=" = UTILS::stringify((long)j));
		}
		return retval;
	}

	inline void SetOrientation(int orient) {m_orientation = orient;}

	inline void OnInsertCol(wxCommandEvent& event)
	{
		int lowInsertPosition, highInsertPosition, min_i = 0;
		GetSelectionRange(lowInsertPosition, highInsertPosition, true, true);
		if (m_orientation == wxHORIZONTAL)
		{
			min_i = lowInsertPosition;
		}
		else if	(m_orientation == wxVERTICAL)
		{
			min_i = m_sel_range.GetLeft();
		}
		lowInsertPosition = min_i;

		this->InsertCols(lowInsertPosition, highInsertPosition - lowInsertPosition + 1);
		for (int i = lowInsertPosition ; i <= highInsertPosition; i++)
		{
			if (m_orientation == wxVERTICAL)
			{
				this->SetCellEditor(0, i, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
				this->SetCellValue(0, i, strChoices[0]);
			}
			else
			{
				this->SetCellEditor(0, i, new wxGridCellChoiceEditor(WXSIZEOF(strChoicesRule), strChoicesRule));
				this->SetCellValue(0, i, strChoicesRule[0]);
			}

			for (int j = 0 ; j < this->GetNumberRows(); j++)
			{
				this->FormatCell(j, i);
			}
		}
		HasChanged = true;
	}

	inline void OnInsertRow(wxCommandEvent& event)
	{
		int lowInsertPosition, highInsertPosition, min_j = 0;
		GetSelectionRange(lowInsertPosition, highInsertPosition, false, true);
		if (m_orientation == wxHORIZONTAL)
		{
			min_j = m_sel_range.GetTop();
		}
		else if	(m_orientation == wxVERTICAL)
		{
			min_j = lowInsertPosition;
		}
		lowInsertPosition = min_j;

		this->InsertRows(lowInsertPosition, highInsertPosition - lowInsertPosition + 1);
		for (int j = lowInsertPosition ; j <= highInsertPosition; j++)
		{
			if (m_orientation == wxVERTICAL)
			{
				this->SetCellEditor(j, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoicesRule), strChoicesRule));
				this->SetCellValue(j, 0, strChoicesRule[0]);
			}
			else
			{
				this->SetCellEditor(j, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
				this->SetCellValue(j, 0, strChoices[0]);
			}

			for (int i = 0 ; i < this->GetNumberCols(); i++)
			{
				this->FormatCell(j, i);
			}
		}
		HasChanged = true;
	}

	inline void OnAppendRow()
	{
		this->AppendRows(1);
		int j = this->GetNumberRows() - 1;
		if (m_orientation == wxVERTICAL)
		{
			this->SetCellEditor(j, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoicesRule), strChoicesRule));
			this->SetCellValue(j, 0, strChoicesRule[0]);
		}
		else
		{
			this->SetCellEditor(j, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
			this->SetCellValue(j, 0, strChoices[0]);
		}

		for (int i = 0 ; i < this->GetNumberCols(); i++)
		{
			this->FormatCell(j, i);
		}
		HasChanged = true;
	}

	inline void OnAppendColumn()
	{
		this->AppendCols(1);
		if (m_orientation == wxHORIZONTAL)
		{
			this->SetCellEditor(this->GetNumberRows() - 1, 0, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
			this->SetCellValue(this->GetNumberRows() - 1, 0, strChoices[0]);
		}
		else if	(m_orientation == wxVERTICAL)
		{
			this->SetCellEditor(0, this->GetNumberCols() - 1, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
			this->SetCellValue(0, this->GetNumberCols() - 1, strChoices[0]);

			this->SetCellEditor(0, this->GetNumberCols() - 1, new wxGridCellChoiceEditor(WXSIZEOF(strChoicesRule), strChoicesRule));
			this->SetCellValue(0, this->GetNumberCols() - 1, strChoicesRule[0]);
		}

		for (int j = 0; j < this->GetNumberRows(); j++)
		{
			this->FormatCell(j, this->GetNumberCols() - 1);
		}
		HasChanged = true;
	}

	inline void OnDeleteCol(wxCommandEvent& event)
	{
		int lowInsertPosition, highInsertPosition;
		if(GetSelectionRange(lowInsertPosition, highInsertPosition, true, true))
			this->DeleteCols(lowInsertPosition, highInsertPosition - lowInsertPosition + 1);
	}

	inline void OnDeleteRow(wxCommandEvent& event)
	{
		int lowInsertPosition, highInsertPosition;
		if(GetSelectionRange(lowInsertPosition, highInsertPosition, false, true))
			this->DeleteRows(lowInsertPosition, highInsertPosition - lowInsertPosition + 1);
	}

	inline void OnClearCells(wxCommandEvent& event)
	{
		//dont clear the IO row or col
		int lowInsertPosition, highInsertPosition, min_j = 0, min_i = 0;
		if (m_orientation == wxHORIZONTAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, true, false);
			min_i = lowInsertPosition;
			min_j = m_sel_range.GetTop();
		}
		else if	(m_orientation == wxVERTICAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, false, false);
			min_j = lowInsertPosition;
			min_i = m_sel_range.GetLeft();
		}

		if (m_sel_range.GetWidth() > 0)
		{
			for (int j = min_j; j <= m_sel_range.GetBottom(); j++)
			{
				for (int i = min_i; i <= m_sel_range.GetRight(); i++)
				{
					this->SetCellValue(j, i, wxEmptyString);
				}
			}
		}
	}

	inline void OnCut(wxCommandEvent& event) {PlaceSelectionOnClipboard(true);}
	inline void OnCopy(wxCommandEvent& event) {PlaceSelectionOnClipboard(false);}
	inline void OnPaste(wxCommandEvent& event) {GetSelectionFromClipboard();}
	inline void OnEditCode(wxCommandEvent& event)
	{
		wstring text = GetSelectionText();
		if (text.substr(0, 3) == L"py(" ||
			text.substr(0, 3) == L"js(")
		{
			wxRect selection = m_sel_range;
			wstring prefix(text.begin(), text.begin() + 3);
			wstring code(text.begin() + 3, text.end() - 1);
			CodeEditorDialog codeEditor(this, wxID_ANY, &code);
			codeEditor.ShowModal();
			text = prefix + code + L")";
			this->SetCellValue(selection.y, selection.x, text);
		}
	}

	bool HasChanged;

private:
	inline void OnContextMenu(wxGridEvent& event)
	{
		wxMenu popupMenu;
		popupMenu.Append(CUT, _T("Cut"));
		popupMenu.Append(COPY, _T("Copy"));
		popupMenu.Append(PASTE, _T("Paste"));
		popupMenu.AppendSeparator();
		popupMenu.Append(CLEAR_CELLS, _T("Clear Cell(s)"));
		popupMenu.AppendSeparator();
		popupMenu.Append(INSERT_COL, _T("Insert Column(s)"));
		popupMenu.Append(INSERT_ROW, _T("Insert Row(s)"));
		popupMenu.AppendSeparator();
		popupMenu.Append(DELETE_COL, _T("Delete Column(s)"));
		popupMenu.Append(DELETE_ROW, _T("Delete Row(s)"));

		wstring text = GetSelectionText();
		if (text.substr(0, 3) == L"py(" ||
			text.substr(0, 3) == L"js(")
		{
			popupMenu.AppendSeparator();
			popupMenu.Append(EDIT_CODE, _T("Edit Code"));
		}

		PopupMenu(&popupMenu);
	}

	inline bool GetSelectionRange(int &min, int &max, bool isCol, bool forInsertOrDelete)
	{
		bool bHasSelection = false;

		int lowestPos = 0;
		if (m_type == RULES_TABLE)
		{
			if (forInsertOrDelete)
				lowestPos = 2;
			else
				lowestPos = 1;
		}

		int lowInsertPosition, highInsertPosition = lowestPos;
		if (isCol)
			lowInsertPosition = this->GetNumberCols();
		else
			lowInsertPosition = this->GetNumberRows();

		wxGridCellCoordsArray cells = this->GetSelectedCells();
		if (cells.size() > 0)
		{
			for (size_t i = 0; i < cells.size(); i++)
			{
				int cellPos = 0;
				if (isCol)
					cellPos = cells[i].GetCol();
				else
					cellPos = cells[i].GetRow();

				if (cellPos > lowestPos && cellPos < lowInsertPosition)
					lowInsertPosition = cellPos;
				if (cellPos > highInsertPosition)
					highInsertPosition = cellPos;
			}
			bHasSelection = true;
		}
		else if ((isCol == true && m_sel_range.GetWidth() > 0) ||
				(isCol == false && m_sel_range.GetHeight() > 0))
		{
			int cellPos = 0;
			if (isCol)
				cellPos = m_sel_range.GetX();
			else
				cellPos = m_sel_range.GetY();

			if (cellPos > lowestPos && cellPos < lowInsertPosition)
				lowInsertPosition = cellPos;
			else if (cellPos <= lowestPos)
				lowInsertPosition = lowestPos;

			if (isCol)
				highInsertPosition = m_sel_range.GetX() + m_sel_range.GetWidth() - 1;
			else
				highInsertPosition = m_sel_range.GetY() + m_sel_range.GetHeight() - 1;

			bHasSelection = true;
		}

		min = lowInsertPosition;
		max = highInsertPosition;
		return bHasSelection;
	}

	inline void OnSelectCell( wxGridEvent& event )
	{
		if (event.Selecting())
		{
			wxRect range(event.GetCol(), event.GetRow(), 1, 1);
			m_sel_range = range;
		}
		else
		{
			wxRect range(0,0,0,0);
			m_sel_range = range;
		}

		event.Skip();
	}

	inline void OnRangeSelected( wxGridRangeSelectEvent& event )
	{
		wxString logBuf;
		event.Skip();
		if (event.Selecting())
		{
			wxRect range(wxPoint(event.GetLeftCol(), event.GetTopRow()),
				wxPoint(event.GetRightCol(), event.GetBottomRow()));
			m_sel_range = range;
		}
		else
		{
			wxRect range(0,0,0,0);
			m_sel_range = range;
		}
	}

	inline void OnCellChange(wxGridEvent& event)
	{
		event.Skip();

		UpdateCellFormat(event.GetRow(), event.GetCol());

		HasChanged = true;
		this->Refresh();
	}

	inline void UpdateCellFormat(int row, int col)
	{
		FormatCell(row, col);
		if (m_orientation == wxHORIZONTAL && col == 0)
		{
			FormatCell(row, col + 1);
		}
		else if (m_orientation == wxVERTICAL && row == 0)
		{
			FormatCell(row + 1, col);
		}
	}

	inline wstring GetSelectionText()
	{
		wstring retval;
		int lowInsertPosition, highInsertPosition, min_j = 0, min_i = 0;
		if (m_orientation == wxHORIZONTAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, true, false);
			min_i = lowInsertPosition;
			min_j = m_sel_range.GetTop();
		}
		else if	(m_orientation == wxVERTICAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, false, false);
			min_j = lowInsertPosition;
			min_i = m_sel_range.GetLeft();
		}

		if (m_sel_range.GetWidth() > 1 || m_sel_range.GetHeight() > 1)
		{
			for (int j = min_j; j <= m_sel_range.GetBottom(); j++)
			{
				wstring rowText;
				if (retval.length() > 0)
					retval += L"\n";
				for (int i = min_i; i <= m_sel_range.GetRight(); i++)
				{
					if (rowText.length() > 0)
						retval += L"\v";
					rowText += this->GetCellValue(j, i);
				}
				retval += rowText;
			}
		}
		else if (m_sel_range.GetWidth() == 1 && m_sel_range.GetHeight() == 1)
		{
			retval = this->GetCellValue(min_j, min_i);
		}
		return retval;
	}

	inline void PlaceSelectionOnClipboard(bool bCut = false)
	{
		wstring text = GetSelectionText();
        wxCommandEvent dummyevent;
		if (bCut)
			OnClearCells(dummyevent);

		//place the result in the clipboard
		 wxClipboard clip;
		 clip.Open();
		 clip.Clear();
		 clip.SetData(new wxTextDataObject(text.c_str()));
		 clip.Flush();
		 clip.Close();
	}

	inline void GetSelectionFromClipboard()
	{
		//get the top corner of selection
		int lowInsertPosition, highInsertPosition;
		if (m_orientation == wxHORIZONTAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, true, false);
		}
		else if	(m_orientation == wxVERTICAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, false, false);
		}

		if (m_sel_range.GetWidth() == 0)
			return;

		int j_offset = m_sel_range.GetTop();
		int i_offset = m_sel_range.GetLeft();

		wxClipboard clip;
		if (clip.Open())
		{
			if (clip.IsSupported(wxDF_TEXT))
			{
				wxTextDataObject data;
				clip.GetData(data);
				wstring val = data.GetText().wc_str();
				if (val.length() > 0)
				{
					vector<wstring> rows = UTILS::Split(val, L"\n");
					//expand table if rows too wide
					while (rows.size() > (size_t)GetNumberRows() - j_offset)
						OnAppendRow();
					for (size_t j = 0; j < rows.size(); j++)
					{
						vector<wstring> cells = UTILS::Split(rows[j], L"\v");
						//expand table if cols too wide
						while (cells.size() > (size_t)GetNumberCols() - i_offset)
							OnAppendColumn();
						for (size_t i = 0; i < cells.size(); i++)
						{
							this->SetCellValue(j + j_offset, i + i_offset, cells[i]);
							UpdateCellFormat(j + j_offset, i + i_offset);
							HasChanged = true;
						}
					}
				}
			}
			clip.Close();
		}
	}

	int			m_orientation;
	int			m_type;
	wxRect		m_sel_range;

	DECLARE_EVENT_TABLE()
};

class MDIChild: public wxMDIChildFrame
{
public:
	MDIChild(wxMDIParentFrame *parent, int orient, int type, vector<OpenLogicTable> *open_tables, LogicTable logic,
		ProjectManager *pm, const wxString& title, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, const long style = 0):
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

		sizer = new wxBoxSizer(wxHORIZONTAL);
		m_table = new LogicGrid(this, m_orientation, -1, type, wxDefaultPosition, wxDefaultSize);
		sizer->Add(m_table, wxEXPAND);

		DisplayTableData(logic);

		//be consistent with window maximizing or smaller windows
		if (m_opened_window_tracker->size() > 1)
		{
			MDIChild *sibling = (MDIChild*)m_opened_window_tracker->at(0).child_window_ptr;
			if (sibling->IsMaximized())
				this->Maximize(true);
		}
		else
			this->Maximize(true);
	}
	~MDIChild(void)
	{
	}

	inline void RotateOrientation()
	{
		StringTable<wstring> tempInput = m_table->GatherValues(INPUTS);
		StringTable<wstring> tempOutput = m_table->GatherValues(OUTPUTS);
		StringTable<wstring> tempStatus = m_table->GatherValues(STATUS);

		int temp = m_orientation_opposite;
		m_orientation_opposite = m_orientation;
		m_orientation = temp;
		m_table->SetOrientation(m_orientation);

		this->Show(false);
		m_table->FillGrid(&tempInput, &tempOutput, &tempStatus);
		this->Show(true);
		this->Refresh();
	}

	inline void DisplayTableData(LogicTable table)
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
			if (m_type == 0 && inputTable->Columns() != outputTable->Columns())
				throw "Tables do not have the same number of columns";

			m_table->FillGrid(inputTable, outputTable, statusTable);
		}
		catch(...)
		{
			ReportError("MDIChild::DisplayTableData");
		}
	}

	inline void RepopulateTranslationsTable(set<wstring> *strings)
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

		m_table->FillGrid(&existingData, NULL, NULL);

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

	inline void InsertCol(wxCommandEvent& event) {m_table->OnInsertCol(event);}
	inline void InsertRow(wxCommandEvent& event) {m_table->OnInsertRow(event);}
	inline void AppendRow() {m_table->OnAppendRow();}
	inline void AppendColumn() {m_table->OnAppendColumn();}
	inline void DeleteCol(wxCommandEvent& event) {m_table->OnDeleteCol(event);}
	inline void DeleteRow(wxCommandEvent& event) {m_table->OnDeleteRow(event);}
	inline void ClearCells(wxCommandEvent& event) {m_table->OnClearCells(event);}
	inline void Cut(wxCommandEvent& event) {return m_table->OnCut(event);}
	inline void Copy(wxCommandEvent& event) {return m_table->OnCopy(event);}
	inline void Paste(wxCommandEvent& event) {return m_table->OnPaste(event);}
	inline void EditCode(wxCommandEvent& event) {return m_table->OnEditCode(event);}

	inline void HighlightRule(int rule) {m_table->HighlightRule(rule);}
	inline bool HasChanged() {return m_table->HasChanged;}
	inline OpenLogicTable Save()
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

		DataSet<wstring> *ds = table.logic_table.GetDataSet();
		ds->Clear();
		ds->AddTable(inputTable);
		ds->AddTable(outputTable);
		ds->AddTable(statusTable);
		m_table->HasChanged = false;

		return table;
	}

	inline wxPoint FindText(wstring textToFind, wxPoint startPos, bool bMatchCase, bool bMatchWholeWord)
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

	inline wxPoint FindAndReplaceText(wstring textToFind, wxPoint startPos, bool bMatchCase, bool bMatchWholeWord, wstring textToReplace = L"")
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
			m_table->HasChanged = true;
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
			m_table->HasChanged = true;
		}

		return retval;
	}

	inline wxPoint GetNextCellPosition(wxPoint pos)
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

	inline wxPoint GetPreviousCellPosition(wxPoint pos)
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

	inline vector<size_t> GetDisabledRules()
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

private:


	inline void OnActivate(wxActivateEvent& event) {}
	inline void OnChildClose(wxCloseEvent& event)
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
				if (!m_pm->SaveDataSet(newData.logic_table.Name, newData.logic_table.Path, newData.logic_table.GetDataSet()))
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
		event.Skip();
	}

	inline bool TestCellTextMatch(int row, int col, wxString find, bool bMatchCase, bool bMatchWholeWord)
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


	LogicGrid				*m_table;
	ProjectManager			*m_pm;
	wxBoxSizer				*sizer;
	wxSplitterWindow		*m_splitter;
	vector<OpenLogicTable>	*m_opened_window_tracker;
	int						m_orientation, m_orientation_opposite;
	int						m_type;

	DECLARE_EVENT_TABLE()
};


#endif
