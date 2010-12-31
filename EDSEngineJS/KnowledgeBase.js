/*
Copyright (C) 2009-2011  Eric D. Schmidt

    The JavaScript code in this page is free software: you can
    redistribute it and/or modify it under the terms of the GNU
    General Public License (GNU GPL) as published by the Free Software
    Foundation, either version 3 of the License, or (at your option)
    any later version.  The code is distributed WITHOUT ANY WARRANTY;
    without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.

    As additional permission under GNU GPL version 3 section 7, you
    may distribute non-source (e.g., minimized or compacted) forms of
    that code without the copy of the GNU GPL normally required by
    section 4, provided you include this license notice and a URL
    through which recipients can access the Corresponding Source.
    <http://www.gnu.org/licenses/>.
*/

//IE doesnt do indexOf for array
function GetIndexOfItem(arr, obj)
{
    for(var i=0; i<arr.length; i++){
        if(arr[i]===obj){
            return i;
        }
    }
    return -1;   
}

var EQUALS = 0x1;
var NOT_EQUAL = 0x2;
var LESS_THAN = 0x4;
var LESS_THAN_EQUAL = 0x8;
var GREATER_THAN = 0x10;
var GREATER_THAN_EQUAL = 0x20;
var RANGE_INCLUSIVE = 0x40;
var RANGE_END_INCLUSIVE = 0x80;
var RANGE_START_INCLUSIVE = 0x100;
var RANGE_NOT_INCLUSIVE = 0x200;
var PYTHON = 0x400; //not used on web
var GETS = 0x800;
var CHAIN = 0x1000;
var JAVASCRIPT = 0x2000;
var INVALID_STRING = 0;
var EMPTY_STRING = 1;
var EXPLICIT_NULL_STRING = 2;
function ReportError(err) 
{
    var vDebug = "";
    for (var prop in err)
    { 
       vDebug += "property: "+ prop+ " value: ["+ err[prop]+ "]\n";
    }
    vDebug += "toString(): " + " value: [" + err.toString() + "]";
    status.rawValue = vDebug; 
    alert(vDebug);
}

function IsIE() 
{
    var ie = (typeof window.ActiveXObject != 'undefined');
    //alert("IE=" + ie);
    return ie;
}

function IsMoz() 
{
    var moz = (typeof document.implementation != 'undefined') && (typeof 
        document.implementation.createDocument != 'undefined');
    //alert("Moz=" + moz);
    return moz;
}

String.prototype.trim = function () {
    return this.replace(/^\s*/, "").replace(/\s*$/, "");
}

var numb = '0123456789';
var lwr = 'abcdefghijklmnopqrstuvwxyz';
var upr = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';

function isValid(parm,val) {
if (parm == "") return true;
for (i=0; i<parm.length; i++) {
if (val.indexOf(parm.charAt(i),0) == -1) return false;
}
return true;
}

function isNumber(parm) {return isValid(parm,numb);}
function isLower(parm) {return isValid(parm,lwr);}
function isUpper(parm) {return isValid(parm,upr);}
function isAlpha(parm) {return isValid(parm,lwr+upr);}
function isAlphanum(parm) {return isValid(parm,lwr+upr+numb);} 

function ArraySize(arr) //for associative array sizes
{
    var l = 0;
    for (var k in arr) {
        l++;
    }
    return l;
}

function XMLSafe(str)
{
    var retval = "";
    try
    {
	    //replace any illegal characters with escapes
	    retval = str.replace("\"", "&quot;");
	    retval = retval.replace("'", "&apos;");
	    retval = retval.replace("<", "&lt;");
	    retval = retval.replace(">", "&gt;");
	    retval = retval.replace("&", "&amp;");
	}
	catch(err)
	{
	    ReportError(err);
	}
	return retval;
}

function Pair()
{
    var first = new Object();
    var second = new Object();
}


//dynamic code, scripting support//////////////////////////////////////
function InstallScript(script)
{
    var tag = document.createElement("script");
    tag.type = "text/javascript";
    tag.text = script;
    tag.id = "customScript";
    document.getElementsByTagName("head").item(0).appendChild(tag);
    return tag.id;
}

function RemoveScriptTag(id)
{
    var child = document.getElementById(id);
    document.getElementsByTagName("head").item(0).removeChild(child);
}

//RuleCell/////////////////////////////////////////////////////////////
function RuleCell()
{
    this.Operation = EQUALS;
    this.Values = new Array();
}

//Bimapper/////////////////////////////////////////////////////////////
function Bimapper()
{
    try
    {
        this.m_IndexToStringsMap = new Array();
        this.m_StringsToIndexMap = new Array();
        this.userStrings = new Array();
        this.maxID = 0;
    }
    catch (err)
    {
        ReportError(err);
    }
}

Bimapper.prototype.AddString = function(id, s)
{
    try
    {
        if (id > this.maxID)
            this.maxID = id;
        this.m_IndexToStringsMap[id] = s;
        this.m_StringsToIndexMap[s] = id;
    }
    catch (err)
    {
        ReportError(err);
    }
}

Bimapper.prototype.AddUserString = function(s)
{
    try
    {
        var retval = this.maxID + 1;
	    this.m_IndexToStringsMap[retval] = s;
	    this.m_StringsToIndexMap[s] = retval;
	    this.userStrings.push(retval);
	    this.maxID = retval;
	    return retval;
    }
    catch (err)
    {
        ReportError(err);
    }
}

Bimapper.prototype.GetStringByID = function(id)
{
    var retval = "";
    try
    {        
        for (var i in this.m_IndexToStringsMap)
        {
            if (i == id)
            {
                retval = this.m_IndexToStringsMap[id];
                break;
            }
        }
        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

Bimapper.prototype.GetIDByString = function(s)
{
    var retval = INVALID_STRING;
    try
    {    
		if (s.length == 0)
		{
			retval = EMPTY_STRING;
		}
		else for (var i in this.m_StringsToIndexMap)
        {
            if (i == s)
            {
                retval = this.m_StringsToIndexMap[s];
                break;
            }
        }        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

Bimapper.prototype.ClearUserStrings = function()
{
    try
    {
        for (var i = 0; i < ArraySize(userStrings); i++)
        {
            var id = this.userStrings[i];
            var s = this.GetStringByID(id);
            
            if (m_StringsToIndexMap[s] != null)
                delete m_StringsToIndexMap[s];
                
            if (m_IndexToStringsMap[id] != null)
                delete m_IndexToStringsMap[id];
        }
        
        this.userStrings = new Array();
    }
    catch (err)
    {
        ReportError(err);
    }
}

//Decode///////////////////////////////////////////////////////////////
function Decode()
{
    try
    {
        this.m_value = 0;
        this.m_tests = new Array();
        this.m_operator = EQUALS;
        this.m_stringsMap = new Bimapper();
        this.m_inputValues = new Array();
    }
    catch (err)
    {
        ReportError(err);
    }
}

Decode.prototype.CreateOutputDecoder = function(outputCell, inputValues, stringMap)
{
    try
    {
        this.m_tests = outputCell.Values;
	    this.m_stringsMap = stringMap;
	    this.m_operator = outputCell.Operation;
	    this.m_inputValues = inputValues;
    }
    catch (err)
    {
        ReportError(err);
    }
}

Decode.prototype.CreateInputDecoder = function(currentValue, inputCell, inputValues, stringMap)
{
    try
    {
        this.m_value = currentValue;
	    this.m_tests = inputCell.Values;
	    this.m_stringsMap = stringMap;
	    this.m_operator = inputCell.Operation;
	    this.m_inputValues = inputValues;

	    this.CheckForInputGets();
    }
    catch (err)
    {
        ReportError(err);
    }
}

Decode.prototype.EvaluateInputCell = function()
{
    var retval = true;
    try
    {            
        //check by operator
		//no string compares needed here for speed
		if (this.m_operator & EQUALS)
		{
			for (var i = 0; i < ArraySize(this.m_tests); i++) //possible OR
			{
				var test = this.m_tests[i];

				if (test == this.m_value && test > 0)
				{
					retval = true;
					break;
				}
				else
					retval = false;

				//explict NULL check
				if (retval == false && test == EXPLICIT_NULL_STRING && this.m_value == EMPTY_STRING)
					retval = true;
			}
		}
		else if (this.m_operator & NOT_EQUAL)
		{
			for (var i = 0; i < ArraySize(this.m_tests); i++) //possible OR
			{
				var test = this.m_tests[i];

				if (test == this.m_value)
				{
					retval = false;
				}

				//explict NULL check
				if (retval == true && test == EXPLICIT_NULL_STRING && this.m_value == EMPTY_STRING)
					retval = false;
			}
		}
		//the other operations require the strings to be de-tokenized for values tests,
		//there are no "ORs" in these cases
		else if (this.m_operator & LESS_THAN || this.m_operator & LESS_THAN_EQUAL || this.m_operator & GREATER_THAN || this.m_operator & GREATER_THAN_EQUAL)
		{
			var currentTest = this.GetString(this.m_tests[0]);
			var currentValue = this.GetString(this.m_value);
			var bIsNum = false;

			if (isNumber(currentTest) == true && isNumber(currentValue) == true)
			{
				bIsNum = true;
			}
			var dCurrentValue = 0.0;
			var dTestValue = 0.0;

            
			if (bIsNum)
			{
				try
				{
					dCurrentValue = parseFloat(currentValue);
					dTestValue = parseFloat(currentTest);
				}
				catch(err)
				{
					alert("Could not create numeric values from string data\n" + err);
					bIsNum = false;
				}
			}


			if (this.m_operator & LESS_THAN)
			{
				if (bIsNum)
				{
					retval = dCurrentValue < dTestValue;
				}
				else
				{
					retval = currentValue < currentTest;
				}
			}
			else if (this.m_operator & LESS_THAN_EQUAL)
			{
				if (bIsNum)
				{
					retval = dCurrentValue <= dTestValue;
				}
				else
				{
					retval = currentValue <= currentTest;
				}
			}
			else if (this.m_operator & GREATER_THAN)
			{
				if (bIsNum)
				{
					retval = dCurrentValue > dTestValue;
				}
				else
				{
					retval = currentValue > currentTest;
				}
			}
			else if (this.m_operator & GREATER_THAN_EQUAL)
			{
				if (bIsNum)
				{
					retval = dCurrentValue >= dTestValue;
				}
				else
				{
					retval = currentValue >= currentTest;
				}
			}
		}
		//will have to deal with the comma seperator to get values
		else if (this.m_operator & RANGE_INCLUSIVE || this.m_operator & RANGE_END_INCLUSIVE || 
			this.m_operator & RANGE_START_INCLUSIVE || this.m_operator & RANGE_NOT_INCLUSIVE)
		{
			var testString = this.GetString(this.m_tests[0]);
			var currentValue = this.GetString(this.m_value);
			var min = 0, max = 0, dCurrentValue = 0;
			var parts = testString.split(",");

			if (parts.length == 2)
			{
				try
				{
					min = parseFloat(parts[0]);
					max = parseFloat(parts[1]);
					if (currentValue != null && currentValue.length > 0)
					    dCurrentValue = parseFloat(currentValue);
				}
				catch(err)
				{
					alert("Could not create numeric values from string data\n" + err);
					throw error;
				}
				
				if (this.m_operator & RANGE_INCLUSIVE)
				{
					retval = (dCurrentValue >= min && dCurrentValue <= max);
				}
				else if (this.m_operator & RANGE_END_INCLUSIVE)
				{
					retval = (dCurrentValue > min && dCurrentValue <= max);
				}
				else if (this.m_operator & RANGE_START_INCLUSIVE)
				{
					retval = (dCurrentValue >= min && dCurrentValue < max);
				}
				else if (this.m_operator & RANGE_NOT_INCLUSIVE)
				{
					retval = (dCurrentValue > min && dCurrentValue < max);
				}
				
			}
		}

		else if(this.m_operator & PYTHON || this.m_operator & JAVASCRIPT)
		{
		    alert("scripting not supported as an input") ;
			throw error;
		}		
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

Decode.prototype.EvaluateOutputCell = function()
{
    var retval = new Array();
    try
    {        
        for (var i = 0; i < ArraySize(this.m_tests); i++)
        {
            var value = this.m_tests[i];
            if (this.m_operator & GETS)
            {
                retval.push(this.ParseStringForGets(value, false));
            }
            else
            {
                retval.push(this.GetString(value));
            }
        }           
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

Decode.prototype.GetString = function(id)
{
    try
    {
        if (id > EMPTY_STRING)
	    {
		    var s = this.m_stringsMap.GetStringByID(id);
		    if(s.length > 0)
			    return s;
		    else
		    {
			    throw "string not found for index: " + id.toString();
		    }
	    }
	    return "";
    }
    catch (err)
    {
        ReportError(err);
    }
}

Decode.prototype.ParseStringForGets = function(id, bForceZero)
{
    var retval = "";
    try
    {
	    var fullString = this.GetString(id);
	    //replace with the actual value
	    var getIndex = fullString.indexOf("get(");
	    if (getIndex >= 0)
	    {
		    do
		    {
			    fullString = this.ReplaceAGet(fullString, bForceZero);
			    retval = fullString;
			    getIndex = fullString.indexOf("get(");
		    } while(getIndex >= 0);
	    }
	    else
		    retval = fullString;
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

Decode.prototype.ReplaceAGet = function(s, bForceZero)
{
    var retval = "";
    try
    {
	    //find the get(xxx) substring.  attrName xxx is between ().
	    var iStartPos = s.indexOf("get(", 0);
	    var iEndPos = s.indexOf(")", iStartPos);
	    if (iStartPos >= 0 && iEndPos > iStartPos)
	    {
	        var attrName = s.substr(iStartPos + 4, iEndPos - iStartPos - 4);
		    var getText = "get(" + attrName + ")";
		    //get the value of the input attr
		    var bFoundAttr = false;
		    if (this.m_inputValues != null && ArraySize(this.m_inputValues) > 0)
		    {
		        var valueIndex = this.m_inputValues[attrName];
			    if (valueIndex != null)
			    {
				    var value = this.GetString(valueIndex);
				    if (value.length > 0)
				    {
					    bFoundAttr = true;
					    retval = s.replace(getText, value);
				    }
			    }
		    }

		    if (!bFoundAttr)
		    {
			    if (bForceZero == true)
			        retval = s.replace(getText, "0");
			    else
				    retval = s.replace(getText, "");
		    }
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

Decode.prototype.CheckForInputGets = function()
{
    try
    {
        if (this.m_operator & GETS)
	    for (var i = 0; i < ArraySize(this.m_tests); i++)
	    {
		    //replace the GET with the actual value (pay a performance penalty here, but convienient)		
		    var val = this.ParseStringForGets(this.m_tests[i], false);
		    this.m_tests[i] = this.m_stringsMap.GetIDByString(val);		
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
}

//RuleTable////////////////////////////////////////////////////////////
function RuleTable()
{
    try
    {
        this.DebugMessage = "";
        this.m_StateParameter = "";
        this.m_InputAttrsValues = new Array();
        this.m_InputAttrsTests = new Array();
        this.m_FormulaInputs = new Array();
        this.m_OutputAttrsValues = new Array();
        this.m_DEBUGGING = false;
        
        this.m_Name = "";
        this.m_Tests = 0;
        this.m_stringsMap = new Bimapper();
        this.m_bGetAll = false;
        this.bHasChain = false;
        this.bHasJS = false; //eval(.....
        this.bNullSet = false;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.CreateRuleTable = function(inputAttrsTests, outputAttrsValues, formulaInputs, stringMap, name, GetAll)
{
    try
    {
        this.m_Name = name;
        this.m_stringsMap = stringMap;
        this.m_InputAttrsTests = inputAttrsTests;
        this.m_OutputAttrsValues = outputAttrsValues;
        this.m_FormulaInputs = formulaInputs;
        
        if (this.m_OutputAttrsValues.length > 0)
            this.m_Tests = this.m_OutputAttrsValues[0].second.length;
        
        this.bHasJS = false;
        this.bHasChain = false;
        this.m_bGetAll = GetAll;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.EvaluateTable = function(bGetAll, bForward)
{
    var retval = new Array();
    try
    {
        var resultCollection = null;
        if (bForward)
            resultCollection = this.m_OutputAttrsValues;
        else
            resultCollection = this.m_InputAttrsTest;
            
        //for all the outputs get the results
        for (var i = 0; i < ArraySize(resultCollection); i++)
        {
            var outputAttrValuePair = resultCollection[i];
            var result = this.EvaluateTableForAttr(outputAttrValuePair.first, bGetAll, bForward);
            retval[outputAttrValuePair.first] = result;
        }        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

RuleTable.prototype.EvaluateTableForAttr = function(outputAttr, bGetAll, bForward)
{
    var retval = new Array();
    try
    {        
	    var values = new Array();
	    var solutions = new Array()
	    var colResults = new Array(this.m_Tests);
	    for (var j = 0; j < colResults.length; j++)
	        colResults[j] = true;
	    
	    var inputCollection = null;
	    var outputCollection = null;
	    if (bForward)
	    {
		    inputCollection = this.m_InputAttrsTests;
		    outputCollection = this.m_OutputAttrsValues;
	    }
	    else
	    {
		    inputCollection = this.m_OutputAttrsValues;
		    outputCollection = this.m_InputAttrsTests;
	    }
	    
	    this.SetInvalidAttrs();
	    
	    if (ArraySize(this.m_InputAttrsValues) > 0 && ArraySize(inputCollection) > 0)
	    {
		    //get the current values of all input attrs
		    for (var i = 0; i < ArraySize(inputCollection); i++)
		    {
		        var inputAttrPair = inputCollection[i];
			    var attr = inputAttrPair.first;
			    var currentInputAttrValuesIndex = this.m_InputAttrsValues[attr];
			    if (currentInputAttrValuesIndex != null)
			    {
				    values.push(currentInputAttrValuesIndex);
			    }
		    }

		    //sweep down the table for all inputs and do test(s)
		    var bHaveSolution = true;
		    var colResultsDefault = new Array(this.m_Tests);
		    for (var j = 0; j < colResultsDefault.length; j++)
		        colResultsDefault[j] = false;
		    colResults = colResultsDefault;
		    for (var testCnt = 0; testCnt < this.m_Tests; testCnt++)
		    {
			    //sweep through the inputs			    
			    var inputCnt = 0;
			    for (var i = 0; i < ArraySize(inputCollection); i++)
			    {
			        var inputAttrPair = inputCollection[i];
				    if (testCnt < ArraySize(inputAttrPair.second))
				    {
				        var decoder = new Decode();
				        decoder.CreateInputDecoder(values[inputCnt], inputAttrPair.second[testCnt], this.m_InputAttrsValues, this.m_stringsMap);
					    bHaveSolution = decoder.EvaluateInputCell();
				    }
				    inputCnt++;
				    if (!bHaveSolution)
					    break;
			    }
			    colResults[testCnt] = bHaveSolution;
			    if (bHaveSolution && !bGetAll)
				    break;
		    } //done column
		} //done inputs

	    //for the give output, the reuslts are
	    var results = new Array();
	    for (var result = 0; result < ArraySize(outputCollection); result++)
	    {
		    if (outputCollection[result].first == outputAttr)
		    {
			    results = outputCollection[result].second;
		    }
	    }

	    //for each true result, add to the solution array
	    for (var result = 0; result < this.m_Tests; result++)
	    {
		    if (colResults[result] && result < ArraySize(results))
		    {
		        var outputCell = results[result];
			    var decoder = new Decode();
			    decoder.CreateOutputDecoder(outputCell, this.m_InputAttrsValues, this.m_stringsMap);
			    if (outputCell.Operation & CHAIN)
				    this.bHasChain = true;
			    if (outputCell.Operation & JAVASCRIPT)
			    {
    			    this.bHasJS = true;
				}
			    var cellOutputs = decoder.EvaluateOutputCell();
			    for (var i = 0; i < cellOutputs.length; i++)
			    {
			        var cellOuputValue = cellOutputs[i];
				    retval.push(cellOuputValue);	
				    if (this.m_DEBUGGING)
				    {
					    var solnVals = solutions[result];
					    if (solnVals == null)
					    {
						    solutions[result] = new Array();				    
					    }
					    solutions[result].push(cellOuputValue);
				    }				    
			    }
		    }
	    }	    
	    
	    if (this.m_DEBUGGING)
	    {
		    this.DebugEval(outputAttr, values, solutions);
	    }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

RuleTable.prototype.SetInputValues = function(values)
{
    try
    {
        this.bNullSet = false;
        this.m_InputAttrsValues = values;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.SetInputValue = function(name, value)
{
    try
    {
        this.bNullSet = false;

        if (ArraySize(this.m_InputAttrsTests) > 0)
        {
            var bFoundTableInput = false;
            for (var i = 0; i < ArraySize(this.m_InputAttrsTests); i++)
            {
                if (this.m_InputAttrsTests[i].first == name)
                {
                    bFoundTableInput = true;
                    var id = this.m_stringsMap.GetIDByString(value);
                    if (id == INVALID_STRING) //wasnt in out existing list
                    {
                        id = this.m_stringsMap.AddUserString(value);
                    }
                    this.m_InputAttrsValues[name] = id;
                    break;
                }
            }
            
            if (!bFoundTableInput) //this came from outside this table, a get() in a cell
		    {
			    var id = this.m_stringsMap.GetIDByString(value);
			    if (id == INVALID_STRING) //wasnt in our existing list
			    {
				    //add a new tokenized string
				    id = this.m_stringsMap.AddUserString(value);
			    }
			    this.m_InputAttrsValues[name] = id;
		    }
        }
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.SetInvalidAttrs = function()
{
    try
    {
        if (!this.bNullSet)
        {
            for (var i = 0; i < ArraySize(this.m_InputAttrsTests); i++)
            {
                var ruleInput = this.m_InputAttrsTests[i];
                var ruleName = ruleInput.first;
                if (this.m_InputAttrsValues[ruleName] == null)
                {
                    this.m_InputAttrsValues[ruleName] = INVALID_STRING;
                }                
            }
        }
        this.bNullSet = true;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.ResetTable = function()
{
    try
    {
        this.m_Tests = 0;
        this.bHasChain = false;
        this.bHasJS = false;
        this.bNullSet = false;
        
        this.DebugMessage = "";
        this.m_InputAttrsValues = new Array();
        this.m_InputAttrsTests = new Array();
        this.m_FormulaInputs = new Array();
        this.m_OutputAttrsValues = new Array();
          
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.HasChain = function()
{
    try
    {
        return this.bHasChain;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.HasJS = function()
{
    try
    {
        return this.bHasJS;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.GetAllOutputAttrNames = function()
{
    var retval = new Array();
    try
    {
        for (var i = 0; i < this.m_OutputAttrsValues.length; i++)
        {
            retval.push(this.m_OutputAttrsValues[i].first);
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

RuleTable.prototype.GetAllInputAttrNames = function()
{
    var retval = new Array();
    try
    {
        for (var i = 0; i < this.m_InputAttrsTests.length; i++)
        {
            retval.push(this.m_InputAttrsTests[i].first);
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

RuleTable.prototype.GetAllInputDependencies = function() //input columns + formulas/gets
{
    var retval = new Array();
    try
    {
        retval = this.GetAllInputAttrNames();
        for (var i = 0; i < this.m_FormulaInputs.length; i++)
        {
            retval.push(this.m_FormulaInputs[i]);
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

RuleTable.prototype.GetAllPossibleOutputs = function(outputName)
{
    var retval = new Array();
    try
    {
        var uniqueList = new Array();
        for (var i = 0; i < this.m_OutputAttrsValues.length; i++)
        {
            var outputAttrPair = this.m_OutputAttrsValues[i];
            if (outputAttrPair.first == outputName)
            {   
                var cells = outputAttrPair.second; //array of RuleCell
                for (var cellCnt = 0; cellCnt < cells.length; cellCnt++)
                {
                    var values = cells[cellCnt].Values;
                    for (var j = 0; j < values.length; j++)
                    {   
                        uniqueList[values[j]] = values[j];
                    }
                }
                break;
            }
        }
        
        for (var value in uniqueList)
        {
            retval.push(this.m_stringsMap.GetStringByID(value));
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

RuleTable.prototype.GetOutputAttrsValues = function()
{
    try
    {
        return this.m_OutputAttrsValues;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.GetInputAttrsTests = function()
{
    try
    {
        return this.m_InputAttrsTests;
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.DebugEval = function(outputAttr, inputValues, solutions)
{
    try
    {
        var xmlBlob = "";
	    xmlBlob += "<TableEval name=\"";
	    xmlBlob += this.m_Name;
	    xmlBlob += "\" output=\"";
	    xmlBlob += outputAttr;
	    xmlBlob += "\">";
    	
	    if (this.m_InputAttrsTests.length == inputValues.length)
	    {
		    xmlBlob += "<Inputs>";
		    for (var i = 0; i < this.m_InputAttrsTests.length; i++)
		    {
		        var currentPair = this.m_InputAttrsTests[i];
			    var attr = currentPair.first;
			    var value = this.m_stringsMap.GetStringByID(inputValues[i]);			
			    xmlBlob += "<Input name = \"";
			    xmlBlob += attr;
			    xmlBlob += "\" value=\"";
			    xmlBlob += value;
			    xmlBlob += "\"/>"; 
		    }
		    xmlBlob += "</Inputs>";
	    } //else something is wrong

	    xmlBlob+= "<Outputs>";
	    for (var index in solutions)
	    {	        
	        for (var i = 0; i < solutions[index].length; i++)
		    {
			    xmlBlob += "<Output value=\"";
			    xmlBlob += solutions[index][i];
			    xmlBlob += "\" index=\"";
			    xmlBlob += index.toString();
			    xmlBlob += "\"/>"; 
		    }
	    }
	    xmlBlob+= "</Outputs>";
	    xmlBlob+= "</TableEval>";

	    this.DebugMessage = xmlBlob;
	    this.DebugMessage  += "\n\n";
    }
    catch (err)
    {
        ReportError(err);
    }
}

RuleTable.prototype.EnableDebugging = function(enable)
{
    this.m_DEBUGGING = enable;
}

RuleTable.prototype.DebugMessage = "";

//TableSet/////////////////////////////////////////////////////////////
function TableSet()
{
    try
    {
        this.m_tables = new Array();
        this.m_inputAttrsByTable = new Array();
        this.m_inputDependenciesByTable = new Array();
        this.m_outputAttrsByTable = new Array();
        this.bInitialized = false;
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.AddTable = function(inputAttrsTests, outputAttrsValues, formulaInputs, stringMap, tableName, GetAll)
{
    try
    {
        var table = new RuleTable();        
        table.CreateRuleTable(inputAttrsTests, outputAttrsValues, formulaInputs, stringMap, tableName, GetAll);
        this.m_tables[tableName] = table;
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.Initialize = function()
{
    try
    {
        for (var tableName in this.m_tables)
        {
            this.LoadTableInfo(this.m_tables[tableName]);
        }
        this.bInitialized = true;
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.GetTable = function(tableName)
{
    try
    {
        return this.m_tables[tableName];
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.GetInputAttrs = function(tableName)
{
    try
    {
        return this.m_inputAttrsByTable[tableName];
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.GetInputDependencies = function(tableName)
{
    try
    {
        return this.m_inputDependenciesByTable[tableName];
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.GetOutputAttrs = function(tableName)
{
    try
    {
        return this.m_outputAttrsByTable[tableName];
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.Count = function()
{
    try
    {
        return ArraySize(this.m_tables);
    }
    catch (err)
    {
        ReportError(err);
    }
}

TableSet.prototype.LoadTableInfo = function(table) {
    try {
        //get the input info for this table
        var inputs = table.GetAllInputAttrNames();
        if (ArraySize(inputs) > 0)
            this.m_inputAttrsByTable[table.m_Name] = inputs;

        //outputs
        var outputs = table.GetAllOutputAttrNames();
        if (ArraySize(outputs) > 0)
            this.m_outputAttrsByTable[table.m_Name] = outputs;

        //dependancies
        var inputDeps = table.GetAllInputDependencies();
        for (var i = 0; i < outputs.length; i++) {
            //check for table chaining
            var values = table.GetAllPossibleOutputs(outputs[i]);
            for (var j = 0; j < values.length; j++) {
                var value = values[j];
                var evalIndex = value.indexOf("eval(");
                if (evalIndex >= 0) {
                    var cmdArgs = value.substr(evalIndex + 5, value.length - evalIndex - 6);
                    var args = cmdArgs.split(",");
                    if (args && args.length > 0) {                        
                        var chainInputs = this.ParseTablesAndChainsForInputs(args[0]); //recursive
                        for (var chainCnt = 0; chainCnt < chainInputs.length; chainCnt++) {
                            inputDeps.push(chainInputs[chainCnt]);
                        }
                    }
                }
            }
        }

        if (ArraySize(inputDeps) > 0)
            this.m_inputDependenciesByTable[table.m_Name] = inputDeps;
    }
    catch (err) {
        ReportError(err);
    }
}

TableSet.prototype.ParseTablesAndChainsForInputs = function(tableName)
{
    var retval = new Array();
    try {
        var table = this.m_tables[tableName];
        retval = table.GetAllInputDependencies();
        var outputs = table.GetAllOutputAttrNames();
        
        for (var i = 0; i < outputs.length; i++)
        {
            var values = table.GetAllPossibleOutputs(outputs[i]);
            for (var j = 0; j < values.length; j++)
            {
                var value = values[j];
                var evalIndex = value.indexOf("eval(");
                if (evalIndex >= 0)
                {
                    var cmdArgs = value.substr(evalIndex + 5, value.length - 6);
                    var args = cmdArgs.split(",");
                    if (args && args.length > 0)
                    {                        
                        var chainInputs = this.ParseTablesAndChainsForInputs(args[0]);
                        for (var chainCnt = 0; chainCnt < chainInputs.length; chainCnt++)
                        {
                            retval.push(chainInputs[chainCnt]);
                        }
                    }
                 }
            }
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

//KnowledgeBase/////////////////////////////////////////////
function loadXMLDoc(file)
{
    var bIsIE = false;
    if (window.XMLHttpRequest)
    {
        xhttp=new window.XMLHttpRequest();
    }
    else // Internet Explorer 5/6
    {
        xhttp=new ActiveXObject("Microsoft.XMLHTTP");      
        bIsIE = true;  
    }
    xhttp.open("GET",file,false);
    xhttp.send("");
    var xmlDoc = xhttp.responseXML;
    
    return xmlDoc;
}

function KnowledgeBase(xmlPath)
{
    var xmlDoc = null;
    
    try
    {
        this.m_TableSet = new TableSet();
        this.m_stringsMap = new Bimapper();
        this.mapBaseIDtoTranslations = new Array();
        this.m_GlobalInputAttrsValues = new Array();
        this.m_jsCode = "";
        this.iRecursingDepth = 0;
        this.m_IsOpen = false;
        this.m_DEBUGGING_MSGS = false;
        this.m_DebugTables = new Array();
        this.m_DebugHandlerFunct = null;
        xmlDoc = loadXMLDoc(xmlPath);
        
        //fill up the tableset
        var debug;
        var debugTables;
        var allTables;
        var inputList;
        var inputAttrsTests;
        var outputList;   
        var outputAttrsValues;  
        var formulaInputNodes;          
        var allTranslaions;
        var nodeJS;
        var bGetAll = false;
        
        if (IsIE() == false)
        {            
            debug = xmlDoc.evaluate("Tables/@debug", xmlDoc, null, XPathResult.STRING_TYPE, null).stringValue;
            debugTables = xmlDoc.evaluate("Tables/@debugtables", xmlDoc, null, XPathResult.STRING_TYPE, null).stringValue;
            allTables = xmlDoc.evaluate("Tables/Table", xmlDoc, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
            
            for (var i = 0; i < allTables.snapshotLength; i++)
            {
                var TableNode = allTables.snapshotItem(i);
                var tableName = TableNode.getAttribute("name");
                bGetAll = false;
                var getAll = TableNode.getAttribute("getall");
                if (getAll.toLowerCase().charAt(0) == 't')
                    bGetAll = true;
                var formulaInputs = new Array(); 
                inputList = xmlDoc.evaluate("Inputs", TableNode, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
                outputList = xmlDoc.evaluate("Outputs", TableNode, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
                formulaInputNodes = xmlDoc.evaluate("FormulaInput", TableNode, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
                if (formulaInputNodes)
                {
                    for (var j = 0; j < formulaInputNodes.snapshotLength; j++)
                    {                        
                        var formulaInputNode = formulaInputNodes.snapshotItem(j);
                        formulaInputs.push(formulaInputNode.textContent);
                    }
                }
                inputAttrsTests = this.GetTableRowFromXML(inputList, xmlDoc);
                outputAttrsValues = this.GetTableRowFromXML(outputList, xmlDoc);
                this.m_TableSet.AddTable(inputAttrsTests, outputAttrsValues, formulaInputs, this.m_stringsMap, tableName, bGetAll);    
                this.m_IsOpen = true;
            }
            
            allTranslaions = xmlDoc.evaluate("//Translations/String", xmlDoc, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
            if (allTranslaions)
            {
                for (var i = 0; i < allTranslaions.snapshotLength; i++)
                {
                    var translationNode = allTranslaions.snapshotItem(i);
                    var id = parseInt(translationNode.getAttribute("id"));
                    for (var j = 0; j < translationNode.attributes.length; j++)
                    {
                        var langType = translationNode.attributes[j].nodeName;                        
                        if (langType != "id")
                        {
                            var langValue = translationNode.attributes[j].nodeValue;  
                            if (this.mapBaseIDtoTranslations[id] != null)
                            {
                                var newTranslation = this.mapBaseIDtoTranslations[id];
                                newTranslation[langType] = langValue;
                                this.mapBaseIDtoTranslations[id] = newTranslation;
                            }
                            else
                            {                                
                                var newTranslation = new Array();
                                newTranslation[langType] = langValue;
                                this.mapBaseIDtoTranslations[id] = newTranslation;
                            }
                        }
                    }
                }
            }
            
            nodeJS = xmlDoc.evaluate("//Javascript", xmlDoc, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null);
            if (nodeJS != null)
            {
				this.m_jsCode = nodeJS.singleNodeValue.textContent + "\n";
			}			
        }
        else // Internet Explorer
        {
            debug = xmlDoc.selectSingleNode("Tables/@debug").value; 
            debugTables = xmlDoc.selectSingleNode("Tables/@debugtables").value;
            allTables = xmlDoc.selectNodes("Tables/Table");
            
            for (var i = 0; i < allTables.length; i++)
            {
                var TableNode = allTables[i];
                var tableName = TableNode.attributes.getNamedItem("name").nodeValue;
                bGetAll = false;
                var getAll = TableNode.attributes.getNamedItem("getall").nodeValue;
                if (getAll.toLowerCase().charAt(0) == 't')
                    bGetAll = true;
                var formulaInputs = new Array(); 
                inputList = TableNode.selectNodes("Inputs");     
                outputList = TableNode.selectNodes("Outputs");             
                formulaInputNodes = TableNode.selectNodes("FormulaInput");
                if (formulaInputNodes)
                {
                    for (var j = 0; j < formulaInputNodes.length; j++)
                    {
                        var formulaInputNode = formulaInputNodes[j];
                        formulaInputs.push(formulaInputNode.firstChild.text);
                    }
                }
                
                inputAttrsTests = this.GetTableRowFromXML(inputList, xmlDoc);
                outputAttrsValues = this.GetTableRowFromXML(outputList, xmlDoc);                       
                this.m_TableSet.AddTable(inputAttrsTests, outputAttrsValues, formulaInputs, this.m_stringsMap, tableName, bGetAll);
                this.m_IsOpen = true;
            }
            
            allTranslaions = xmlDoc.selectNodes("//Translations/String");
            if (allTranslaions)
            {
                for (var i = 0; i < allTranslaions.length; i++)
                {
                    var translationNode = allTranslaions[i];
                    var id = parseInt(translationNode.attributes.getNamedItem("id").nodeValue);
                    for (var j = 0; j < translationNode.attributes.length; j++)
                    {
                        var langType = translationNode.attributes[j].nodeName;
                        if (langType != "id")
                        {
                            var langValue = translationNode.attributes[j].nodeValue;
                            if (this.mapBaseIDtoTranslations[id] != null)
                            {
                                var newTranslation = this.mapBaseIDtoTranslations[id];
                                newTranslation[langType] = langValue;
                                this.mapBaseIDtoTranslations[id] = newTranslation;
                            }
                            else
                            {
                                var newTranslation = new Array();
                                newTranslation[langType] = langValue;
                                this.mapBaseIDtoTranslations[id] = newTranslation;
                            }
                        }
                    }
                }
            }
            
            nodeJS = xmlDoc.selectSingleNode("//Javascript");
            if (nodeJS != null)
				this.m_jsCode = nodeJS.text + "\n";
        }
        
        if (debug == "true")
	    {
		    this.m_DEBUGGING_MSGS = true;
		    
		    if (debugTables != null && debugTables.length > 0)
				this.m_DebugTables = debugTables.split(",");		    
	    }
        
        this.m_TableSet.Initialize();  
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.SetDebugHandler = function(func)
{
    this.m_DebugHandlerFunct = func;
}

KnowledgeBase.prototype.DebugThisTable = function(tableName)
{
    try
    {
        if (this.m_DEBUGGING_MSGS == true)
        {
            if (ArraySize(this.m_DebugTables) > 0)
            {
                if (GetIndexOfItem(this.m_DebugTables, tableName) >= 0)
                    return true;
                else
                    return false;
            }
            else
                return true;
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return false;
}

KnowledgeBase.prototype.IsOpen = function()
{
    try
    {
        return this.m_IsOpen;
    }
    catch(err)
    {
        ReportError(err);
    }    
}

KnowledgeBase.prototype.GetTableRowFromXML = function(nodes, xmlDoc)
{   
    var retval = new Array();
    try
    {
        if (IsIE() == false)
        {
            for (var i = 0; i < nodes.snapshotLength; i++)
            {
                var currentAttrRow = new Pair();
                var currrentInputAttr = nodes.snapshotItem(i);
                var values = xmlDoc.evaluate("Value", currrentInputAttr, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
                var attrNode = xmlDoc.evaluate("Attr", currrentInputAttr, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null);
                var attrName = "";
                if (attrNode != null && attrNode.singleNodeValue != null)
					attrName = attrNode.singleNodeValue.textContent;    
				if (attrName.length > 0)
				{            
					currentAttrRow.first = attrName;
					currentAttrRow.second = new Array();
					
					for (var j = 0; j < values.snapshotLength; j++)
					{                    
						var cell = new RuleCell();
						var currentValue = values.snapshotItem(j);
						var idValue = currentValue.getAttribute("id");
						if (idValue)
						{
							var cellValues = currentValue.textContent.split("|");
							var ids = idValue.split(",");
							if (cellValues.length != ids.length)
								throw "Bad OR";
								
							for (var idCnt = 0; idCnt < ids.length; idCnt++)
							{
								var id = parseInt(ids[idCnt], 10);
								var value = cellValues[idCnt];
								this.m_stringsMap.AddString(id, value);
								cell.Values.push(id); 
							}
						}

						var operValue = currentValue.getAttribute("operation");
						var oper = 0;
						if (operValue)
						{
							oper = parseInt(operValue, 10);
						}
						cell.Operation = oper;

						currentAttrRow.second.push(cell);
					}              
                    retval.push(currentAttrRow);        
                }        
            }
        }
        else
        {            
            for (var i = 0; i < nodes.length; i++)
            {
                var currentAttrRow = new Pair();
                var currrentInputAttr = nodes[i];
                var values = currrentInputAttr.selectNodes("Value");
                var attrNode = currrentInputAttr.selectSingleNode("Attr");
                var attrName = "";
                if (attrNode != null && attrNode.childNodes.length > 0)
                    attrName = attrNode.childNodes(0).nodeValue;
                if (attrName.length > 0)
                {
                    currentAttrRow.first = attrName;
                    currentAttrRow.second = new Array();
                    
                    for (var j = 0; j < values.length; j++)
                    {
                        var currentValue = values[j];
                        var idNode = currentValue.attributes.getNamedItem("id");
                        var cell = new RuleCell();
                        if (idNode)
                        {
                            var cellValues = currentValue.firstChild.text.split("|");
                            var ids = idNode.nodeValue.toString().split(",");
                            if (cellValues.length != ids.length)
                                throw "Bad OR";
                                
                            for (var idCnt = 0; idCnt < ids.length; idCnt++)
					        {
					            var id = parseInt(ids[idCnt], 10);
					            var value = cellValues[idCnt];
					            this.m_stringsMap.AddString(id, value);
					            cell.Values.push(id); 
					        }
                        }
                        
                        var operNode = currentValue.attributes.getNamedItem("operation");
                        var oper = 0;
                        if (operNode)
                        {
                            oper = parseInt(operNode.nodeValue, 10);
                        }
                        cell.Operation = oper;
                        
                        currentAttrRow.second.push(cell);     
                    }
                    retval.push(currentAttrRow);     
				}
            }
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}


KnowledgeBase.prototype.TableCount = function() 
{    
    try
    {
        return this.m_TableSet.Count();
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.TableIsGetAll = function(tableName)
{
    var table = this.m_TableSet.GetTable(tableName);
    return table.m_bGetAll;
}

KnowledgeBase.prototype.EvaluateTableForAttr = function(tableName, outputAttr)
{
    try
    {
        return this.EvaluateTableForAttrWithParam(tableName, outputAttr, "", this.TableIsGetAll(tableName));
    }
    catch (Error)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.EvaluateTableForAttrGet = function(tableName, outputAttr, bGetAll)
{    
    try
    {
        return this.EvaluateTableForAttrWithParam(tableName, outputAttr, "", bGetAll);
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.EvaluateTable = function(tableName)
{    
    try
    {
        return this.EvaluateTableWithParam(tableName, "", this.TableIsGetAll(tableName));
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.EvaluateTableGet = function(tableName, bGetAll)
{    
    try
    {
        return this.EvaluateTableWithParam(tableName, "", bGetAll);
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.EvaluateTableForAttrWithParam = function(tableName, outputAttr, param)
{
    try
    {
        return this.EvaluateTableForAttrWithParamGet(tableName, outputAttr, param, this.TableIsGetAll(tableName));
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.EvaluateTableForAttrWithParamGet = function(tableName, outputAttr, param, bGetAll) 
{
    var retval = new Array();
    try
    {        
        if (this.iRecursingDepth == 0)
            this.m_StateParameter = param;
        this.iRecursingDepth++;
        
        var table = this.m_TableSet.GetTable(tableName);
        table.EnableDebugging(this.DebugThisTable(tableName));

        table.SetInputValues(this.m_GlobalInputAttrsValues);
        
        var results = table.EvaluateTableForAttr(outputAttr, bGetAll, true);
        //check for existance of table chain
	    if (table.HasChain() == true)
	    {
            var eraseResults = new Array();
		    var newResults = new Array();
		    for (var i = 0; i < results.length; i++)
		    {
		        var text = results[i];
		        var evalIndex = text.indexOf("eval(")
			    if (evalIndex >= 0)
			    {
				    var cmd = text.substr(evalIndex + 5, text.length - evalIndex - 6);
				    var args = cmd.split(",");
				    var chainedResults = new Array();
				    if (args.length == 2)
				    {				        
					    var chainTableName = args[0].trim();
					    var chainAttrName = args[1].trim();
					    var debugVals = "";

					    chainedResults = this.EvaluateTableForAttrWithParam(chainTableName, chainAttrName, param, this.TableIsGetAll(chainTableName), true);
					    for (var j = 0; j < chainedResults.length; j++)
					    {
					        var result = chainedResults[j];
						    newResults.push(result);
						    if (this.DebugThisTable(chainTableName))
						    {
							    if (debugVals.length > 0)
								    debugVals+="|";
							    else
								    debugVals = ":";
								    
								debugVals += XMLSafe(result);
						    }					    
					    }
				    }
				    
				    if (this.DebugThisTable(tableName) && chainedResults.length > 0)
				    {   //replace the eval( string with the actual value
					    table.DebugMessage = table.DebugMessage.replace(text, text + debugVals);					
				    }
			    }
			    else
			    {
				    newResults.push(text);
			    }
		    }

		    results = new Array();
		    for (var i = 0; i < newResults.length; i++)
		    {
		        var result = newResults[i];
			    results.push(result);			
		    }
	    }
	    
	    //check for existance of runtime scripting, JavaScript only for web obviously
	    if (table.HasJS() == true)
	    {
	        var scriptTag = null;
		    var eraseResults = new Array();
		    var newResults = new Array();
		    for (var i = 0; i < results.length; i++)
		    {
		        var text = results[i];
                var evalIndex = text.indexOf("js(")
                if (evalIndex >= 0)
                {
                    var customCode = text.substr(evalIndex + 3, text.length - 4);                    
                    var val = "";
                    //everything must return as a string
                    var lines = customCode.split("\n");
                    //for brevity and clarity, add the return to a single line expression
                    if (lines.length == 1)
                    {
                        var returnIndex = lines[0].indexOf("return");
                        if (!(returnIndex >= 0))
                        {
                            lines[0] = "return (" + lines[0] + ");";
                        }
                    }
                    var codeBody = "{";
                    for (var j = 0; j < lines.length; j++)
                        codeBody += lines[j] + "\n";
                    codeBody += "}";
                    
                    var jsCode = this.m_jsCode + "\n" + "var param = \"" + this.m_StateParameter + "\";\n";
                    jsCode += "function myfunc()\n" + codeBody + "\n";
                    jsCode += "function getparam(){return param;}\n";
                    scriptTag = InstallScript(jsCode);
                    var res = myfunc();
                    val = res.toString();
                    this.m_StateParameter = getparam();
                    RemoveScriptTag(scriptTag);
                    newResults.push(val);                                    
                    if (this.DebugThisTable(tableName))
				    {	
				        var debugVals = ":" + XMLSafe(val);  
				        //replace the js( string with the actual value
					    table.DebugMessage = table.DebugMessage.replace(text, text + debugVals);					
				    }
                }
                else
                {
                    newResults.push(text);
                }
		    }

		    results = new Array();
		    for (var i = 0; i < newResults.length; i++)
		    {
		        var result = newResults[i];
			    results.push(result);			
		    }
	    }
	    
	    this.iRecursingDepth--;
	    
	    if (this.DebugThisTable(tableName) == true && this.m_DebugHandlerFunct != null)
	    {
		    this.m_DebugHandlerFunct(table.DebugMessage);
	    }
	
	    retval = results;	    
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

KnowledgeBase.prototype.EvaluateTableWithParam = function(tableName, param)
{
    try
    {
        return this.EvaluateTableWithParamGet(tableName, param, this.TableIsGetAll(tableName));
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.EvaluateTableWithParamGet = function(tableName, param, bGetAll) 
{
    var retval = new Array();
    try
    {
        var table = this.m_TableSet.GetTable(tableName);
        var outputAttrsValues = table.GetOutputAttrsValues();
        
        for (var i = 0; i < ArraySize(outputAttrsValues); i++)
        {
            var resPair = outputAttrsValues[i];
            var outputName = resPair.first;
            var result = this.EvaluateTableForAttrWithParam(tableName, outputName, param, bGetAll);
            retval[outputName] = result;
        }        
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

KnowledgeBase.prototype.ReverseEvaluateTable = function(tableName, bGetAll)
{
    var retval = new Array();
    try
    {
        var table = this.m_TableSet.GetTable(tableName);
        table.EnableDebugging(this.DebugThisTable(tableName));
        table.SetInputValues(this.m_GlobalInputAttrsValues);
        var outputCollection = table.GetInputAttrsTests();
        //for all the outputs get the results
        for (var i = 0; i < ArraySize(outputCollection); i++)
        {
            var resPair = outputCollection[i];
            var outputName = resPair.first;
            var result = table.EvaluateTableForAttr(outputName, bGetAll, false);
            retval[outputName] = result;
        }
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

KnowledgeBase.prototype.ReverseEvaluateTableForAttr = function(tableName, outputAttr)
{
    var retval = new Array();
    try
    {
        retval = ReverseEvaluateTableForAttrGet(tableName, outputAttr, this.TableIsGetAll(tableName));
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

KnowledgeBase.prototype.ReverseEvaluateTableForAttrGet = function(tableName, outputAttr, bGetAll)
{
    var retval = new Array();
    try
    {
        //no chaining or scripting in reverse        
        var table = this.m_TableSet.GetTable(tableName);
        table.EnableDebugging(this.DebugThisTable(tableName));
        table.SetInputValues(this.m_GlobalInputAttrsValues);
        retval = table.EvaluateTableForAttr(outputAttr, bGetAll, false);
    }
    catch (err)
    {
        ReportError(err);
    }
    return retval;
}

KnowledgeBase.prototype.GetEvalParameter = function () {
    return this.m_StateParameter;
}

KnowledgeBase.prototype.GetInputValueCount = function() {
    return ArraySize(this.m_GlobalInputAttrsValues);
}

KnowledgeBase.prototype.SetInputValue = function(valName, value)
{
    try
    {
        var id = this.m_stringsMap.GetIDByString(value);
        if (id == INVALID_STRING) //wasnt in our existing list
        {
            //add a new tokenized string
            id = this.m_stringsMap.AddUserString(value);
        }
        this.m_GlobalInputAttrsValues[valName] = id;
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.SetInputValues = function(values)
{
    try
    {
        this.m_GlobalInputAttrsValues = values;
    }
    catch(err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.ResetTable = function(tableName)
{
    try
    {
        this.m_stringsMap.ClearUserStrings();
        this.iRecursingDepth = 0;

        var table = this.m_TableSet.GetTable(tableName);
        table.ResetTable();
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.GetInputAttrs = function(tableName)
{
    try
    {
        return this.m_TableSet.GetInputAttrs(tableName);
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.GetInputDependencies = function(tableName)
{
    try
    {
        return this.m_TableSet.GetInputDependencies(tableName);
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.GetOutputAttrs = function(tableName)
{
    try
    {
        return this.m_TableSet.GetOutputAttrs(tableName);
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.GetAllPossibleOutputs = function(tableName, outputName)
{
    try
    {
        var table = this.m_TableSet.GetTable(tableName);
        return table.GetAllPossibleOutputs(outputName);
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.Localize = function(baseValue, locale)
{
    try
    {
        return this.Translate(baseValue, "", locale);
    }
    catch (err)
    {
        ReportError(err);
    }
}

KnowledgeBase.prototype.DeLocalize = function(localValue)
{
    try
    {
        for (var id in this.mapBaseIDtoTranslations)
        {
            for (var translations in this.mapBaseIDtoTranslations[id])
            {
                if (this.mapBaseIDtoTranslations[id][translations] == localValue)
                {
                    return this.m_stringsMap.GetStringByID(id);
                }                
            }
        }        
    }
    catch (err)
    {
        ReportError(err);
    }
    return localValue;
}

KnowledgeBase.prototype.Translate = function(source, sourceLocale, destLocale)
{
    try
    {
        var id = INVALID_STRING;
        if (sourceLocale.length == 0)
            id = this.m_stringsMap.GetIDByString(source);
        else
        {
            for (var base_id in this.mapBaseIDtoTranslations)
            {
                for (var translations in this.mapBaseIDtoTranslations[base_id])
                {
                    if (translations == sourceLocale && this.mapBaseIDtoTranslations[id][translations] == source)
                    {
                        id = base_id;
                        break;
                    }
                }
                if (id != INVALID_STRING) break;
            }
        }
        
        if (id != INVALID_STRING && this.mapBaseIDtoTranslations[id] != null)
        {
            for (var translations in this.mapBaseIDtoTranslations[id])
            {
                if (translations == destLocale)
                    return this.mapBaseIDtoTranslations[id][translations];
            }
        }        
    }
    catch (err)
    {
        ReportError(err);
    }
    return source;
}