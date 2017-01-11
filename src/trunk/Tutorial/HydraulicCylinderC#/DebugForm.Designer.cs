namespace HydraulicCylinder
{
    partial class DebugForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.Debugger = new System.Windows.Forms.Integration.ElementHost();
            this.logicianDebugCtrl1 = new LogicianDebuggerWPF.LogicianDebugCtrl();
            this.SuspendLayout();
            // 
            // Debugger
            // 
            this.Debugger.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Debugger.Location = new System.Drawing.Point(0, 0);
            this.Debugger.Name = "Debugger";
            this.Debugger.Size = new System.Drawing.Size(757, 829);
            this.Debugger.TabIndex = 0;
            this.Debugger.Text = "elementHost1";
            this.Debugger.Child = this.logicianDebugCtrl1;
            // 
            // DebugForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(757, 829);
            this.Controls.Add(this.Debugger);
            this.Name = "DebugForm";
            this.Text = "DebugForm";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Integration.ElementHost Debugger;
        private LogicianDebuggerWPF.LogicianDebugCtrl logicianDebugCtrl1;

    }
}