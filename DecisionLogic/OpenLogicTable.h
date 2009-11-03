/////////////////////////////////////////////////////////////////////////////
// Name:        OpenLogicTable.h
// Purpose:     Helps manage pointers to currently open rules tables
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

#include "MDIChild.h"
#include "LogicTable.h"

struct OpenLogicTable
{
	LogicTable	logic_table;
	void*		*child_window_ptr;
};