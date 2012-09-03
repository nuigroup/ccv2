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
using System.Xml;

namespace NodeGraphControl.Xml
{
    static class SerializationUtils
    {
        public static string GetFullTypeName(object o)
        {
            return o.GetType().FullName;
        }
    }
    /// <summary>
    /// Logic representation of a node tree that can be serialized in XML
    /// </summary>
    public class XmlTree
    {
        /// <summary>
        /// The ancestor XmlTreeNode of all ones.
        /// </summary>
        public XmlTreeNode m_rootNode;

        /// <summary>
        /// Construct: Creates a new XmlTree and its named root
        /// </summary>
        /// <param name="p_RootName">The name of the Root Node</param>
        public XmlTree(string p_RootName)
        {
            m_rootNode = new XmlTreeNode(p_RootName, null);
        }
        /// <summary>
        /// Saves the current XMLTree into a file
        /// </summary>
        /// <param name="filename">The filename to save</param>
        public void SaveXML(string p_filename)
        {
            XmlWriterSettings xSettings = new XmlWriterSettings();
            xSettings.Indent = true;
            xSettings.IndentChars = "     ";
            XmlWriter xWriter = XmlWriter.Create(p_filename, xSettings);
            
            // Inits recursive parse of all the XML
            m_rootNode.WriteNodeXml(xWriter);

            xWriter.Flush();
            xWriter.Close();
        }
        /// <summary>
        /// Saves the current XMLTree to a Stream
        /// </summary>
        /// <param name="p_Stream"></param>
        public void SaveXML(Stream p_Stream)
        {
            XmlWriterSettings xSettings = new XmlWriterSettings();
            xSettings.Indent = true;
            xSettings.IndentChars = "     ";
            XmlWriter xWriter = XmlWriter.Create(p_Stream, xSettings);

            // Inits recursive parse of all the XML
            m_rootNode.WriteNodeXml(xWriter);

            xWriter.Flush();
            xWriter.Close();
        }

        /// <summary>
        /// Loads an XML File into current XMLTree, destroying any objects in It.
        /// </summary>
        /// <param name="filename">The filename to load.</param>
        public void LoadXML(string p_filename)
        {
            try
            {
                XmlReaderSettings xKeySettings = new XmlReaderSettings();
                xKeySettings.IgnoreWhitespace = true;
                xKeySettings.IgnoreComments = true;

                XmlReader xReader = XmlReader.Create(p_filename, xKeySettings);
                if (!xReader.IsEmptyElement)
                {
                    xReader.Read();
                    if (xReader.NodeType == XmlNodeType.XmlDeclaration) xReader.Read();

                    m_rootNode = new XmlTreeNode(xReader, null);
                }

                m_rootNode.GetType();
                xReader.Close();

            }
            catch (Exception pe)
            {

                pe.GetHashCode();
            }

        }

        /// <summary>
        /// Loads an XML Stream into current XMLTree, destroying any objects in It.
        /// </summary>
        /// <param name="p_Stream"></param>
        public void LoadXML(Stream p_Stream)
        {
            XmlReaderSettings xKeySettings = new XmlReaderSettings();
            xKeySettings.IgnoreWhitespace = true;
            xKeySettings.IgnoreComments = true;

            XmlReader xReader = XmlReader.Create(p_Stream, xKeySettings);
            if (!xReader.IsEmptyElement)
            {
                xReader.Read();
                if (xReader.NodeType == XmlNodeType.XmlDeclaration) xReader.Read();

                m_rootNode = new XmlTreeNode(xReader, null);
            }

            m_rootNode.GetType();
            xReader.Close();
        }

        public static XmlTree FromFile(string p_FileName)
        {
            XmlTree v_OutXMLTree = new XmlTree("Temporary");
            v_OutXMLTree.LoadXML(p_FileName);
            return v_OutXMLTree;
        }
    }

    /// <summary>
    /// Logic representation of a XML Tree Node (element), that can have children, properties, etc.
    /// </summary>
    public class XmlTreeNode
    {
        /// <summary>
        /// Node Name, string identifier for the node.
        /// Will be  "<NodeName/>"
        /// </summary>
        public string m_nodeName;
        /// <summary>
        /// Node Value, for empty nodes, a CDATA string value containing text.
        /// </summary>
        public string m_value;
        /// <summary>
        /// The list of all children of the current node.
        /// </summary>
        public List<XmlTreeNode> m_childNodes;
        /// <summary>
        /// Reference to the parent node
        /// </summary>
        public XmlTreeNode m_parentNode;
        /// <summary>
        /// A list containing all attributes of the current node
        /// </summary>
        public Dictionary<string, string> m_attributes;
        /// <summary>
        /// Boolean Flag: tells if the node only contains CDATA or other nodes.
        /// (DEBUG NOTE: THIS IS NOT RIGHT AS IT IS AS public CAN CREATE SOME ERRORS,
        /// HAVE TO REWRITE SOME CODE TO MAKE IT READ ONLY)
        /// </summary>
        public bool m_bIsTextOnly;


        /// <summary>
        /// Construct: Creates a XmlTreeNode and sets It in a hierarchy.
        /// </summary>
        /// <param name="p_nodeName">The Node Name</param>
        /// <param name="p_parent">Reference to the parent XmlTreeNode</param>
        public XmlTreeNode(string p_nodeName, XmlTreeNode p_parent)
        {
            m_nodeName = p_nodeName;
            m_childNodes = new List<XmlTreeNode>();
            m_parentNode = p_parent;
            m_attributes = new Dictionary<string, string>();
            m_bIsTextOnly = false;
            m_value = null;
        }

        /// <summary>
        /// Construct: Creates a XmlTreeNode Based on readings of a XML File
        /// (SHOULD NOT BE USED, PLEASE USE XMLTREE.LOADXML() INSTEAD)
        /// </summary>
        /// <param name="p_XmlReader">Reference to the XmlReader</param>
        /// <param name="p_parent">Reference to the Parent Node</param>
        public XmlTreeNode(XmlReader p_XmlReader, XmlTreeNode p_parent)
        {
            XmlTreeNode tempNode;

            m_nodeName = p_XmlReader.Name;
            m_parentNode = p_parent;
            if (p_XmlReader.NodeType == XmlNodeType.Text)
            {
                m_bIsTextOnly = true;
                m_value = p_XmlReader.Value;

            }
            else
            {
                m_bIsTextOnly = false;
                m_childNodes = new List<XmlTreeNode>();
                m_attributes = new Dictionary<string, string>();

                if (p_XmlReader.HasAttributes)
                {
                    while (p_XmlReader.MoveToNextAttribute())
                    {
                        m_attributes.Add(p_XmlReader.Name, p_XmlReader.Value);
                    }

                }

                p_XmlReader.MoveToElement();

                if (!p_XmlReader.IsEmptyElement)
                {
                    // Si c'est une valeur....
                    if (p_XmlReader.HasValue)
                    {
                        p_XmlReader.Read();
                        m_value = p_XmlReader.Value;

                    }



                    p_XmlReader.Read();

                    // Parse all direct children
                    if (p_XmlReader != null)
                    {
                        while (!((p_XmlReader.NodeType == XmlNodeType.EndElement) && (p_XmlReader.Name == m_nodeName)))
                        {
                            //System.Console.Beep();
                            tempNode = new XmlTreeNode(p_XmlReader, this);
                            if (tempNode.m_bIsTextOnly)
                            {
                                this.m_value = tempNode.m_value;
                            }
                            else
                            {
                                m_childNodes.Add(tempNode);
                            }
                            p_XmlReader.Read();

                        }
                    }
                }
            }

        }

        /// <summary>
        /// Adds a parameter to the list, if it's not already present.
        /// Will be outputted as name="Value" in XML.
        /// </summary>
        /// <param name="p_paramName">Name of the parameter</param>
        /// <param name="p_paramValue">Value of the parameter</param>
        public void AddParameter(string p_paramName, string p_paramValue)
        {
            if (!m_attributes.ContainsKey(p_paramName))
            {
                m_attributes.Add(p_paramName, p_paramValue);
            }
            else
            {
                throw new XmlTreeNodeParameterAlreadyExistsException();
            }
        }

        /// <summary>
        /// (Conception) Creates a new ChildNode for the current node.
        /// </summary>
        /// <param name="p_nodename">Name of the child</param>
        /// <returns>Reference to the child</returns>
        public XmlTreeNode AddChild(string p_nodename)
        {
            XmlTreeNode newNode = new XmlTreeNode(p_nodename, this);
            this.m_childNodes.Add(newNode);
            return (newNode);
        }

        /// <summary>
        /// (Adoption) Adds an existing Child to this node's children.
        /// NOTE: WHEN ADOPTING, FORMER PARENT NODE REFERENCE FOR THE ADOPTED NODE WILL BE DESTROYED.
        /// </summary>
        /// <param name="p_node">Reference to the node to adopt.</param>
        public void AddChild(XmlTreeNode p_node)
        {
            this.m_childNodes.Add(p_node);

            p_node.m_parentNode = this;


        }

        /// <summary>
        /// Tells if this node is root of a hierarchy
        /// </summary>
        /// <returns>True if root, false if not.</returns>
        public bool IsRoot()
        {
            if (m_parentNode == null) return true;
            else return false;
        }

        /// <summary>
        /// Tells if this node has children
        /// </summary>
        /// <returns>True if It has children, false if not.</returns>
        public bool HasChild()
        {
            if (m_childNodes.Count > 0) return true;
            else return false;
        }

        /// <summary>
        /// Returns true if the node has at least one child of the name of p_NodeName
        /// </summary>
        /// <param name="p_NodeName">name of the child</param>
        /// <returns>true if any, false otherwise</returns>
        public bool HasChild(string p_NodeName)
        {
            if (m_childNodes.Count > 0)
            {
                bool v_Result = false;

                foreach (XmlTreeNode i_Node in this.m_childNodes)
                {
                    if (i_Node.m_nodeName == p_NodeName) return true;
                }
                return v_Result;

            }
            else return false;

        }

        /// <summary>
        /// Returns a list of all the children corresponding to the name
        /// </summary>
        /// <param name="p_Name">the name</param>
        /// <returns>the list of XmlTreeNode</returns>
        public List<XmlTreeNode> GetTheseChildren(String p_Name)
        {
            List<XmlTreeNode> v_SelectedChildren = new List<XmlTreeNode>();

            foreach (XmlTreeNode i_Node in this.m_childNodes)
            {
                if (i_Node.m_nodeName == p_Name)
                {
                    v_SelectedChildren.Add(i_Node);
                }
            }
            return v_SelectedChildren;
        }

        /// <summary>
        /// Returns the first child correpsonding to the name
        /// </summary>
        /// <param name="p_Name">the name</param>
        /// <returns>a XmlTreeNode if found, null otherwise</returns>
        public XmlTreeNode GetFirstChild(String p_Name)
        {
            foreach (XmlTreeNode i_Node in this.m_childNodes)
            {
                if (i_Node.m_nodeName == p_Name)
                {
                    return i_Node;
                }
            }
            return null;

        }

        /// <summary>
        /// XML Node Serialization Method, should not be used,
        /// please use XmlTree.SaveXML() instead.
        /// </summary>
        /// <param name="p_xWriter">Reference to the XMLWriter.</param>
        public void WriteNodeXml(XmlWriter p_xWriter)
        {
            p_xWriter.WriteStartElement(this.m_nodeName);
            foreach (KeyValuePair<string, string> attr in m_attributes)
            {
                p_xWriter.WriteStartAttribute(attr.Key);
                p_xWriter.WriteValue(attr.Value);
                p_xWriter.WriteEndAttribute();

            }
            if (m_value != null) p_xWriter.WriteValue(m_value + "\n");
            foreach (XmlTreeNode xn in this.m_childNodes)
            {
                xn.WriteNodeXml(p_xWriter);
            }

            p_xWriter.WriteEndElement();
        }

    }

    /// <summary>
    /// Exception: Thrown if a Parameter already exists in current XmlTreeNode
    /// </summary>
    public class XmlTreeNodeParameterAlreadyExistsException : Exception
    {

    }

    /// <summary>
    /// Exception: User Throws it when an unexpected node is encountered.
    /// </summary>
    public class XmlTreeNodeUnexpectedNodeException : Exception
    {

    }
}
