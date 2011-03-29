package LogicianNative 
{
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule LLC
	 */
	import LogicianNative.Bimapper;
	public class RuleTable 
	{
		public var DebugMessage:String = "";
		private var m_InputAttrsValues:Array = new Array();
		private var m_InputAttrsTests:Array = new Array();
		private var m_FormulaInputs:Array = new Array();
		private var m_OutputAttrsValues:Array = new Array();
		private var m_DEBUGGING:Boolean = false;
		
		private var m_Name:String = "";
		private var m_Tests:int = 0;
		private var m_stringsMap:Bimapper = new Bimapper();
		private var m_bGetAll:Boolean = false;
		private var bHasChain:Boolean = false;
		private var bHasJS:Boolean = false; //eval(.....
		private var bNullSet:Boolean = false;
		
		private const PYTHON:int = 0x400; //not used on web
		private const GETS:int = 0x800;
		private const CHAIN:int = 0x1000;
		private const JAVASCRIPT:int = 0x2000;	
		private const INVALID_STRING:int = 0;
		private const EMPTY_STRING:int = 1;
		private const EXPLICIT_NULL_STRING:int = 2;
			
		public function RuleTable() 
		{
			
		}
		
		public function Name():String { return m_Name; }
		public function GetAll():Boolean { return m_bGetAll; }
		
		public function CreateRuleTable(inputAttrsTests:Array, outputAttrsValues:Array, formulaInputs:Array, stringMap:Bimapper, name:String, GetAll:Boolean):void
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
            catch (err:Error)
            {
                ReportError(err);
            }
        }
		
		public function EvaluateTable(bGetAll:Boolean, bForward:Boolean):Array
        {
            var retval:Array = new Array();
            try
            {
                var resultCollection:Array = null;
                if (bForward)
                    resultCollection = this.m_OutputAttrsValues;
                else
                    resultCollection = this.m_InputAttrsTests;
            
                //for all the outputs get the results
                for (var i:int = 0; i < ArraySize(resultCollection); i++)
                {
                    var outputAttrValuePair:Array = resultCollection[i];
                    var result:Array = this.EvaluateTableForAttr(outputAttrValuePair.first, bGetAll, bForward);
                    retval[outputAttrValuePair.first] = result;
                }        
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		public function EvaluateTableForAttr(outputAttr:String, bGetAll:Boolean, bForward:Boolean):Array
        {
            var retval:Array = new Array();
            try
            {        
	            var values:Array = new Array();
	            var solutions:Array = new Array()
	            var colResults:Array = new Array(this.m_Tests);
	            for (var j:int = 0; j < colResults.length; j++)
	                colResults[j] = true;
	    
	            var inputCollection:Array = null;
	            var outputCollection:Array = null;
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
		            for (var i:int = 0; i < ArraySize(inputCollection); i++)
		            {
		                var inputAttrPair:Array = inputCollection[i];
			            var attr:String = inputAttrPair.first;
			            var currentInputAttrValuesIndex:Object = this.m_InputAttrsValues[attr];
			            if (currentInputAttrValuesIndex != null)
			            {
				            values.push(currentInputAttrValuesIndex as int);
			            }
		            }

		            //sweep down the table for all inputs and do test(s)
		            var bHaveSolution:Boolean = true;
		            var colResultsDefault:Array = new Array(this.m_Tests);
		            for (j = 0; j < colResultsDefault.length; j++)
		                colResultsDefault[j] = false;
		            colResults = colResultsDefault;
		            for (var testCnt:int = 0; testCnt < this.m_Tests; testCnt++)
		            {
			            //sweep through the inputs			    
			            var inputCnt:int = 0;
			            for (i = 0; i < ArraySize(inputCollection); i++)
			            {
			                inputAttrPair = inputCollection[i];
				            if (testCnt < ArraySize(inputAttrPair.second))
				            {
				                var decoder:Decode = new Decode();
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
	            var results:Array = new Array();
	            for (var result:int = 0; result < ArraySize(outputCollection); result++)
	            {
		            if (outputCollection[result].first == outputAttr)
		            {
			            results = outputCollection[result].second;
		            }
	            }

	            //for each true result, add to the solution array
	            for (result = 0; result < this.m_Tests; result++)
	            {
		            if (colResults[result] && result < ArraySize(results))
		            {
		                var outputCell:RuleCell = results[result];
			            decoder = new Decode();
			            decoder.CreateOutputDecoder(outputCell, this.m_InputAttrsValues, this.m_stringsMap);
			            if (outputCell.Operation & CHAIN)
				            this.bHasChain = true;
			            if (outputCell.Operation & JAVASCRIPT)
			            {
    			            this.bHasJS = true;
				        }
			            var cellOutputs:Array = decoder.EvaluateOutputCell();
			            for (i = 0; i < cellOutputs.length; i++)
			            {
			                var cellOuputValue:String = cellOutputs[i];
				            retval.push(cellOuputValue);	
				            if (this.m_DEBUGGING)
				            {
					            var solnVals:Object = solutions[result];
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
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		public function SetInputValues(values:Array):void
        {
            try
            {
                this.bNullSet = false;
                this.m_InputAttrsValues = values;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
        }
		
		public function SetInputValue(name:String, value:String):void
        {
            try
            {
                this.bNullSet = false;

                if (ArraySize(this.m_InputAttrsTests) > 0)
                {
                    var bFoundTableInput:Boolean = false;
                    for (var i:int = 0; i < ArraySize(this.m_InputAttrsTests); i++)
                    {
                        if (this.m_InputAttrsTests[i].first == name)
                        {
                            bFoundTableInput = true;
                            var id:int = this.m_stringsMap.GetIDByString(value);
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
			            id = this.m_stringsMap.GetIDByString(value);
			            if (id == INVALID_STRING) //wasnt in our existing list
			            {
				            //add a new tokenized string
				            id = this.m_stringsMap.AddUserString(value);
			            }
			            this.m_InputAttrsValues[name] = id;
		            }
                }
            }
            catch (err:Error)
            {
                ReportError(err);
            }
        }
		
		public function SetInvalidAttrs():void
        {
            try
            {
                if (!this.bNullSet)
                {
                    for (var i:int = 0; i < ArraySize(this.m_InputAttrsTests); i++)
                    {
                        var ruleInput:Object = this.m_InputAttrsTests[i];
                        var ruleName:String = ruleInput.first;
                        if (this.m_InputAttrsValues[ruleName] == null)
                        {
                            this.m_InputAttrsValues[ruleName] = INVALID_STRING;
                        }                
                    }
                }
                this.bNullSet = true;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
        }
		
		public function ResetTable():void
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
            catch (err:Error)
            {
                ReportError(err);
            }
        }
		
		public function HasChain():Boolean
        {
            try
            {
                return this.bHasChain;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return false;
        }
		
		public function HasJS():Boolean
        {
            try
            {
                return this.bHasJS;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return false;
        }
		
		public function GetAllOutputAttrNames():Array
        {
            var retval:Array = new Array();
            try
            {
                for (var i:int = 0; i < this.m_OutputAttrsValues.length; i++)
                {
                    retval.push(this.m_OutputAttrsValues[i].first);
                }
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		public function GetAllInputAttrNames():Array
        {
            var retval:Array = new Array();
            try
            {
                for (var i:int = 0; i < this.m_InputAttrsTests.length; i++)
                {
                    retval.push(this.m_InputAttrsTests[i].first);
                }
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		public function GetAllInputDependencies():Array //input columns + formulas/gets
        {
            var retval:Array = new Array();
            try
            {
                retval = this.GetAllInputAttrNames();
                for (var i:int = 0; i < this.m_FormulaInputs.length; i++)
                {
                    retval.push(this.m_FormulaInputs[i]);
                }
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		public function GetAllPossibleOutputs(outputName:String):Array
        {
            var retval:Array = new Array();
            try
            {
                var uniqueList:Array = new Array();
                for (var i:int = 0; i < this.m_OutputAttrsValues.length; i++)
                {
                    var outputAttrPair:Array = this.m_OutputAttrsValues[i];
                    if (outputAttrPair.first == outputName)
                    {   
                        var cells:Array = outputAttrPair.second; //array of RuleCell
                        for (var cellCnt:int = 0; cellCnt < cells.length; cellCnt++)
                        {
                            var values:Array = cells[cellCnt].Values;
                            for (var j:int = 0; j < values.length; j++)
                            {   
                                uniqueList[values[j]] = values[j];
                            }
                        }
                        break;
                    }
                }
        
                for (var value:Object in uniqueList)
                {
                    retval.push(this.m_stringsMap.GetStringByID(value as int));
                }
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		public function GetOutputAttrsValues():Array
        {
            try
            {
                return this.m_OutputAttrsValues;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return null;
        }
		
		public function GetInputAttrsTests():Array
        {
            try
            {
                return this.m_InputAttrsTests;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return null;
        }
		
		private function DebugEval(outputAttr:String, inputValues:Array, solutions:Array):void
        {
            try
            {
                var xmlBlob:String = "";
	            xmlBlob += "<TableEval name=\"";
	            xmlBlob += this.m_Name;
	            xmlBlob += "\" output=\"";
	            xmlBlob += outputAttr;
	            xmlBlob += "\">";
    	
	            if (this.m_InputAttrsTests.length == inputValues.length)
	            {
		            xmlBlob += "<Inputs>";
		            for (var i:int = 0; i < this.m_InputAttrsTests.length; i++)
		            {
		                var currentPair:Array = this.m_InputAttrsTests[i];
			            var attr:String = currentPair.first;
			            var value:String = this.m_stringsMap.GetStringByID(inputValues[i]);			
			            xmlBlob += "<Input name = \"";
			            xmlBlob += attr;
			            xmlBlob += "\" value=\"";
			            xmlBlob += value;
			            xmlBlob += "\"/>"; 
		            }
		            xmlBlob += "</Inputs>";
	            } //else something is wrong

	            xmlBlob+= "<Outputs>";
	            for (var index:Object in solutions)
	            {	        
	                for (i = 0; i < solutions[index].length; i++)
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
            catch (err:Error)
            {
                ReportError(err);
            }
        }
		
		public function EnableDebugging(enable:Boolean):void
        {
            this.m_DEBUGGING = enable;
        }
		
		private function ReportError(err:Error):void
		{
			trace(err.message);
			throw err;
		}
		
		private function ArraySize(arr:Array):int //for associative array sizes
		{
			var l:int = 0;
			for (var k:Object in arr) {
				l++;
			}
			return l;
		}
	}

}