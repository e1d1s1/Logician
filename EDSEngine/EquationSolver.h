/*
This file is part of the EDSEngine Library.
Copyright (C) 2009 Eric D. Schmidt

    EDSEngine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EDSEngine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with EDSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace EquationHelper{

	class EquationSolver{
		private: 
			EquationSolver();
			static string doOperation(const string&, char, const string&);
			static void correctedString(string&);
			static void removeSpaces(string&);
			static string parse(const string&);
			static bool isSolvable(const string&);
			static void calculate(vector<string>&, vector<char>&, const string&);

		public:
			static string solve(const string&, int = 50);
	};

	typedef EquationSolver ES;
}
