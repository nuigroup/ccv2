using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NuiApiWrapper
{
    public class Response<T>
        where T : new()
    {
        /// <summary>
        /// Success status. To get response GetResponse should be used
        /// </summary>
        public bool    success;

        /// <summary>
        /// Do NOT use directly, call GetResponse instead
        /// </summary>
        public T       descriptor;
        
        public T GetResponse()
        {
            if (success)
                return descriptor;
            else
                return default(T);
        }
    }
}
