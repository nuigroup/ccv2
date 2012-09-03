/*

Copyright (c) 2011, Thomas ICHE
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
following conditions are met:

        * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
          in the documentation and/or other materials provided with the distribution.
        * Neither the name of PeeWeeK.NET nor the names of its contributors may be used to endorse or promote products derived from this 
          software without specific prior written permission.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using NodeGraphControl;
using Utilities;
using NuiApiWrapper;

namespace NodeGraphLayoutEdit
{
    public partial class LayoutEditor : Form
    {
        public static Point m_MouseLoc;
        CustomNodes.ReferenceRoot m_ReferenceRoot;
        private Boolean defaultSettings = true;
        private IniFile iniFile;
        private NodeGraphControl.NodeGraphPanel nodeGraphPanelAlt;
        public LayoutEditor()
        {
            LoadSettings();

           // CreateGraphView();

            InitializeComponent();

            // LOAD DEFAULT SETTINGS
            try
            {
                this.nodeGraphPanel.EnableDrawDebug = Convert.ToBoolean(iniFile.Section("General").Get("Debug"));      
                this.nodeGraphPanel.ShowGrid = Convert.ToBoolean(iniFile.Section("View").Get("ShowGrid"));
                this.nodeGraphPanel.GridPadding = Convert.ToInt16(iniFile.Section("View").Get("GridPadding"));

                this.nodeGraphPanel.LoadPipeline(NuiState.Instance.GetPipeline("root"));
                //Convert.ToBoolean(value));
            }
            catch (FormatException)
            {
                MessageBox.Show("Error Parsing Settings...");
            }

        }

        private void LoadSettings()
        {
            try
            {

                if (defaultSettings) // Generate Settings
                {
                    iniFile = new IniFile();
                    iniFile.Section("General").Comment = "Configuration File for CL Studio Graph Editor";
                    iniFile.Section("General").Set("Theme", "Dark");
                    iniFile.Section("General").Set("Debug", "True", comment: "Enables debug view.");
                    iniFile.Section("View").Set("ShowGrid", "True");
                    iniFile.Section("View").Set("GridPadding", "400");
                    iniFile.Section("Service").Set("APIV1", "http://localhost:5555/");
                    iniFile.Save("data/config.ini");
                    //toolStripStatusLabel1.Text = "Settings Loaded...";
                }
                else
                { // Use file...
                    iniFile = new IniFile("data/config.ini");
                    // iniFile.Section("General").Get("Theme"); 
                    // iniFile.Section("General").Get("Debug");
                    //foreach (var section in iniFile.Sections())
                    //  MessageBox.Show(iniFile.Section("General").Get("Debug"));
                }
            }
            catch(Exception ex)
            {
                // ERROR SETTING DEFUALT SETTINGS
                MessageBox.Show("Error Loading Settings...");
            }


            m_MouseLoc = Point.Empty;
        }

        private void CreateGraphView()
        {
            this.nodeGraphPanelAlt = new NodeGraphControl.NodeGraphPanel();
            this.Controls.Add(this.nodeGraphPanelAlt);


            this.nodeGraphPanelAlt.BackColor = System.Drawing.Color.DarkGray;
            this.nodeGraphPanelAlt.ConnectorFillColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.nodeGraphPanelAlt.ConnectorFillSelectedColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.nodeGraphPanelAlt.ConnectorHitZoneBleed = 4;
            this.nodeGraphPanelAlt.ConnectorOutlineColor = System.Drawing.Color.FromArgb(((int)(((byte)(32)))), ((int)(((byte)(32)))), ((int)(((byte)(32)))));
            this.nodeGraphPanelAlt.ConnectorOutlineSelectedColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.nodeGraphPanelAlt.ConnectorTextColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.nodeGraphPanelAlt.ContextMenuStrip = this.contextMenuStrip;
            this.nodeGraphPanelAlt.Dock = System.Windows.Forms.DockStyle.Right;
            this.nodeGraphPanelAlt.DrawShadow = true;
            this.nodeGraphPanelAlt.EnableDrawDebug = true;
            this.nodeGraphPanelAlt.GridAlpha = ((byte)(16));
            this.nodeGraphPanelAlt.GridPadding = 256;
            this.nodeGraphPanelAlt.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(180)))), ((int)(((byte)(180)))));
            this.nodeGraphPanelAlt.LinkEditableColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(255)))), ((int)(((byte)(0)))));
            this.nodeGraphPanelAlt.LinkHardness = 2F;
            this.nodeGraphPanelAlt.LinkVisualStyle = NodeGraphControl.LinkVisualStyle.Curve;
            this.nodeGraphPanelAlt.Location = new System.Drawing.Point(0, 24);
            this.nodeGraphPanelAlt.Name = "nodeGraphPanelAlt";
            this.nodeGraphPanelAlt.NodeConnectorFont = new System.Drawing.Font("Tahoma", 7F);
            this.nodeGraphPanelAlt.NodeConnectorTextZoomTreshold = 0.8F;
            this.nodeGraphPanelAlt.NodeFillColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
            this.nodeGraphPanelAlt.NodeFillSelectedColor = System.Drawing.Color.FromArgb(((int)(((byte)(160)))), ((int)(((byte)(128)))), ((int)(((byte)(100)))));
            this.nodeGraphPanelAlt.NodeHeaderColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.nodeGraphPanelAlt.NodeHeaderSize = 24;
            this.nodeGraphPanelAlt.NodeOutlineColor = System.Drawing.Color.FromArgb(((int)(((byte)(180)))), ((int)(((byte)(180)))), ((int)(((byte)(180)))));
            this.nodeGraphPanelAlt.NodeOutlineSelectedColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(160)))), ((int)(((byte)(128)))));
            this.nodeGraphPanelAlt.NodeScaledConnectorFont = new System.Drawing.Font("Tahoma", 7F);
            this.nodeGraphPanelAlt.NodeScaledTitleFont = new System.Drawing.Font("Tahoma", 8F);
            this.nodeGraphPanelAlt.NodeSignalInvalidColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.nodeGraphPanelAlt.NodeSignalValidColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(255)))), ((int)(((byte)(0)))));
            this.nodeGraphPanelAlt.NodeTextColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.nodeGraphPanelAlt.NodeTextShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.nodeGraphPanelAlt.NodeTitleFont = new System.Drawing.Font("Tahoma", 8F);
            this.nodeGraphPanelAlt.NodeTitleZoomThreshold = 0.5F;
            this.nodeGraphPanelAlt.SelectionFillColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(128)))), ((int)(((byte)(90)))), ((int)(((byte)(30)))));
            this.nodeGraphPanelAlt.SelectionOutlineColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(180)))), ((int)(((byte)(60)))));
            this.nodeGraphPanelAlt.ShowGrid = true;
            this.nodeGraphPanelAlt.Size = new System.Drawing.Size(400, 400);
            this.nodeGraphPanelAlt.SmoothBehavior = true;
            //this.nodeGraphPanelAlt.TabIndex = 4;
            //this.nodeGraphPanelAlt.onSelectionChanged += new NodeGraphControl.NodeGraphPanelSelectionEventHandler(this.nodeGraphPanel_onSelectionChanged);
            //this.nodeGraphPanelAlt.onSelectionCleared += new NodeGraphControl.NodeGraphPanelSelectionEventHandler(this.nodeGraphPanel_onSelectionCleared);
            //this.nodeGraphPanelAlt.Load += new System.EventHandler(this.nodeGraphPanel_Load);
            //this.nodeGraphPanelAlt.MouseMove += new System.Windows.Forms.MouseEventHandler(this.nodeGraphPanel_MouseMove);
        }



        private void contextMenuStrip_Opening(object sender, CancelEventArgs e)
        {
            
        }

        private void dummyToolStripMenuItem_Click(object sender, EventArgs e)
        {
           
        }

        private void addToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.AdditionNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        private void nodeGraphPanel_onSelectionChanged(object sender, NodeGraphPanelSelectionEventArgs args)
        {
            if (args.NewSelectionCount == 1)
            {
                this.propertyGrid1.SelectedObject = this.nodeGraphPanel.View.SelectedItems[0];
            }
        }

        private void nodeGraphPanel_onSelectionCleared(object sender, NodeGraphPanelSelectionEventArgs args)
        {
            this.propertyGrid1.SelectedObject = this.nodeGraphPanel;
        }

        private void integerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
        }

        private void subtractToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.SubtractNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }


        private void multiplyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.MultiplyNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        private void divideToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.DivideNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        private void absToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.AbsNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        private void powerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.PowerNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        
        
      

        private void copyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            nodeGraphPanel.View.CopySelectionToClipboard();
        }

        private void pasteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            nodeGraphPanel.View.PasteSelectionFromClipBoard();
        }

       
        private void alignToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
        }

        private void parametersToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!propertyGrid1.Visible)
            {
                propertyGrid1.Visible = true;
            }
            else {
                propertyGrid1.Visible = false;
            }
        }

        private void ifBranchToolStripMenuItem_Click(object sender, EventArgs e)
        {
           
        }

       
        private void loadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LoadGraph();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveGraph();
        }

        private void statusToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!statusStrip.Visible)
            {
                statusStrip.Visible = true;
            }
            else
            {
                statusStrip.Visible = false;
            }
        }

        private void testToolStripMenuItem_Click(object sender, EventArgs e)
        {
            /*
            DataTypes.NodeGraphTestResultData v_Result = m_ReferenceRoot.Process() as DataTypes.NodeGraphTestResultData;

            string v_StdOut = "Test Results: ";

            int i = 0;

            foreach (float i_Data in v_Result.m_Data.Values)
            {
                v_StdOut += "[" + i.ToString() + "]:" + i_Data.ToString() + " | ";
                i++;
            }

            string v_StdErr = " Errors: ";
            i = 0;
            foreach (string i_Error in v_Result.m_Errors.ErrorMessages)
            {
                v_StdErr += " "+ i_Error + " - ";
            }
            
            //MessageBox.Show(v_StdOut + v_StdErr);
            toolStripStatusLabel1.Text = v_StdOut + v_StdErr;
             */ 
        }

        private void termsTrademarksToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void alignToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            try
            {
                int count = 0;
                int medX = 0;

                foreach (NodeGraphNode i_node in nodeGraphPanel.View.SelectedItems)
                {
                    count++;
                    medX += i_node.X;
                }
                medX = (int)(medX / count);
                foreach (NodeGraphNode i_node in nodeGraphPanel.View.SelectedItems)
                {
                    i_node.X = medX;
                }
                nodeGraphPanel.Refresh();
            }
            catch
            {
                // Error Aligning Nodes
                toolStripStatusLabel1.Text = "Error Aligning...";
            }
           
        }

        private void refreshToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            nodeGraphPanel.Refresh();
        }

        private void floatToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.FloatConstNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        private void nodeGraphPanel_Load(object sender, EventArgs e)
        {
           //EnableDrawDebug
          
        }

        private void gridToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!nodeGraphPanel.ShowGrid)
            {
                nodeGraphPanel.ShowGrid = true;
            }
            else
            {
                nodeGraphPanel.ShowGrid = false;
            }             
        }

        private void voidToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new NodeGraphNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        private void toolStripTextBox1_Click(object sender, EventArgs e)
        {

        }

        private void iFToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            this.nodeGraphPanel.AddNode(new CustomNodes.IfNode(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true));
        }

        private void cLEyeCameraToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void prefrencesToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
           SaveGraph();
        }
        private bool LoadGraph()
        {

            try
            {
                if (openFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    this.nodeGraphPanel.LoadCurrentView(openFileDialog.FileName);
                }

                // Get Back Reference root
                foreach (NodeGraphNode i_Node in this.nodeGraphPanel.View.NodeCollection)
                {
                    /*
                    if (i_Node is CustomNodes.ReferenceRoot)
                    {
                        this.m_ReferenceRoot = i_Node as CustomNodes.ReferenceRoot;
                    }
                     */
                }
                return true;
            }
            catch
            {
                toolStripStatusLabel1.Visible = true;
                toolStripStatusLabel1.Text = "Error Loading Graph";
                return false;
            }
        }
        private bool SaveGraph() {

            try
            {                
                if (saveFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    if (System.IO.File.Exists(saveFileDialog.FileName))
                    {
                        //if (MessageBox.Show(saveFileDialog.FileName + " Exists, overwrite?", "Overwrite Confirmation", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Cancel)
                        //{
                        //    return true;
                        //}
                    }
                    this.nodeGraphPanel.SaveCurrentView(saveFileDialog.FileName);
                }
                return true;
            }
            catch
            {                
                toolStripStatusLabel1.Visible = true;
                toolStripStatusLabel1.Text = "Error Saving Graph";
                return false;
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            iniFile.Save("data/config.ini");
            if (MessageBox.Show(saveFileDialog.FileName + " Are you sure you want to quit?", "Quiting", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.OK)
            {                
                Application.Exit(); 
            }
           
        }

        private void openRecentToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
           // this.nodeGraphPanel.LoadCurrentView(openFileDialog.FileName);
            //this.nodeGraphPanel.
        }

        private void templateToolStripMenuItem2_Click(object sender, EventArgs e)
        {
            Point v_ViewPos = nodeGraphPanel.ControlToView(m_MouseLoc);
            CustomNodes.TemplateModule myModule = new CustomNodes.TemplateModule(v_ViewPos.X, v_ViewPos.Y, nodeGraphPanel.View, true);
            this.nodeGraphPanel.AddNode(myModule);


           
        }

        private void rootRefernceToolStripMenuItem_Click(object sender, EventArgs e)
        {
             m_ReferenceRoot = new CustomNodes.ReferenceRoot(200, 1, nodeGraphPanel.View);
             this.nodeGraphPanel.AddNode(m_ReferenceRoot);
        }

        private void nodeGraphPanel_MouseMove(object sender, MouseEventArgs e)
        {
            m_MouseLoc = e.Location;
        }

        private void menuToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!menuStrip1.Visible)
            {
                menuStrip1.Visible = true;
            }
            else
            {
                menuStrip1.Visible = false;
            }
        }



        private void curvedToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.nodeGraphPanel.LinkVisualStyle = NodeGraphControl.LinkVisualStyle.Curve;
        }

        private void directToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.nodeGraphPanel.LinkVisualStyle = NodeGraphControl.LinkVisualStyle.Direct;
        }

        private void rectangleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.nodeGraphPanel.LinkVisualStyle = NodeGraphControl.LinkVisualStyle.Rectangle;
        }

    }
}
        