/////////////////////////////////////////////////////////////////////////////
// Name:        stdafx.h
// Purpose:     Headers and defines
// Author:      Eric D. Schmidt
// Modified by:
// Created:     07/01/2010
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

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#if defined(WIN32)
// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#endif

// TODO: reference additional headers your program requires here
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <wx/msgdlg.h>

#ifdef POSIX
const wchar_t PATHSEP = L'/';
#else
const wchar_t PATHSEP = L'\\';
#endif

#define ID_YES wxYES

using namespace std;

inline void ReportErrorW(std::wstring functionName, std::wstring errorMsg = L"")
{
	wstring err = L"Error in: " + functionName;
	if (errorMsg.length() > 0)
		err += L"\n" + errorMsg;

	wxMessageBox(err, _T("Error"));
}

inline void ReportError(std::string functionName, std::string errorMsg = "")
{
	std::wstring err;
	err.assign(errorMsg.begin(), errorMsg.end());
	std::wstring function;	
	function.assign(functionName.begin(), functionName.end());
	ReportErrorW(function, err);
}

const wstring GLOBALORS_TABLE_NAME = L"GlobalORs";
const wstring TRANSLATIONS_TABLE_NAME = L"Translations";