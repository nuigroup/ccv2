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
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using NuiApiWrapper;

namespace NodeGraphControl
{
    /// <summary>
    /// Represents a base node for use in a NodeGraphView
    /// </summary>
    public class NodeGraphNode 
    {
        /// <summary>
        /// Whether the node can be selected
        /// </summary>
        [Browsable(false)]
        public bool CanBeSelected { get { return this.m_bCanBeSelected; } set { this.m_bCanBeSelected = value; } }
        /// <summary>
        /// The Display name of the node
        /// </summary>
        [Browsable(false)]
        public string Name { get { return this.GetName(); } }
        /// <summary>
        /// X Position (ViewSpace) of the Node
        /// </summary>
        [Browsable(false)]
        public int X;
        /// <summary>
        /// Y Position (ViewSpace) of the Node
        /// </summary>
        [Browsable(false)]
        public int Y;
        /// <summary>
        /// Width (ViewSpace) of the node
        /// </summary>
        [Browsable(false)]
        public int Width { get { return this.m_Width; } set { this.m_Width = value; this.UpdateHitRectangle(); } }
        /// <summary>
        /// Height (ViewSpace) of the node
        /// </summary>
        [Browsable(false)]
        public int Height { get { return this.m_Height; } set { this.m_Height = value; this.UpdateHitRectangle(); } }
        /// <summary>
        /// The NodeGraphView associated to this node
        /// </summary>
        [Browsable(false)]
        public NodeGraphView ParentView { get { return this.m_oView; } }
        /// <summary>
        /// Whether the node is highlighted
        /// </summary>
        [Browsable(false)]
        public bool Highlighted;
        /// <summary>
        /// The Hit (Mouse Click) rectangle of the Node
        /// </summary>
        [Browsable(false)]
        public Rectangle HitRectangle;
        /// <summary>
        /// The list of NodeGraphConnectors owned by this Node
        /// </summary>
        [Browsable(false)]
        public List<NodeGraphConnector> Connectors { get { return this.m_Connectors; } }

        /// <summary>
        /// The displayed Commentary of the node
        /// </summary>
        [Category("NodeGraphNode")]
        public string Comment { get { return this.m_sComment; } set { this.m_sComment = value; } }

        private int m_Height;
        private int m_Width;
        private bool m_bCanBeSelected;
        private string m_sComment;

        [Browsable(false)]
        protected String m_sName;
        protected List<NodeGraphConnector> m_Connectors;
        protected NodeGraphView m_oView;

        public event PaintEventHandler onPostDraw;


        /// <summary>
        /// Creates a new NodeGraphNode into the NodeGraphView, given coordinates and ability to be selected
        /// </summary>
        /// <param name="p_X"></param>
        /// <param name="p_Y"></param>
        /// <param name="p_View"></param>
        /// <param name="p_CanBeSelected"></param>
        public NodeGraphNode(int p_X, int p_Y, NodeGraphView p_View, bool p_CanBeSelected)
        {
            this.X = p_X;
            this.Y = p_Y;
            this.m_oView = p_View;
            this.m_Width = 150;
            this.m_Height = 75;
            this.m_sName = "Base Node";
            this.m_bCanBeSelected = p_CanBeSelected;
            this.Highlighted = false;
            this.m_sComment = "";

            UpdateHitRectangle();

            this.m_Connectors = new List<NodeGraphConnector>();
            
        
        }

        /// <summary>
        /// CONVERTATION : Creates a NodeGraphNode from a XML Serialized Copy
        /// </summary>
        /// <param name="p_Input"></param>
        /// <param name="p_View"></param>
        public NodeGraphNode(ModuleDescriptor descriptor, NodeGraphView p_View)
        {
            this.m_oView = p_View;
            this.m_sName = descriptor.name;
            this.m_sComment = descriptor.description;
            ///! TODO : X,Y ? set later?
            //this.X = ;
            //this.Y = ;
            this.Width = 80;
            this.Height = 40;
            this.m_bCanBeSelected = true;
            this.m_Connectors = new List<NodeGraphConnector>();
            foreach (var inputEndpoint in descriptor.inputEndpoints)
            {
                this.m_Connectors.Add(new NodeGraphConnector(inputEndpoint, this, ConnectorType.InputConnector));
            }

            foreach (var outputEndpoint in descriptor.outputEndpoints)
            {
                this.m_Connectors.Add(new NodeGraphConnector(outputEndpoint, this, ConnectorType.OutputConnector));
            }
        }

        /// <summary>
        /// SERIALIZATION: Creates a NodeGraphNode from a XML Serialized Copy
        /// </summary>
        /// <param name="p_Input"></param>
        /// <param name="p_View"></param>
        public NodeGraphNode(Xml.XmlTreeNode p_Input, NodeGraphView p_View)
        {
            this.m_oView = p_View;
            this.m_sName = p_Input.m_attributes["Name"];
            this.m_sComment = p_Input.m_attributes["Comment"];
            this.X = int.Parse(p_Input.m_attributes["X"]);
            this.Y = int.Parse(p_Input.m_attributes["Y"]);
            this.Width = int.Parse(p_Input.m_attributes["Width"]);
            this.Height = int.Parse(p_Input.m_attributes["Height"]);
            this.m_bCanBeSelected = bool.Parse(p_Input.m_attributes["CanBeSelected"]);
            this.m_Connectors = new List<NodeGraphConnector>();
        }

        /// <summary>
        /// Gets the connector index, given the connector object reference
        /// </summary>
        /// <param name="p_Connector">the connector reference</param>
        /// <returns>the connector index</returns>
        public int GetConnectorIndex(NodeGraphConnector p_Connector)
        {
            for (int i = 0; i < this.m_Connectors.Count; i++)
            {
                if (this.m_Connectors[i] == p_Connector) return i;
            }
            return -1;
        }


        /// <summary>
        /// returns connector at index in list of connectors of special type
        /// </summary>
        /// <param name="index">index of connector in list of connectors of the same type</param>
        /// <param name="type">type of connector to search</param>
        /// <returns></returns>
        public NodeGraphConnector GetConnector(int index, ConnectorType type)
        {
            for (int i = 0; i < Connectors.Count; i++)
            {
                if(Connectors[i].Type == type)
                    index--;
                if (index < 0)
                    return Connectors[i];
            }
            return null;
        }

        /// <summary>
        /// Returns the name of the node: can be overriden to match custom names.
        /// </summary>
        /// <returns></returns>
        protected virtual string GetName()
        {
            return this.m_sName;
        }

        /// <summary>
        /// Updates HitRectangle (when moved)
        /// </summary>
        public void UpdateHitRectangle()
        {
            this.HitRectangle = new Rectangle(X, Y, Width, Height);
        }

        /// <summary>
        /// Intercepts a mouse hit and returns a NodeGraphConnector if hit by the mouse, null otherwise
        /// </summary>
        /// <param name="p_ScreenPosition"></param>
        /// <returns></returns>
        public NodeGraphConnector GetConnectorMouseHit(Point p_ScreenPosition)
        {
            Rectangle v_HitRectangle = new Rectangle(p_ScreenPosition,Size.Empty);

            foreach (NodeGraphConnector i_Connector in this.m_Connectors)
            {
                if (v_HitRectangle.IntersectsWith(i_Connector.GetHitArea()))
                {
                    return i_Connector;
                }
            }
            return null;
        }


        /// <summary>
        /// Draws the node
        /// </summary>
        /// <param name="e"></param>
        public virtual void Draw(PaintEventArgs e)
        {

            Point CtrlPos = m_oView.ParentPanel.ViewToControl(new Point(X, Y));
            int ScaledX = CtrlPos.X;
            int ScaledY = CtrlPos.Y;

            Rectangle ViewRectangle =   new Rectangle(
                                                        CtrlPos.X, 
                                                        CtrlPos.Y,
                                                        (int)(this.HitRectangle.Width * m_oView.CurrentViewZoom),
                                                        (int)(this.HitRectangle.Height * m_oView.CurrentViewZoom)
                                                    );
            // NODE SHADOW
            if (this.ParentView.ParentPanel.DrawShadow)
            {
                e.Graphics.DrawImage(NodeGraphResources.NodeShadow, ParentView.ParentPanel.ViewToControl(new Rectangle( this.X - (int)(0.1f * this.Width) + 4, 
                                                                                                                        this.Y - (int)(0.1f * this.Height) + 4,
                                                                                                                        this.Width + (int)(0.2f * this.Width)-4,
                                                                                                                        this.Height + (int)(0.2f * this.Height)-4)
                                                                                                         ));
            }
            // NODE
            if (!this.Highlighted)
            {
                e.Graphics.FillRectangle(m_oView.ParentPanel.NodeFill, ViewRectangle);
                e.Graphics.FillRectangle(m_oView.ParentPanel.NodeHeaderFill, new Rectangle(ViewRectangle.X, ViewRectangle.Y, ViewRectangle.Width, (int)(m_oView.ParentPanel.NodeHeaderSize * m_oView.CurrentViewZoom)));
                e.Graphics.DrawRectangle(m_oView.ParentPanel.NodeOutline, ViewRectangle);
            }
            else
            {
                e.Graphics.FillRectangle(m_oView.ParentPanel.NodeFillSelected, ViewRectangle);
                e.Graphics.FillRectangle(m_oView.ParentPanel.NodeHeaderFill, new Rectangle(ViewRectangle.X, ViewRectangle.Y, ViewRectangle.Width, (int)(m_oView.ParentPanel.NodeHeaderSize * m_oView.CurrentViewZoom)));
                e.Graphics.DrawRectangle(m_oView.ParentPanel.NodeOutlineSelected, ViewRectangle);
            }




            // VALID/INVALID NODE
            Point v_CtrlSignalPosition = m_oView.ParentPanel.ViewToControl(new Point(X + Width - 20, Y + 4));
            Rectangle v_SignalRectangle = new Rectangle(v_CtrlSignalPosition.X, v_CtrlSignalPosition.Y, (int)(16 * m_oView.CurrentViewZoom), (int)(16 * m_oView.CurrentViewZoom));

            if (this.IsValid())
            {
                e.Graphics.DrawImage(NodeGraphResources.NodeValid, v_SignalRectangle);
            }
            else
            {
                e.Graphics.DrawImage(NodeGraphResources.NodeInvalid, v_SignalRectangle);
            }

            

            /// IF SUFFICENT ZOOM LEVEL = DRAW TEXT
            if (m_oView.CurrentViewZoom > m_oView.ParentPanel.NodeTitleZoomThreshold)
            {
                // DrawText
                e.Graphics.DrawString(this.Name, m_oView.ParentPanel.NodeScaledTitleFont, m_oView.ParentPanel.NodeTextShadow, new Point(ScaledX + (int)(2 * m_oView.CurrentViewZoom) + 1, ScaledY + (int)(2 * m_oView.CurrentViewZoom) + 1));
                e.Graphics.DrawString(this.Name, m_oView.ParentPanel.NodeScaledTitleFont, m_oView.ParentPanel.NodeText, new Point(ScaledX + (int)(2 * m_oView.CurrentViewZoom), ScaledY + (int)(2 * m_oView.CurrentViewZoom)));
            }
            
            


            //InputConnectors
            for (int i_ConnectorIndex = 0; i_ConnectorIndex < this.m_Connectors.Count; i_ConnectorIndex++)
            {
                this.m_Connectors[i_ConnectorIndex].Draw(e, i_ConnectorIndex) ;

            }

            // Comment
            if (this.m_sComment != "")
            {
                e.Graphics.DrawString(this.m_sComment, m_oView.ParentPanel.NodeScaledTitleFont, m_oView.ParentPanel.NodeText, new Point(ScaledX, ScaledY - (int)(18 * m_oView.CurrentViewZoom)));
            }

            // Post-draw event
            if (this.onPostDraw != null) onPostDraw(this, e);
        
        }

        /// <summary>
        /// VIRTUAL: Returns if logic validity has been approved (all input connector linked by default) : can be overriden to get custom approval
        /// </summary>
        /// <returns></returns>
        public virtual bool IsValid()
        {
            bool v_Return = true;

            foreach (NodeGraphConnector i_Connector in this.m_Connectors)
            {
                if (i_Connector.Type == ConnectorType.InputConnector)
                {
                    if (!i_Connector.CanProcess())
                    {
                        v_Return = false;
                    }
                }
            }

            return v_Return;
        }

        /// <summary>
        /// Returns input data as a NodeGraphListData
        /// </summary>
        /// <returns></returns>
        public NodeGraphListData GetInputData()
        {
            NodeGraphListData v_Output = new NodeGraphListData();

            foreach (NodeGraphConnector i_Connector in this.m_Connectors)
            {
                if (i_Connector.Type == ConnectorType.InputConnector)
                {
                    v_Output.AddData(i_Connector.Process());
                }
            }

            return v_Output;
        }

        /// <summary>
        /// VIRTUAL: Processes the node and returns NodeGraphData according to the process. Must be overriden because returns null;
        /// </summary>
        /// <returns></returns>
        public virtual NodeGraphData Process()
        {
            return null;
        }

        /// <summary>
        /// VIRTUAL: Serializes the current Node to XML Serialized copy. Must be overriden to implement custom members.
        /// </summary>
        /// <param name="p_Parent"></param>
        /// <returns></returns>
        public virtual Xml.XmlTreeNode SerializeToXML(Xml.XmlTreeNode p_Parent)
        {
            Xml.XmlTreeNode v_Out = new Xml.XmlTreeNode(Xml.SerializationUtils.GetFullTypeName(this), p_Parent);

            v_Out.AddParameter("Name", this.Name);
            v_Out.AddParameter("X", this.X.ToString());
            v_Out.AddParameter("Y", this.Y.ToString());
            v_Out.AddParameter("Width", this.Width.ToString());
            v_Out.AddParameter("Height", this.Height.ToString());
            v_Out.AddParameter("Comment", this.Comment.ToString());
            v_Out.AddParameter("CanBeSelected", this.CanBeSelected.ToString());
            return v_Out;
        }

        /// <summary>
        /// STATIC: Deserializes a new NodeGraphNode given a XML Serialized Copy
        /// </summary>
        /// <param name="p_Node"></param>
        /// <param name="p_View"></param>
        /// <returns></returns>
        public static NodeGraphNode DeserializeFromXML(Xml.XmlTreeNode p_Node, NodeGraphView p_View)
        {
            string className = p_Node.m_nodeName;

            object[] arguments = { p_Node, p_View };

            object v_Out = System.Reflection.Assembly.GetEntryAssembly().CreateInstance(className, false,
                                                                                    System.Reflection.BindingFlags.CreateInstance,
                                                                                    null,
                                                                                    arguments, System.Globalization.CultureInfo.GetCultureInfo("fr-fr"),
                                                                                    null);

            return v_Out as NodeGraphNode;
        }

        /// <summary>
        /// STATIC: Returns a new instance from Assembly, name and a list of arguments
        /// </summary>
        /// <param name="p_TypeName"></param>
        /// <param name="p_View"></param>
        /// <returns></returns>
        public static NodeGraphNode GenerateInstanceFromName(System.Reflection.Assembly p_Assembly, string p_TypeName, object[] p_ConstructorArgs)
        {
            object v_Out = p_Assembly.CreateInstance(p_TypeName, false,
                                                     System.Reflection.BindingFlags.CreateInstance,
                                                     null,
                                                     p_ConstructorArgs, System.Globalization.CultureInfo.GetCultureInfo("fr-fr"),
                                                     null);

            return v_Out as NodeGraphNode;
        }

        public bool HitTest(Point p)
        {
            
            return true;
        }

        public static NodeGraphNode FromModuleDescriptor(NuiApiWrapper.ModuleDescriptor module, NodeGraphView p_View)
        {
//             string className = typeof(NodeGraphNode).Name;
// 
//             object[] arguments = { module, p_View };
// 
//             object v_Out = System.Reflection.Assembly.GetEntryAssembly().CreateInstance(className, false,
//                                                                                     System.Reflection.BindingFlags.CreateInstance,
//                                                                                     null,
//                                                                                     arguments, System.Globalization.CultureInfo.GetCultureInfo("fr-fr"),
//                                                                                     null);

            return new NodeGraphNode(module, p_View);
        }
    }


}
