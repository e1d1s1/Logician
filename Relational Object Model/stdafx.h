/*
This file is part of the Relational Object Model 2 Library.
Copyright (C) 2009 Eric D. Schmidt

    Relational Object Model 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Relational Object Model 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Relational Object Model 2.  If not, see <http://www.gnu.org/licenses/>.
*/
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

#ifndef ROM2
#define ROM2 1
#endif

// TODO: reference additional headers your program requires here
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
//#include "XMLWrapper.h"

using namespace std;

inline void ReportROMErrorW(std::wstring errorMsg)
{
	
}

inline void ReportROMError(std::string errorMsg)
{
	std::wstring err;
	err.assign(errorMsg.begin(), errorMsg.end());
	ReportROMErrorW(err);
}