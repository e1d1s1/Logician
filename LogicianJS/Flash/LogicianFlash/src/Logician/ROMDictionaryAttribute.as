package Logician
{
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule LLC
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
	public class ROMDictionaryAttribute 
	{
		
		public function ROMDictionaryAttribute() { m_ROMDictionaryAttribute = null;	}		
		public function CreateROMDictionaryAttribute():Boolean
		{
			var o:Object = ExternalInterface.call("CreateROMDictionaryAttribute");
			if (o != null)
			{
				m_ROMDictionaryAttribute = o.m_guid;	
				if (m_ROMDictionaryAttribute != null)
					return true;
				else
					return false;
			}			
			return false;
		}
		
		public function GetName():String { if (m_ROMDictionaryAttribute != null) { var retval:String = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Name;"); Name = retval; return retval; } else return null; }
		public function SetName(name:String):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Name = \"" + name + "\";"); Name = name; }
		public function GetDescription():String { if (m_ROMDictionaryAttribute != null) { var retval:String = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Description;"); Description = retval; return retval;} else return null; }
		public function SetDescription(desc:String):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Description = \"" + desc + "\";"); Description = desc; }
		public function GetDefaultValue():String { if (m_ROMDictionaryAttribute != null) { var retval:String = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").DefaultValue;"); DefaultValue = retval; return retval;} else return null; }
		public function SetDefaultValue(value:String):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").DefaultValue = \"" + value + "\";"); DefaultValue = value; }
		public function GetRuleTable():String { if (m_ROMDictionaryAttribute != null) { var retval:String = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").RuleTable;"); RuleTable = retval; return retval;} else return null; }
		public function SetRuleTable(table:String):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").RuleTable = \"" + table + "\";"); RuleTable = table; }
		public function GetAttributeType():int { if (m_ROMDictionaryAttribute != null) { var retval:int = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").AttributeType;"); AttributeType = retval; return retval;} else return 0; }
		public function SetAttributeType(type:int):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").AttributeType = " + type + ";"); AttributeType = type; }
		public function GetValueChanged():Boolean { if (m_ROMDictionaryAttribute != null) { var retval:Boolean = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").ValueChanged;"); ValueChanged = retval; return retval; } else return false; }
		public function SetValueChanged(bval:Boolean):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").ValueChanged = " + bval.toString() + ";"); ValueChanged = bval; }
		public function GetChangedByUser():Boolean { if (m_ROMDictionaryAttribute != null) { var retval:Boolean = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").ChangedByUser;"); ChangedByUser = retval; return retval;} else return false; }
		public function SetChangedByUser(bval:Boolean):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").ChangedByUser = " + bval.toString() + ";"); ChangedByUser = bval; }		
		public function GetValid():Boolean { if (m_ROMDictionaryAttribute != null) { var retval:Boolean = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Valid;"); Valid = retval; return retval;} else return false; }
		public function SetValid(bval:Boolean):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Valid = " + bval.toString() + ";"); Valid = bval; }
		public function GetVisible():Boolean { if (m_ROMDictionaryAttribute != null) { var retval:Boolean = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Visible;"); Visible = retval; return retval;} else return false; }
		public function SetVisible(bval:Boolean):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Visible = " + bval.toString() + ";"); Visible = bval; }
		public function GetEnabled():Boolean { if (m_ROMDictionaryAttribute != null) { var retval:Boolean = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Enabled;"); Enabled = retval; return retval;} else return false; }
		public function SetEnabled(bval:Boolean):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").Enabled = " + bval.toString() + ";"); Enabled = bval; }
		public function GetPossibleValues():Array/*of string*/ { if (m_ROMDictionaryAttribute != null) { var retval:Array = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").PossibleValues;"); PossibleValues = retval; return retval;} else return null; }
		public function SetPossibleValues(values:Array):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").PossibleValues = \"" + ArrayToDelimitedString(values) + "\".split(\"|\");"); PossibleValues = values; }
		public function GetAvailableValues():Array/*of string*/ { if (m_ROMDictionaryAttribute != null) { var retval:Array = ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").AvailableValues;"); AvailableValues = retval; return retval;} else return null; }
		public function SetAvailableValues(values:Array):void { if (m_ROMDictionaryAttribute != null) ExternalInterface.call("eval", "GetROMDictAttrObject(\"" + m_ROMDictionaryAttribute + "\").AvailableValues = \"" + ArrayToDelimitedString(values) + "\".split(\"|\");"); AvailableValues = values; }
		
		public function SetGUID(guid:String):void { m_ROMDictionaryAttribute = guid; }
		
		public var Name:String = "";
		public var Description:String = "";
		public var DefaultValue:String = "";
		public var RuleTable:String = "";
		public var AttributeType:int = 0;
		public var ValueChanged:Boolean = false;
		public var ChangedByUser:Boolean = false;
		public var Valid:Boolean = false;
		public var Visible:Boolean = false;
		public var Enabled:Boolean = false;
		public var PossibleValues:Array = new Array();
		public var AvailableValues:Array = new Array();
		
		private var m_ROMDictionaryAttribute:String = null;
		
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
	}

}