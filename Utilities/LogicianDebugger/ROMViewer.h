/*!
 * A control for exploring ROMNode content.
 */

#ifndef _WX_ROMVIEWER_H_BASE_
#define _WX_ROMVIEWER_H_BASE_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include <wx/treectrl.h>
#include <wx/radiobox.h>
#include <wx/grid.h>
#include <wx/textctrl.h>
#include <wx/splitter.h>
#include "StringTable.h"



#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ROMNode.h"
using namespace ROM;

enum 
{
	TREE = 100,
	RBMODE,
	GET,
	SET,
	QUERY,
	REFRESH
};

class ROMData : public wxTreeItemData
{
public:
	ROMData(){}
	ROMData(ROMNode* node){m_node = node;}

	virtual ROMNode* GetNode() { return m_node; }
	virtual ~ROMData() {}	
protected:	
	ROMNode* m_node;
};

class wxROMViewerCtrl : public wxPanel
{
	DECLARE_DYNAMIC_CLASS(wxROMViewerCtrl)		
    DECLARE_EVENT_TABLE()

public:
    // Constructors
    wxROMViewerCtrl() { Init(); }
	virtual ~wxROMViewerCtrl() {}

    wxROMViewerCtrl(ROMNode* node, wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize):
	wxPanel(parent, id, pos, size)
    {
        Init();
        Create(parent, id, pos, size);
		m_rootNode = node;
		defaultSize = size;
		Update();
    }

    // Creation
    bool Create(wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize)
	{
		bLoading = true;
		wxBoxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

		m_splitter = new wxSplitterWindow(this, wxID_ANY, pos, size);
		m_splitter->SetSashGravity(0.0);

		wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
		wxPanel *left = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxSize(100, 100));		

		rightSizer = new wxBoxSizer(wxVERTICAL);
		wxPanel *right = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxSize(400, 100));

		m_tree = new wxTreeCtrl(left, TREE, wxDefaultPosition, wxSize(400, 100)); 
		leftSizer->Add(m_tree, 1, wxEXPAND|wxSHRINK);
		left->SetSizer(leftSizer);
		

		wxBoxSizer *attrSizer = new wxBoxSizer(wxHORIZONTAL);
		wxButton *buttonGet = new wxButton(right, GET, L"Get Attribute");
		wxButton *buttonSet = new wxButton(right, SET, L"Set Attribute");
		m_txtName = new wxTextCtrl(right, wxID_ANY, L"name");
		m_txtValue = new wxTextCtrl(right, wxID_ANY, L"value");
		m_checkImmediate = new wxCheckBox(right, wxID_ANY, L"Immediate");
		attrSizer->Add(buttonGet);
		attrSizer->Add(buttonSet);
		attrSizer->Add(m_txtName, 2, wxEXPAND|wxSHRINK);
		attrSizer->Add(m_txtValue, 3, wxEXPAND|wxSHRINK);
		attrSizer->Add(m_checkImmediate, 0, wxCENTER);
		rightSizer->Add(attrSizer, 0, wxTOP|wxLEFT);

		wxBoxSizer *quereySizer = new wxBoxSizer(wxHORIZONTAL);
		wxButton *buttonQuery = new wxButton(right, QUERY, L"XPATH Query");
		m_txtQuery = new wxTextCtrl(right, wxID_ANY);
		m_checkQueryValue = new wxCheckBox(right, wxID_ANY, L"Query is for object(s)");
		quereySizer->Add(buttonQuery, 2);
		quereySizer->Add(m_txtQuery, 6, wxEXPAND|wxSHRINK);
		quereySizer->Add(m_checkQueryValue, 0, wxCENTER);
		rightSizer->Add(quereySizer, 0);

		wxBoxSizer *refreshSizer = new wxBoxSizer(wxHORIZONTAL);
		wxButton *btnRefresh = new wxButton(right, REFRESH, L"Refresh");
		wxString choicesView[2] = {_("Attribute"), _("XML")};
		m_rbMode = new wxRadioBox(right, RBMODE, _("View Mode"), wxDefaultPosition, wxSize(150, 50),
			2, choicesView);
		refreshSizer->Add(btnRefresh, 0);
		refreshSizer->Add(m_rbMode, 0);
		rightSizer->Add(refreshSizer, 0);

		m_grid = new wxGrid(right, wxID_ANY, wxDefaultPosition, wxSize(400, 100));
		m_grid->CreateGrid(1,1);
		rightSizer->Add(m_grid, 1, wxEXPAND|wxSHRINK);
		rightSizer->Show(m_grid);
		
		m_text = new wxTextCtrl(right, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 100), wxTE_MULTILINE | wxHSCROLL | wxTE_RICH);
		rightSizer->Add(m_text, 1, wxEXPAND|wxALL);
		rightSizer->Hide(m_text);

		rightSizer->Layout();
		right->SetSizer(rightSizer);		

		m_splitter->SplitVertically(left, right, 150);

		sizerTop->Add(m_splitter, 1, wxEXPAND|wxSHRINK);

		this->SetSizer(sizerTop);
		sizerTop->SetSizeHints(parent);
		sizerTop->Fit(parent);

		bLoading = false;
		return true;
	}

    // Common initialization
    void Init() { m_rootNode = NULL; bLoading = false;}

    // Overrides
    wxSize DoGetBestSize() const { return defaultSize; }
	void OnSize(wxSizeEvent& event)
	{
		wxSize size = GetClientSize();
		m_splitter->SetSize(size.GetWidth(), size.GetHeight());
		event.Skip();
	}

    // Accessors
    void SetROMData(ROMNode* rootNode) { m_rootNode = rootNode; };
    ROMNode* GetROMData() { return m_rootNode; };

	//display the data
	void AddBranchAndChildren(wxTreeItemId parent, ROMNode* node)
	{
		wxString caption = node->GetROMObjectID().c_str();
		ROMData* data = new ROMData(node);
		wxTreeItemId current;

		if (parent.IsOk())
		{
			current = m_tree->AppendItem(parent, caption, -1, -1, data);
		}

		vector<ROMNode*> children = node->GetAllChildren(false);
		for (size_t i = 0; i < children.size(); i++)
		{
			AddBranchAndChildren(current, children[i]);
		}
	}

	void BuildTree()
	{
		m_tree->DeleteAllItems();
		wxString caption = m_rootNode->GetROMObjectID().c_str();
		ROMData* data = new ROMData(m_rootNode);
		m_tree->AddRoot(caption, -1, -1, data);
		
		vector<ROMNode*> children = m_rootNode->GetAllChildren(false);
		for (size_t i = 0; i < children.size(); i++)
		{
			AddBranchAndChildren(m_tree->GetRootItem(), children[i]);
		}
		m_tree->Update();
	}

	void Update()
	{
		bLoading = true;
		ROMNode *node = NULL;
		wxTreeItemId item = m_tree->GetSelection();
		if (item.IsOk())
		{
			ROMData *data = (ROMData*)m_tree->GetItemData(item);
			if (data)
			{
				node = data->GetNode();
			}
		}

		BuildTree();
		item = NULL;
		if (node)
		{
			item = FindItem(m_tree->GetRootItem(), node);
		}

		if (item.IsOk())
		{
			m_tree->SelectItem(item);
			DisplayAttributeGrid(item);
		}
		else
		{
			wxTreeItemId root = m_tree->GetRootItem();
			if (root.IsOk())
			{
				m_tree->SelectItem(root);
				DisplayAttributeGrid(root);
			}
		}

		bLoading = false;
	}

	void FillGrid(StringTable<wstring> table)
	{
		//grow or shrink the grid to match
		while (m_grid->GetNumberCols() < table.Columns<wstring>())
		{
			m_grid->AppendCols(1);
		}
		while (m_grid->GetNumberCols() > table.Columns<wstring>())
		{
			m_grid->DeleteCols(m_grid->GetNumberCols() - 1, 1);
		}

		while (m_grid->GetNumberRows() < table.Rows<wstring>())
		{
			m_grid->AppendRows(1);
		}
		while (m_grid->GetNumberRows() > table.Rows<wstring>())
		{
			m_grid->DeleteRows(m_grid->GetNumberRows() - 1, 1);
		}

		for (size_t i = 0; i < table.Columns<wstring>(); i++)
		{
			wstring headerText = L"attr";
			if (i > 1)
				headerText = L"sub-value";
			else if (i == 1)
				headerText = L"value";
				
			m_grid->SetColLabelValue(i, headerText);
			for (size_t j = 0; j < table.Rows<wstring>(); j++)
			{
				m_grid->SetCellValue(j, i, table.GetItem<wstring>(j, i));
			}
		}
	}

	void DisplayAttributeGrid(wxTreeItemId item)
	{
		m_grid->ClearGrid();
		if (m_grid->GetNumberRows() > 0)
			m_grid->DeleteRows(0, m_grid->GetNumberRows());
		if (m_grid->GetNumberCols() > 0)
			m_grid->DeleteCols(0, m_grid->GetNumberCols());		
		if (item.IsOk())
		{
			ROMData* data = (ROMData*)m_tree->GetItemData(item);
			if (data != NULL)
			{
				//build the table
				StringTable<wstring> table;
				ROMNode* node = data->GetNode();
				if (node != NULL)
				{
					FASTMAP_MAPS allAttrs = node->GetAllAttributes();
					size_t rowIndex = 0;
					for (FASTMAP_MAPS::iterator itAttrs = allAttrs.begin(); itAttrs != allAttrs.end(); itAttrs++)
					{
						size_t colIndex = 0;
						wstring attrName = itAttrs->first;
						FASTMAP attrValuePairs = itAttrs->second;
						if (table.Columns<wstring>() <= colIndex)
						{
							table.InsertColumn(attrName, colIndex);
						}
						table.AddRow<wstring>();
						table.SetItem<wstring>(rowIndex, colIndex, attrName);
						colIndex++;
						if (table.Columns<wstring>() <= colIndex)
						{
							table.InsertColumn<wstring>(L"value", colIndex);
						}
						table.SetItem<wstring>(rowIndex, colIndex, attrValuePairs[L"value"]);

						for (FASTMAP::iterator itAttrValuePair = attrValuePairs.begin(); itAttrValuePair != attrValuePairs.end(); itAttrValuePair++)
						{							
							wstring valueName = itAttrValuePair->first;
							wstring cellText;
							if (valueName != L"value")
							{
								colIndex++;
								cellText = valueName + L": ";							
								cellText += itAttrValuePair->second;
								if (table.Columns<wstring>() <= colIndex)
								{
									table.InsertColumn<wstring>(L"sub-value", colIndex);
								}
								table.SetItem<wstring>(rowIndex, colIndex, cellText);
							}
						}
						rowIndex++;
					}
				}				
				FillGrid(table);
			}	
		}
		m_grid->AutoSizeColumns(false);
	}	

	void DisplayXML(wxTreeItemId item)
	{
		m_text->Clear();
		if (item.IsOk())
		{
			ROMData* data = (ROMData*)m_tree->GetItemData(item);
			ROMNode* node = data->GetNode();
			wstring xml = node->SaveXML(true);
			m_text->WriteText(xml);
		}
	}	

	//events
	void TreeItemSelected(wxTreeEvent& event)
	{
		if (bLoading)
			return;
		wxTreeItemId item = m_tree->GetSelection();
		if (item.IsOk())
		{
			if (m_rbMode->GetSelection() == 0)
			{
				DisplayAttributeGrid(item);
			}
			else
			{
				DisplayXML(item);
			}
		}
	}

	void OnModeChange(wxCommandEvent& event)
	{
		if (bLoading)
			return;
		wxTreeItemId item = m_tree->GetSelection();
		if (m_rbMode->GetSelection() == 0)
		{
			rightSizer->Show(m_grid);
			rightSizer->Hide(m_text);
			DisplayAttributeGrid(item);
		}
		else
		{
			rightSizer->Hide(m_grid);
			rightSizer->Show(m_text);
			DisplayXML(item);
		}
		rightSizer->Layout();
	}

	void OnGetAttribute(wxCommandEvent& event)
	{
		wstring attrName = m_txtName->GetValue();
		if (attrName.length() > 0)
		{
			wxTreeItemId item = m_tree->GetSelection();
			if (item.IsOk())
			{
				ROMData *data = (ROMData*)m_tree->GetItemData(item);
				ROMNode *node = data->GetNode();
				if (node)
				{
					wstring subAttrName = L"value";
					size_t index = attrName.find(L":");
					if (index != string::npos)
					{
						subAttrName = attrName.substr(index + 1);
						attrName = attrName.substr(0, index);
					}
					bool immediate = m_checkImmediate->GetValue();
					wstring value = node->GetAttribute(attrName, subAttrName, immediate);
					m_txtValue->SetValue(value);
				}
			}
		}
	}

	void OnSetAttribute(wxCommandEvent& event)
	{
		wstring attrName = m_txtName->GetValue();
		wstring attrValue = m_txtValue->GetValue();
		if (attrName.length() > 0)
		{
			wxTreeItemId item = m_tree->GetSelection();
			if (item.IsOk())
			{
				ROMData *data = (ROMData*)m_tree->GetItemData(item);
				ROMNode *node = data->GetNode();
				if (node)
				{
					wstring subAttrName = L"value";
					size_t index = attrName.find(L":");
					if (index != string::npos)
					{
						subAttrName = attrName.substr(index + 1);
						attrName = attrName.substr(0, index);
					}
					node->SetAttribute(attrName, subAttrName, attrValue);
					if (m_rbMode->GetSelection() == 0)
					{
						DisplayAttributeGrid(item);
					}
					else
					{
						DisplayXML(item);
					}
				}
			}
		}
	}

	void OnQuery(wxCommandEvent& event)
	{
		wstring query = m_txtQuery->GetValue();
		if (query.length() > 0)
		{
			wxTreeItemId item = m_tree->GetSelection();
			if (item.IsOk())
			{
				ROMData *data = (ROMData*)m_tree->GetItemData(item);
				ROMNode *node = data->GetNode();
				if (node)
				{
					try
					{
						if (m_checkQueryValue->GetValue()) //rule table input type attr query
						{
							vector<ROMNode*> results = node->FindObjects(query);						
							wchar_t buff[32] = L"";
							swprintf(buff, 32, L"%d", results.size());
							wstring value = buff;
							value += L" Objects Found";
							wxMessageBox(value, L"XPATH Result"); 

							//highlight the objects in the tree
							for (vector<ROMNode*>::iterator it = results.begin(); it != results.end(); it++)
							{
								wxTreeItemId item = FindItem(m_tree->GetRootItem(), *it);
								if (item.IsOk())
								{
									m_tree->SelectItem(item);
									m_tree->SetItemTextColour(item, wxColor(L"red"));
								}
							}
						}
						else
						{
							wstring value = L"value: ";
							value += node->EvaluateXPATH(query);
							wxMessageBox(value, L"XPATH Result"); 
						}
					}
					catch(...)
					{
						wxMessageBox(L"Query error", L"Error");
					}
				}
			}
		}
	}

	void OnRefresh(wxCommandEvent& event)
	{
		Update();
	}

	wxTreeItemId FindItem(wxTreeItemId root, ROMNode* nodeToSeachFor)
	{
		wxTreeItemId item=root, child;
		wxTreeItemIdValue cookie;
		bool bFound;

		while(item.IsOk())
		{
			ROMData* data = (ROMData*)m_tree->GetItemData(item);
			ROMNode* node = data->GetNode();
			bFound = node->GetROMGUID() == nodeToSeachFor->GetROMGUID();
			if(bFound) return item;
			child = m_tree->GetFirstChild(item, cookie);
			if(child.IsOk()) child = FindItem(child, nodeToSeachFor);
			if(child.IsOk()) return child;
			item = m_tree->GetNextSibling(item);
		}

		return item;
	}

protected:
    ROMNode		*m_rootNode;
	wxSplitterWindow *m_splitter;
	wxTreeCtrl	*m_tree;
	wxGrid		*m_grid;
	wxTextCtrl	*m_text;
	wxRadioBox	*m_rbMode;
	wxBoxSizer	*rightSizer;
	wxTextCtrl	*m_txtName, *m_txtValue;
	wxCheckBox	*m_checkImmediate, *m_checkQueryValue;
	wxTextCtrl	*m_txtQuery;
	bool		bLoading;
	wxSize		defaultSize;
};

BEGIN_EVENT_TABLE(wxROMViewerCtrl, wxControl)
	EVT_TREE_SEL_CHANGED(TREE, wxROMViewerCtrl::TreeItemSelected)
	EVT_SIZE (wxROMViewerCtrl::OnSize)
	EVT_RADIOBOX (RBMODE, wxROMViewerCtrl::OnModeChange)
	EVT_BUTTON (GET, wxROMViewerCtrl::OnGetAttribute)
	EVT_BUTTON (SET, wxROMViewerCtrl::OnSetAttribute)
	EVT_BUTTON (QUERY, wxROMViewerCtrl::OnQuery)
	EVT_BUTTON (REFRESH, wxROMViewerCtrl::OnRefresh)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxROMViewerCtrl, wxControl)
#endif