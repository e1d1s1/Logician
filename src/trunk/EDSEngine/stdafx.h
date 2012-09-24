/*
This file is part of the EDSEngine Library.
Copyright (C) 2009-2011 Eric D. Schmidt, DigiRule Solutions LLC

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
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

// TODO: reference additional headers your program requires here
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

#ifdef _MSC_VER
	#if (_MSC_VER <= 1600)
		#include <hash_map>
		typedef stdext::hash_map<size_t, wstring> MAPUINTWSTR;
		typedef stdext::hash_map<wstring, size_t> MAPWSTRUINT;
		typedef stdext::hash_map<string, size_t> MAPSTRUINT;
		typedef stdext::hash_map<wstring, wstring> MAPWSTRS;
		typedef stdext::hash_map<size_t, stdext::hash_map<wstring, wstring> > MAPUINTMAP;
		typedef stdext::hash_map<wstring, stdext::hash_map<wstring, wstring> > MAPWSTRMAP;
	#else
		#include <unordered_map>
		typedef std::unordered_map<size_t, wstring> MAPUINTWSTR;
		typedef std::unordered_map<wstring, size_t> MAPWSTRUINT;
		typedef std::unordered_map<string, size_t> MAPSTRUINT;
		typedef std::unordered_map<wstring, wstring> MAPWSTRS;
		typedef std::unordered_map<size_t, std::unordered_map<wstring, wstring> > MAPUINTMAP;
		typedef std::unordered_map<wstring, std::unordered_map<wstring, wstring> > MAPWSTRMAP;
	#endif
#else
	#include <unordered_map>
	typedef std::unordered_map<size_t, wstring> MAPUINTWSTR;
	typedef std::unordered_map<wstring, size_t> MAPWSTRUINT;
	typedef std::unordered_map<string, size_t> MAPSTRUINT;
	typedef std::unordered_map<wstring, wstring> MAPWSTRS;
	typedef std::unordered_map<size_t, std::unordered_map<wstring, wstring> > MAPUINTMAP;
	typedef std::unordered_map<wstring, std::unordered_map<wstring, wstring> > MAPWSTRMAP;
#endif

const unsigned short INVALID_STRING = 0;
const unsigned short EMPTY_STRING = 1;
const unsigned short EXPLICIT_NULL_STRING = 2;

inline void ReportErrorW(std::wstring errorMsg)
{
	std::wcerr<<errorMsg<<std::endl;
}

inline void ReportError(std::string errorMsg)
{
	std::wstring err;
	err.assign(errorMsg.begin(), errorMsg.end());
	ReportErrorW(err);
}
