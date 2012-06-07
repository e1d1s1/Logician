package Logician
{
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule Solutions LLC
	This file is part of the LogicianFlash Library.
	Copyright (C) 2009 - 2011 Eric D. Schmidt

		LogicianFlash is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation, either version 3 of the License, or
		(at your option) any later version.

		LogicianFlash is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with LogicianFlash.  If not, see <http://www.gnu.org/licenses/>.
	*/
	import flash.external.ExternalInterface;
	
	public class LinearEngine
	{
		public function LinearEngine(node:ROMNode, dictionaryTable:String) { CreateLinearEngine(node, dictionaryTable); }
		public function CreateLinearEngine(node:ROMNode, dictionaryTable:String):Boolean
		{
			DebugCallback = null;
			m_LinearEngine = null;
			m_LinearEngine = ExternalInterface.call("CreateLinearEngineByGUID", node.GetGUID(), dictionaryTable);			
			if (m_LinearEngine != null)
				return true;
			else
				return false;			
			return false;
		}
		
		public var DebugCallback:Function = null;
		public function SetDebugging(set:Boolean):void { if (m_LinearEngine != null) ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").GenerateTableDebugMessages(" + set.toString() + ");"); }
		public function PumpDebugMessages():void
		{
			if (m_LinearEngine != null && DebugCallback != null)
			{
				var o:Object = ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").GetTableDebugMessages();");
				if (o != null)
				{
					var msg:String = o.toString();
					if (msg.length > 0)
						DebugCallback(msg);
				}
			}
		}
		
		public function LoadDictionary(dictionaryTable:String):void { if (m_LinearEngine != null) ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").LoadDictionary(\"" + dictionaryTable + "\");"); }
		public function GetDictionaryAttr(dictAttrName:String):ROMDictionaryAttribute { if (m_LinearEngine != null) return ObjectToAttr(ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").GetDictionaryAttr(\"" + dictAttrName + "\");")); else return null; }
		public function GetAllDictionaryAttrs():Array { if (m_LinearEngine != null) return ArrayToAttrArray(ExternalInterface.call("eval", "GetROMDictAttrArray(GetEngineObject(\"" + m_LinearEngine + "\").GetAllDictionaryAttrs());")); else return null; }
		public function EvaluateForAttributeAndDependentsArray(dictAttrName:String, newValues:Array, bEvalDependents:Boolean):void { if (m_LinearEngine != null) ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").EvaluateForAttribute(\"" + dictAttrName + "\", \"" + ArrayToDelimitedString(newValues) + "\".split(\"|\"), " + bEvalDependents.toString() + ");"); }
		public function EvaluateForAttributeArray(dictAttrName:String, newValues:Array):void { EvaluateForAttributeAndDependentsArray(dictAttrName, newValues, true); }
		public function EvaluateForAttributeAndDependents(dictAttrName:String, newValue:String, bEvalDependents:Boolean):void { if (m_LinearEngine != null) ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").EvaluateForAttribute(\"" + dictAttrName + "\", \"" + newValue + "\", " + bEvalDependents.toString() + ");"); }
		public function EvaluateForAttribute(dictAttrName:String, newValue:String):void { EvaluateForAttributeAndDependents(dictAttrName, newValue, true); }
		public function EvaluateAll():void { if (m_LinearEngine != null) ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").EvaluateAll();"); }
		public function GetEvalList():Array { if (m_LinearEngine != null) return ArrayToAttrArray(ExternalInterface.call("eval", "GetROMDictAttrArray(GetEngineObject(\"" + m_LinearEngine + "\").GetEvalList());")); else return null; }
		public function GetTriggers():Array { if (m_LinearEngine != null) return ObjArrayToAttrDict(ExternalInterface.call("eval", "DictionaryToObjArray(GetEngineObject(\"" + m_LinearEngine + "\").GetTriggers());")); else return null; }
		public function DictionaryIsValid():Boolean { if (m_LinearEngine != null) return ExternalInterface.call("eval", "GetEngineObject(\"" + m_LinearEngine + "\").DictionaryIsValid();"); else return false; }
		public function GetInvalidateMode():int { if (m_ROMDictionaryAttribute != null) { var retval:int = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").InvalidateMode;"); return retval;} else return 0; }
		public function SetInvalidateMode(type:int):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").InvalidateMode = " + type + ";");}
		public function GetResetBehavior():int { if (m_ROMDictionaryAttribute != null) { var retval:int = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").ResetBehavior;"); return retval;} else return 0; }
		public function SetResetBehavior(type:int):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").ResetBehavior = " + type + ";");}
		public function GetTrackUserBehavior():int { if (m_ROMDictionaryAttribute != null) { var retval:int = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").TrackUserBehavior;"); return retval;} else return 0; }
		public function SetTrackUserBehavior(type:int):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").TrackUserBehavior = " + type + ";");}
				
		private function ObjectToAttr(obj:Object):ROMDictionaryAttribute
		{
			var retval:ROMDictionaryAttribute = new ROMDictionaryAttribute();
			if (obj != null)
			{
				retval.Name = obj.Name;
				retval.Description = obj.Description;
				retval.DefaultValue = obj.DefaultValue;
				retval.RuleTable = obj.RuleTable;
				retval.AttributeType = obj.AttributeType;
				retval.ValueChanged = obj.ValueChanged;
				retval.ChangedByUser = obj.ChangedByUser;
				retval.Valid = obj.Valid;
				retval.Visible = obj.Visible;
				retval.Enabled = obj.Enabled;
				retval.PossibleValues = obj.PossibleValues;
				retval.AvailableValues = obj.AvailableValues;
				retval.Index = obj.Index;
				retval.SetGUID(obj.m_guid);
			}
			else
				retval = null;
			return retval;
		}
		
		private function ArrayToAttrArray(arr:Array):Array
		{
			var retval:Array = new Array();
			if (arr != null)
			{
				for (var i:int = 0; i < arr.length; i++)
				{
					var attr:ROMDictionaryAttribute = ObjectToAttr(arr[i]);
					retval.push(attr);			
				}			
			}
			return retval;
		}
		
		private function ArrayToDelimitedString(arr:Array):String 
		{
			var retval:String = "";
			for (var i:int = 0; i < arr.length; i++)
			{
				retval += arr[i];
				if (i < arr.length - 1)
					retval += "|";
			}
			return retval;
		}
		
		private function ObjArrayToAttrDict(objArray:Array):Array 
		{
			var retval:Array = new Array();
			if (objArray != null) for (var i:int = 0; i < objArray.length; i++)
			{
				var obj:Object = objArray[i];
				var key:String = obj.key;
				var values:Array = obj.values; //array of objects
				var attrArray:Array = new Array();
				for (var j:int = 0; j < values.length; j++)
				{
					var attrName:Object = values[j];
					if (attrName != null) 
						attrArray.push(attrName);
				}
				if (key != null && key.length > 0 && attrArray != null)
					retval[key] = attrArray;
			}
			return retval;
		}
		
		private var m_LinearEngine:String = null;
	}

}