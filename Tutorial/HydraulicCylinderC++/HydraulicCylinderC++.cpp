/////////////////////////////////////////////////////////////////////////////
// Name:        HydraulicCylinderC++.cpp
// Purpose:     Sample C++ Logician app
// Author:      Eric D. Schmidt
// Created:     10/1/2010
/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

#include "HydraulicCylinderC++.h"
#include <map>
#include <iostream>
#include <fstream>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#ifdef WIN32
#include <windows.h>
#endif
// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
	EVT_MENU(Minimal_Open,  MyFrame::OnOpen)
	EVT_MENU(Minimal_Save,  MyFrame::OnSave)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)

	EVT_TEXT(ComboCtrl, MyFrame::ComboEvaluate)
	EVT_TEXT(EditCtrl, MyFrame::TextChanged)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame(_T("Logician C++ Test App"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(478, 506))
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

	fileMenu->Append(Minimal_Open, _T("&Open\tAlt-O"), _T("Open a saved configuration"));
	fileMenu->Append(Minimal_Save, _T("&Save\tAlt-S"), _T("Save the current configuration"));
    fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Logician C++ Test App"));
#endif // wxUSE_STATUSBAR

	//configuration controls
	panel = new wxPanel(this, wxID_ANY);
	wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *sizerGrid = new wxFlexGridSizer(2);

	wxStaticText *stCylinderSeries = new wxStaticText(panel, LabelCtrl, _T("lbCylinderSeries"));
	sizerGrid->Add(stCylinderSeries);
	wxComboBox *comboCylinderSeries = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("CylinderSeries"));
	sizerGrid->Add(comboCylinderSeries);

	wxStaticText *stMountingStyle = new wxStaticText(panel, LabelCtrl, _T("lbMountingStyle"));
	sizerGrid->Add(stMountingStyle);
	wxComboBox *comboMountingStyle = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("MountingStyle"));
	sizerGrid->Add(comboMountingStyle);

	wxGridSizer *sizeBoreRod = new wxGridSizer(2);
	wxStaticText *stBoreDiameter = new wxStaticText(panel, LabelCtrl, _T("lbBoreDiameter"));
	wxStaticText *stRodDiameter = new wxStaticText(panel, LabelCtrl, _T("lbRodDiameter"));
	sizeBoreRod->Add(stBoreDiameter);
	sizeBoreRod->Add(stRodDiameter);
	wxComboBox *comboBoreDiameter = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("BoreDiameter"));
	wxComboBox *comboRodDiameter = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("RodDiameter"));
	sizeBoreRod->Add(comboBoreDiameter);
	sizeBoreRod->Add(comboRodDiameter);
	sizerGrid->AddSpacer(0);
	sizerGrid->Add(sizeBoreRod);

	wxStaticText *stRodEndTypes = new wxStaticText(panel, LabelCtrl, _T("lbRodEndTypes"));
	sizerGrid->Add(stRodEndTypes);
	wxComboBox *comboRodEndTypes = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("RodEndTypes"));
	sizerGrid->Add(comboRodEndTypes);

	wxStaticText *stSealingSystem = new wxStaticText(panel, LabelCtrl, _T("lbSealingSystem"));
	sizerGrid->Add(stSealingSystem);
	wxComboBox *comboSealingSystem = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("SealingSystem"));
	sizerGrid->Add(comboSealingSystem);

	wxStaticText *stPorts = new wxStaticText(panel, LabelCtrl, _T("lbPorts"));
	sizerGrid->Add(stPorts);
	wxComboBox *comboPorts = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("Ports"));
	sizerGrid->Add(comboPorts);

	wxGridSizer *sizerPort = new wxGridSizer(2);
	wxStaticText *stPortLocationHead = new wxStaticText(panel, LabelCtrl, _T("lbPortLocationHead"));
	wxStaticText *stPortLocationCap = new wxStaticText(panel, LabelCtrl, _T("lbPortLocationCap"));
	sizerPort->Add(stPortLocationHead);
	sizerPort->Add(stPortLocationCap);
	wxComboBox *comboPortLocationHead = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("PortLocationHead"));
	wxComboBox *comboPortLocationCap = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("PortLocationCap"));
	sizerPort->Add(comboPortLocationHead);
	sizerPort->Add(comboPortLocationCap);
	sizerGrid->AddSpacer(0);
	sizerGrid->Add(sizerPort);

	wxGridSizer *sizerCushion = new wxGridSizer(2);
	wxStaticText *stCushionLocationHead = new wxStaticText(panel, LabelCtrl, _T("lbCushionHead"));
	wxStaticText *stCushionLocationCap = new wxStaticText(panel, LabelCtrl, _T("lbCushionCap"));
	sizerCushion->Add(stCushionLocationHead);
	sizerCushion->Add(stCushionLocationCap);
	wxComboBox *comboCushionLocationHead = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("CushionHead"));
	wxComboBox *comboCushionLocationCap = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("CushionCap"));
	sizerCushion->Add(comboCushionLocationHead);
	sizerCushion->Add(comboCushionLocationCap);
	sizerGrid->AddStretchSpacer();
	sizerGrid->Add(sizerCushion);

	wxStaticText *stCylinderStroke = new wxStaticText(panel, LabelCtrl, _T("lbCylinderStroke"));
	sizerGrid->Add(stCylinderStroke);
	wxTextCtrl *editCylinderStroke = new wxTextCtrl(panel, EditCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, wxDefaultValidator, _T("CylinderStroke"));
	sizerGrid->Add(editCylinderStroke);

	wxStaticText *stAdditionalOption = new wxStaticText(panel, LabelCtrl, _T("lbAdditionalOption"));
	sizerGrid->Add(stAdditionalOption);
	wxComboBox *comboAdditionalOption = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("AdditionalOption"));
	sizerGrid->Add(comboAdditionalOption);

	sizerGrid->AddStretchSpacer();
	wxGridSizer *sizerOptions = new wxGridSizer(2);
	wxStaticText *stExtraRodProtection = new wxStaticText(panel, LabelCtrl, _T("lbExtraRodProtection"));
	wxTextCtrl *editExtraRodProtection = new wxTextCtrl(panel, EditCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, wxDefaultValidator, _T("ExtraRodProtection"));
	wxStaticText *stPosition = new wxStaticText(panel, wxID_ANY, _T("Position"));
	sizerOptions->Add(stExtraRodProtection);
	sizerOptions->Add(stPosition);
	sizerOptions->Add(editExtraRodProtection);

	wxGridSizer *sizerPosition = new wxGridSizer(2);
	wxStaticText *stOptionHead= new wxStaticText(panel, LabelCtrl, _T("lbOptionHead"));
	wxStaticText *stOptionCap = new wxStaticText(panel, LabelCtrl, _T("lbOptionCap"));
	wxComboBox *comboOptionHead = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("OptionHead"));
	wxComboBox *comboOptionCap = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("OptionCap"));
	sizerPosition->Add(stOptionHead);
	sizerPosition->Add(stOptionCap);
	sizerPosition->Add(comboOptionHead);
	sizerPosition->Add(comboOptionCap);
	sizerOptions->Add(sizerPosition);
	sizerGrid->Add(sizerOptions);

	wxStaticText *stCatnumLabel= new wxStaticText(panel, wxID_ANY, _T("Catalog#:"));
	wxStaticText *stCatnum= new wxStaticText(panel, Catalog, _T("XXXXXXXX"));

	sizerGrid->Add(stCatnumLabel);
	sizerGrid->Add(stCatnum);

	sizerTop->Add(sizerGrid, 1);
	panel->SetSizer(sizerTop);

	m_engine = NULL;
	bLoadingItems = false;
	SetupApplication();

	sizerTop->Fit(this);
	sizerTop->SetSizeHints(this);
}

MyFrame::~MyFrame()
{
	if (m_engine)
		delete m_engine;
	if (m_rootNode)
		delete m_rootNode;
}

void MyFrame::SetupApplication()
{
	m_rootNode = new ROMNode("HydraulicCylinder");
	if (!m_rootNode->LoadRules("HydraulicCylinderRules.xml"))
	{
		wxMessageBox(_T("Error loading rule file"));
		Close(true);
	}

	m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
	m_engine->EvaluateAll();

	UpdateControls();
}

void MyFrame::UpdateControls()
{
	bLoadingItems = true;

	map<wstring, ROMDictionaryAttribute> *allAttrs = m_engine->GetAllDictionaryAttrs();
	for (map<wstring, ROMDictionaryAttribute>::iterator it = allAttrs->begin(); it != allAttrs->end(); it++)
	{
		SetControlUI(it->second);
	}
	bLoadingItems = false;
	UpdateCatalog();
}

void MyFrame::UpdateCatalog()
{
	vector<wstring> allChars = m_rootNode->EvaluateTable(L"CatalogNumber", L"Code", true);
	wstring Catnum;
	for (vector<wstring>::iterator it = allChars.begin(); it != allChars.end(); it++)
		Catnum += *it;

	wxStaticText* catLabel = (wxStaticText*)this->FindWindowById(Catalog);
	if (catLabel != NULL)
		catLabel->SetLabel(Catnum);
}

void MyFrame::SetControlUI(ROMDictionaryAttribute attr)
{
	wxControl* ctrl = NULL;
	wxStaticText* label = NULL;
	ctrl = (wxControl*)this->FindWindowByName(attr.Name);
	label = (wxStaticText*)this->FindWindowByName(L"lb" + attr.Name, panel);
	if (!label)
	{
		label = (wxStaticText*)this->FindWindowByLabel(attr.Description, panel);
	}

	if (ctrl)
	{
		wxComboBox* combo = dynamic_cast<wxComboBox*>(ctrl);
		wxStaticText* staticText = dynamic_cast<wxStaticText*>(ctrl);
		wxTextCtrl* edit = dynamic_cast<wxTextCtrl*>(ctrl);
		if (combo)
		{
			combo->Freeze();
			combo->Clear();
			for (vector<wstring>::iterator it = attr.AvailableValues.begin(); it != attr.AvailableValues.end(); it++)
				combo->AppendString(*it);
			combo->SetSelection(combo->FindString(m_rootNode->GetAttribute(attr.Name)));
			combo->Thaw();
		}
		else if (staticText && staticText->GetId() != LabelCtrl)
		{
			staticText->SetLabel(m_rootNode->GetAttribute(attr.Name));
		}
		else if (edit)
		{
			edit->SetValue(m_rootNode->GetAttribute(attr.Name));
		}
		ctrl->Enable(attr.Enabled);
		//#ifdef WIN32
		//wstring msg = attr.Name;
		//if (attr.Enabled)
		//	msg += L" true";
		//else
		//	msg += L" false";
		//msg += L"\n";
		//OutputDebugString(msg.c_str());
		//#endif

		if (label != NULL && label->GetId() == LabelCtrl)
		{
			label->SetLabel(attr.Description);
			wxFont font;
			if (attr.ChangedByUser == true)
				font.SetWeight(wxFONTWEIGHT_BOLD);
			else
				font.SetWeight(wxFONTWEIGHT_NORMAL);


			if (attr.Valid == true)
			{
				if (attr.Enabled)
					label->SetForegroundColour(wxColor(L"BLACK"));
				else
					label->SetForegroundColour(wxColor(L"DIM GREY"));
			}
			else
				label->SetForegroundColour(wxColor(L"RED"));

			label->SetFont(font);
		}
	}
}

// event handlers
void MyFrame::ComboEvaluate(wxCommandEvent& event)
{
	if (bLoadingItems)
		return;
	wxComboBox* sender = (wxComboBox*)event.GetEventObject();
	m_engine->EvaluateForAttribute(sender->GetName().c_str(), sender->GetValue().c_str());
    UpdateControls();
}

void MyFrame::TextChanged(wxCommandEvent& event)
{
	if (bLoadingItems)
		return;
	wxTextCtrl* sender = (wxTextCtrl*)event.GetEventObject();
	m_engine->EvaluateForAttribute(sender->GetName().c_str(), sender->GetValue().c_str());
    UpdateControls();
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T("Welcome to %s!\n")
                    _T("\n")
                    _T("This is the minimal wxWidgets sample\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About wxWidgets minimal sample"),
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog openFileDialog(this, _T("Open XML file"), _T(""), _T(""),
		_T("XML files (*.xml)|*.xml"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk())
    {
        wxLogError(_T("Cannot open file '%s'."), openFileDialog.GetPath());
        return;
    }

	wxTextInputStream text(input_stream, wxT("\x09"), wxConvUTF8 );
	wxString docXML;
	while (input_stream.IsOk() && !input_stream.Eof())
	{
		wxString line = text.ReadLine();
		docXML+=line;
	}

	//reload
	if (m_rootNode->LoadXML(docXML.c_str()))
	{
		if (m_engine)
		{
			delete m_engine;
			m_engine = NULL;
		}
		m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
		m_engine->EvaluateAll();

		UpdateControls();
	}
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	 wxFileDialog saveFileDialog(this, _T("Save XML file"), _T(""), _T(""),
		 _T("XML files (*.xml)|*.xml"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;

        std::wofstream outFile;
        outFile.open(saveFileDialog.GetPath().mbc_str(), std::ios::out | std::ios::binary);
		//outFile.open(saveFileDialog.GetPath().c_str(), std::ios::out | std::ios::binary);
        if (!outFile.is_open())
        {
            wxLogError(_T("Cannot save current contents in file '%s'."), saveFileDialog.GetPath());
            return;
        }

		wstring xml = m_rootNode->SaveXML(true);
		outFile<<xml;
		outFile.close();
}
