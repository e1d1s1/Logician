package ROMViewer
{
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.TextEvent;
	
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule LLC
	 */
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule LLC
	 */
	import Logician.ROMNode;
	import ROMViewer.TreeNode;
	import mx.containers.Canvas;
	import mx.core.UIComponent;
	import mx.controls.CheckBox;
	import mx.controls.Tree;
	import mx.controls.Button;
	import mx.controls.Text;
    import mx.controls.TextArea;
	import mx.controls.TextInput;
	import mx.controls.RadioButton;
	import mx.controls.Alert;
	import mx.controls.DataGrid;
	import mx.controls.dataGridClasses.DataGridColumn;
	import mx.containers.HDividedBox;
    import flash.events.Event;

	public class ROMViewer extends UIComponent 
	{		
		public function ROMViewer() 
		{
			super();
		}
		
		private var layoutCanvas:Canvas;
		private var box:HDividedBox;
		private var treeCtrl:Tree;
		private var textCtrl:TextArea;
		private var btnGet:Button;
		private var btnSet:Button;
		private var txtName:TextInput;
		private var txtValue:TextInput;
		private var chkImmediate:CheckBox;
		private var btnQuery:Button;
		private var txtQuery:TextInput;
		private var chkQueryValue:CheckBox
		private var btnRefresh:Button;
		private var lbViewMode:Text;
		private var rbAttr:RadioButton;
		private var rbXML:RadioButton;
		private var grid:DataGrid;

		private var _rootNode:ROMNode;
		
		override protected function createChildren():void 
		{
			super.createChildren();
			
			if (!box)
			{
				box = new HDividedBox();
				box.percentWidth = 100;
				box.percentHeight = 100;
				box.width = 800;
				box.height = 640;
				addChild(box);
			}
			
			if (!treeCtrl)
			{
				treeCtrl = new Tree();
				treeCtrl.width = 150;
				treeCtrl.height = 640;
				treeCtrl.addEventListener("change", handleItemSelected);
				box.addChild(treeCtrl);
			}
			
			if (!layoutCanvas)
			{
				layoutCanvas = new Canvas();
				layoutCanvas.x = 155;
				layoutCanvas.width = 800 - 155;
				layoutCanvas.height = 640;	
				box.addChild(layoutCanvas);
			}			
			
			if (!textCtrl)
			{
				textCtrl = new TextArea();
				textCtrl.y = 120;
				textCtrl.width = layoutCanvas.width;
				textCtrl.height = 520;	
				textCtrl.visible = false;
				layoutCanvas.addChild(textCtrl);
			}
			
			if (!grid)
			{
				grid = new DataGrid();
				grid.y = 120;
				grid.width = layoutCanvas.width;
				grid.height = 480;	
				layoutCanvas.addChild(grid);
			}
			
			if (!btnGet)
			{
				btnGet = new Button();
				btnGet.height = 25;
				btnGet.width = 95;
				btnGet.label = "Get Attribute";
				btnGet.addEventListener("click", handleGet);
				layoutCanvas.addChild(btnGet);
			}
			
			if (!btnSet)
			{
				btnSet = new Button();
				btnSet.height = 25;
				btnSet.width = 95;
				btnSet.x = 100;
				btnSet.label = "Set Attribute";
				btnSet.addEventListener("click", handleSet);
				layoutCanvas.addChild(btnSet);
			}
			
			if (!txtName)
			{
				txtName = new TextInput();
				txtName.text = "name";
				txtName.width = 175;
				txtName.height = 25;
				txtName.x = 200;
				layoutCanvas.addChild(txtName);
			}
			
			if (!txtValue)
			{
				txtValue = new TextInput();
				txtValue.text = "value";
				txtValue.width = 175;
				txtValue.height = 25;
				txtValue.x = 380;
				layoutCanvas.addChild(txtValue);
			}
			
			if (!chkImmediate)
			{
				chkImmediate = new CheckBox();
				chkImmediate.x = 560;
				chkImmediate.label = "Immediate";
				layoutCanvas.addChild(chkImmediate);
			}
			
			//querey row
			if (!btnQuery)
			{
				btnQuery = new Button();
				btnQuery.height = 25;
				btnQuery.width = 95;
				btnQuery.y = 30;
				btnQuery.label = "XPATH Query";
				btnQuery.addEventListener("click", handleQuery);
				layoutCanvas.addChild(btnQuery);
			}
			
			if (!txtQuery)
			{
				txtQuery = new TextInput();
				txtQuery.text = "//Attribute[@id='attr1']/@value";
				txtQuery.width = 405;
				txtQuery.height = 25;
				txtQuery.x = 100;
				txtQuery.y = 30;
				layoutCanvas.addChild(txtQuery);
			}
			
			if (!chkQueryValue)
			{
				chkQueryValue = new CheckBox();
				chkQueryValue.x = 510;
				chkQueryValue.y = 30;
				chkQueryValue.label = "Query is for object(s)";
				layoutCanvas.addChild(chkQueryValue);
			}
			
			//refresh row
			if (!btnRefresh)
			{
				btnRefresh = new Button();
				btnRefresh.height = 25;
				btnRefresh.width = 95;
				btnRefresh.y = 60;
				btnRefresh.label = "Refresh";
				btnRefresh.addEventListener("click", handleRefresh);
				layoutCanvas.addChild(btnRefresh);
			}
			
			if (!lbViewMode)
			{
				lbViewMode = new Text();
				lbViewMode.height = 25;
				lbViewMode.width = 100;
				lbViewMode.x = 100;
				lbViewMode.y = 60;
				lbViewMode.text = "View Mode";
				layoutCanvas.addChild(lbViewMode);
			}
			
			if (!rbAttr)
			{
				rbAttr = new RadioButton();
				rbAttr.groupName = "View";
				rbAttr.id = "rbAttr";
				rbAttr.value = "Attribute";
				rbAttr.label = "Attribute";
				rbAttr.width = 75;
				rbAttr.height = 20;
				rbAttr.x = 100;
				rbAttr.y = 80;
				rbAttr.selected = true;
				rbAttr.addEventListener("click", handleAttr);
				layoutCanvas.addChild(rbAttr);
			}
			
			if (!rbXML)
			{
				rbXML = new RadioButton();
				rbXML.groupName = "View";
				rbXML.id = "rbXML";
				rbXML.value = "XML";
				rbXML.label = "XML";
				rbXML.x = 175;
				rbXML.y = 80;
				rbXML.width = 125;
				rbXML.height = 20;
				rbXML.selected = false;				
				rbXML.addEventListener("click", handleXML);
				layoutCanvas.addChild(rbXML);
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
		
		private function handleSet(eventObj:Event):void 
		{
			var attrName:String = txtName.text;
			var attrValue:String = txtValue.text;
			if (attrName.length > 0)
			{
				var item:TreeNode = treeCtrl.selectedItem as TreeNode;
				if (item != null)
				{
					var node:ROMNode = item.data as ROMNode;
					if (node != null)
					{
						var subAttrName:String = "value";
						var index:int = attrName.indexOf(':');
						if (index >= 0)
						{
							subAttrName = attrName.substr(index + 1);
							attrName = attrName.substr(0, index);
						}
						node.SetAttributeValue(attrName, subAttrName, attrValue);
						if (rbAttr.selected == true)
						{
							DisplayAttributeGrid(item);
						}
						else
						{
							DisplayXML(item);
						}
					}
				}
			}
		}
		
		private function handleGet(eventObj:Event):void 
		{
			var attrName:String = txtName.text;
			if (attrName.length > 0)
			{
				var item:TreeNode = treeCtrl.selectedItem as TreeNode;
				if (item != null)
				{
					var node:ROMNode = item.data as ROMNode;
					if (node != null)
					{
						var subAttrName:String = "value";
						var index:int = attrName.indexOf(':');
						if (index >= 0)
						{
							subAttrName = attrName.substr(index + 1);
							attrName = attrName.substr(0, index);
						}
						var immediate:Boolean = chkImmediate.selected;
						var value:String = node.GetAttributeValue(attrName, subAttrName, immediate);
						txtValue.text = value;
					}					
				}
			}
		}	
		
		
		private function handleQuery(eventObj:Event):void 
		{
			var qurey:String = txtQuery.text;
			if (qurey.length > 0)
			{
				var item:TreeNode = treeCtrl.selectedItem as TreeNode;
				if (item != null)
				{
					var node:ROMNode = item.data as ROMNode;
					if (node != null)
					{
						try
						{
							if (chkQueryValue.selected == true)
							{
								var results:Array = node.FindObjects(qurey);
								var msg:String = results.length.toString() + " Objects Found";
								Alert.show(msg);
								
								//highlight the objects in the tree
								for (var i:int = 0; i < results.length; i++ )
								{
									var nodeRes:ROMNode = results[i] as ROMNode;
									item = FindTreeItem(nodeRes);
									if (item != null)
									{
										SetTreeItemSelected(item);
									}
								}
							}
							else
							{
								var msgVal:String = "value: ";
								msgVal += node.EvaluateXPATH(qurey);
								Alert.show(msgVal);
							}
						}
						catch(err:Error)
						{
							Alert.show(err.message);
						}
					}
				}
			}
		}
		
		private function handleRefresh(eventObj:Event):void
		{
			Update();
			
			
		}
		
		private function handleAttr(eventObj:Event):void 
		{
			if (_rootNode == null)
				return;
				
			if (rbAttr != null && rbAttr.selected == true)
			{
				textCtrl.visible = false;
				grid.visible = true;
				var item:TreeNode = treeCtrl.selectedItem as TreeNode;
				if (item != null)
				{
					DisplayAttributeGrid(item);
				}
			}
		}
		
		private function handleXML(eventObj:Event):void 
		{
			if (_rootNode == null)
				return;
				
			if (rbXML != null && rbXML.selected == true)
			{
				textCtrl.visible = true;
				grid.visible = false;
				var item:TreeNode = treeCtrl.selectedItem as TreeNode;
				if (item != null)
				{
					DisplayXML(item);
				}
			}
		}
			
		public function SetROMNode(rom:ROMNode):void {
			_rootNode = rom;
			_rootNode.DebugCallback = Update;
		}
		public function GetROMNode():ROMNode {
			return _rootNode;
		}
		
		private function FindTreeItem(node:ROMNode):TreeNode
		{
			return null;
		}
		
		private function SetTreeItemSelected(item:TreeNode):void
		{
			
		}
		
		private function DisplayAttributeGrid(item:TreeNode):void
		{
			grid.dataProvider = null;
			if (item != null)
			{
				var node:ROMNode = item.data as ROMNode;
				if (node != null)
				{
					var table:Array = new Array();
					var allAttrs:Array = node.GetAllAttributes();
					
					for (var attr:String in allAttrs)
					{				
						var row:Array = new Array();
						var attrValuePair:Array = allAttrs[attr];
						var attrName:String = attr;
						var attrValue:String = attrValuePair["value"];
						row["attr"] = attrName;
						row["value"] = attrValue;
						var cnt:int = 0;
						for (var key:String in attrValuePair)
						{							
							if (key != "value")
							{								
								var colKey:String = "subvalue" + cnt.toString();
								var val:String = attrValuePair[key];
								row[colKey] = key + ": " +  val;
								cnt++;
							}							
						}	
						table.push(row);
					}
					
					FillGrid(table);
				}
			}
		}
		
		private function FillGrid(table:Array):void 
		{
			grid.dataProvider = null;
			grid.columns = new Array();		
			var cols:Array = grid.columns;
			
			var AdditionalCols:int = 0;
			var longestRow:Array = null;
			
			for (var i:int = 0; i < table.length; i++)
			{
				var row:Array = table[i] as Array;
				var cnt:int = 0;
				if (row != null)
				{					
					for (var rowKey:String in row)
					{
						cnt++;
						if (cnt > AdditionalCols)
						{
							longestRow = row;
							AdditionalCols = cnt;
						}
					}
				}
			}
			
			
			var attrCol:DataGridColumn = new DataGridColumn("attr");
			var valueCol:DataGridColumn = new DataGridColumn("value");
			cols.push(attrCol);
			cols.push(valueCol);
			for (var key:String in longestRow)
			{				
				if (key != "attr" && key != "value")
				{
					var newCol:DataGridColumn = new DataGridColumn(key);
					newCol.headerText = "sub-value";
					cols.push(newCol);
				}				
			}
			grid.columns = cols;
			
			grid.dataProvider = table;
		}
		
		private function DisplayXML(item:TreeNode):void
		{
			textCtrl.text = "";
			if (item != null)
			{
				var node:ROMNode = item.data as ROMNode;
				if (node != null)
				{
					var xml:String = node.SaveXML(true);
					textCtrl.text = xml;
				}
			}
		}
		
		public function Update():void 
		{
			if (_rootNode == null)
				return;
				
			var node:ROMNode = null;
			var item:TreeNode = treeCtrl.selectedItem as TreeNode;
			if (item != null)
				node = item.data as ROMNode;
				
			BuildTree();
			item = null;
			if (node != null)
				item = FindTreeItem(node);
				
			if (item != null)
			{
				SetTreeItemSelected(item);
				DisplayAttributeGrid(item);
			}
			else
			{
				if (treeCtrl.dataProvider != null && treeCtrl.dataProvider.length > 0)
				{
					SetTreeItemSelected(treeCtrl.dataProvider[0]);
					DisplayAttributeGrid(treeCtrl.dataProvider[0]);
				}
			}
		}
		
		private function handleItemSelected(event:Event):void 
		{
			var item:TreeNode = treeCtrl.selectedItem as TreeNode;
			if (item != null)
			{
				if (rbAttr.selected)
					DisplayAttributeGrid(item);
				else
					DisplayXML(item);
			}
		}
		
		private function BuildTree():void
		{
			treeCtrl.dataProvider = null;
			var caption:String = _rootNode.GetROMObjectID();
			var current:TreeNode = new TreeNode();
			current.label = caption;
			current.data = _rootNode;
			
			var children:Array = _rootNode.GetAllChildren(false);
			if (children != null && children.length > 0) for (var i:int = 0; i < children.length; i++)
			{
				var child:ROMNode = children[i] as ROMNode;
				AddBranchAndChildren(current, child);
			}			
			
			treeCtrl.dataProvider = current;			
		}
		
		private function AddBranchAndChildren(parent:TreeNode, node:ROMNode):void
		{
			var caption:String = node.GetROMObjectID();
			var current:TreeNode = null;
			if (parent != null)
			{
				current = new TreeNode();
				current.label = caption;
				current.data = node;
				parent.addChild(current);
			}
			
			var children:Array = node.GetAllChildren(false);
			if (children != null && children.length > 0) for (var i:int = 0; i < children.length; i++)
			{
				var child:ROMNode = children[i] as ROMNode;
				AddBranchAndChildren(current, child);
			}
		}

	}
	
}