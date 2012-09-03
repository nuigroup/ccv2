using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WrapperTest
{
    public static class SampleData
    {
        public static string connection = @"
{
    ""success"": 1,
    ""descriptor"": {
	""sourceModule"" : 6,
	""sourcePort"" : 2,
	""destinationModule"" : 7,
	""destinationPort"": 3,
	""deepCopy"": 1,
	""asyncMode"": 0,
	""buffered"": 1,
	""bufferSize"": 0,
	""lastPacket"": 1,
	""overflow"": 1,}
}";
        public static string module = @"{
    ""success"": 1,
    ""descriptor"": {
		""name"" : ""pipeline1"",
		""description"" : ""description"",
		""author"": ""author"",
		""inputEndpoints"": [
			{
				""index"" : 1,
				""descriptor"" : ""audio""
			},
			{
				""index"" : 2,
				""descriptor"" : ""video""
			}],
		""outputEndpoints"": [
			{
				""index"" : 1,
				""descriptor"" : ""tree""
			}]
    }
}
";
        public static string pipeline = @"
{
    ""success"": 1,
    ""descriptor"": {
		""name"" : "" pipelineName "",
		""description"" : ""description"",
		""author"": ""author"",
		""modules"":[
			""name"" : ""moduleX"",
			""description"" : ""description"",
			""author"": ""author"",
			""inputEndpoints"": [
			{
				""index"" : 0,
				""descriptor"" : ""audio""
			},
			{
				""index"" : 1,
				""descriptor"" : ""video""
			}],
			""outputEndpoints"": [
				{
					""index"" : 0,
					""descriptor"" : ""intermediate""
				}
			],
			""name"" : ""moduleY"",
			""description"" : ""description"",
			""author"": ""author"",
			""inputEndpoints"": [
			{
				""index"" : 0,
				""descriptor"" : ""intermediate""
			}],
			""outputEndpoints"": [
				{
					""index"" : 0,
					""descriptor"" : ""tree""
				}
			]],
		""inputEndpoints"": [
			{
				""index"" : 1,
				""descriptor"" : ""audio""
			},
			{
				""index"" : 2,
				""descriptor"" : ""video""
			}],
		""outputEndpoints"": [
			{
				""index"" : 1,
				""descriptor"" : ""tree""
			}],
		""connections"" : [
			{
				""sourceModule"" : 0,
				""sourcePort"" : 0,
				""destinationModule"" : 1,
				""destinationPort"": 0,
				""deepCopy"": 0,
				""asyncMode"": 0,
				""buffered"": 0,
				""bufferSize"": 100,
				""lastPacket"": 0,
				""overflow"": 0,
			}]
    }
}
";
    }
}
