#include "boost/cstdint.hpp"
#include "pasync.h"

#include "json/json.h"
#include "networking.h"
#include "jsonrpc.h"
#include "jsonrpc_tcpserver.h"

#include "nuiModule.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"

//! TODO : optional arguments methods
class nuiJsonRpcApi : public pt::thread
{
public:
	static nuiJsonRpcApi *getInstance();
	bool init(std::string address, int port);
	void startApi();
	bool isInitialized();
	bool isFinished();
protected:
private:
	bool finished;
	bool want_quit;

	void setFailure(Json::Value &responce);
	void setSuccess(Json::Value &responce);

	nuiJsonRpcApi();
	Json::Rpc::TcpServer *server;
	void execute();
	void cleanup();

	Json::Value serialize_pipeline(nuiModuleDescriptor* descriptor);
	Json::Value serialize_module(nuiModuleDescriptor* descriptor);
	Json::Value serialize_endpoint(nuiEndpointDescriptor *descriptor);
	Json::Value serialize_connection(nuiDataStreamDescriptor *descriptor);

	bool nui_list_dynamic(const Json::Value& root, Json::Value& response);
	bool nui_list_pipelines(const Json::Value& root, Json::Value& response);
	bool nui_workflow_start(const Json::Value& root, Json::Value& response);
	bool nui_workflow_stop(const Json::Value& root, Json::Value& response);
	bool nui_workflow_quit(const Json::Value& root, Json::Value& response);
	bool nui_create_pipeline(const Json::Value& root, Json::Value& response);
	bool nui_create_module(const Json::Value& root, Json::Value& response);
	bool nui_create_connection(const Json::Value& root, Json::Value& response);
	bool nui_update_pipeline(const Json::Value& root, Json::Value& response);
	bool nui_update_pipelineProperty(const Json::Value& root, Json::Value& response);
	bool nui_update_moduleProperty(const Json::Value& root, Json::Value& response);
	bool nui_update_endpoint(const Json::Value& root, Json::Value& response);
	bool nui_update_connection(const Json::Value& root, Json::Value& response);
	bool nui_update_endpointCount(const Json::Value& root, Json::Value& response);
	bool nui_delete_pipeline(const Json::Value& root, Json::Value& response);
	bool nui_delete_module(const Json::Value& root, Json::Value& response);
	bool nui_delete_endpoint(const Json::Value& root, Json::Value& response);
	bool nui_delete_connection(const Json::Value& root, Json::Value& response);
	bool nui_get_current(const Json::Value& root, Json::Value& response);
	bool nui_get_pipeline(const Json::Value& root, Json::Value& response);
	bool nui_get_module(const Json::Value& root, Json::Value& response);
	bool nui_get_connection(const Json::Value& root, Json::Value& response);
	bool nui_navigate_push(const Json::Value& root, Json::Value& response);
	bool nui_navigate_pop(const Json::Value& root, Json::Value& response);
	bool nui_save_pipeline(const Json::Value& root, Json::Value& response);
};