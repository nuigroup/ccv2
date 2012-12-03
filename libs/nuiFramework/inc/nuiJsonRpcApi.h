#ifndef NUI_JSONAPI_H
#define NUI_JSONAPI_H

#pragma once

#include "pasync.h"

#include <sstream>

#include "nuiFrameworkManager.h"

#include "json/json.h"
#include "networking.h"
#include "jsonrpc.h"
#include "jsonrpc_tcpserver.h"

#include "nuiModule.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"

//! \class nuiJsonRpcApi
//! Class to handle commands from client to pipeline
class nuiJsonRpcApi : public pt::thread
{
public:
    //! Singleton
	static nuiJsonRpcApi *getInstance();
    
    //! creates json rpc server on specified address and port
	bool init(std::string address, int port);

    //! starts thread that will listen to json rpc calls
	void startApi();

    //! sets flag ordering thread to stop
	void stopApi(bool killServer = false);

    //! checks whether rpc commands were created (init called on JsonRpcApi)
	bool isInitialized();

    //! checks whether the thread was stopped and cleanup called
	bool isFinished();

    static Json::Value serialize_workflow(nuiModuleDescriptor* descriptor);
    static Json::Value serialize_pipeline(nuiModuleDescriptor* descriptor);
    static Json::Value serialize_module(nuiModuleDescriptor* descriptor);
    static Json::Value serialize_endpoint(nuiEndpointDescriptor *descriptor);
    static Json::Value serialize_connection(nuiDataStreamDescriptor *descriptor);
protected:
private:
    nuiJsonRpcApi();

    Json::Rpc::TcpServer *server;
	bool finished;
	bool want_quit;

	void setFailure(Json::Value &response);
	void setFailure(Json::Value &response, std::string message);
	void setSuccess(Json::Value &response);

	void execute();
	void cleanup();

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
	bool nui_save_workflow(const Json::Value& root, Json::Value& response);
};

#endif // NUI_JSON_API