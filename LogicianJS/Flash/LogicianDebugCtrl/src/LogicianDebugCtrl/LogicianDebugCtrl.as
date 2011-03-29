package LogicianDebugCtrl 
{
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule LLC
	 */
	import EDSDebug.EDSDebug;
	import ROMViewer.ROMViewer;
	import Logician.ROMNode;
	import mx.core.UIComponent;
	import mx.containers.VDividedBox;
	import mx.containers.Canvas;
	public class LogicianDebugCtrl extends UIComponent 
	{		
		private var edsDebugCtrl:EDSDebug;
		private var romViewerCtrl:ROMViewer;
		private var topCanvas:Canvas;
		private var botCanvas:Canvas;
		private var box:VDividedBox;
		public function LogicianDebugCtrl() 
		{
			super();
		}
		
		override protected function createChildren():void 
		{
			super.createChildren();
			
			if (!box)
			{
				box = new VDividedBox();
				box.percentWidth = 100;
				box.percentHeight = 100;
				box.height = 960;
				box.width = 800;
				addChild(box);
			}
			
			if (!topCanvas)
			{
				topCanvas = new Canvas();
				topCanvas.width = 800;
				topCanvas.height = 640;
				box.addChild(topCanvas);
			}
			
			if (!botCanvas)
			{
				botCanvas = new Canvas();
				botCanvas.width = 800;
				botCanvas.height = 320;
				box.addChild(botCanvas);			
			}
			
			if (!romViewerCtrl)
			{
				romViewerCtrl = new ROMViewer();
				romViewerCtrl.height = 640;
				romViewerCtrl.width = 800;
				topCanvas.addChild(romViewerCtrl);
			}
			
			if (!edsDebugCtrl)
			{
				edsDebugCtrl = new EDSDebug();
				edsDebugCtrl.width = 800;
				edsDebugCtrl.height = 320;
				botCanvas.addChild(edsDebugCtrl);
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
		
		public function Update():void
		{
			romViewerCtrl.Update();
		}
		
		public function SetROMNode(rom:ROMNode):void {
			romViewerCtrl.SetROMNode(rom);
			edsDebugCtrl.SetROMNode(rom);
		}
		public function GetROMNode():ROMNode {
			return romViewerCtrl.GetROMNode();
		}
	}

}