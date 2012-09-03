using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NuiApiWrapper
{
    public class PipelineDescriptor
    {
        public string name;
        public string description;
        public string author;
        public ModuleDescriptor[] modules;
        public EndpointDescriptor[] inputEndpoints;
        public EndpointDescriptor[] outputEndpoints;
        public ConnectionDescriptor[] connections;
    }
}
