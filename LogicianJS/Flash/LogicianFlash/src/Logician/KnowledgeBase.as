package Logician
{
	import flash.external.ExternalInterface;
	/**
	 * ...
	 * @author Eric D. Schmidt, eLogician LLC
	 */
	public class KnowledgeBase 
	{		
		public function KnowledgeBase(knowledge_file:String) { m_KnowledgeBase = null;  CreateKnowledgeBase(knowledge_file); }		
		public function CreateKnowledgeBase(knowledge_file:String):Boolean
		{
			var o:Object = ExternalInterface.call("CreateKnowledgeBase", knowledge_file);
			if (o != null)
			{
				m_KnowledgeBase = o.m_guid;			
				DebugCallback = null;
				if (m_KnowledgeBase != null)
					return true;
				else
					return false;
			}
			return false;
		}
		public function CreateKnowledgeBaseFromString(knowledgeXML:String):Boolean
		{
			var o:Object = ExternalInterface.call("CreateKnowledgeBaseFromString", knowledgeXML);
			m_KnowledgeBase = o.m_guid;
			DebugCallback = null;
			if (m_KnowledgeBase != null)
				return true;
			else
				return false;
		}
		public function DestroyKnowledgeBase(guid:String):void { if (m_KnowledgeBase != null) ExternalInterface.call("DestroyKnowledgeBaseObject", m_KnowledgeBase); m_KnowledgeBase = null;}
		
		public var DebugCallback:Function = null;
		public function SetDebugging(set:Boolean):void { if (m_KnowledgeBase != null) ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").GenerateDebugMessages(" + set.toString() + ");");}
		
		public function TableCount():int { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").TableCount();"); else return 0; }
		public function IsOpen():Boolean { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").IsOpen();"); else return false; }
		public function TableHasScript(tableName:String):Boolean { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").TableHasScript(\"" + tableName + "\");"); else return false; }
		public function TableIsGetAll(tableName:String):Boolean { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").TableIsGetAll(\"" + tableName + "\");"); else return false; }
		
		public function EvaluateTableForAttrWithParam(tableName:String, outputAttr:String, param:String):Array { if (m_KnowledgeBase != null) { var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").EvaluateTableForAttrWithParam(\"" + tableName + "\", \"" + outputAttr + "\", \"" + param + "\");"); Debug(); return retval; } else return null; }
		public function EvaluateTableForAttrWithParamGet(tableName:String, outputAttr:String, param:String, bGetAll:Boolean):Array { if (m_KnowledgeBase != null) { var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").EvaluateTableForAttrWithParam(\"" + tableName + "\", \"" + outputAttr + "\", \"" + param + "\", " + bGetAll.toString() + ");"); Debug(); return retval;} else return null;}		
		public function EvaluateTableWithParam(tableName:String, param:String):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").EvaluateTableWithParam(\"" + tableName + "\", \"" + param + "\");"); Debug(); return ObjArrayToDictionary(retval);} else return null;}
		public function EvaluateTableWithParamGet(tableName:String, param:String, bGetAll:Boolean):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").EvaluateTableWithParam(\"" + tableName + "\", \"" + param + "\" , " + bGetAll.toString() + ");"); Debug(); return ObjArrayToDictionary(retval);} else return null; }
		public function EvaluateTableForAttr(tableName:String, outputAttr:String):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").EvaluateTableForAttr(\"" + tableName + "\", \"" + outputAttr + "\");"); Debug(); return retval;} else return null; }
		public function EvaluateTableForAttrGet(tableName:String, outputAttr:String, bGetAll:Boolean):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").EvaluateTableForAttr(\"" + tableName + "\", \"" + outputAttr + "\", " + bGetAll.toString() + ");"); Debug(); return retval;} else return null; }
		public function EvaluateTable(tableName:String):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "DictionaryToObjArray(GetObject(\"" + m_KnowledgeBase + "\").EvaluateTable(\"" + tableName + "\"));"); Debug(); return ObjArrayToDictionary(retval);} else return null; }
		public function EvaluateTableGet(tableName:String, bGetAll:Boolean):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "DictionaryToObjArray(GetObject(\"" + m_KnowledgeBase + "\").EvaluateTable(\"" + tableName + "\", " + bGetAll.toString() + "));"); Debug(); return ObjArrayToDictionary(retval);} else return null; }
		public function ReverseEvaluateTableForAttr(tableName:String, outputAttr:String):Array { if (m_KnowledgeBase != null) { var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").ReverseEvaluateTableForAttr(\"" + tableName + "\", \"" + outputAttr + "\");"); Debug(); return retval;} else return null; }
		public function ReverseEvaluateTableForAttrGet(tableName:String, outputAttr:String, bGetAll:Boolean):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").ReverseEvaluateTableForAttr(\"" + tableName + "\", \"" + outputAttr + "\", " + bGetAll.toString() + ");"); Debug(); return retval;} else return null; }
		public function ReverseEvaluateTable(tableName:String):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "DictionaryToObjArray(GetObject(\"" + m_KnowledgeBase + "\").ReverseEvaluateTable(\"" + tableName + "\"));"); Debug(); return ObjArrayToDictionary(retval);} else return null; }
		public function ReverseEvaluateTableGet(tableName:String, bGetAll:Boolean):Array { if (m_KnowledgeBase != null) {var retval:Array = ExternalInterface.call("eval", "DictionaryToObjArray(GetObject(\"" + m_KnowledgeBase + "\").ReverseEvaluateTable(\"" + tableName + "\", " + bGetAll.toString() + "));"); Debug(); return ObjArrayToDictionary(retval);} else return null; }
		public function GetEvalParameter():String { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").GetEvalParameter();"); else return null; }
		public function SetInputValue(name:String, value:String):void { if (m_KnowledgeBase != null) ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").SetInputValue(\"" + name + "\", \"" + value + "\");"); }
		public function ResetTable(tableName:String):void { if (m_KnowledgeBase != null) ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").ResetTable(\"" + tableName + "\");"); }
		
		public function GetInputAttrs(tableName:String):Array { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").GetInputAttrs(\"" + tableName + "\");"); else return null; }
		public function GetInputDependencies(tableName:String):Array { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").GetInputDependencies(\"" + tableName + "\");"); else return null; }
		public function GetOutputAttrs(tableName:String):Array { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").GetOutputAttrs(\"" + tableName + "\");"); else return null; }
		public function GetAllPossibleOutputs(tableName:String, outputAttr:String):Array { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").GetAllPossibleOutputs(\"" + tableName + "\", \""+ outputAttr + "\");"); else return null; }
		
		public function Localize(baseValue:String, locale:String):String { return Translate(baseValue, "", locale); }
		public function DeLocalize(localeValue:String):String { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").DeLocalize(\"" + localeValue + "\");"); else return null; }
		public function Translate(source:String, sourceLocale:String, destLocale:String):String { if (m_KnowledgeBase != null) return ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").Translate(\"" + source + "\", \"" + sourceLocale + "\", \"" + destLocale + "\");"); else return null; }
		
		private function Debug():void
		{
			if (m_KnowledgeBase != null && DebugCallback != null)
			{
				var msg:String = ExternalInterface.call("eval", "GetObject(\"" + m_KnowledgeBase + "\").GetDebugMessages();");
				if (msg != null && msg.length > 0)
				{
					DebugCallback(msg);
				}			
			}			
		}
		
		private function ObjArrayToDictionary(objArray:Array):Array 
		{
			var retval:Array = new Array();
			if (objArray != null) for (var i:int = 0; i < objArray.length; i++)
			{
				var obj:Object = objArray[i];
				var key:String = obj.key;
				var vals:Array = obj.values;
				if (key != null && key.length > 0)
					retval[key] = vals;
			}
			return retval;
		}
		
		private var m_KnowledgeBase:String = null; //guid tracker
	}

}