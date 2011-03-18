package LogicianNative 
{	
	/**
	 * ...
	 * @author Eric D. Schmidt, eLogician LLC
	 */
	import LogicianNative.Bimapper;
	import LogicianNative.RuleCell;
	public class Decode 
	{
		public const EQUALS:int = 0x1;
		public const NOT_EQUAL:int = 0x2;
		public const LESS_THAN:int = 0x4;
		public const LESS_THAN_EQUAL:int = 0x8;
		public const GREATER_THAN:int = 0x10;
		public const GREATER_THAN_EQUAL:int = 0x20;
		public const RANGE_INCLUSIVE:int = 0x40;
		public const RANGE_END_INCLUSIVE:int = 0x80;
		public const RANGE_START_INCLUSIVE:int = 0x100;
		public const RANGE_NOT_INCLUSIVE:int = 0x200;
		public const PYTHON:int = 0x400; //not used on web
		public const GETS:int = 0x800;
		public const CHAIN:int = 0x1000;
		public const JAVASCRIPT:int = 0x2000;	
		
		public const INVALID_STRING:int = 0;
		public const EMPTY_STRING:int = 1;
		public const EXPLICIT_NULL_STRING:int = 2;
		
		public function Decode() 
		{
			
		}
		
		public function CreateOutputDecoder(outputCell:RuleCell, inputValues:Array, stringMap:Bimapper):void
        {
            try
            {
                this.m_tests = outputCell.Values;
	            this.m_stringsMap = stringMap;
	            this.m_operator = outputCell.Operation;
	            this.m_inputValues = inputValues;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
        }

        public function CreateInputDecoder(currentValue:int, inputCell:RuleCell, inputValues:Array, stringMap:Bimapper):void
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
            catch (err:Error)
            {
                ReportError(err);
            }
        }

        public function EvaluateInputCell():Boolean
        {
            var retval:Boolean = true;
            try
            {            
                //check by operator
		        //no string compares needed here for speed
		        if (this.m_operator & EQUALS)
		        {
			        for (var i:int = 0; i < ArraySize(this.m_tests); i++) //possible OR
			        {
				        var test:int = this.m_tests[i];

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
			        for (i = 0; i < ArraySize(this.m_tests); i++) //possible OR
			        {
				        test = this.m_tests[i];

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
			        var currentTest:String = this.GetString(this.m_tests[0]);
			        var currentValue:String = this.GetString(this.m_value);
			        var bIsNum:Boolean = false;

			        if (isNumber(currentTest) == true && isNumber(currentValue) == true)
			        {
				        bIsNum = true;
			        }
			        var dCurrentValue:Number = 0.0;
			        var dTestValue:Number = 0.0;

            
			        if (bIsNum)
			        {
				        try
				        {
					        dCurrentValue = parseFloat(currentValue);
					        dTestValue = parseFloat(currentTest);
				        }
				        catch(err:Error)
				        {
					        trace("Could not create numeric values from string data\n" + err);
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
			        var testString:String = this.GetString(this.m_tests[0]);
			        currentValue = this.GetString(this.m_value);
			        var min:Number = 0, max:Number = 0; dCurrentValue = 0;
			        var parts:Array = testString.split(",");

			        if (parts.length == 2)
			        {
				        try
				        {
					        min = parseFloat(parts[0]);
					        max = parseFloat(parts[1]);
					        if (currentValue != null && currentValue.length > 0)
					            dCurrentValue = parseFloat(currentValue);
				        }
				        catch(err:Error)
				        {
					        trace("Could not create numeric values from string data\n" + err);
					        throw new Error("Could not create numeric values from string data\n" + err);
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
		            trace("scripting not supported as an input") ;
			        throw new Error("scripting not supported as an input");
		        }		
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }

        public function EvaluateOutputCell():Array
        {
            var retval:Array = new Array();
            try
            {        
                for (var i:int = 0; i < ArraySize(this.m_tests); i++)
                {
                    var value:int = this.m_tests[i];
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
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }

        public function GetString(id:int):String
        {
            try
            {
                if (id > EMPTY_STRING)
	            {
		            var s:String = this.m_stringsMap.GetStringByID(id);
		            if(s.length > 0)
			            return s;
		            else
		            {
			            throw "string not found for index: " + id.toString();
		            }
	            }	            
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return "";
        }

        public function ParseStringForGets(id:int, bForceZero:Boolean):String
        {
            var retval:String = "";
            try
            {
	            var fullString:String = this.GetString(id);
	            //replace with the actual value
	            var getIndex:int = fullString.indexOf("get(");
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
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }

        public function ReplaceAGet(s:String, bForceZero:Boolean):String
        {
            var retval:String = "";
            try
            {
	            //find the get(xxx) substring.  attrName xxx is between ().
	            var iStartPos:int = s.indexOf("get(", 0);
	            var iEndPos:int = s.indexOf(")", iStartPos);
	            if (iStartPos >= 0 && iEndPos > iStartPos)
	            {
	                var attrName:String = s.substr(iStartPos + 4, iEndPos - iStartPos - 4);
		            var getText:String = "get(" + attrName + ")";
		            //get the value of the input attr
		            var bFoundAttr:Boolean = false;
		            if (this.m_inputValues != null && ArraySize(this.m_inputValues) > 0)
		            {
		                var valueIndex:Object = this.m_inputValues[attrName];
			            if (valueIndex != null)
			            {
				            var value:String = this.GetString(int(valueIndex));
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
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }

        public function CheckForInputGets():void
        {
            try
            {
                if (this.m_operator & GETS)
	            for (var i:int = 0; i < ArraySize(this.m_tests); i++)
	            {
		            //replace the GET with the actual value (pay a performance penalty here, but convienient)		
		            var val:String = this.ParseStringForGets(this.m_tests[i], false);
		            this.m_tests[i] = this.m_stringsMap.GetIDByString(val);		
	            }
            }
            catch (err:Error)
            {
                ReportError(err);
            }
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
		
		private var numb:String = '0123456789';
		private var lwr:String = 'abcdefghijklmnopqrstuvwxyz';
		private var upr:String = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
		private function isNumber(parm:String):Boolean {return isValid(parm,numb);}
		private function isLower(parm:String):Boolean {return isValid(parm,lwr);}
		private function isUpper(parm:String):Boolean {return isValid(parm,upr);}
		private function isAlpha(parm:String):Boolean {return isValid(parm,lwr+upr);}
		private function isValid(parm:String, val:String):Boolean {
			if (parm == "") return true;
			for (var i:int=0; i<parm.length; i++) {
			if (val.indexOf(parm.charAt(i),0) == -1) return false;
			}
			return true;
		}
		
		private var m_value:int = 0;
        private var m_tests:Array = new Array();
        private var m_operator:int = EQUALS;
        private var m_stringsMap:Bimapper = new Bimapper();
        private var m_inputValues:Array = new Array();
	}

}