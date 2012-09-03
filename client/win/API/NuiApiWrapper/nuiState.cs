#define TEST

using System;
using System.Collections.Generic;
using NuiApiWrapper;

using System.Collections;
using System.Net;
using Jayrock.Json;
using Jayrock.JsonRpc;

using System.Web.Services;
using Jayrock.Json.Conversion;

namespace NuiApiWrapper
{
    public class NuiState
    {
        private NuiState() { }

        private static NuiState instance;
        public static NuiState Instance
        {
            get
            {
                if (instance == null)
                    instance = new NuiState();
                return instance;
            }
        }

        JsonRpcClient client;

        //! list of modules available for building pipelines
        private List<ModuleDescriptor> availableModules;

        //! list of pipelines available for building pipelines
        private List<PipelineDescriptor> availablePipelines;

        //! current level, root is 0;
        private int level = 0;

        //! descriptor for current pipeline
        private PipelineDescriptor currentPipeline;

        public void Connect(string url = "http://localhost:8080/")
        {
            client = new JsonRpcClient();
            client.Url = url;
        }

#if TEST
    #region TestData

        private string test_RootPipelineJSON = @"
{
        ""success"" : 1,
        ""descriptor"" :
        {
		""name"" : ""root"",
		""description"" : ""description"",
		""author"": ""noname"",
		""modules"":[
			{""name"" : ""module1"",
			""description"" : ""this is module 1"",
			""author"": ""alushnikov1"",
			""inputEndpoints"": [],
			""outputEndpoints"": [
				{
					""index"" : 0,
					""descriptor"" : ""stream""
				}
			]},
			{""name"" : ""pipeline1"",
			""description"" : ""this is pipeline 1, small pipeline for testing"",
			""author"": ""alushnikov_pipeline"",
			""inputEndpoints"": [
			{
				""index"" : 0,
				""descriptor"" : ""stream""
			}],
			""outputEndpoints"": [
				{
					""index"" : 0,
					""descriptor"" : ""stream""
				}
			]},
            {""name"" : ""module3"",
			""description"" : ""this is module 3"",
			""author"": ""alushnikov3"",
			""inputEndpoints"": [
			{
				""index"" : 0,
				""descriptor"" : ""stream""
			}],
			""outputEndpoints"": []},
        ],
		""inputEndpoints"": [],
		""outputEndpoints"": [],
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
			},
            {
				""sourceModule"" : 1,
				""sourcePort"" : 0,
				""destinationModule"" : 2,
				""destinationPort"": 0,
				""deepCopy"": 0,
				""asyncMode"": 0,
				""buffered"": 0,
				""bufferSize"": 100,
				""lastPacket"": 0,
				""overflow"": 0,
			},
        ]
        }
    }
";
        private string test_Pipeline1JSON = @"{
        ""success"" : 1,
        ""descriptor"" : {
		""name"" : ""pipeline1"",
		""description"" : ""this is pipeline 1, small pipeline for testing"",
		""author"": ""alushnikov_pipeline"",
		""modules"":
        [
			{""name"" : ""module2"",
			""description"" : ""this is module 2"",
			""author"": ""alushnikov2"",
			""inputEndpoints"": 
            [
				{
					""index"" : 0,
					""descriptor"" : ""stream""
				}
            ],
			""outputEndpoints"" : 
            [
				{
					""index"" : 0,
					""descriptor"" : ""stream""
				}
			]},
        ],
		""inputEndpoints"" : 
        [
				{
					""index"" : 0,
					""descriptor"" : ""stream""
				}
        ],
		""outputEndpoints"": [],
		""connections"" : [
            {
				""sourceModule"" : 0x0FFFFFFF,
				""sourcePort"" : 0,
				""destinationModule"" : 0,
				""destinationPort"": 0,
				""deepCopy"": 0,
				""asyncMode"": 0,
				""buffered"": 0,
				""bufferSize"": 100,
				""lastPacket"": 0,
				""overflow"": 0,
			},
        ]
    }}";
    #endregion
#endif

        /************************************************************************/
        /* NAVIGATE                                                             */
        /************************************************************************/
        //! move into next pipeline 
#if TEST
        public PipelineDescriptor NavigatePush(int moduleIdx)
        {
            ImportContext impctx = new ImportContext();
            impctx.Register(new ListImporter<ModuleDescriptor>());
            impctx.Register(new ListImporter<ConnectionDescriptor>());
            impctx.Register(new ListImporter<EndpointDescriptor>());

            PipelineDescriptor newPipeline = ((Response<PipelineDescriptor>)impctx.Import(
                typeof(Response<PipelineDescriptor>),
                JsonText.CreateReader(test_Pipeline1JSON))).GetResponse();

            NuiState.Instance.level++;
            currentPipeline = newPipeline;

            return newPipeline;
        }
#else
        //! TODO : discuss
        //! should really be module index and return null if we can navigate into selected module
        public PipelineDescriptor NavigatePush(int moduleIdx)
        {
            PipelineDescriptor newPipeline  = (PipelineDescriptor) NuiState.Instance.client.Invoke( 
                typeof(PipelineDescriptor),
                "web_navigate_push");

            NuiState.Instance.level++;
            currentPipeline = newPipeline;

            return newPipeline;
        }
#endif

#if TEST
        //! move out of current pipeline
        public PipelineDescriptor NavigatePop()
        {
            ImportContext impctx = new ImportContext();
            impctx.Register(new ListImporter<ModuleDescriptor>());
            impctx.Register(new ListImporter<ConnectionDescriptor>());
            impctx.Register(new ListImporter<EndpointDescriptor>());

            PipelineDescriptor newPipeline = ((Response<PipelineDescriptor>)impctx.Import(
                typeof(Response<PipelineDescriptor>),
                JsonText.CreateReader(test_RootPipelineJSON))).GetResponse();

            NuiState.Instance.level--;
            currentPipeline = newPipeline;

            return newPipeline;
        }
#else
        //! move out of current pipeline
        public PipelineDescriptor NavigatePop()
        {
            PipelineDescriptor newPipeline = (PipelineDescriptor)NuiState.Instance.client.Invoke(
                typeof(PipelineDescriptor),
                "web_navigate_pop");

            NuiState.Instance.level--;
            currentPipeline = newPipeline;

            return newPipeline;
        }
#endif

        /************************************************************************/
        /* LIST                                                                 */
        /************************************************************************/
#if TEST
        //! lists dynamic modules
        public string[] ListDynamic()
        {
            string[] listDynamic = {"module1", "module2", "module3"};

            return listDynamic;
        }
#else
        //! lists dynamic modules
        public string[] ListDynamic()
        {
            string[] listDynamic = (string[])(new ArrayList((ICollection)
                NuiState.Instance.client.Invoke(
                    typeof(PipelineDescriptor),
                    "web_list_dynamic")).ToArray(typeof(string)));

            return listDynamic;
        }
#endif


#if TEST
        //! list pipeline modules
        public string[] ListPipeline()
        {
            string[] listPipelines = {"root", "pipeline1"};

            return listPipelines;
        }
#else
        //! list pipeline modules
        public string[] ListPipeline()
        {
            string[] listPipelines = (string[])(new ArrayList((ICollection)
                NuiState.Instance.client.Invoke(
                    typeof(PipelineDescriptor),
                    "web_list_pipeline")).ToArray(typeof(string)));

            return listPipelines;
        }
#endif


        /************************************************************************/
        /* WORKFLOW                                                             */
        /************************************************************************/
        public bool WorkflowStart()
        {
            return (bool)NuiState.Instance.client.Invoke(
                    typeof(bool),
                    "web_workflow_start");
        }

        public bool WorkflowStop()
        {
            return (bool)NuiState.Instance.client.Invoke(
                    typeof(bool),
                    "web_workflow_stop");
        }

        public bool WorkflowQuit()
        {
            return (bool)NuiState.Instance.client.Invoke(
                    typeof(bool),
                    "web_workflow_quit");
        }

        /************************************************************************/
        /* CREATE                                                               */
        /************************************************************************/
        //! creates entirely new pipeline descriptor
        public PipelineDescriptor CreatePipeline(string name)
        {
            PipelineDescriptor pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_create_pipeline",
                name);

            return pipeline;
        }

        //! creates new module descriptor in pipeline
        public ModuleDescriptor CreateModule(string pipelineName, string moduleName)
        {
            ModuleDescriptor module = (ModuleDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_create_module",
                pipelineName, moduleName);

            return module;
        }

        //! create new connection
        public bool CreateConnection(string pipelineName,
            int srcModuleIdx, int srcModulePort,
            int dstModuleIdx, int dstModulePort)
        {
            bool status = (bool)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_create_module",
                pipelineName, srcModuleIdx, srcModulePort, dstModuleIdx, dstModulePort);

            return status;
        }

        /************************************************************************/
        /* UPDATE                                                               */
        /************************************************************************/
        public PipelineDescriptor UpdatePipeline(string pipelineName, string newName = null, string newDescription = null, string author = null)
        {
            PipelineDescriptor pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_update_pipeline",
                pipelineName, newName, newDescription, author);

            return pipeline;
        }

        public PipelineDescriptor UpdatePipelineProperty(string pipelineName,
            string key, object value, string description)
        {
            PipelineDescriptor pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_update_pipelineProperty",
                pipelineName, key, value, description);

            return pipeline;
        }

        public PipelineDescriptor UpdateModuleProperty(string pipelineName,
            int moduleIdx, string key, object value)
        {
            PipelineDescriptor pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_update_moduleProperty",
                pipelineName, moduleIdx, key, value);

            return pipeline;
        }

        public EndpointDescriptor UpdateEndpoint(string type, int endpointIdx, string descriptor, int newIndex = -1)
        {
            EndpointDescriptor endpoint;
            if (newIndex < 0)
            {
                endpoint = (EndpointDescriptor)NuiState.Instance.client.InvokeVargs(
                    typeof(EndpointDescriptor),
                    "web_update_endpoint",
                    type, endpointIdx, descriptor, null);
            }
            else 
            {
                endpoint = (EndpointDescriptor)NuiState.Instance.client.InvokeVargs(
                    typeof(EndpointDescriptor),
                    "web_update_endpoint",
                    type, endpointIdx, descriptor, newIndex);
            }

            return endpoint;
        }

        public ConnectionDescriptor UpdateConnection(string pipelineName,
            int srcModuleIdx, int srcModulePort,
            int dstModuleIdx, int dstModulePort, params KeyValuePair<string,object>[] keyValues)
        {
            var connection = (ConnectionDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(ConnectionDescriptor),
                "web_update_endpoint",
                pipelineName, srcModuleIdx, srcModulePort,
                dstModuleIdx, dstModulePort, keyValues);

            return connection;
        }

        public int UpdateEndpointCount(string pipelineName, string type, int newCount)
        {
            var count = (int)NuiState.Instance.client.InvokeVargs(
                typeof(int),
                "web_update_endpointCount",
                pipelineName, type, newCount);

            return count;
        }

        /************************************************************************/
        /* DELETE                                                               */
        /************************************************************************/
        public bool DeletePipeline(string pipelineName)
        {
            var status = (bool)NuiState.Instance.client.InvokeVargs(
                typeof(bool),
                "web_delete_pipeline",
                pipelineName);

            return status;
        }

        public PipelineDescriptor DeleteModule(string pipelineName, int moduleIndex)
        {
            var pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                 typeof(PipelineDescriptor),
                 "web_delete_module",
                 pipelineName, moduleIndex);

            return pipeline;
        }

        public PipelineDescriptor DeleteConnection(string pipelineName,
            int srcModuleIdx, int srcModulePort,
            int dstModuleIdx, int dstModulePort)
        {
            var pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_delete_connection",
                pipelineName, srcModuleIdx, srcModulePort, dstModuleIdx, dstModulePort);

            return pipeline;
        }

        /************************************************************************/
        /* GET                                                                  */
        /************************************************************************/
#if TEST
        public PipelineDescriptor GetPipeline(string pipelineName)
        {
            PipelineDescriptor pipeline = null;
            ImportContext impctx = new ImportContext();
            impctx.Register(new ListImporter<ModuleDescriptor>());
            impctx.Register(new ListImporter<EndpointDescriptor>());
            impctx.Register(new ListImporter<ConnectionDescriptor>());

            switch (pipelineName)
            {
                case "root": 
                    pipeline = ((Response<PipelineDescriptor>)impctx.Import(
                        typeof(Response<PipelineDescriptor>),
                        JsonText.CreateReader(test_RootPipelineJSON))).GetResponse();
                    break;

                case "pipeline1":
                    pipeline = ((Response<PipelineDescriptor>)impctx.Import(
                        typeof(Response<PipelineDescriptor>),
                        JsonText.CreateReader(test_Pipeline1JSON))).GetResponse();
                    break;
            }

            return pipeline;
        }
#else
        public PipelineDescriptor GetPipeline(string pipelineName)
        {
            var pipeline = (PipelineDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(PipelineDescriptor),
                "web_get_pipeline",
                pipelineName);

            return pipeline;
        }
#endif


#if TEST
        public ModuleDescriptor GetModule(string pipelineName, int moduleIdx)
        {
            //! TODO : test data
            return null;
        }
#else
        public ModuleDescriptor GetModule(string pipelineName, int moduleIdx)
        {
            var module = (ModuleDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(ModuleDescriptor),
                "web_get_module",
                pipelineName, moduleIdx);

            return module;
        }
#endif


#if TEST
        public ConnectionDescriptor GetConnection(string pipelineName, int connectionIdx)
        {
            //! TODO : test data
            return null;
        }
#else
        public ConnectionDescriptor GetConnection(string pipelineName, int connectionIdx)
        {
            var connection = (ConnectionDescriptor)NuiState.Instance.client.InvokeVargs(
                typeof(ConnectionDescriptor),
                "web_get_connection",
                pipelineName, connectionIdx);

            return connection;
        }
#endif

        /************************************************************************/
        /* SAVE                                                                 */
        /************************************************************************/
        public bool Save(string pipelineName, string fileName)
        {
            var response = (bool)NuiState.Instance.client.InvokeVargs(
                typeof(bool),
                "web_save_pipeline",
                pipelineName, fileName);

            return response;
        }
    }
}