package Logician
{	
	/**
	 * ...
	 * @author Eric D. Schmidt, eLogician LLC
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
	 
	public class ROMDictionary 
	{		
		public function ROMDictionary(node:ROMNode) { CreateROMDictionary(node); }
		public function CreateROMDictionary(node:ROMNode):Boolean
		{
			DebugCallback = null;
			m_ROMDictionary = null;
			m_ROMDictionary = ExternalInterface.call("CreateROMDictionaryByGUID", node.GetGUID());			
			if (m_ROMDictionary != null)
				return true;
			else
				return false;			
			return false;
		}
		
		public var DebugCallback:Function = null;
		public function SetDebugging(set:Boolean):void { if (m_ROMDictionary != null) ExternalInterface.call("eval", "GetROMDictObject(\"" + m_ROMDictionary + "\").GenerateTableDebugMessages(" + set.toString() + ");"); }
		public function PumpDebugMessages():void
		{
			if (m_ROMDictionary != null && DebugCallback != null)
			{
				var o:Object = ExternalInterface.call("eval", "GetROMDictObject(\"" + m_ROMDictionary + "\").GetTableDebugMessages();");
				if (o != null)
				{
					var msg:String = o.toString();
					if (msg.length > 0)
						DebugCallback(msg);
				}
			}
		}
		
		public function LoadDictionary(dictionaryTable:String):void { if (m_ROMDictionary != null) ExternalInterface.call("eval", "GetROMDictObject(\"" + m_ROMDictionary + "\").LoadDictionary(\"" + dictionaryTable + "\");"); }
		public function GetDictionaryAttr(dictAttrName:String):ROMDictionaryAttribute { if (m_ROMDictionary != null) return ObjectToAttr(ExternalInterface.call("eval", "GetROMDictObject(\"" + m_ROMDictionary + "\").GetDictionaryAttr(\"" + dictAttrName + "\");")); else return null; }
		public function GetAllDictionaryAttrs():Array { if (m_ROMDictionary != null) return ArrayToAttrArray(ExternalInterface.call("eval", "GetROMDictAttrArray(GetROMDictObject(\"" + m_ROMDictionary + "\").GetAllDictionaryAttrs());")); else return null; }
		
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
		
		private var m_ROMDictionary:String = null;
		
	}
	
	

}