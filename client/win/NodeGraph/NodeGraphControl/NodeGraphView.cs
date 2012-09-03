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
using System.Text;
using System.IO;
using System.Windows.Forms;
using NodeGraphControl.Xml;
using NuiApiWrapper;

namespace NodeGraphControl
{
    /// <summary>
    /// Encapsulates a node view (nodes & links)
    /// </summary>
    public class NodeGraphView
    {
        /// <summary>
        /// The node Collection contained in this view
        /// </summary>
        public List<NodeGraphNode> NodeCollection
        {
            get { return m_NodeCollection; }
            set { m_NodeCollection = value; }
        }

        /// <summary>
        /// The collection of input and output endpoint of pipeline contained in this view
        /// </summary>
        public List<NodeGraphConnector> NodeConnectorCollection
        {
            get { return m_NodeConnectorCollection; }
            set { m_NodeConnectorCollection = value; }
        } 

        /// <summary>
        /// The collection of currently Selected nodes in this view
        /// </summary>
        public List<NodeGraphNode> SelectedItems
        {
            get { return m_SelectedItems; }
            set { m_SelectedItems = value; }
        }
        /// <summary>
        /// The collection of Links created in this view
        /// </summary>
        public List<NodeGraphLink> Links
        {
            get { return m_Links; }
            set { m_Links = value; }
        }
        /// <summary>
        /// The panel that contains this view
        /// </summary>
        public NodeGraphPanel ParentPanel
        {
            get { return m_oPanel; }
            set { m_oPanel = value; }
        }
        /// <summary>
        /// The orthographic X coordinate of the current View
        /// </summary>
        public int ViewX;
        /// <summary>
        /// The orthographic Y coordinate of the current View
        /// </summary>
        public int ViewY;
        /// <summary>
        /// The zoom factor of the current view
        /// </summary>
        public float ViewZoom;
        /// <summary>
        /// Current zoom Factor, used on smooth behavior
        /// </summary>
        public float CurrentViewZoom;

        /// <summary>
        /// Point to which we translate on zooming
        /// </summary>
        public System.Drawing.Point ZoomTranslate;


        private List<NodeGraphNode> m_NodeCollection;
        private List<NodeGraphConnector> m_NodeConnectorCollection;

        private List<NodeGraphNode> m_SelectedItems;
        private List<NodeGraphLink> m_Links;


        private NodeGraphPanel m_oPanel;

        /// <summary>
        /// Creates a new NodeGraphView in a NodeGraphpanel
        /// </summary>
        /// <param name="p_Panel"></param>
        public NodeGraphView(NodeGraphPanel p_Panel)
        {
            this.m_NodeCollection = new List<NodeGraphNode>();
            this.m_NodeConnectorCollection = new List<NodeGraphConnector>();
            this.m_SelectedItems = new List<NodeGraphNode>();
            this.m_Links = new List<NodeGraphLink>();
            this.ViewX = 0;
            this.ViewY = 0;
            this.ViewZoom = 1.0f;
            this.CurrentViewZoom = 1.0f;
            this.m_oPanel = p_Panel;
        }


        /// <summary>
        /// SERIALIZATION: Creates a NodeGraphView from a Pipeline Descriptor
        /// </summary>
        /// <param name="p_Pipeline">Pipeline descriptor</param>
        /// <param name="p_Panel">the panel that will contain this NodeGraphView</param>
        public NodeGraphView(PipelineDescriptor p_Pipeline, NodeGraphPanel p_Panel)
        {
            //System.Globalization.CultureInfo v_IntlCultureInfo = new System.Globalization.CultureInfo("en-us");
            this.m_oPanel = p_Panel;
            this.ViewX = 0;
            this.ViewY = 0;
            this.ViewZoom = 1.0f;

            this.m_NodeCollection = new List<NodeGraphNode>();

            foreach (var module in p_Pipeline.modules)
            {
                this.m_NodeCollection.Add(NodeGraphNode.FromModuleDescriptor(module, this));
            }

            m_NodeConnectorCollection = new List<NodeGraphConnector>();

            foreach (var endpoint in p_Pipeline.inputEndpoints)
            {
                this.m_NodeConnectorCollection.Add(new NodeGraphConnector(endpoint, this, ConnectorType.InputConnector));
            }

            foreach (var endpoint in p_Pipeline.outputEndpoints)
            {
                this.m_NodeConnectorCollection.Add(new NodeGraphConnector(endpoint, this, ConnectorType.OutputConnector));
            }

            this.m_Links = new List<NodeGraphLink>();

            foreach (var connection in p_Pipeline.connections)
            {
                this.m_Links.Add(NodeGraphLink.FromConnectionDescriptor(connection, this));
            }

            for(int i = 0 ; i<this.m_NodeCollection.Count ; i++)
            {
                //! TODO : normal algorythm for module placement
                this.m_NodeCollection[i].X = i * 100;
                this.m_NodeCollection[i].Y = i * 100;
                this.m_NodeCollection[i].UpdateHitRectangle();
            }

            this.m_SelectedItems = new List<NodeGraphNode>();

        }

        /// <summary>
        /// SERIALIZATION: Creates a NodeGraphView from a Serialized XML Copy
        /// </summary>
        /// <param name="p_XmlTreeNode">the parent XmlTreeNode used in serialization</param>
        /// <param name="p_Panel">the panel that will contain this NodeGraphView</param>
        public NodeGraphView(XmlTreeNode p_XmlTreeNode, NodeGraphPanel p_Panel)
        {
            System.Globalization.CultureInfo v_IntlCultureInfo = new System.Globalization.CultureInfo("en-us");
            this.m_oPanel = p_Panel;
            this.ViewX = int.Parse(p_XmlTreeNode.m_attributes["ViewX"]);
            this.ViewY = int.Parse(p_XmlTreeNode.m_attributes["ViewY"]);
            this.ViewZoom = float.Parse(p_XmlTreeNode.m_attributes["ViewZoom"], v_IntlCultureInfo);


            this.m_NodeCollection = new List<NodeGraphNode>();

            XmlTreeNode v_NodesXml = p_XmlTreeNode.GetFirstChild("NodeGraphNodeCollection");
            foreach (XmlTreeNode i_ChildNode in v_NodesXml.m_childNodes)
            {
                    this.m_NodeCollection.Add(NodeGraphNode.DeserializeFromXML(i_ChildNode, this));
            }


            this.m_Links = new List<NodeGraphLink>();

            XmlTreeNode v_LinksXml = p_XmlTreeNode.GetFirstChild("NodeGraphLinkCollection");
            foreach (XmlTreeNode i_ChildLink in v_LinksXml.m_childNodes)
            {
                    this.m_Links.Add(NodeGraphLink.DeserializeFromXML(i_ChildLink, this));
            }

            this.m_SelectedItems = new List<NodeGraphNode>();

        }

        /// <summary>
        /// Returns the Node Index of the NodeGraphNode in this view's collection
        /// </summary>
        /// <param name="p_Node"></param>
        /// <returns></returns>
        public int GetNodeIndex(NodeGraphNode p_Node)
        {
            for (int i = 0; i < this.m_NodeCollection.Count; i++)
            {
                if (this.m_NodeCollection[i] == p_Node) return i;
            }
            return -1;
        }

        /// <summary>
        /// Returns the Node Index of the NodeGraphNode in this view's current selection
        /// </summary>
        /// <param name="p_Node"></param>
        /// <returns></returns>
        public int GetSelectionNodeIndex(NodeGraphNode p_Node)
        {
            for (int i = 0; i < this.m_SelectedItems.Count; i++)
            {
                if (this.m_SelectedItems[i] == p_Node) return i;
            }
            return -1;
        }

        /// <summary>
        /// SERIALIZATION: Serializes current object and all of its children to an XML Node
        /// </summary>
        /// <param name="p_Parent">Parent XML Node used in serialization</param>
        /// <returns>the XML serialized copy of the object</returns>
        public XmlTreeNode SerializeToXML(XmlTreeNode p_Parent)
        {
            XmlTreeNode v_Out = new XmlTreeNode(Xml.SerializationUtils.GetFullTypeName(this),p_Parent);
            v_Out.AddParameter("ViewX", ViewX.ToString());
            v_Out.AddParameter("ViewY", ViewY.ToString());
            v_Out.AddParameter("ViewZoom", ViewZoom.ToString(System.Globalization.CultureInfo.GetCultureInfo("en-us")));

            // Process Nodes
            XmlTreeNode v_NodeCollection = v_Out.AddChild("NodeGraphNodeCollection");
            foreach (NodeGraphNode i_Node in this.NodeCollection)
            {
                v_NodeCollection.AddChild(i_Node.SerializeToXML(v_NodeCollection));
            }

            // Process Links

            XmlTreeNode v_LinksCollection = v_Out.AddChild("NodeGraphLinkCollection");
            foreach (NodeGraphLink i_Link in this.Links)
            {
                v_LinksCollection.AddChild(i_Link.SerializeToXML(v_LinksCollection));
            }

            return v_Out;

        }


        /// <summary>
        /// CLIPBOARD: Copies the selection as a list of Nodes into ClipBoard.
        /// </summary>
        public void CopySelectionToClipboard()
        {
            XmlTree v_ClipboardCopy = new XmlTree("NodeGraphCopy");
            XmlTreeNode v_NodeRoot = v_ClipboardCopy.m_rootNode.AddChild("Nodes");
            XmlTreeNode v_LinksRoot = v_ClipboardCopy.m_rootNode.AddChild("Links");
            // Nodes
            foreach (NodeGraphNode i_Node in this.m_SelectedItems)
            {
                v_NodeRoot.AddChild(i_Node.SerializeToXML(v_ClipboardCopy.m_rootNode));
            }
            // Links

            XmlTreeNode v_CurrentLink;

            foreach (NodeGraphLink i_Link in this.m_Links)
            {
                // if the node is connecting copied nodes
                if (this.m_SelectedItems.Contains(i_Link.Input.Parent) && this.m_SelectedItems.Contains(i_Link.Output.Parent))
                {
                    v_CurrentLink = new XmlTreeNode("ToBeRelinked", v_LinksRoot);
                    v_CurrentLink.AddParameter("InputNodeId", this.GetSelectionNodeIndex(i_Link.Input.Parent).ToString());
                    v_CurrentLink.AddParameter("InputNodeConnectorIdx", i_Link.Input.Parent.GetConnectorIndex(i_Link.Input).ToString());
                    v_CurrentLink.AddParameter("OutputNodeId", this.GetSelectionNodeIndex(i_Link.Output.Parent).ToString());
                    v_CurrentLink.AddParameter("OutputNodeConnectorIdx", i_Link.Output.Parent.GetConnectorIndex(i_Link.Output).ToString());
                    v_LinksRoot.AddChild(v_CurrentLink);
                }

            }


            Clipboard.Clear();
            string v_TempPath = Path.GetTempPath() + "NodeGraphClipboard.xml";
            v_ClipboardCopy.SaveXML(v_TempPath);
            System.Collections.Specialized.StringCollection v_ClipBoardContent = new System.Collections.Specialized.StringCollection();
            v_ClipBoardContent.Add(v_TempPath);
            Clipboard.SetFileDropList(v_ClipBoardContent);

        }
        /// <summary>
        /// CLIPBOARD: If contains a NodeGraphClipboard.xml, deserializes and add nodes to current view.
        /// </summary>
        public void PasteSelectionFromClipBoard()
        {
            if (Clipboard.ContainsFileDropList())
            {
                if (Clipboard.GetFileDropList().Contains(Path.GetTempPath() + "NodeGraphClipboard.xml"))
                {
                    XmlTree v_Contents = XmlTree.FromFile(Path.GetTempPath() + "NodeGraphClipboard.xml");
                    XmlTreeNode v_ContentsRoot = v_Contents.m_rootNode;
                    XmlTreeNode v_NodesRoot = v_ContentsRoot.GetFirstChild("Nodes");
                    XmlTreeNode v_LinksRoot = v_ContentsRoot.GetFirstChild("Links");

                    int PreviousNodeCount = this.m_NodeCollection.Count;

                    NodeGraphNode v_CurrentNode;

                    foreach (XmlTreeNode i_Node in v_NodesRoot.m_childNodes)
                    {
                        v_CurrentNode = NodeGraphNode.DeserializeFromXML(i_Node, this);
                        v_CurrentNode.X += 10;
                        v_CurrentNode.Y += 10;
                        v_CurrentNode.UpdateHitRectangle();
                        this.NodeCollection.Add(v_CurrentNode);
                    }


                    int v_InId, v_InConnectorIdx, v_OutId, v_OutConnectorIdx;

                    foreach (XmlTreeNode i_Link in v_LinksRoot.m_childNodes)
                    {
                        v_InId = int.Parse(i_Link.m_attributes["InputNodeId"]);
                        v_InConnectorIdx = int.Parse(i_Link.m_attributes["InputNodeConnectorIdx"]);
                        v_OutId = int.Parse(i_Link.m_attributes["OutputNodeId"]);
                        v_OutConnectorIdx = int.Parse(i_Link.m_attributes["OutputNodeConnectorIdx"]);


                        // Relinking
                        this.m_Links.Add(new NodeGraphLink(
                            // P_INPUT
                            this.m_NodeCollection[PreviousNodeCount + v_InId].Connectors[v_InConnectorIdx],
                            // P_OUTPUT
                            this.m_NodeCollection[PreviousNodeCount + v_OutId].Connectors[v_OutConnectorIdx]
                            ));
                            
                        
                    }


                }
                ParentPanel.Refresh();


            }


        }

    }
}
