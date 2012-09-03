using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NuiApiWrapper
{
    public static class NuiUtils
    {
        public static bool IsPipeline(string moduleName)
        {
            return NuiState.Instance.ListPipeline().Contains(moduleName);
        }
    }
}
