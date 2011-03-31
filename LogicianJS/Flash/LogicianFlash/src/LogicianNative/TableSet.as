package LogicianNative 
{
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule Solutions LLC
	 */
	import LogicianNative.Bimapper;
	import LogicianNative.RuleTable;	
	public class TableSet 
	{
		private var m_tables:Array = new Array();
        private var m_inputAttrsByTable:Array = new Array();
        private var m_inputDependenciesByTable:Array = new Array();
        private var m_outputAttrsByTable:Array = new Array();
        private var bInitialized:Boolean = false;
		
		public function TableSet() 
		{
			
		}
		
		public function AddTable(inputAttrsTests:Array, outputAttrsValues:Array, formulaInputs:Array, stringMap:Bimapper, tableName:String, GetAll:Boolean):void {
            try {
                var table:RuleTable = new RuleTable();
                table.CreateRuleTable(inputAttrsTests, outputAttrsValues, formulaInputs, stringMap, tableName, GetAll);
                this.m_tables[tableName] = table;
            }
            catch (err:Error) {
                ReportError(err);
            }
        }
		
		public function Initialize():void 
		{
            try {
                for (var tableName:Object in this.m_tables) {
                    this.LoadTableInfo(this.m_tables[tableName]);
                }
                this.bInitialized = true;
            }
            catch (err:Error) {
                ReportError(err);
            }
        }
		
		public function GetTable(tableName:String):RuleTable
		{
            try {
                return this.m_tables[tableName];
            }
            catch (err:Error) {
                ReportError(err);
            }
			return null;
        }
		
		public function GetInputAttrs(tableName:String):Array
		{
            try {
                return this.m_inputAttrsByTable[tableName];
            }
            catch (err:Error) {
                ReportError(err);
            }
			return null;
        }
		
		public function GetInputDependencies(tableName:String):Array
		{
            try {
                return this.m_inputDependenciesByTable[tableName];
            }
            catch (err:Error) {
                ReportError(err);
            }
			return null;
        }
		
		public function GetOutputAttrs(tableName:String):Array
		{
            try {
                return this.m_outputAttrsByTable[tableName];
            }
            catch (err:Error) {
                ReportError(err);
            }
			return null;
        }
		
		public function Count():int
		{
            try {
                return ArraySize(this.m_tables);
            }
            catch (err:Error) {
                ReportError(err);
            }
			return 0;
        }
		
		public function LoadTableInfo(table:RuleTable):void
		{
            try {
                //get the input info for this table
                var inputs:Array = table.GetAllInputAttrNames();
                if (ArraySize(inputs) > 0)
                    this.m_inputAttrsByTable[table.Name()] = inputs;

                //outputs
                var outputs:Array = table.GetAllOutputAttrNames();
                if (ArraySize(outputs) > 0)
                    this.m_outputAttrsByTable[table.Name()] = outputs;

                //dependancies
                var inputDeps:Array = table.GetAllInputDependencies();
                for (var i:int = 0; i < outputs.length; i++) {
                    //check for table chaining
                    var values:Array = table.GetAllPossibleOutputs(outputs[i]);
                    for (var j:int = 0; j < values.length; j++) {
                        var value:String = values[j];
                        var evalIndex:int = value.indexOf("eval(");
                        if (evalIndex >= 0) {
                            var cmdArgs:String = value.substr(evalIndex + 5, value.length - evalIndex - 6);
                            var args:Array = cmdArgs.split(",");
                            if (args && args.length > 0) {
                                var chainInputs:Array = this.ParseTablesAndChainsForInputs(args[0]); //recursive
                                for (var chainCnt:int = 0; chainCnt < chainInputs.length; chainCnt++) {
                                    inputDeps.push(chainInputs[chainCnt]);
                                }
                            }
                        }
                    }
                }

                if (ArraySize(inputDeps) > 0)
                    this.m_inputDependenciesByTable[table.Name()] = inputDeps;
            }
            catch (err:Error) {
                ReportError(err);
            }
        }
		
		private function ParseTablesAndChainsForInputs(tableName:String):Array 
		{
            var retval:Array = new Array();
            try {
                var table:RuleTable = this.m_tables[tableName];
                retval = table.GetAllInputDependencies();
                var outputs:Array = table.GetAllOutputAttrNames();

                for (var i:int = 0; i < outputs.length; i++) {
                    var values:Array = table.GetAllPossibleOutputs(outputs[i]);
                    for (var j:int = 0; j < values.length; j++) {
                        var value:String = values[j];
                        var evalIndex:int = value.indexOf("eval(");
                        if (evalIndex >= 0) {
                            var cmdArgs:String = value.substr(evalIndex + 5, value.length - 6);
                            var args:Array = cmdArgs.split(",");
                            if (args && args.length > 0) {
                                var chainInputs:Array = this.ParseTablesAndChainsForInputs(args[0]);
                                for (var chainCnt:int = 0; chainCnt < chainInputs.length; chainCnt++) {
                                    retval.push(chainInputs[chainCnt]);
                                }
                            }
                        }
                    }
                }
            }
            catch (err:Error) {
                ReportError(err);
            }
            return retval;
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