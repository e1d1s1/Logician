package Logician
{
	import flash.external.ExternalInterface;
	import mx.controls.Alert;
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
	public class ROMNode 
	{		
		public function ROMNode(id:String) { m_ROMNode = null; CreateROMNode(id); }		
		public function CreateROMNode(id:String):Boolean
		{
			if (id != null)
			{
				var o:Object = ExternalInterface.call("CreateROMNode", id);
				if (o != null)
				{
					m_ROMNode = o.m_guid;	
					m_id = o.m_id;
					DebugCallback = null;
					if (m_ROMNode != null)
						return true;
					else
						return false;
				}
			}
			return false;
		}
		private function _createROMNodeFromGUID(guid:String):ROMNode 
		{ 
			var retval:ROMNode = new ROMNode(""); 
			retval.m_ROMNode = guid; 
			var obj:Object = ExternalInterface.call("eval", "GetROMObjectGenericGUID(\"" + guid + "\");"); 
			if (obj != null) 
				retval.m_id = obj.m_id;
			else
				retval = null;
			return retval; }
		public function DestroyROMObject(guid:String):void { if (m_ROMNode != null) ExternalInterface.call("DestroyROMObject", m_ROMNode); m_ROMNode = null;}
		
		public var DebugCallback:Function = null;
		public function SetDebugging(set:Boolean):void { if (m_ROMNode != null) ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GenerateTableDebugMessages(" + set.toString() + ");"); }
		public function PumpDebugMessages():void
		{
			if (m_ROMNode != null && DebugCallback != null)
			{
				var o:Object = ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GetTableDebugMessages();");
				if (o != null)
				{
					var msg:String = o.toString();
					if (msg.length > 0)
						DebugCallback(msg);
				}
			}
		}
		
		//relational functions
		public function GetRoot():ROMNode { var retObject:Object = null; if (m_ROMNode != null) retObject = ExternalInterface.call("eval", "GetROMObjectGeneric(GetROMObject(\"" + m_ROMNode + "\").GetRoot());"); if (retObject != null)	return _createROMNodeFromGUID(retObject.m_guid); else return null;}
		public function Parent():ROMNode { var retObject:Object = null; if (m_ROMNode != null) retObject = ExternalInterface.call("eval", "GetROMObjectGeneric(GetROMObject(\"" + m_ROMNode + "\").Parent());"); if (retObject != null) return _createROMNodeFromGUID(retObject.m_guid); else return null;}
		public function GetAllChildren(recurs:Boolean):Array/*of ROMNode*/ { var retval:Array = new Array(); var retObjArray:Array = null; if (m_ROMNode != null) retObjArray = ExternalInterface.call("eval", "GetROMObjectArray(GetROMObject(\"" + m_ROMNode + "\").GetAllChildren(" + recurs.toString() + "));"); if (retObjArray != null) for (var idx:int = 0; idx < retObjArray.length; idx++ ) retval.push(_createROMNodeFromGUID(retObjArray[idx].m_guid)); return retval; }
		public function FindObjects(xpath:String):Array/*of ROMNode*/ { var retval:Array = new Array(); var retObjArray:Array = null; if (m_ROMNode != null) retObjArray = ExternalInterface.call("eval", "GetROMObjectArray(GetROMObject(\"" + m_ROMNode + "\").FindObjects(\"" + xpath + "\"));"); if (retObjArray != null) for (var idx:int = 0; idx < retObjArray.length; idx++ ) retval.push(_createROMNodeFromGUID(retObjArray[idx].m_guid)); return retval; }
		public function FindAllObjectsByID(id:String, recurs:Boolean):Array/*of ROMNode*/ { var retval:Array = new Array(); var retObjArray:Array = null; if (m_ROMNode != null) retObjArray = ExternalInterface.call("eval", "GetROMObjectArray(GetROMObject(\"" + m_ROMNode + "\").FindAllObjectsByID(\"" + id + "\", " + recurs.toString() + "));"); if (retObjArray != null) for (var idx:int = 0; idx < retObjArray.length; idx++ ) retval.push(_createROMNodeFromGUID(retObjArray[idx].m_guid)); return retval; }
		public function FindObjectByGUID(guid:String):ROMNode { var retObject:Object = null; if (m_ROMNode != null) retObject = ExternalInterface.call("eval", "GetROMObjectGeneric(GetROMObject(\"" + m_ROMNode + "\").FindObjectByGUID(\"" + guid + "\"));"); if (retObject != null) return _createROMNodeFromGUID(retObject.m_guid); else return null;}
		public function RemoveChildROMObject(child:ROMNode):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").RemoveChildROMObject(GetROMObject(\"" + child.m_ROMNode + "\"));"); else return false;}
		public function AddChildROMObject(child:ROMNode):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").AddChildROMObject(GetROMObject(\"" + child.m_ROMNode + "\"));"); else return false; }
		public function RemoveFromParent():Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").RemoveFromParent();"); else return false;}
		public function GetAllFriends():Array/*of ROMNode*/ { var retval:Array = new Array(); var retObjArray:Array = null; if (m_ROMNode != null) retObjArray = ExternalInterface.call("eval", "GetROMObjectArray(GetROMObject(\"" + m_ROMNode + "\").GetAllFriends());"); if (retObjArray != null) for (var idx:int = 0; idx < retObjArray.length; idx++ ) retval.push(_createROMNodeFromGUID(retObjArray[idx].m_guid)); return retval; }
		public function AddFriend(friendObj:ROMNode):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").AddFriend(GetROMObject(\"" + friendObj.m_ROMNode + "\"));"); else return false;}
		public function RemoveFriend(friendObj:ROMNode):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").RemoveFriend(GetROMObject(\"" + friendObj.m_ROMNode + "\"));"); else return false; }
		public function RemoveAllFriends():Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").RemoveAllFriends();"); else return false;}
		public function Clone():ROMNode { var retObject:Object = null; if (m_ROMNode != null) retObject = ExternalInterface.call("eval", "GetROMObjectGeneric(GetROMObject(\"" + m_ROMNode + "\").Clone());"); if (retObject != null) return _createROMNodeFromGUID(retObject.m_guid); else return null; }
		
		//attribute functions
		public function GetAttributeValue(id:String, name:String, immediate:Boolean):String { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GetAttributeValue(\"" + id + "\", \"" + name + "\", " + immediate.toString() + ");"); else return null;} 
		public function GetAttribute(id:String, immediate:Boolean):String { return GetAttributeValue(id, "value", immediate); }
		public function GetAttributeValueExists(id:String, name:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GetAttributeExists(\"" + id + "\", \"" + name + "\");"); else return false; }
		public function GetAttributeExists(id:String):Boolean { return GetAttributeValueExists(id, "value"); }
		public function SetAttributeValue(id:String, name:String, value:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").SetAttributeValue(\"" + id + "\", \"" + name + "\", \"" + value + "\");"); else return false;}
		public function SetAttribute(id:String, value:String):Boolean { return SetAttributeValue(id, "value", value); }
		public function RemoveAttributeValue(id:String, name:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").RemoveAttributeValue(\"" + id + "\", \"" + name + "\");"); else return false;}
		public function RemoveAttribute(id:String):Boolean { return RemoveAttributeValue(id, "value"); }
		public function SetROMObjectValue(name:String, value:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").SetROMObjectValue(\"" + name + "\", \"" + value + "\");"); else return false;}
		public function GetROMObjectValue(name:String):String {if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GetROMObjectValue(\"" + name + "\");"); else return null;}
		public function RemoveROMObjectValue(name:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").RemoveROMObjectValue(\"" + name + "\");"); else return false; }
		public function GetROMObjectID():String {if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GetROMObjectID();"); else return null;}
		public function SetROMObjectID(id:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").SetROMObjectID(\"" + id + "\");"); else return false; }
		public function GetROMGUID():String {if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GetROMGUID();"); else return null;}
		public function GetAllAttributes():Array { if (m_ROMNode != null) { var retval:Array = ExternalInterface.call("eval", "AttributeDictionaryToObjArray(GetROMObject(\"" + m_ROMNode + "\").GetAllAttributes());"); return ObjArrayToAttributeDictionary(retval); } else return null; }
		
		//rules
		public function LoadRules(knowledge_file:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").LoadRules(\"" + knowledge_file + "\");"); else return false;}
		public function LoadRulesFromString(xml:String):Boolean { if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").LoadRulesFromString(\"" + xml + "\");"); else return false; }
		public function EvaluateTableForAttrGet(evalTable:String, output:String, bGetAll:Boolean):Array { if (m_ROMNode != null) { var retval:Array = ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").EvaluateTableForAttr(\"" + evalTable + "\", \"" + output + "\", " + bGetAll.toString() + ");"); PumpDebugMessages(); return retval;} else return null; }
		public function EvaluateTableForAttr(evalTable:String, output:String):Array { if (m_ROMNode != null) { var retval:Array = ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").EvaluateTableForAttr(\"" + evalTable + "\", \"" + output + "\");"); PumpDebugMessages(); return retval;} else return null; }
		public function EvaluateTableGet(tableName:String, bGetAll:Boolean):Array { if (m_ROMNode != null) {var retval:Array = ExternalInterface.call("eval", "DictionaryToObjArray(GetROMObject(\"" + m_ROMNode + "\").EvaluateTable(\"" + tableName + "\", " + bGetAll.toString() + "));"); PumpDebugMessages(); return ObjArrayToDictionary(retval);} else return null; }
		public function EvaluateTable(tableName:String):Array { if (m_ROMNode != null) {var retval:Array = ExternalInterface.call("eval", "DictionaryToObjArray(GetROMObject(\"" + m_ROMNode + "\").EvaluateTable(\"" + tableName + "\"));"); PumpDebugMessages(); return ObjArrayToDictionary(retval);} else return null; }
		public function GetFirstTableResult(tableName:String, output:String):String { var retval:String = null; if (m_ROMNode != null) retval = ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").GetFirstTableResult(\"" + tableName + "\", \"" + output + "\");"); PumpDebugMessages(); return retval; }
		
		//IO
		public function SaveXML(indented:Boolean):String {if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").SaveXML(" + indented.toString() + ");"); else return null;}
		public function LoadXML(xmlStr:String):Boolean { if (m_ROMNode != null) { var retval:Boolean = ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").LoadXML(cleanString(decodeFromHex(\"" + encodeToHex(xmlStr) + "\")));"); m_ROMNode = ExternalInterface.call("UpdateROMGUID", m_ROMNode); return retval; } else return false; }
		
		//XPATH
		public function EvaluateXPATHForGUID(xpath:String, guid:String):String {if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").EvaluateXPATH(\"" + xpath + "\", \"" + guid + "\");"); else return null;}
		public function EvaluateXPATH(xpath:String):String {if (m_ROMNode != null) return ExternalInterface.call("eval", "GetROMObject(\"" + m_ROMNode + "\").EvaluateXPATH(\"" + xpath + "\");"); else return null;}
		
		private function encodeToHex(str:String):String{
			var r:String="";
			var e:int=str.length;
			var c:int=0;
			var h:String;
			while(c<e){
				h=str.charCodeAt(c++).toString(16);
				while(h.length<3) h="0"+h;
				r+=h;
			}
			return r;
		}
		
		private function decodeFromHex(str:String):String{
			var r:String="";
			var e:int=str.length;
			var s:int;
			while(e>=0){
				s=e-3;
				r=String.fromCharCode("0x"+str.substring(s,e))+r;
				e=s;
			}
			return r;
		}
		
		private function ObjArrayToAttributeDictionary(objArray:Array):Array
		{
			var retval:Array = new Array();
			if (objArray != null)
			{
				for (var i:int = 0; i < objArray.length; i++)			
				{
					var obj:Object = objArray[i];
					var key:String = obj.key;
					var attrValuePairs:Array = obj.values;
					if (key != null && key.length > 0)
					{
						var dict:Array = new Array();
						for (var j:int = 0; j < attrValuePairs.length - 1; j+=2)
						{
							dict[attrValuePairs[j]] = attrValuePairs[j + 1];
						}
						retval[key] = dict;
					}
				}
			}
			return retval;		
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
		
		public function GetGUID():String {return m_ROMNode;}
		
		private var m_ROMNode:String = null; //guid tracker
		private var m_id:String = null;
	}
}
