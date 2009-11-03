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
#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "EquationSolver.h"
#include "Numerics.h"

using namespace EquationHelper;
using namespace Numerics;
using std::size_t;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ios;

/**
 * Private constructor - does nothing.
 */
ES::EquationSolver(){}

/**
 * Performs the specified operation against the
 * argument strings. The operation is dependant on
 * the value of op.
 */
string ES::doOperation(const string& lhs, char op, const string& rhs){

	Double bdLhs = lhs;
	Double bdRhs = rhs;
	Double temp;
	switch(op){
		case '^':
			temp( pow( bdLhs, bdRhs ) );
			break;
		case '*':
			temp( bdLhs * bdRhs );
			break;
		case '/':
			temp( bdLhs / bdRhs );
			break;
		case '+':
			temp( bdLhs + bdRhs );
			break;
		case '%':
			temp( fmod(bdLhs, bdRhs) );
			break;
	}
	return temp.getString();
}

/**
 * Returns the string with its enclosing paranthesis
 * stripped from it.
 */
void ES::correctedString(string& arg){

	size_t pos1 = arg.find_first_of("(");
	size_t pos2 = arg.find_last_of(")");

	if(pos1 >= 0 && pos1 < arg.length() && pos2 >= 0 && pos2 <= arg.length())
		arg[pos1] = arg[pos2] = ' ';
}

/**
 * Remove spaces from the argument string.
 */
void ES::removeSpaces(string& argu){

	string temp = "";
	for(size_t i = 0; i < argu.length(); i++)
		if(argu[i] != ' ')
			temp += argu[i];
	argu = temp;
}

/**
 * The brains of the program.
 * Solves expressions by using recursion for complex expressions.
 */
string ES::parse(const string& param){

	string expression = param;
	correctedString(expression);
	removeSpaces(expression);
	string finalExpression = "";

	bool operatorEncountered = true;
	for(size_t i = 0; i < expression.length(); i++){
		if(expression[i] == '('){
			string placeHolder = "(";
			int valuesCounted = 1;
			operatorEncountered = false;
			for(size_t j = i + 1; valuesCounted != 0; j++){
				if(expression[j] == '(')
					valuesCounted++;
				else if(expression[j] == ')')
					valuesCounted--;

				placeHolder += expression[j];
			}

			string evaluatedString = parse(placeHolder);
			finalExpression += evaluatedString;
			i += (placeHolder.length() - 1);
		}else{
			if(expression[i] == '-' && operatorEncountered == false)
				finalExpression += '+';

			finalExpression += expression[i];
			if((expression[i] == '+'
			|| expression[i] == '/'
			|| expression[i] == '^'
			|| expression[i] == '*'
			|| expression[i] == '%'
			|| expression[i] == '-'))
				operatorEncountered = true;
			else if(expression[i] != ' ')
				operatorEncountered = false;
		}
	}

	removeSpaces(finalExpression);
	string perfectExpression = "";

	for(size_t i = 0; i < finalExpression.length(); i++){
		if((i + 1) < finalExpression.length())
			if(finalExpression[i] == '-' && finalExpression[i + 1] == '-')
				i += 2;
		perfectExpression += finalExpression[i];
	}
	finalExpression = perfectExpression;

	vector<string> totalNumbers;
	vector<char> totalOperations;
	//cout << finalExpression << endl;

	for(size_t i = 0; i < finalExpression.length(); i++){
		if((finalExpression[i] >= '0' && finalExpression[i] <= '9')
		|| finalExpression[i] == '-' || finalExpression[i] == '.'){
			string temp = "";	//
			for(size_t j = i; j < finalExpression.length(); j++){
				if((finalExpression[j] >= '0' && finalExpression[j] <= '9')
				|| finalExpression[j] == '-' || finalExpression[j] == '.'){
						temp += finalExpression[j];
				}else break;
			}
			totalNumbers.push_back(temp);
			i += temp.length() == 0 ? 0 : (temp.length() - 1);
		}else if(finalExpression[i] == '*'
			  || finalExpression[i] == '/'
			  || finalExpression[i] == '^'
			  || finalExpression[i] == '+'
			  || finalExpression[i] == '%'
			  ){
			totalOperations.push_back(finalExpression[i]);
		}
	}

	ES::calculate(totalNumbers, totalOperations, "^");
	ES::calculate(totalNumbers, totalOperations, "*/%");
	ES::calculate(totalNumbers, totalOperations, "+");

	return totalNumbers[0];
}

/**
 * Calculates the numbers in the first vector using the operands in the 2nd vector,
 * based on the expressions allowed which are determined by the string argument.
 */
void ES::calculate(vector<string>& totalNumbers, vector<char>& totalOperations,
				   const string& arg){

	for(unsigned int i = 0; i < static_cast<unsigned int>(totalOperations.size()); i++){
		if( arg.find(totalOperations[i]) != arg.npos){
			totalNumbers[i] = doOperation(totalNumbers[i], totalOperations[i], totalNumbers[i + 1]);

			size_t oldNumberLength = totalNumbers.size();
			size_t oldOperatorLength = totalOperations.size();
			size_t nextNumberLength = oldNumberLength - 1;
			size_t nextOperatorLength = oldOperatorLength - 1;
			size_t sCount = 0;
			size_t oCount = 0;
			vector<string> temp1 ( nextNumberLength );
			vector<char> temp2 ( nextOperatorLength );

			for(size_t j = 0; j < oldNumberLength; j++){
				if(j != static_cast<unsigned int>(i + 1)){
					temp1[sCount++] = totalNumbers[j];
				}
				if(j != i && j < oldOperatorLength){
					temp2[oCount++] = totalOperations[j];
				}
			}
			totalNumbers = temp1;
			totalOperations = temp2;

			i--;
		}
	}
}

/**
 * Returns true if the equation is solvable (not really),
 * returns false otherwise.
 *
 * This function is truly a misnomer, because more restrictions
 * should be put in place.
 */
bool ES::isSolvable(const string& eq){

	int paranthesisCount = 0;
	for(size_t i = 0; i < eq.length(); i++){
		if(eq[i] == '(')
			paranthesisCount++;
		else if(eq[i] == ')')
			paranthesisCount--;

		if(paranthesisCount < 0)
			return false;
	}
	return paranthesisCount == 0;
}

/**
 * An attempt to solve a string-expression, given
 * a precision value.
 */
string ES::solve(const string& eq, int prec){

	if(isSolvable(eq)){

		stringstream ss (stringstream::in | stringstream::out);
		//cout << eq << endl;
		string value;

		value += '(';
		value += eq;
		value += ')';

		#ifdef __GNUC__
		ss.setf((std::_Ios_Fmtflags)0, ios::floatfield);
		#else
		ss.setf(0, ios::floatfield);
		#endif

		ss.precision(prec);
		ss << parse(value);

		return ss.str();
	}else return "";
}
