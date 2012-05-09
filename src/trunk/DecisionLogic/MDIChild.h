/////////////////////////////////////////////////////////////////////////////
// Name:        MDIChild.h
// Purpose:     GUI impelmentation of rules table
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
// Copyright:   (c) 2009 - 2012 Eric D. Schmidt, DigiRule Solutions LLC
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

#ifndef MDICHILD
#define MDICHILD

#include "stdafx.h"
#include <wx/mdi.h>
#include <wx/splitter.h>
#include <wx/grid.h>
#include <wx/generic/gridctrl.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/clipbrd.h>
#include <set>
#include <stack>
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
const long DARKGREEN = 4227072;
const long VERYLIGHTGREEN = 15204319;
const long LIGHTBLUE = 16766906;
const long LIGHTYELLOW = 11337470;
const long DARKRED = 460702;
const long _BLACK = 000000;

const wxString strChoices[2] = {_T("Input"), _T("Output")};
const wxString strChoicesRule[2] = {_T("Enabled"), _T("Disabled")};

enum
{
	COPYPASTE = 0,
	DEL = 1,
	INSERT = 2,
	CUT = wxID_CUT,
	COPY = wxID_COPY,
	PASTE = wxID_PASTE,
	UNDO = wxID_UNDO,
	REDO = wxID_REDO,
	INSERT_COL = 1000,
	INSERT_ROW,
	DELETE_COL,
	DELETE_ROW,
	CLEAR_CELLS,
	EDIT_CODE,
	JUMP,
	JUMPOR
};

class LogicGrid: public wxGrid
{
public:
	LogicGrid(wxWindow *parent, int orient, wxWindowID id, int type, void(*updateCallback)(void), bool(*ChildOpenTableCallback)(wstring), map<wstring, vector<wstring> > *gORs,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS,
		const wxString& name = wxPanelNameStr):
	wxGrid(parent, id, pos, size, style, name)
	{
		m_orientation = orient;
		HasChanged = false;
		m_type = type;
		m_updateCallback = updateCallback;
		m_OpenTableCallback = ChildOpenTableCallback;
		m_ors = gORs;
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
		bool formatDisabled = false;

		this->SetReadOnly(row, col, false);

		if (m_type == RULES_TABLE)
		{
			if (m_orientation == wxHORIZONTAL)
			{
				if (col == 0)
				{
					formatIO = true;
					if (row == 0)
						return;
				}
				else if (col == 1)
				{
					formatAttr = true;
					if (row == 0)
					{
						this->SetReadOnly(row, col, true);
						return;
					}
				}
				else if (col > 1)
				{
					if (this->GetCellValue(0, col) == L"Disabled")
						formatDisabled = true;
				}
			}
			else
			{
				if (row == 0)
				{
					formatIO = true;
					if (col == 0)
						return;
				}
				else if (row == 1)
				{
					formatAttr = true;
					if (col == 0)
					{
						this->SetReadOnly(row, col, true);
						return;
					}
				}
				else if (row > 1)
				{
					if (this->GetCellValue(row, 0) == L"Disabled")
						formatDisabled = true;
				}
			}

			//default text color, style
			wxFont cellFont = this->GetDefaultCellFont();
			cellFont.SetWeight(wxNORMAL);
			this->SetCellTextColour(row, col, wxColour(_T("BLACK")));

			if (!formatDisabled)
			{
				if (m_orientation == wxHORIZONTAL && row == 0)
				{
					//default the whole col
					for (int j = 1; j < this->GetNumberRows(); j++)
					{
						this->SetCellTextColour(j, col, wxColour(_T("BLACK")));
					}
				}
				else if (m_orientation == wxVERTICAL && col == 0)
				{
					//default the whole row
					for (int i = 1; i < this->GetNumberCols(); i++)
					{
						this->SetCellTextColour(row, i, wxColour(_T("BLACK")));
					}
				}
			}

			if (formatDisabled)
			{
				this->SetCellTextColour(row, col, wxColour(DARKGREEN));
				if (m_orientation == wxHORIZONTAL && row == 0)
				{
					cellFont.SetWeight(wxBOLD);
					//do the whole col
					for (int j = 1; j < this->GetNumberRows(); j++)
					{
						this->SetCellTextColour(j, col, wxColour(DARKGREEN));
					}
				}
				else if (m_orientation == wxVERTICAL && col == 0)
				{
					cellFont.SetWeight(wxBOLD);
					//do the whole row
					for (int i = 1; i < this->GetNumberCols(); i++)
					{
						this->SetCellTextColour(row, i, wxColour(DARKGREEN));
					}
				}
			}
			else if (formatIO)
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
				//check for or
				wstring value = this->GetCellValue(row, col).wc_str();
				if (m_ors != NULL && m_ors->size() > 0 && m_ors->find(value) != m_ors->end())
				{
					cellFont.SetWeight(wxBOLD);
					this->SetCellTextColour(row, col, wxColour(DARKRED));
				}
			}

			this->SetCellFont(row, col, cellFont);
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

		if (row == 0 && col == 0)
			this->SetReadOnly(row, col, false);
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

	inline void FillGrid(StringTable<wstring> table)
	{
		//grow or shrink the grid to match
		while (this->GetNumberCols() < table.Columns())
		{
			this->AppendCols(1);
		}
		while (this->GetNumberCols() > table.Columns())
		{
			this->DeleteCols(this->GetNumberCols() - 1, 1);
		}

		while (this->GetNumberRows() < table.Rows())
		{
			this->AppendRows(1);
		}
		while (this->GetNumberRows() > table.Rows())
		{
			this->DeleteRows(this->GetNumberRows() - 1, 1);
		}

		for (size_t i = 0; i < table.Columns(); i++)
		{
			for (size_t j = 0; j < table.Rows(); j++)
			{
				this->SetCellValue(j, i, table.GetItem(j, i));
				UpdateCellFormat(j, i);
			}
		}
	}

	inline StringTable<wstring> GetRawTableData()
	{
		StringTable<wstring> retval;

		size_t col = 0;
		while (retval.Columns() < this->GetNumberCols())
		{
			retval.AddColumn((wstring)this->GetColLabelValue(col));
			col++;
		}

		while (retval.Rows() < this->GetNumberRows())
			retval.AddRow();

		for (size_t i = 0; i < this->GetNumberCols(); i++)
		{
			for (size_t j = 0; j < this->GetNumberRows(); j++)
			{
				retval.SetItem(j, i, (wstring)this->GetCellValue(j, i));
			}
		}

		return retval;
	}

	inline void FillGrid(StringTable<wstring> *inputtable, StringTable<wstring> *outputtable, StringTable<wstring> *statustable, bool bGetAll)
	{
		int io_item_offset = 0;
		int status_offset = 0;
		if (m_type == RULES_TABLE)
		{
			io_item_offset = 1;
			status_offset = 1;
		}

		int rows = 0, cols = 0;
		if (m_type == RULES_TABLE && outputtable != NULL && statustable != NULL)
		{
			rows = inputtable->Rows() + outputtable->Rows() + statustable->Rows();
			cols = outputtable->Columns();
		}
		else
		{
			rows = inputtable->Rows();
			cols = inputtable->Columns();
		}

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

		if (m_type == RULES_TABLE)
		{
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

			this->SetCellRenderer(0, 0, new wxGridCellBoolRenderer());
			this->SetCellEditor(0, 0, new wxGridCellBoolEditor());
			if (bGetAll)
				this->SetCellValue(0,0,_T("1"));
			else
				this->SetCellValue(0,0,_T(""));

			//IO lables
			wxString strChoices[2] = {_T("Input"), _T("Output")};
			if (m_orientation == wxHORIZONTAL)
			{
				this->SetRowLabelValue(0, _T("Get All? / Status"));
				this->SetColLabelValue(0, _T("IO"));
				this->SetColLabelValue(1, _T("Attr"));
				this->SetRowLabelValue(1, _T("2"));
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
				this->SetRowLabelValue(0, _T("Get All? / IO"));
				this->SetRowLabelValue(1, _T("Attr"));
				this->SetColLabelValue(0, _T("Status"));
				this->SetColLabelValue(1, _T("B"));
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
				this->SetRowLabelValue(0, label);
				this->SetColLabelValue(0, _T("A"));
				for (int i = 0; i < this->GetNumberCols(); i++)
					this->SetColMinimalWidth(i, 100);
			}
			else
			{
				this->SetRowLabelValue(0, _T("1"));
				this->SetColLabelValue(0, label);
				this->SetColMinimalWidth(0, 100);
			}
		}

		this->SetColLabelSize(wxGRID_AUTOSIZE);
		this->SetRowLabelSize(wxGRID_AUTOSIZE);
		this->AutoSizeColumns(false);

		if (m_type == RULES_TABLE)
		{
			for (int i = 1; i < this->GetNumberCols(); i++)
			{
				//if the autosize made the col real skinny, bump it up some
				if (this->GetColWidth(i) < 50)
					this->SetColSize(i, 50);
			}
		}

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
				if (io_type == STATUS)
					return retval;
				retval.TableName = GLOBALORS_TABLE_NAME;
				rule_offset = 1;
			}
			else if (m_type == TRANSLATIONS_TABLE)
			{
				if (io_type == STATUS)
					return retval;
				retval.TableName = TRANSLATIONS_TABLE_NAME;
				rule_offset = 1;
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
				{
					retval.AddRow();
					if (io_type == STATUS)
						break;
				}
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
				{
					retval.AddRow();
					if (io_type == STATUS)
						break;
				}
			}

			if (m_type == RULES_TABLE && io_type == STATUS)
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
							if (i > 1)
								retval.SetItem(j, i, value);
						}
						else
						{
							if (j > 1)
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
				wstring attrName, test_io;
				if (m_orientation == wxHORIZONTAL)
					attrName = this->GetCellValue(j, name_idx);
				for (i = 0; i < this->GetNumberCols(); i++)
				{
					if (m_orientation == wxVERTICAL)
						attrName = this->GetCellValue(name_idx, i);

					if (m_type == RULES_TABLE)
					{
						if (m_orientation == wxHORIZONTAL)
						{
							if (io_type != STATUS && j > 0)
								test_io = this->GetCellValue(j, 0);
						}
						else
						{
							if (io_type != STATUS && i > 0)
								test_io = this->GetCellValue(0, i);
						}
					}

					if ((attrName.length() == 0 && io_type != STATUS) || (test_io != io && m_type == RULES_TABLE))
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
    inline void OnInsertCol(wxCommandEvent& WXUNUSED(event))
    {
        InsertCol();
    }
	inline void InsertCol()
	{
		int lowInsertPosition, highInsertPosition, min_i = 0;
		GetSelectionRange(lowInsertPosition, highInsertPosition, true, INSERT);
		if (m_orientation == wxHORIZONTAL)
		{
			min_i = lowInsertPosition;
		}
		else if	(m_orientation == wxVERTICAL)
		{
			min_i = m_sel_range.GetLeft();
			if (min_i == 0)
			{
				min_i = 1;
				highInsertPosition = 1;
			}
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
		UpdateUndo();
	}

    inline void OnInsertRow(wxCommandEvent& WXUNUSED(event))
    {
        InsertRow();
    }
	inline void InsertRow()
	{
		int lowInsertPosition, highInsertPosition, min_j = 0;
		GetSelectionRange(lowInsertPosition, highInsertPosition, false, INSERT);
		if (m_orientation == wxHORIZONTAL)
		{
			min_j = m_sel_range.GetTop();
			if (min_j == 0 && highInsertPosition == 0)
			{
				min_j = 1;
				highInsertPosition = 1;
			}
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
		UpdateUndo();
	}

	inline void OnAppendRow(bool allowUndo)
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
		if (allowUndo)
			UpdateUndo();
	}

	inline void OnAppendColumn(bool allowUndo)
	{
		this->AppendCols(1);
		int i = this->GetNumberCols() - 1;
		if (m_orientation == wxHORIZONTAL)
		{
			this->SetCellEditor(0, i, new wxGridCellChoiceEditor(WXSIZEOF(strChoicesRule), strChoicesRule));
			this->SetCellValue(0, i, strChoicesRule[0]);
		}
		else if	(m_orientation == wxVERTICAL)
		{
			this->SetCellEditor(0, i, new wxGridCellChoiceEditor(WXSIZEOF(strChoices), strChoices));
			this->SetCellValue(0, i, strChoices[1]);
		}

		for (int j = 0; j < this->GetNumberRows(); j++)
		{
			this->FormatCell(j, this->GetNumberCols() - 1);
		}
		if (allowUndo)
			UpdateUndo();
	}

    inline void OnDeleteCol(wxCommandEvent& event)
    {
        DeleteCol();
    }
	inline void DeleteCol()
	{
		int lowInsertPosition, highInsertPosition;
		if(GetSelectionRange(lowInsertPosition, highInsertPosition, true, DEL))
		{
			this->DeleteCols(lowInsertPosition, highInsertPosition - lowInsertPosition + 1);
			UpdateUndo();
		}
	}

    inline void OnDeleteRow(wxCommandEvent& event)
    {
        DeleteRow();
    }
	inline void DeleteRow()
	{
		int lowInsertPosition, highInsertPosition;
		if(GetSelectionRange(lowInsertPosition, highInsertPosition, false, DEL))
		{
			this->DeleteRows(lowInsertPosition, highInsertPosition - lowInsertPosition + 1);
			UpdateUndo();
		}
	}

    inline void OnClearCells(wxCommandEvent& event)
    {
        ClearCells();
    }
	inline void ClearCells()
	{
		//dont clear the IO row or col
		int lowInsertPosition, highInsertPosition, min_j = 0, min_i = 0;
		if (m_orientation == wxHORIZONTAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, true, DEL);
			min_i = lowInsertPosition;
			min_j = m_sel_range.GetTop();
		}
		else if	(m_orientation == wxVERTICAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, false, DEL);
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
			UpdateUndo();
		}
	}

	inline void OnCut(wxCommandEvent& event) {PlaceSelectionOnClipboard(true);}
	inline void OnCopy(wxCommandEvent& event) {PlaceSelectionOnClipboard(false);}
	inline void OnPaste(wxCommandEvent& event) {GetSelectionFromClipboard();}
	inline void Cut() {PlaceSelectionOnClipboard(true);}
	inline void Copy() {PlaceSelectionOnClipboard(false);}
	inline void Paste() {GetSelectionFromClipboard();}
	inline void OnJump(wxCommandEvent& event)
	{
		wstring text = GetSelectionText();
		if (text.length() > 0)
		{
			if (text.substr(0, 5) == L"eval(")
			{
				wstring tableName = text.substr(5, text.find(L",", 0) - 5);
				m_OpenTableCallback(tableName);
			}
			else
				m_OpenTableCallback(text);
		}
	}

	inline void OnJumpOR(wxCommandEvent& event)
	{
		m_OpenTableCallback(GLOBALORS_TABLE_NAME);
	}

    inline void OnEditCode(wxCommandEvent& event)
    {
        EditCode();
    }
	inline void EditCode()
	{
		wstring text = GetSelectionText();
		if (text.substr(0, 3) == L"py(" ||
			text.substr(0, 3) == L"js(")
		{
			wxRect selection = m_sel_range;
			wstring prefix(text.begin(), text.begin() + 3);
			wstring code(text.begin() + 3, text.end() - 1);
			wstring lang = L"C++";
			if (text.substr(0, 2) == L"py")
				lang = L"Python";
			CodeEditorDialog codeEditor(this, wxID_ANY, &code, lang);
			codeEditor.ShowModal();
			HasChanged = true;
			text = prefix + code + L")";
			this->SetCellValue(selection.y, selection.x, text);
		}
	}

	inline void OnEditorShown( wxGridEvent& event)
	{
		event.Skip();
	}

	inline void OnEditorHidden(wxGridEvent& event)
	{
		event.Skip();
	}

	inline void OnLeftClick(wxGridEvent& event)
	{
		//event.Skip();
		if (event.GetRow() == 0 && event.GetCol() == 0 && this->m_type == RULES_TABLE) // the GetAll checkbox
		{
			if (this->GetCellValue(0, 0).length() == 0)
				this->SetCellValue(0, 0, _T("1"));
			else
				this->SetCellValue(0, 0, _T(""));
			event.Veto();
			this->Refresh();
			UpdateUndo();
		}
		else
			event.Skip();
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

		bool bSep = true;
		bool bJumpTable = true;
		bool bJumpOR = false;
		bool bEditCode = false;

		if ((event.GetRow() > 1 && m_orientation == wxVERTICAL ||
			event.GetCol() > 1 && m_orientation == wxHORIZONTAL) &&
			this->GetCellTextColour(event.GetRow(), event.GetCol()) == DARKRED)
		{
			bSep = true;
			bJumpOR = true;
		}

		wstring text = GetSelectionText();
		if (text.substr(0, 3) == L"py(" ||
			text.substr(0, 3) == L"js(")
		{
			bEditCode = true;
		}
		//else if (text.substr(0, 5) == L"eval(")
		//{
		//	bSep = true;
		//	bJumpTable = true;
		//}

		if (bSep)
			popupMenu.AppendSeparator();

		if (bJumpTable)
			popupMenu.Append(JUMP, _T("Jump to Table"));

		if (bJumpOR)
			popupMenu.Append(JUMPOR, _T("Jump to OR"));

		if (bEditCode)
		{
			popupMenu.AppendSeparator();
			popupMenu.Append(EDIT_CODE, _T("Edit Code"));
		}

		PopupMenu(&popupMenu);
	}

	inline bool GetSelectionRange(int &min, int &max, bool isCol, int type)
	{
		bool bHasSelection = false;

		int lowestPos = 0;
		if (m_type == RULES_TABLE)
		{
			lowestPos = type;
		}

		int lowInsertPosition, highInsertPosition = lowestPos;
		if (isCol)
			lowInsertPosition = this->GetNumberCols();
		else
			lowInsertPosition = this->GetNumberRows();

		wxArrayInt grpSelection;
		if (isCol)
			grpSelection = this->GetSelectedCols();
		else
			grpSelection = this->GetSelectedRows();
		wxGridCellCoordsArray cells = this->GetSelectedCells();
		if (grpSelection.size() > 0)
		{
			for (size_t i = 0; i < grpSelection.size(); i++)
			{
				int cellPos = grpSelection[i];

				if (cellPos >= lowestPos && cellPos < lowInsertPosition)
					lowInsertPosition = cellPos;
				if (cellPos > highInsertPosition)
					highInsertPosition = cellPos;
			}
			bHasSelection = true;
		}
		else if (cells.size() > 0)
		{
			for (size_t i = 0; i < cells.size(); i++)
			{
				int cellPos = 0;
				if (isCol)
					cellPos = cells[i].GetCol();
				else
					cellPos = cells[i].GetRow();

				if (cellPos >= lowestPos && cellPos < lowInsertPosition)
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

			if (cellPos >= lowestPos && cellPos < lowInsertPosition)
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

		this->Refresh();
		UpdateUndo();
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
			GetSelectionRange(lowInsertPosition, highInsertPosition, true, COPYPASTE);
			min_i = lowInsertPosition;
			min_j = m_sel_range.GetTop();
		}
		else if	(m_orientation == wxVERTICAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, false, COPYPASTE);
			min_j = lowInsertPosition;
			min_i = m_sel_range.GetLeft();
		}

		if (m_sel_range.GetWidth() > 1 || m_sel_range.GetHeight() > 1)
		{
			for (int j = min_j; j <= m_sel_range.GetBottom(); j++)
			{
				wstring rowText;
				if (j - min_j > 0)
					retval += L"\n";
				for (int i = min_i; i <= m_sel_range.GetRight(); i++)
				{
					if (i - min_i > 0)
						rowText += L"\t";
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
			GetSelectionRange(lowInsertPosition, highInsertPosition, true, COPYPASTE);
		}
		else if	(m_orientation == wxVERTICAL)
		{
			GetSelectionRange(lowInsertPosition, highInsertPosition, false, COPYPASTE);
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
					if (rows.size() == 1)
					{
						vector<wstring> cells = UTILS::Split(rows[0], L"\t");
						if (cells.size() == 1)
						{
							for (size_t x = i_offset; x <= m_sel_range.GetRight(); x++)
							{
								for (size_t y = j_offset; y <= m_sel_range.GetBottom(); y++)
								{
									this->SetCellValue(y, x, rows[0]);
									UpdateCellFormat(y, x);
								}
							}
						}
						else
						{
							//expand table if cols too wide
							while (cells.size() > (size_t)GetNumberCols() - i_offset)
								OnAppendColumn(false);
							for (size_t i = 0; i < cells.size(); i++)
							{
								this->SetCellValue(j_offset, i + i_offset, cells[i]);
								UpdateCellFormat(j_offset, i + i_offset);
							}
						}
					}
					else
					{
						//expand table if rows too wide
						while (rows.size() > (size_t)GetNumberRows() - j_offset)
							OnAppendRow(false);
						for (size_t j = 0; j < rows.size(); j++)
						{
							vector<wstring> cells = UTILS::Split(rows[j], L"\t");
							//expand table if cols too wide
							while (cells.size() > (size_t)GetNumberCols() - i_offset)
								OnAppendColumn(false);
							for (size_t i = 0; i < cells.size(); i++)
							{
								this->SetCellValue(j + j_offset, i + i_offset, cells[i]);
								UpdateCellFormat(j + j_offset, i + i_offset);
							}
						}
					}
					UpdateUndo();
				}
			}
			clip.Close();
		}
	}

	inline void UpdateUndo()
	{
		m_updateCallback();
	}

	int								m_orientation;
	int								m_type;
	wxRect							m_sel_range;
	void							(*m_updateCallback)(void);
	bool							(*m_OpenTableCallback)(wstring tableName);
	map<wstring, vector<wstring> >	*m_ors;

	DECLARE_EVENT_TABLE()
};

class MDIChild: public wxMDIChildFrame
{
public:
	MDIChild(wxMDIParentFrame *parent, void(*ChildCloseCallback)(wstring), bool(*ChildOpenTableCallback)(wstring),
		bool(*ChildSaveTableCallback)(OpenLogicTable*),	int orient, int type,
		vector<OpenLogicTable> *open_tables, LogicTable logic,
		ProjectManager *pm, const wxString& title, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, const long style = 0);
	~MDIChild(void);

	void RotateOrientation();
	void DisplayTableData(LogicTable table);
	void DisplayRawTableData(StringTable<wstring> table);
	void RepopulateTranslationsTable(set<wstring> *strings);

	void InsertCol();
	void InsertRow();
	void AppendRow();
	void AppendColumn();
	void DeleteCol();
	void DeleteRow();
	void ClearCells();
	void Undo();
	void Redo();
	void Cut();
	void Copy();
	void Paste();
	void EditCode();

	void HighlightRule(int rule);
	bool HasChanged();
	OpenLogicTable Save();

	wxPoint FindText(wstring textToFind, wxPoint startPos, bool bMatchCase, bool bMatchWholeWord);
	wxPoint FindAndReplaceText(wstring textToFind, wxPoint startPos, bool bMatchCase, bool bMatchWholeWord, wstring textToReplace = L"");
	wxPoint GetNextCellPosition(wxPoint pos);
	wxPoint GetPreviousCellPosition(wxPoint pos);

	vector<size_t> GetDisabledRules();
	void SignalTableChangedCallback();

private:
	void OnActivate(wxActivateEvent& event);
	void OnChildClose(wxCloseEvent& event);
	bool TestCellTextMatch(int row, int col, wxString find, bool bMatchCase, bool bMatchWholeWord);
	OpenLogicTable GetCurrentTable();



	LogicGrid				*m_table;
	ProjectManager			*m_pm;
	wxBoxSizer				*sizer;
	wxSplitterWindow		*m_splitter;
	vector<OpenLogicTable>	*m_opened_window_tracker;
	int						m_orientation, m_orientation_opposite;
	int						m_type;
	stack<StringTable<wstring> >		stUndo;
	stack<StringTable<wstring> >		stRedo;
	void (*m_ChildClosedCallback)(wstring tableName);
	bool (*m_ChildSaveCallback)(OpenLogicTable *data);
	bool (*m_OpenTableCallback)(wstring tableName);

	DECLARE_EVENT_TABLE()
};


#endif
