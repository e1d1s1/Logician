package LogicianNative 
{
	
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule Solutions LLC
	 */
	import LogicianNative.Bimapper;
	import LogicianNative.TableSet;
	import LogicianNative.Pair;
	import flash.xml.*;
	import memorphic.xpath.*;
	public class KnowledgeBase 
	{
		private var xmlDoc:XML = null;
		private var m_TableSet:TableSet = new TableSet();
        private var m_stringsMap:Bimapper = new Bimapper();
        private var mapBaseIDtoTranslations:Array = new Array();
        private var m_GlobalInputAttrsValues:Array = new Array();
        private var m_jsCode:String = "";
        private var iRecursingDepth:int = 0;
        private var m_IsOpen:Boolean = false;
        private var m_DEBUGGING_MSGS:Boolean = false;
        private var m_DebugTables:Array = new Array();
        private var m_DebugHandlerFunct:Function = null;
        private var m_bGenerateMsg:Boolean = false;
        private var m_LastDebugMessage:String = "";
        private var m_guid:String = MakeGUID();
		private var m_StateParameter:String = "";
		
		public function KnowledgeBase() 
		{
			
		}
		
		private function _parseXML(xmlDoc:XML):Boolean
        {    
            //fill up the tableset
            var debug:String;
            var debugTables:String;
            var allTables:XMLList;
            var inputList:XMLList;
            var inputAttrsTests:Array;
            var outputList:XMLList;   
            var outputAttrsValues:Array;  
            var formulaInputNodes:XMLList;          
            var allTranslaions:XMLList;
            var nodeJS:XML;
            var bGetAll:Boolean = false;			
			
			var debugQuery:XPathQuery = new XPathQuery("Tables/@debug");
			debug = debugQuery.exec(xmlDoc);
			var debugTablesQuery:XPathQuery = new XPathQuery("Tables/@debugtables");
			debugTables = debugTablesQuery.exec(xmlDoc);
			var allTablesQuery:XPathQuery = new XPathQuery("Tables/Table");
			allTables = allTablesQuery.exec(xmlDoc);
	
			for (var i:uint = 0; i < allTables.length; i++)
			{
				var TableNode:XML = allTables[i];
				var tableName:String = TableNode.attributes["name"];
				bGetAll = false;
				var getAll:String = TableNode.attributes["getall"];
				if (getAll.toLowerCase().charAt(0) == 't')
					bGetAll = true;
				var formulaInputs:Array = new Array(); 
				var inputsQuery:XPathQuery = new XPathQuery("Inputs");
				inputList = inputsQuery.exec(TableNode);
				var outputsQuery:XPathQuery = new XPathQuery("Outputs");
				outputList = outputsQuery.exec(TableNode);
				var formulaInputQuery:XPathQuery = new XPathQuery("FormulaInput");
				formulaInputNodes = formulaInputQuery.exec(TableNode);
				if (formulaInputNodes)
				{
					for (var j:uint = 0; j < formulaInputNodes.length; j++)
					{
						var formulaInputNode:XML = formulaInputNodes[j];
						formulaInputs.push(formulaInputNode.firstChild.toString());
					}
				}
		
				inputAttrsTests = this.GetTableRowFromXML(inputList, xmlDoc);
				outputAttrsValues = this.GetTableRowFromXML(outputList, xmlDoc);                       
				this.m_TableSet.AddTable(inputAttrsTests, outputAttrsValues, formulaInputs, this.m_stringsMap, tableName, bGetAll);
				this.m_IsOpen = true;
			}
	
			var allTranslaionsQuery:XPathQuery = new XPathQuery("//Translations/String");			
			allTranslaions = allTranslaionsQuery.exec(xmlDoc);
			if (allTranslaions)
			{
				for (i = 0; i < allTranslaions.length; i++)
				{
					var translationNode:XML = allTranslaions[i];
					var id:uint = parseInt(translationNode.attributes.getNamedItem("id").nodeValue);
					for (j = 0; j < translationNode.attributes.length; j++)
					{
						var langType:String = translationNode.attributes[j].toString();
						if (langType != "id")
						{
							var langValue:String = translationNode.attributes[j].toString();
							if (this.mapBaseIDtoTranslations[id] != null)
							{
								var newTranslation:Array = this.mapBaseIDtoTranslations[id];
								newTranslation[langType] = langValue;
								this.mapBaseIDtoTranslations[id] = newTranslation;
							}
							else
							{
								newTranslation = new Array();
								newTranslation[langType] = langValue;
								this.mapBaseIDtoTranslations[id] = newTranslation;
							}
						}
					}
				}
			}
	
			var nodeJSQuery:XPathQuery = new XPathQuery("//Javascript");
			nodeJS = nodeJSQuery.exec(xmlDoc);
			if (nodeJS != null && nodeJS.length > 0)
				this.m_jsCode = nodeJS[0].toString() + "\n";
            
    
            if (debug == "true")
            {
	            this.m_DEBUGGING_MSGS = true;
	    
	            if (debugTables != null && debugTables.length > 0)
			        this.m_DebugTables = debugTables.split(",");		    
            }
    
            this.m_TableSet.Initialize();
            return true;
        }
		
		public function SetDebugHandler(func:Function):void
        {
            this.m_DebugHandlerFunct = func;
        }

        public function GenerateDebugMessages(bGenerate:Boolean):void
        {
            this.m_DEBUGGING_MSGS = bGenerate;
            this.m_bGenerateMsg = bGenerate;
            if (!this.m_bGenerateMsg)
                this.m_LastDebugMessage = "";
        }

        public function GetDebugMessages():String
        {
            var msg:String = this.m_LastDebugMessage;
            this.m_LastDebugMessage = "";
            return msg;
        }

        private function SendDebug(msg:String):void
        {
            if (this.m_bGenerateMsg == true)
            {
                this.m_LastDebugMessage += msg;
            }

            if (this.m_DebugHandlerFunct != null)
            {
                this.m_DebugHandlerFunct(msg);
            }            
        }

        private function DebugThisTable(tableName:String):Boolean
        {
            try
            {
                if (this.m_DEBUGGING_MSGS == true)
                {
                    if (ArraySize(this.m_DebugTables) > 0)
                    {
                        if (this.m_DebugTables.indexOf(tableName) >= 0)
                            return true;
                        else
                            return false;
                    }
                    else
                        return true;
                }
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return false;
        }

        public function IsOpen():Boolean
        {
            try
            {
                return this.m_IsOpen;
            }
            catch(err:Error)
            {
                ReportError(err);
            }    
			return false;
        }
		
		private function GetTableRowFromXML(nodes:XMLList, xmlDoc:XML):Array
        {   
            var retval:Array = new Array();
            try
            {               
				for (var i:uint = 0; i < nodes.length; i++)
				{
					var currentAttrRow:Pair = new Pair();
					var currrentInputAttr:XML = nodes[i];
					var valuesQuery:XPathQuery = new XPathQuery("Value");
					var values:XMLList = valuesQuery.exec(currrentInputAttr);
					var attrQuery:XPathQuery = new XPathQuery("Attr");
					var attrNodes:XMLList = attrQuery.exec(currrentInputAttr);
					var attrNode:XML = null;
					if (attrNodes.length > 0)
						attrNode = attrNodes[0];						
					var attrName:String = "";
					if (attrNode != null && attrNode.children().length > 0)
						attrName = attrNode.children()[0];
					if (attrName.length > 0)
					{
						currentAttrRow.first = attrName;
						currentAttrRow.second = new Array();
				
						for (var j:uint = 0; j < values.length; j++)
						{
							var currentValue:XML = values[j];
							var idNode:XML = currentValue.attributes["id"];
							var cell:RuleCell = new RuleCell();
							if (idNode && currentValue.children().length > 0)
							{
								var cellValues:Array = currentValue.children()[0].toString().split("|");
								var ids:Array = idNode.toString().split(",");
								if (cellValues.length != ids.length)
									throw "Bad OR";
							
								for (var idCnt:uint = 0; idCnt < ids.length; idCnt++)
								{
									var id:uint = parseInt(ids[idCnt], 10);
									var value:String = cellValues[idCnt];
									this.m_stringsMap.AddString(id, value);
									cell.Values.push(id); 
								}
							}
					
							var operNode:XML = currentValue.attributes["operation"];
							var oper:uint = 0;
							if (operNode)
							{
								oper = parseInt(operNode.toString(), 10);
							}
							cell.Operation = oper;
					
							currentAttrRow.second.push(cell);     
						}
						retval.push(currentAttrRow);     
					}
				}
                
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		private function MakeGUID():String
		{
			try 
			{
				var chars:Array = '0123456789abcdef'.split('');

				var uuid:Array = [], rnd:Function = Math.random, r:uint;
				uuid[8] = uuid[13] = uuid[18] = uuid[23] = '-';
				uuid[14] = '4'; // version 4

				for (var i:int = 0; i < 36; i++) {
					if (!uuid[i]) {
						r = 0 | rnd() * 16;
						uuid[i] = chars[(i == 19) ? (r & 0x3) | 0x8 : r & 0xf];
					}
				}

				return uuid.join('');
			}
			catch (err:Error) {
				ReportError(err);
			}
			return null;
		}
		
		public function TableCount():int
        {   
            try
            {
                return this.m_TableSet.Count();
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return 0;
        }

        public function TableIsGetAll(tableName:String):Boolean
        {
			try
			{
				var table:RuleTable = this.m_TableSet.GetTable(tableName);
				return table.GetAll();
			}
			catch (err:Error)
			{
				ReportError(err);
			}
			return false;
        }
		
		public function EvaluateTableForAttr(tableName:String, outputAttr:String, bGetAll:Boolean):Array
        {
            try 
            {
                if (bGetAll == undefined)
                    bGetAll = this.TableIsGetAll(tableName);
                return this.EvaluateTableForAttrWithParam(tableName, outputAttr, "", bGetAll);
            }
            catch (err:Error) {
                ReportError(err);
            }
			return null;
        }

        public function EvaluateTable(tableName:String, bGetAll:Boolean):Array
        {    
            try
            {
                if (bGetAll == undefined)
                    bGetAll = this.TableIsGetAll(tableName);
                return this.EvaluateTableWithParam(tableName, "", bGetAll);
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return null;
        }
		
		public function EvaluateTableForAttrWithParam(tableName:String, outputAttr:String, param:String, bGetAll:Boolean):Array
        {
            var retval:Array = new Array();
            try
            {
                if (bGetAll == undefined)
                    bGetAll = this.TableIsGetAll(tableName);
					
				var table:RuleTable = this.m_TableSet.GetTable(tableName);
				
                if (this.iRecursingDepth == 0)
                    this.m_StateParameter = param;
                this.iRecursingDepth++;
        
                table.EnableDebugging(this.DebugThisTable(tableName));
                table.SetInputValues(this.m_GlobalInputAttrsValues);
        
                var results:Array = table.EvaluateTableForAttr(outputAttr, bGetAll, true);
                //check for existance of table chain
	            if (table.HasChain() == true)
	            {
                    var eraseResults:Array = new Array();
		            var newResults:Array = new Array();
		            for (var i:int = 0; i < results.length; i++)
		            {
		                var text:String = results[i];
		                var evalIndex:int = text.indexOf("eval(")
			            if (evalIndex >= 0)
			            {
				            var cmd:String = text.substr(evalIndex + 5, text.length - evalIndex - 6);
				            var args:Array = cmd.split(",");
				            var chainedResults:Array = new Array();
				            if (args.length == 2)
				            {				        
					            var chainTableName:String = args[0].trim();
					            var chainAttrName:String = args[1].trim();
					            var debugVals:String = "";

					            chainedResults = this.EvaluateTableForAttrWithParam(chainTableName, chainAttrName, param, this.TableIsGetAll(chainTableName));
					            for (var j:int = 0; j < chainedResults.length; j++)
					            {
					                var result:String = chainedResults[j];
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
		                var result:String = newResults[i];
			            results.push(result);			
		            }
	            }
	    
	            //check for existance of runtime scripting, JavaScript only for web obviously
	            if (table.HasJS() == true)
	            {
	                var scriptTag:String = null;
		            var eraseResults:Array = new Array();
		            var newResults:Array = new Array();
		            for (var i = 0; i < results.length; i++)
		            {
		                var text:String = results[i];
                        var evalIndex:int = text.indexOf("js(")
                        if (evalIndex >= 0)
                        {
                            var customCode:String = text.substr(evalIndex + 3, text.length - 4);                    
                            var val:String = "";
                            //everything must return as a string
                            var lines:Array = customCode.split("\n");
                            //for brevity and clarity, add the return to a single line expression
                            if (lines.length == 1)
                            {
                                var returnIndex:int = lines[0].indexOf("return");
                                if (!(returnIndex >= 0))
                                {
                                    lines[0] = "return (" + lines[0] + ");";
                                }
                            }
                            var codeBody:String = "{";
                            for (var j:int = 0; j < lines.length; j++)
                                codeBody += lines[j] + "\n";
                            codeBody += "}";
                    
                            var jsCode:String = this.m_jsCode + "\n" + "var param = \"" + this.m_StateParameter + "\";\n";
                            jsCode += "function myfunc()\n" + codeBody + "\n";
                            jsCode += "function getparam(){return param;}\n";
                            scriptTag = InstallScript(jsCode);
                            var res:Object = myfunc();
                            val = res.toString();
                            this.m_StateParameter = getparam();
                            RemoveScriptTag(scriptTag);
                            newResults.push(val);                                    
                            if (this.DebugThisTable(tableName))
				            {	
				                var debugVals:String = ":" + XMLSafe(val);  
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
		                var result:String = newResults[i];
			            results.push(result);			
		            }
	            }
	    
	            this.iRecursingDepth--;
	    
	            if (this.DebugThisTable(tableName) == true)
	            {
		            this.SendDebug(table.DebugMessage);
	            }
	
	            retval = results;	    
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }
		
		public function EvaluateTableWithParam(tableName:String, param:String, bGetAll:Boolean):Array
        {
            var retval:Array = new Array();
            try
            {
                if (bGetAll == undefined)
                    bGetAll = this.TableIsGetAll(tableName);
                var table:RuleTable = this.m_TableSet.GetTable(tableName);
                var outputAttrsValues:Array = table.GetOutputAttrsValues();
        
                for (var i:int = 0; i < ArraySize(outputAttrsValues); i++)
                {
                    var resPair:Pair = outputAttrsValues[i];
                    var outputName:String = resPair.first.toString();
                    var result:Array = this.EvaluateTableForAttrWithParam(tableName, outputName, param, bGetAll);
                    retval[outputName] = result;
                }        
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
        }        
		
		
		
		private function InstallScript(script):String
		{
			var tag:Object = document.createElement("script");
			tag.type = "text/javascript";
			tag.text = script;
			tag.id = "customScript";
			document.getElementsByTagName("head").item(0).appendChild(tag);
			return tag.id;
		}
		
		private function RemoveScriptTag(id):void
		{
			var child:Object = document.getElementById(id);
			document.getElementsByTagName("head").item(0).removeChild(child);
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
		
		private function XMLSafe(str:String):String
		{
			var retval:String = "";
			try
			{
				//replace any illegal characters with escapes
				retval = str.replace("\"", "&quot;");
				retval = retval.replace("'", "&apos;");
				retval = retval.replace("<", "&lt;");
				retval = retval.replace(">", "&gt;");
				retval = retval.replace("&", "&amp;");
			}
			catch(err:Error)
			{
				ReportError(err);
			}
			return retval;
		}
	}

}