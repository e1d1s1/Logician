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
#include "EDSEngine/KnowledgeBase.h"
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

	wxStaticText *stMountingStyle = new wxStaticText(panel, LabelCtrl, _T("lbMountingType"));
	sizerGrid->Add(stMountingStyle);
	wxComboBox *comboMountingStyle = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("MountingType"));
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

    wxStaticText *stPistonTypes = new wxStaticText(panel, LabelCtrl, _T("lbPistonType"));
	sizerGrid->Add(stPistonTypes);
	wxComboBox *comboPistonTypes = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("PistonType"));
	sizerGrid->Add(comboPistonTypes);

	wxStaticText *stSealingSystem = new wxStaticText(panel, LabelCtrl, _T("lbPistonSeals"));
	sizerGrid->Add(stSealingSystem);
	wxComboBox *comboSealingSystem = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("PistonSeals"));
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

    wxStaticText *stCushion = new wxStaticText(panel, LabelCtrl, _T("lbCushion"));
	sizerGrid->Add(stCushion);
	wxComboBox *comboCushion = new wxComboBox(panel, ComboCtrl, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("Cushion"));
	sizerGrid->Add(comboCushion);

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

	wxStaticText *stCatnumLabel= new wxStaticText(panel, wxID_ANY, _T("Catalog#:"));
	wxStaticText *stCatnum= new wxStaticText(panel, Catalog, _T("XXXXXXXX"));

	sizerGrid->Add(stCatnumLabel);
	sizerGrid->Add(stCatnum);

	sizerTop->Add(sizerGrid, 1);
#ifdef USEDEBUGGER
	m_debugger = NULL;
#endif
	m_engine = NULL;
	bLoadingItems = false;
	SetupApplication();
#ifdef USEDEBUGGER
	m_debugger = new wxLogicianDebugCtrl(m_rootNode, panel, wxID_ANY, wxDefaultPosition,
		wxSize(640, 480));
	sizerTop->Add(m_debugger, 1, wxEXPAND|wxSHRINK);
#endif
	panel->SetSizer(sizerTop);
	sizerTop->Fit(this);
	sizerTop->SetSizeHints(this);
}

MyFrame::~MyFrame()
{
	if (m_engine)
		delete m_engine;
	if (m_rootNode)
		delete m_rootNode;
	if (m_rules)
		delete m_rules;
}

void MyFrame::SetupApplication()
{
	m_rootNode = new ROMNode("HydraulicCylinder");
	m_rules = new EDS::CKnowledgeBase("HydraulicCylinderRules.xml");
	m_rules->SetInputValueGetter([](const string& name, void* obj)
	{
		return ((ROMNode*)obj)->GetAttribute(name, false);
	});

	if (!m_rules->IsOpen())
	{
		wxMessageBox(_T("Error loading rule file"));
		Close(true);
	}

	m_rootNode->SetKnowledgeBase(m_rules);
	m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
	//m_engine->InitializeEngine();
	m_engine->EvaluateAll();

	UpdateControls();
}

void MyFrame::UpdateControls()
{
	bLoadingItems = true;

	auto allAttrs = m_engine->GetAllDictionaryAttrs();
	for (auto it = allAttrs->begin(); it != allAttrs->end(); it++)
	{
		if (it->second->GetValueChanged())
			SetControlUI(it->second);
	}
	bLoadingItems = false;
	UpdateCatalog();
#ifdef USEDEBUGGER
	if (m_debugger)
		m_debugger->Update();
#endif
}

void MyFrame::UpdateCatalog()
{
	vector<string> allChars = m_rootNode->EvaluateTable("CatalogNumber", "Code", true);
	string Catnum;
	for (const auto& it : allChars)
		Catnum += it;

	wxStaticText* catLabel = (wxStaticText*)this->FindWindowById(Catalog);
	if (catLabel != NULL)
		catLabel->SetLabel(Catnum);
}

void MyFrame::SetControlUI(IROMDictionaryAttribute* attr)
{
	wxControl* ctrl = NULL;
	wxStaticText* label = NULL;
	ctrl = (wxControl*)this->FindWindowByName(attr->GetName());
	label = (wxStaticText*)this->FindWindowByName("lb" + attr->GetName(), panel);
	if (!label)
	{
		label = (wxStaticText*)this->FindWindowByLabel(attr->GetDescription(), panel);
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
			auto available = attr->GetAvailableValues();
			for (auto it = available.begin(); it != available.end(); it++)
				combo->AppendString(*it);
			combo->SetSelection(combo->FindString(m_rootNode->GetAttribute(attr->GetName())));
			combo->Thaw();
		}
		else if (staticText && staticText->GetId() != LabelCtrl)
		{
			staticText->SetLabel(m_rootNode->GetAttribute(attr->GetName()));
		}
		else if (edit)
		{
			edit->SetValue(m_rootNode->GetAttribute(attr->GetName()));
		}
		ctrl->Enable(attr->GetEnabled());
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
			label->SetLabel(attr->GetDescription());
			wxFont font;
			if (attr->GetChangedByUser() == true)
				font.SetWeight(wxFONTWEIGHT_BOLD);
			else
				font.SetWeight(wxFONTWEIGHT_NORMAL);


			if (attr->GetValid() == true)
			{
				if (attr->GetEnabled())
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
	m_engine->EvaluateForAttribute(sender->GetName().ToStdString(), sender->GetValue().ToStdString());
    UpdateControls();
}

void MyFrame::TextChanged(wxCommandEvent& event)
{
	if (bLoadingItems)
		return;
	wxTextCtrl* sender = (wxTextCtrl*)event.GetEventObject();
	m_engine->EvaluateForAttribute(sender->GetName().ToStdString(), sender->GetValue().ToStdString());
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

	string filepath = openFileDialog.GetPath().ToStdString();

	//wxFileInputStream input_stream(openFileDialog.GetPath());
	//if (!input_stream.IsOk())
 //   {
 //       wxLogError(_T("Cannot open file '%s'."), openFileDialog.GetPath());
 //       return;
 //   }

	//wxTextInputStream text(input_stream, wxT("\x09"), wxConvUTF8 );
	//wxString docXML;
	//while (input_stream.IsOk() && !input_stream.Eof())
	//{
	//	wxString line = text.ReadLine();
	//	docXML+=line;
	//}

	//reload
	if (filepath.length() > 0)
	{
		if (m_engine)
		{
			delete m_engine;
			m_engine = nullptr;
		}
		if (m_rootNode)
		{
			delete m_rootNode;
			m_rootNode = nullptr;
		}

		ROMNode* newNode = ROMNode::LoadXML(filepath, nullptr);

		if (newNode != nullptr)
		{
			m_rootNode = newNode;
			m_rootNode->SetKnowledgeBase(m_rules);
			m_engine = new LinearEngine(m_rootNode, "HydraulicCylinderDictionary");
			m_engine->EvaluateAll();

			UpdateControls();
		}
	}
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	 wxFileDialog saveFileDialog(this, _T("Save XML file"), _T(""), _T(""),
		 _T("XML files (*.xml)|*.xml"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;

        std::ofstream outFile;
        outFile.open(saveFileDialog.GetPath().mbc_str(), std::ios::out | std::ios::binary);
		//outFile.open(saveFileDialog.GetPath().c_str(), std::ios::out | std::ios::binary);
        if (!outFile.is_open())
        {
            wxLogError(_T("Cannot save current contents in file '%s'."), saveFileDialog.GetPath());
            return;
        }

		string xml = m_rootNode->SaveXML(true);
		outFile<<xml;
		outFile.close();
}
