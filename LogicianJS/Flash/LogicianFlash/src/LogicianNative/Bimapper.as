package LogicianNative 
{
	/**
	 * ...
	 * @author Eric D. Schmidt, eLogician LLC
	 */
	public class Bimapper 
	{	
		public const INVALID_STRING:int = 0;
		public const EMPTY_STRING:int = 1;
		public const EXPLICIT_NULL_STRING:int = 2;
		
		public function Bimapper() 
		{
			
		}
		
		public function AddString(id:int, s:String):void
		{
			try
            {
                if (id > this.maxID)
                    this.maxID = id;
                this.m_IndexToStringsMap[id] = s;
                this.m_StringsToIndexMap[s] = id;
            }
            catch (err:Error)
            {
                ReportError(err);
            }		
		}
		
		public function AddUserString(s:String):int
		{
			try
            {
                var retval:int = this.maxID + 1;
	            this.m_IndexToStringsMap[retval] = s;
	            this.m_StringsToIndexMap[s] = retval;
	            this.userStrings.push(retval);
	            this.maxID = retval;
	            return retval;
            }
            catch (err:Error)
            {
                ReportError(err);
            }
			return 0;
		}
		
		public function GetStringByID(id:int):String
		{
			var retval:String = "";
            try
            {        
                for (var i:Object in this.m_IndexToStringsMap)
                {
                    if (i.toString() == id)
                    {
                        retval = this.m_IndexToStringsMap[id];
                        break;
                    }
                }
        
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
		}
		
		public function GetIDByString(s:String):int
		{
			var retval:int = INVALID_STRING;
            try
            {    
		        if (s.length == 0)
		        {
			        retval = EMPTY_STRING;
		        }
		        else for (var i:String in this.m_StringsToIndexMap)
                {
                    if (i == s)
                    {
                        retval = this.m_StringsToIndexMap[s];
                        break;
                    }
                }        
            }
            catch (err:Error)
            {
                ReportError(err);
            }
            return retval;
		}
		
		public function ClearUserStrings():void
		{
			try
            {
                for (var i:int = 0; i < ArraySize(userStrings); i++)
                {
                    var id:int = this.userStrings[i];
                    var s:String = this.GetStringByID(id);
            
                    if (m_StringsToIndexMap[s] != null)
                        delete m_StringsToIndexMap[s];
                
                    if (m_IndexToStringsMap[id] != null)
                        delete m_IndexToStringsMap[id];
                }
        
                this.userStrings = new Array();
            }
            catch (err:Error)
            {
                ReportError(err);
            }
		}
		
		private var m_IndexToStringsMap:Array = new Array();
		private var m_StringsToIndexMap:Array = new Array();
		private var userStrings:Array = new Array();
		private var maxID:int = 0;		
		
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