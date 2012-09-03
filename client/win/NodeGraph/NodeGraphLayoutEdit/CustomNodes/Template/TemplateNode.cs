using System;
using System.Collections.Generic;
using System.Text;
using NodeGraphControl;
using NodeGraphControl.Xml;

namespace NodeGraphLayoutEdit.CustomNodes
{
    public class TemplateModule : NodeGraphNode
    {
        public TemplateModule(int p_X, int p_Y, NodeGraphView p_View, bool p_CanBeSelected)
            : base(p_X, p_Y, p_View, p_CanBeSelected)
        {
            this.m_sName = "TemplateModule";
            this.m_Connectors.Add(new NodeGraphConnector("Input 0", this, ConnectorType.InputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("Input 1", this, ConnectorType.InputConnector, 1));
            this.m_Connectors.Add(new NodeGraphConnector("Output 0", this, ConnectorType.OutputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("Output 1", this, ConnectorType.OutputConnector, 1));
            this.Height = 100;

        }

        public TemplateModule(XmlTreeNode p_TreeNode, NodeGraphView p_View)
            : base(p_TreeNode, p_View)
        {
            this.m_sName = "TemplateModule";
            this.m_Connectors.Add(new NodeGraphConnector("Input 0", this, ConnectorType.InputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("Input 1", this, ConnectorType.InputConnector, 1));
            this.m_Connectors.Add(new NodeGraphConnector("Output 0", this, ConnectorType.OutputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("Output 1", this, ConnectorType.OutputConnector, 1));
            this.Height = 100;
        }

        public override NodeGraphData Process()
        {
            NodeGraphData v_input0 = m_Connectors[0].Process();
            NodeGraphData v_input1 = m_Connectors[1].Process();
            NodeGraphInvalidData v_OutError = new NodeGraphInvalidData();
            if (v_input0 is NodeGraphInvalidData) v_OutError.Merge(v_input0 as NodeGraphInvalidData);
            if (v_input1 is NodeGraphInvalidData) v_OutError.Merge(v_input1 as NodeGraphInvalidData);
            if (v_OutError.InvalidNodes.Count > 0) return v_OutError;

            float input0 = (m_Connectors[0].Process() as DataTypes.NodeGraphFloatData).Value;
            float input1 = (m_Connectors[1].Process() as DataTypes.NodeGraphFloatData).Value;
            return new DataTypes.NodeGraphFloatData(input0 + input1);
        }
    }
}
