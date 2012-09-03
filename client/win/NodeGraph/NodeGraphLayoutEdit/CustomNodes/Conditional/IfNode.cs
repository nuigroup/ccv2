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

// Needed for the object to run
using NodeGraphControl;
using NodeGraphControl.Xml;

// We will be using data types
using NodeGraphLayoutEdit.DataTypes;



namespace NodeGraphLayoutEdit.CustomNodes
{
    /// <summary>
    /// Sample Custom Node with comments & full functionalities.
    /// </summary>
    public class IfNode : NodeGraphNode
    {
        
        [Category("If Node")]
        public float DefaultValue
        {
            get { return m_fDefaultValue; }
            set { m_fDefaultValue = value; }
        }
        [Category("If Node")]
        public IfNodeBehavior Behavior
        {
            get { return m_eBehavior; }
            set { m_eBehavior = value; }
        }


        private float m_fDefaultValue;
        private IfNodeBehavior m_eBehavior;



        /// <summary>
        /// EDITOR CONSTRUCTOR: Builds the node.
        /// </summary>
        /// <param name="p_X">X Position (in the view)</param>
        /// <param name="p_Y">Y Position (in the view)</param>
        /// <param name="p_View">NodeGraphView Object that will contain the node</param>
        /// <param name="p_CanBeSelected">if the node can be selected</param>
        public IfNode(int p_X, int p_Y, NodeGraphView p_View, bool p_CanBeSelected)
            : base(p_X, p_Y, p_View, p_CanBeSelected)
        {
            this.m_sName = "If condition";
            this.m_Connectors.Add(new NodeGraphConnector("A", this, ConnectorType.InputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("B", this, ConnectorType.InputConnector, 1));
            this.m_Connectors.Add(new NodeGraphConnector("A>B", this, ConnectorType.InputConnector, 2));
            this.m_Connectors.Add(new NodeGraphConnector("A=B", this, ConnectorType.InputConnector, 3));
            this.m_Connectors.Add(new NodeGraphConnector("A<B", this, ConnectorType.InputConnector, 4));
            this.m_Connectors.Add(new NodeGraphConnector("Value", this, ConnectorType.OutputConnector, 0));
            this.Height = 110;

            // Sets our members to default values
            this.m_eBehavior = IfNodeBehavior.ErrorOnMissingInput;
            this.m_fDefaultValue = 0.0f;

        }
        /// <summary>
        /// XML CONSTRUCTOR: Should not be called by API Users, used for deserialization
        /// </summary>
        /// <param name="p_TreeNode">XmlTreeNode used for Deserialization</param>
        /// <param name="p_View">NodeGraphView Object that will contain the node</param>
        public IfNode(XmlTreeNode p_TreeNode, NodeGraphView p_View)
            : base(p_TreeNode, p_View)
        {
            this.m_sName = "If condition";
            this.m_Connectors.Add(new NodeGraphConnector("A", this, ConnectorType.InputConnector, 0));
            this.m_Connectors.Add(new NodeGraphConnector("B", this, ConnectorType.InputConnector, 1));
            this.m_Connectors.Add(new NodeGraphConnector("A>B", this, ConnectorType.InputConnector, 2));
            this.m_Connectors.Add(new NodeGraphConnector("A=B", this, ConnectorType.InputConnector, 3));
            this.m_Connectors.Add(new NodeGraphConnector("A<B", this, ConnectorType.InputConnector, 4));
            this.m_Connectors.Add(new NodeGraphConnector("Value", this, ConnectorType.OutputConnector, 0));
            this.Height = 110;

            // Parses XML to get back our values.

                    // Since m_eBehavior is an enum, i've stocked it as a string, then use it to compare
                    if (p_TreeNode.m_attributes["Behavior"] == "ErrorOnMissingInput") this.m_eBehavior = IfNodeBehavior.ErrorOnMissingInput;
                    else this.m_eBehavior = IfNodeBehavior.ReturnDefaultValue;

                    // Float values can be a pain because of the type of the dot. It's always encoded as en-us CultureInfo
                    this.m_fDefaultValue = float.Parse(p_TreeNode.m_attributes["DefaultValue"], System.Globalization.CultureInfo.GetCultureInfo("en-us"));

        }

        /// <summary>
        /// Overrides XML Serialization by using base then adding our custom members
        /// </summary>
        /// <param name="p_Parent">Parent Tree Node used in Serialization</param>
        /// <returns>a Serialized Copy of our object</returns>
        public override XmlTreeNode SerializeToXML(XmlTreeNode p_Parent)
        {
            // Base SerializeToXML provides initial work for the XMLTreeNode
            XmlTreeNode v_Out =  base.SerializeToXML(p_Parent);

                // ... then I will encode our custom data
                v_Out.AddParameter("Behavior", m_eBehavior.ToString());
                v_Out.AddParameter("DefaultValue", m_fDefaultValue.ToString(System.Globalization.CultureInfo.GetCultureInfo("en-us")));

            return v_Out;
        }


        /// <summary>
        /// The Process function, returns NodeGraphData (generic) but can return more complex types (in our case, NodeGraphFloatData)
        /// </summary>
        /// <returns>NodeGraphFloatData</returns>
        public override NodeGraphData Process()
        {
            NodeGraphInvalidData v_Errors = new NodeGraphInvalidData();
            bool v_HasErrors = false;

            float valueA = 0.0f;
            float valueB = 0.0f;
            float valueOut = 0.0f;

            // First we get the A & B values
            NodeGraphData A = m_Connectors[0].Process();
            NodeGraphData B = m_Connectors[1].Process();
            NodeGraphData Result;
                        
            
                    // 1st Check: validity of incoming Data
                    if ((A is NodeGraphInvalidData)) { v_Errors.Merge(A as NodeGraphInvalidData); v_HasErrors = true; }
                    if ((B is NodeGraphInvalidData)) { v_Errors.Merge(B as NodeGraphInvalidData); v_HasErrors = true; }

                    // 2nd Check: validity of type Data
                    if (A is NodeGraphFloatData) valueA = (A as NodeGraphFloatData).Value;
                    else { v_Errors.AddInvalidNode(this, "A Input is not NodeGraphFloatData"); v_HasErrors = true; }
                    if (B is NodeGraphFloatData) valueB = (B as NodeGraphFloatData).Value;
                    else { v_Errors.AddInvalidNode(this, "B Input is not NodeGraphFloatData"); v_HasErrors = true; }




                    // If we are ok to proceed...
                    if (!v_HasErrors)
                    {
                        // Comparison and third test
                        if (valueA == valueB)
                        {
                            // Get the output
                            Result = m_Connectors[3].Process();
                            // If this is an error, add to the current errors
                            if ((Result is NodeGraphInvalidData)) { v_Errors.Merge(Result as NodeGraphInvalidData); v_HasErrors = true; }
                            else
                            {
                                // If the type is good....
                                if (Result is NodeGraphFloatData) valueOut = (Result as NodeGraphFloatData).Value;
                                else // .. or add an error
                                { v_Errors.AddInvalidNode(this, "A==B Input is not NodeGraphFloatData"); v_HasErrors = true; }
                            }
                        }
                        // Same thing for other tests...
                        else if (valueA < valueB)
                        {
                            Result = m_Connectors[4].Process();
                            if ((Result is NodeGraphInvalidData)) { v_Errors.Merge(Result as NodeGraphInvalidData); v_HasErrors = true; }
                            else
                            {
                                if (Result is NodeGraphFloatData) valueOut = (Result as NodeGraphFloatData).Value;
                                else { v_Errors.AddInvalidNode(this, "A<B Input is not NodeGraphFloatData"); v_HasErrors = true; }
                            }

                        }
                        else // (A > B) 
                        {
                            Result = m_Connectors[2].Process();
                            if ((Result is NodeGraphInvalidData)) { v_Errors.Merge(Result as NodeGraphInvalidData); v_HasErrors = true; }
                            else
                            {
                                if (Result is NodeGraphFloatData) valueOut = (Result as NodeGraphFloatData).Value;
                                else { v_Errors.AddInvalidNode(this, "A>B Input is not NodeGraphFloatData"); v_HasErrors = true; }
                            }
                        }
                    }



                    // Finally... we are returning data, whether it's good or not
                    if (v_HasErrors)
                    {
                        if (m_eBehavior == IfNodeBehavior.ErrorOnMissingInput)
                        {
                            return v_Errors;
                        }
                        else
                        {
                            return new NodeGraphFloatData(this.m_fDefaultValue);
                        }
                    }
                    else
                    {
                        return new NodeGraphFloatData(valueOut);
                    }


        }


        /// <summary>
        /// Here's the enum that controls the behavior of the node.
        /// </summary>
        public enum IfNodeBehavior
        {
            // Returns a NodeGraphInvalidData on failure
            ErrorOnMissingInput,
            // Returns default value on failure
            ReturnDefaultValue
        }
    }



}
