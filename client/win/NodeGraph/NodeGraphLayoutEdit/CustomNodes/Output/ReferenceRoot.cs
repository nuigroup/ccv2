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
using NodeGraphControl;
using NodeGraphLayoutEdit.DataTypes;
using NodeGraphControl.Xml;

namespace NodeGraphLayoutEdit.CustomNodes
{
    public class ReferenceRoot:NodeGraphNode
    {
        public ReferenceRoot(int p_X, int p_Y, NodeGraphView p_View)
            : base(p_X, p_Y, p_View, false)
        {
            this.m_sName = "Output";
            this.m_Connectors.Add(new NodeGraphConnector("Result 1", this, ConnectorType.InputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("Result 2", this, ConnectorType.InputConnector, 1));
            this.m_Connectors.Add(new NodeGraphConnector("Result 3", this, ConnectorType.InputConnector, 2));
            this.Height = 86;

        }

        public ReferenceRoot(XmlTreeNode p_TreeNode, NodeGraphView p_View)
            : base(p_TreeNode, p_View)
        {
            this.m_sName = "Output";
            this.m_Connectors.Add(new NodeGraphConnector("Result 1", this, ConnectorType.InputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("Result 2", this, ConnectorType.InputConnector, 1));
            this.m_Connectors.Add(new NodeGraphConnector("Result 3", this, ConnectorType.InputConnector, 2));
            this.Height = 86;
        }

        public override NodeGraphData Process()
        {
            NodeGraphFloatListData v_OutputData = new NodeGraphFloatListData();
            NodeGraphInvalidData v_Errors = new NodeGraphInvalidData();

            NodeGraphListData v_InputData = this.GetInputData();

            foreach (NodeGraphData i_InputData in v_InputData.Data)
            {
                if (i_InputData is NodeGraphFloatData)
                {
                    v_OutputData.Values.Add((i_InputData as NodeGraphFloatData).Value);
                }
                else if (i_InputData is NodeGraphInvalidData)
                {
                    v_Errors.Merge(i_InputData as NodeGraphInvalidData);

                }

            }


            return new NodeGraphTestResultData(v_OutputData, v_Errors);
        }
    }
}
