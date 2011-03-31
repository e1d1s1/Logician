/////////////////////////////////////////////////////////////////////////////
// Name:        WorkerClass.h
// Purpose:     Application event handling
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

#include "ProjectManager.h"
#include "LogicTable.h"
#include "MDIChild.h"
#include "DebugOptions.h"
#include "CompileOptions.h"
#include "CodeEditor.h"
#include "OpenLogicTable.h"
#include "GUIClass.h"

using namespace std;

class WorkerClass
{
public:
	WorkerClass(GUIClass *gui, int orient);
	~WorkerClass(void);

	//implementation of menus
	bool NewProject();
	bool OpenProject(wstring fileName);
	bool Save(OpenLogicTable *targetTable = NULL);
	bool LoadTable(wstring name);
	void NewTable(wstring name = L"");
	void InsertTable();
	void DeleteTable(wstring nameDelete = L"");
	void RenameTable(wstring oldTableName = L"", wstring newTableName = L"");
	bool MoveTable(wstring table, wstring oldLoc, wstring newLoc);
	void NewGroup();
	void RenameGroup(wstring oldGroupName = L"", wstring newGroupName = L"");
	void DeleteGroup();
	wstring CompileXML(wstring tempFilePath = L"");
	void CompileZip();
	void ShowDebugOptions();
	void SetOrientation(int orient);
	void GenerateTranslations();
	void ShowCompileOptions();

	//debugging
	void SetProjectDebugging(bool enabled, int server_id);
	void LogTextLine(wstring text) {m_gui->LogText(text); m_gui->LogText(L"\n");}

	//pass through menu commands to child windows
	void InsertCol();
	void InsertRow();
	void DeleteCol();
	void DeleteRow();
	void AppendRow();
	void AppendColumn();
	void ClearCells();
	void Redo();
	void Undo();
	void Copy();
	void Cut();
	void Paste();
	void EditCode();
	void CloseTable();

	wstring GetProjectName();
	bool CheckSave();
	void ChildWindowHasClosed(wstring tableName);
	bool ValidateFolderName(wstring name);
	ProjectManager* GetProjectManager() {return &m_pm;}

	bool bIsSaved;
	vector<OpenLogicTable> open_tables;

private:
	void AddTableToProject(wstring name, bool bCreateNew, bool bSystemTable);
	void GetSettings();
	void SaveApplicationSettings();


	ProjectManager m_pm;
	int m_orientation;
	DebugOptions m_debugOptions;
	CompileOptions m_compileOptions;
	bool m_debug_status;

	GUIClass *m_gui;
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
