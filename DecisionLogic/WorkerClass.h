/////////////////////////////////////////////////////////////////////////////
// Name:        WorkerClass.h
// Purpose:     Application event handling
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include <string>
#include <wx/grid.h>
#include <wx/treectrl.h>
#include <wx/laywin.h>
#if defined(WIN32)
	#include <winsock.h>
#endif
#include <wx/socket.h>
#include <wx/config.h>

#include "ProjectManager.h"
#include "LogicTable.h"
#include "MDIChild.h"
#include "DebugOptions.h"
#include "CompileOptions.h"
#include "CodeEditor.h"
#include "OpenLogicTable.h"

using namespace std;

class WorkerClass
{
public:
	WorkerClass(wxMDIParentFrame *parent, wxTreeCtrl *tree, wxTextCtrl *log, int orient);
	~WorkerClass(void);

	//implementation of menus
	bool NewProject();
	bool OpenProject(wstring fileName);
	bool Save();
	bool LoadTable(wstring name);
	void NewTable(wstring name = L"");
	void DeleteTable();
	void NewGroup();
	void DeleteGroup();
	wstring CompileXML(wstring tempFilePath = L"");
	void CompileZip();
	void ShowDebugOptions();
	void SetOrientation(int orient);
	void GenerateRecentFileList(wxMenu *listMenu, int RecentFile_ID_BEGIN, int RecentFile_ID_END);
	void GenerateTranslations();
	void ShowCompileOptions();

	//debugging
	void SetProjectDebugging(bool enabled, int server_id);
	void LogTextLine(wstring text) {m_log->AppendText(text); m_log->AppendText(L"\n");}
	void ParseSocket(wxSocketEvent& event);
	void ServerEvent(wxSocketEvent& event, int socket_id);

	//pass through menu commands to child windows
	void InsertCol(wxCommandEvent& event);
	void InsertRow(wxCommandEvent& event);
	void DeleteCol(wxCommandEvent& event);
	void DeleteRow(wxCommandEvent& event);
	void AppendRow(wxCommandEvent& event);
	void AppendColumn(wxCommandEvent& event);
	void ClearCells(wxCommandEvent& event);
	void Redo(wxCommandEvent& event);
	void Undo(wxCommandEvent& event);
	void Copy(wxCommandEvent& event);
	void Cut(wxCommandEvent& event);
	void Paste(wxCommandEvent& event);
	void EditCode(wxCommandEvent& event);

	//find
	void FindTextInActiveTable(wstring strToFind, wxPoint *startPos,
		bool bMatchCase, bool bMatchWholeWord, bool bDoReplace = false, wstring strReplace = L"");
	bool FindTextInAnyTable(wstring strToFind, wxPoint *startPos, wstring *last_found_name,
		bool bMatchCase, bool bMatchWholeWord, bool bDoReplace = false, wstring strReplace = L"");

	bool bIsSaved;
	vector<OpenLogicTable> open_tables;
	wxGrid *grid_input_table, *grid_output_table;


private:
	bool CheckSave();
	wxTreeItemId AddTreeNode(wxTreeItemId parent, wstring name);
	wstring GetTreeNodePath(wxTreeCtrl *tree, wstring nodeName);
	void DeleteTreeNode(wstring name);
	void FillDataTable(StringTable<wstring> *table, wxGrid *grid);
	wxTreeItemId* FindItemNamed(wxTreeCtrl *tree, const std::wstring &name);
	void AddAllProjectNodes();
	void GetSettings();
	void SaveApplicationSettings();
	void DebugInfoReceived(wstring buff);
	void HighlightTableAndRule(wstring tableName, size_t iSolnIdx);
	bool TestStringTextMatch(wxString test, wxString find, bool bMatchCase, bool bMatchWholeWord);

	ProjectManager m_pm;
	vector<OpenLogicTable> m_opened_windows;
	int m_orientation;

	wxConfig *config;
	int m_MaxRecentFiles;
	DebugOptions m_debugOptions;
	CompileOptions m_compileOptions;
	bool m_debug_status;
	wxSocketServer *m_server;

	wxMDIParentFrame *m_parentFrame;
	wxTreeCtrl *m_tree;
	wxTreeItemId *root_node;
	wxTreeItemId wxtid_active_group;
	wxTextCtrl *m_log;
};

/*
class ClientThread : public wxThread
{
public:
	ClientThread::ClientThread(WorkerClass *worker, string con)
			: wxThread()
	{
		m_worker = worker;
		m_connection = con;
	}

	void ClientThread::WriteText(const wxString& text)
	{
		wxString msg;

		// before doing any GUI calls we must ensure that this thread is the only
		// one doing it!

		wxMutexGuiEnter();

		msg << text;
		m_worker->LogTextLine((wstring)msg);

		wxMutexGuiLeave();
	}

	virtual void ClientThread::OnExit()
	{
		wxCriticalSectionLocker locker(m_worker->critsect);

		//wxArrayThread& threads = m_app->m_threads;
		//threads.Remove(this);

		//if ( threads.IsEmpty() )
		{
			// signal the main thread that there are no more threads left if it is
			// waiting for us
			//if ( m_app->m_waitingUntilAllDone )
			{
				//m_app->m_waitingUntilAllDone = false;

				m_worker->semaphore.Post();
			}
		}
	}

	virtual void *ClientThread::Entry()
	{
		string host = "localhost", port = "11000";
		char* buff = new char[m_connection.length() + 1];
		strcpy(buff, m_connection.c_str());
		char *pch = strtok(buff, ":");
		if (pch != NULL)
			host = pch;
		pch = strtok(NULL, ":");
		if (pch != NULL)
		{
			port = pch;
			try
			{
				boost::asio::io_service io_service;

				tcp::resolver resolver(io_service);
				tcp::resolver::query query(tcp::v4(), host.c_str(), port.c_str());
				tcp::resolver::iterator iterator = resolver.resolve(query);

				tcp::socket s(io_service);
				s.connect(*iterator);

				using namespace std; // For strlen.

				char reply[1024];
				size_t reply_length = boost::asio::read(s,
					boost::asio::buffer(reply, sizeof(reply)));

				m_worker->LogTextLine(UTILS::ToWString(reply));
			}
			catch (std::exception& e)
			{
				string ex = "Exception: ";
				ex += e.what();
				m_worker->LogTextLine(UTILS::ToWString(ex));
			}
		}
		else
		{
			m_worker->LogTextLine(L"Not a valid debugging connection string");
		}
		delete [] buff;

		return NULL;
	}

private:
    WorkerClass *m_worker;
	wxSemaphore *m_semaphore;
	wxCriticalSection *m_critsect;
	string m_connection;

};
*/
