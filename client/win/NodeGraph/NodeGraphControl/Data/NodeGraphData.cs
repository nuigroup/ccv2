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

namespace NodeGraphControl
{
    /// <summary>
    /// Base type for Node Process Data
    /// </summary>
    public abstract class NodeGraphData
    {
    }

    /// <summary>
    /// Base type for Invalid Data
    /// </summary>
    public class NodeGraphInvalidData : NodeGraphData
    {
        /// <summary>
        /// A list of invalid Nodes that couldn't be processed
        /// </summary>
        public List<NodeGraphNode> InvalidNodes { get { return this.m_lInvalidNodes; } }
        /// <summary>
        /// A list of error messages gathered while parsing data
        /// </summary>
        public List<string> ErrorMessages { get { return this.m_lErrorMessages; } }

        private List<string> m_lErrorMessages;
        private List<NodeGraphNode> m_lInvalidNodes;
        
        /// <summary>
        /// Creates a new NodeGraphInvalidData
        /// </summary>
        public NodeGraphInvalidData()
            : base()
        {
            this.m_lErrorMessages = new List<string>();
            this.m_lInvalidNodes = new List<NodeGraphNode>();
        }

        /// <summary>
        /// Creates a new NodeGraphInvalidData based on an Invalid node and given an Error message
        /// </summary>
        /// <param name="p_InvalidNode">the Invalid node</param>
        /// <param name="p_ErrorMessage">the Error message</param>
        public NodeGraphInvalidData(NodeGraphNode p_InvalidNode, string p_ErrorMessage) : base()
        {
            this.m_lErrorMessages = new List<string>();
            this.m_lInvalidNodes = new List<NodeGraphNode>();
            this.AddInvalidNode(p_InvalidNode, p_ErrorMessage);
            
        }

        /// <summary>
        /// Adds an invalid node to the current NodeGraphInvalidData
        /// </summary>
        /// <param name="p_InvalidNode">the Invalid node</param>
        /// <param name="p_ErrorMessage">the Error message</param>
        public void AddInvalidNode(NodeGraphNode p_InvalidNode,  string p_ErrorMessage)
        {
            if(!this.m_lInvalidNodes.Contains(p_InvalidNode)) this.m_lInvalidNodes.Add(p_InvalidNode);
            this.m_lErrorMessages.Add(p_InvalidNode.Name + ":" + p_ErrorMessage);
        }

        /// <summary>
        /// Merges two Invalid NodeGraphData
        /// </summary>
        /// <param name="p_Data">the Data to merge to the current object</param>
        public void Merge(NodeGraphInvalidData p_Data)
        {
            for (int i = 0; i < p_Data.InvalidNodes.Count; i++)
            {
                m_lInvalidNodes.Add(p_Data.InvalidNodes[i]);
                m_lErrorMessages.Add(p_Data.ErrorMessages[i]);
            }

        }

    }
}
