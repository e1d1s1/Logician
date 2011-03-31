package ROMViewer 
{
	/**
	 * ...
	 * @author Eric D. Schmidt, DigiRule Solutions LLC
	 */
	import mx.collections.ArrayCollection;
	public class TreeNode 
	{
		public var label:String;
		public var children:ArrayCollection;
		public var data:Object;
			
		public function TreeNode() 
		{
			
		}
		
		public function addChild(node:ROMViewer.TreeNode):void
		{
			if (this.children == null)
				this.children = new ArrayCollection();
			this.children.addItem(node);
		}
		
	}

}