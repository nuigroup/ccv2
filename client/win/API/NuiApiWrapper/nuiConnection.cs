using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NuiApiWrapper
{
    /** 
     * public static string connection = @"
     * {
     * "success": 1,
     * "descriptor": {
     * "sourceModule" : 0,
     * "sourcePort" : 0,
     * "destinationModule" : 1,
     * "destinationPort": 0,
     * "deepCopy": 0,
     * "asyncMode": 0,
     * "buffered": 0,
     * "bufferSize": 0,
     * "lastPacket": 0,
     * "overflow": 0, 
     * }
     */

    public class ConnectionDescriptor
    {
        public int sourceModule;
        public int sourcePort;
        public int destinationModule;
        public int destinationPort;
        public bool deepCopy;
        public int asyncMode;
        public int buffered;
        public int bufferSize;
        public bool lastPacket;
        public int overflow;
    }
}
