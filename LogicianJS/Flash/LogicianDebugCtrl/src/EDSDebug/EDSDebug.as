package EDSDebug 
{
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule Solutions LLC
	 */
	import flash.display.SimpleButton;
	import Logician.KnowledgeBase;
	import Logician.ROMNode;
	import mx.core.UIComponent;
	import mx.controls.Button;
    import mx.controls.TextArea;
	import mx.controls.TextInput;
	import mx.controls.RadioButton;
    import flash.events.Event;

	public class EDSDebug extends UIComponent 
	{		
		public function EDSDebug() 
		{
			super();
		}
		
		private var rbAll:RadioButton;
		private var rbSelected:RadioButton;
		private var txtTable:TextInput;
		private var btnClear:Button;		
		private var txtLog:TextArea;
		private var _Knowledgebase:KnowledgeBase;
		private var _rom:ROMNode;
		
		override protected function createChildren():void 
		{
			super.createChildren();
			
			if (!rbAll)
			{
				rbAll = new RadioButton();
				rbAll.groupName = "Mode";
				rbAll.id = "rbAll";
				rbAll.value = "All";
				rbAll.label = "All";
				rbAll.width = 50;
				rbAll.height = 20;
				rbAll.selected = true;
				addChild(rbAll);
			}
			
			if (!rbSelected)
			{
				rbSelected = new RadioButton();
				rbSelected.groupName = "Mode";
				rbSelected.id = "rbSelected";
				rbSelected.value = "Selected Table";
				rbSelected.label = "Selected Table";
				rbSelected.x = 55;
				rbSelected.width = 125;
				rbSelected.height = 20;
				rbSelected.selected = false;				
				addChild(rbSelected);
			}
			
			if (!txtTable)
			{
				txtTable = new TextInput();
				txtTable.x = 160;
				txtTable.width = 150;	
				txtTable.height = 20;
				addChild(txtTable);
			}
			
			if (!btnClear)
			{
				btnClear = new Button();
				btnClear.label = "Clear";
				btnClear.x = 325;
				btnClear.width = 50;
				btnClear.height = 20;
				btnClear.addEventListener("click", handleClear);
				addChild(btnClear);
			}
			
			if (!txtLog)
			{
				txtLog = new TextArea();
				txtLog.y = 25;
				txtLog.width = 640;	
				txtLog.height = 480;
				addChild(txtLog);
			}
		}
		
		override protected function commitProperties():void {
            super.commitProperties();
        }
		
		override protected function measure():void 
		{
			super.measure();	
		}
		
		override protected function updateDisplayList(unscaledWidth:Number, unscaledHeight:Number):void 
		{
            super.updateDisplayList(unscaledWidth, unscaledHeight);			
		}
		
		private function handleClear(eventObj:Event):void 
		{
			txtLog.text = "";
		}
		
		public function SetKnowledgeBase(knowledge:KnowledgeBase):void {
			_Knowledgebase = knowledge;
			_Knowledgebase.DebugCallback = Update;
		}
		public function GetKnowledgeBase():KnowledgeBase {
			return _Knowledgebase;
		}
		
		public function SetROMNode(rom:ROMNode):void {
			_rom = rom;
			_rom.DebugCallback = Update;
		}
		public function GetROMNode():ROMNode {
			return _rom;
		}
		
		public function Update(msg:String):void 
		{
			if (rbAll.selected)
			{
				txtLog.text += s + "\n";
			}
			else
			{
				var msgs:Array = msg.split("\n");
				for (var s:String in msgs)
				{
					var start:int = 17;//sizeof("<TableEval name=\"");
					var end:int = msg.indexOf('"', start);
					var tableName:String = s.substr(start, end - start);
					var filter:String = txtTable.text;
					if (rbAll.selected == true || filter == null || filter.length == 0 || filter.indexOf(tableName) >= 0)
					{
						txtLog.text += s + "\n";
					}
				}
			}
		}

	}

}