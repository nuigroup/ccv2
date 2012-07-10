#include "nuiJsonRpcApi.h"
#include "nuiFrameworkManager.h"

nuiJsonRpcApi *nuiJsonRpcApi::getInstance()
{
	static nuiJsonRpcApi *instance = NULL;
	if(instance == NULL)
		instance = new nuiJsonRpcApi();
	return instance;
};

bool nuiJsonRpcApi::init(std::string address, int port)
{
	if(server == NULL)
		server = new Json::Rpc::TcpServer(std::string("127.0.0.1"), 7500);

	if(!server->Bind())
	{
		delete server;
		return false;
	}
	if(!server->Listen())
	{
		delete server;
		return false;
	}

	this->finished = false;

	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_list_dynamic,std::string("nui_list_dynamic")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_list_pipelines,std::string("nui_list_pipelines")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_workflow_start,std::string("nui_workflow_start")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_workflow_stop,std::string("nui_workflow_stop")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_workflow_quit,std::string("nui_workflow_quit")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_create_pipeline,std::string("nui_create_pipeline")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_create_module,std::string("nui_create_module")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_create_connection,std::string("nui_create_connection")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_update_pipeline,std::string("nui_update_pipeline")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_update_pipelineProperty,std::string("nui_update_pipelineProperty")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_update_moduleProperty,std::string("nui_update_moduleProperty")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_update_endpoint,std::string("nui_update_endpoint")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_update_connection,std::string("nui_update_connection")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_update_endpointCount,std::string("nui_update_endpointCount")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_delete_pipeline,std::string("nui_delete_pipeline")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_delete_module,std::string("nui_delete_module")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_delete_endpoint,std::string("nui_delete_endpoint")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_delete_connection,std::string("nui_delete_connection")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_get_current,std::string("nui_get_current")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_get_pipeline,std::string("nui_get_pipeline")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_get_module,std::string("nui_get_module")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_get_connection,std::string("nui_get_connection")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_navigate_push,std::string("nui_navigate_push")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_navigate_pop,std::string("nui_navigate_pop")));
	server->AddMethod(new Json::Rpc::RpcMethod<nuiJsonRpcApi>(*this, &nuiJsonRpcApi::nui_save_pipeline,std::string("nui_save_pipeline")));

	return true;
};

bool nuiJsonRpcApi::isInitialized()
{
	if(server!=NULL)
		return true;
	return false;
};

void nuiJsonRpcApi::startApi()
{
	if(isInitialized())
		start();
};

void nuiJsonRpcApi::execute()
{
	while(!want_quit)
	{
		server->WaitMessage(0);	
	}
	finished = true;
}

void nuiJsonRpcApi::cleanup() { };

nuiJsonRpcApi::nuiJsonRpcApi() : pt::thread(false) 
{
	this->want_quit = false;
	this->server = NULL;
};

bool nuiJsonRpcApi::nui_list_dynamic( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::vector<std::string>* list;
	list = nuiFrameworkManager::getInstance()->listDynamicModules();

	Json::Value* jModules = new Json::Value();
	std::vector<std::string>::iterator it;
	for(it = list->begin() ; it!=list->end();it++)
		jModules->append(*it);

	setSuccess(response);
	response["list"] = *jModules;
	
	return true;
}

bool nuiJsonRpcApi::nui_list_pipelines( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string hosterName = root["params"]["hostername"].asString();

	std::vector<std::string> *list;
	list = nuiFrameworkManager::getInstance()->listPipelines(hosterName);

	Json::Value* jModules = new Json::Value();
	std::vector<std::string>::iterator it;
	for(it = list->begin() ; it!=list->end();it++)
		jModules->append(*it);

	setSuccess(response);
	response["list"] = *jModules;
	
	return true;
}

bool nuiJsonRpcApi::nui_workflow_start( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	nuiFrameworkManagerErrorCode error = nuiFrameworkManager::getInstance()->workflowStart();
	if(error == NUI_FRAMEWORK_MANAGER_OK)
	{
		setSuccess(response);
		return true;
	}
	else
	{
		setFailure(response);
		return false;
	}
}

bool nuiJsonRpcApi::nui_workflow_stop( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	nuiFrameworkManagerErrorCode error = nuiFrameworkManager::getInstance()->workflowStop();
	if(error == NUI_FRAMEWORK_MANAGER_OK)
	{
		setSuccess(response);
		return true;
	}
	else
	{
		setFailure(response);
		return false;
	}
}

bool nuiJsonRpcApi::nui_workflow_quit( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	this->want_quit = true;

	setSuccess(response);

	return true;

}

bool nuiJsonRpcApi::nui_create_pipeline( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();

	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance()->createPipeline(pipeline);

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_create_module( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string module = root["params"]["module"].asString();

	nuiModuleDescriptor *descriptor = nuiFrameworkManager::getInstance()->createModule(pipeline,module);
	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_create_connection( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();	
	int srcIndex = root["params"]["source"].asInt();
	int srcPort = root["params"]["sourcePort"].asInt();
	int dstIndex = root["params"]["destination"].asInt();
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiDataStreamDescriptor* descriptor = nuiFrameworkManager::getInstance()->
		createConnection(pipeline,srcIndex,dstIndex,srcPort,dstPort);

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_connection(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_pipeline( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString(); 
	std::string newName = root["params"]["name"].asString();
	std::string newDescription = root["params"]["description"].asString();
	std::string newAuthor = root["params"]["author"].asString();

	nuiModuleDescriptor* descr = nuiFrameworkManager::getInstance()->getPipeline(pipeline);

	if(descr == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		if(newName != "") descr->setName(newName);
		descr->setAuthor(newAuthor);
		descr->setDescription(newDescription);
		nuiModuleDescriptor* res = nuiFrameworkManager::getInstance()->updatePipeline(pipeline, descr);
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(res);
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_pipelineProperty( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string key = root["params"]["key"].asString();
	std::string value = root["params"]["value"].asString();
	std::string description = root["params"]["description"].asString();

	nuiModuleDescriptor* descr = nuiFrameworkManager::getInstance()->getPipeline(pipeline);

	std::map<std::string, nuiProperty*> props = descr->getProperties();
	std::map<std::string, nuiProperty*>::iterator property = props.find(key);
	property->second->set(value);
	property->second->setDescription(description);

	nuiModuleDescriptor* descriptor = 
		nuiFrameworkManager::getInstance()->updatePipeline(pipeline, descr);

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_moduleProperty( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string key = root["params"]["key"].asString();
	std::string value = root["params"]["value"].asString();
	std::string description = root["params"]["description"].asString();
	int moduleIndex = root["params"]["module"].asInt();

	nuiModuleDescriptor* descr = nuiFrameworkManager::getInstance()->getModule(pipeline, moduleIndex);

	std::map<std::string, nuiProperty*> props = descr->getProperties();
	std::map<std::string, nuiProperty*>::iterator property = props.find(key);
	property->second->set(value);
	property->second->setDescription(description);

	nuiModuleDescriptor* descriptor = 
		nuiFrameworkManager::getInstance()->updateModule(pipeline,moduleIndex, descr);

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_module(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_endpoint( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString(); 
	std::string type = root["params"]["type"].asString();
	std::string newDescriptor = root["params"]["newDescriptor"].asString();
	int index = root["params"]["index"].asInt();
	int newIndex = root["params"]["newIndex"].asInt();

	nuiEndpointDescriptor* current = NULL;
	nuiEndpointDescriptor* descriptor = NULL;
	if(type == "input")
	{
		current = nuiFrameworkManager::getInstance()->getInputEndpoint(pipeline,index);
		if(current == NULL)
		{
			setFailure(response);
			return false;
		}
		current->setDescriptor(newDescriptor);
		current->setIndex(newIndex);
		descriptor = nuiFrameworkManager::getInstance()->updateInputEndpoint(pipeline, index, current);
	}
	else if(type == "output")
	{
		current = nuiFrameworkManager::getInstance()->getOutputEndpoint(pipeline,index);
		if(current == NULL)
		{
			setFailure(response);
			return false;
		}
		current->setDescriptor(newDescriptor);
		current->setIndex(newIndex);
		descriptor = nuiFrameworkManager::getInstance()->updateOutputEndpoint(pipeline, index, current);
	}
	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_endpoint(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_connection( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString(); 
	int srcIndex = root["params"]["source"].asInt(); 
	int srcPort = root["params"]["sourcePort"].asInt(); 
	int dstIndex = root["params"]["destination"].asInt(); 
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiDataStreamDescriptor *current = nuiFrameworkManager::getInstance()->
		getConnection(pipeline, srcIndex,dstIndex,srcPort,dstPort);

	if(current == NULL)
	{
		setFailure(response);
		return false;
	}

	int deepCopy = response["deepCopy"].asInt();
	int asyncMode = response["asyncMode"].asInt();
	int buffered = response["buffered"].asInt();
	int bufferSize = response["bufferSize"].asInt();
	int lastPacket = response["lastPacket"].asInt();
	int overflow = response["overflow"].asInt();

	nuiDataStreamDescriptor *descriptor = nuiFrameworkManager::getInstance()->
		updateConnection(pipeline,srcIndex,dstIndex,srcPort,dstPort,current); 

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_connection(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_update_endpointCount( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString(); 
	std::string type = root["params"]["type"].asString(); 
	int newCount = root["params"]["count"].asInt();

	nuiModuleDescriptor* current = NULL;
	int countUpdated = -1;
	if(type == "input")
	{
		current = nuiFrameworkManager::getInstance()->getPipeline(pipeline);
		if(current == NULL)
		{
			setFailure(response);
			return false;
		}
		countUpdated = nuiFrameworkManager::getInstance()->setInputEndpointCount(pipeline, newCount);
	}
	else if(type == "output")
	{
		current = nuiFrameworkManager::getInstance()->getPipeline(pipeline);
		if(current == NULL)
		{
			setFailure(response);
			return false;
		}
		countUpdated = nuiFrameworkManager::getInstance()->setOutputEndpointCount(pipeline, newCount);
	}
	if(countUpdated == -1)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["count"] = countUpdated;
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_pipeline( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	nuiFrameworkManagerErrorCode error = 
		nuiFrameworkManager::getInstance()->deletePipeline(pipeline);
	if(error != NUI_FRAMEWORK_MANAGER_OK)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_module( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int index = root["params"]["moduleId"].asInt();

	nuiModuleDescriptor* descriptor = 
		nuiFrameworkManager::getInstance()->deleteModule(pipeline, index);

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_endpoint( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	std::string type = root["params"]["type"].asString();
	int index = root["params"]["index"].asInt();

	nuiModuleDescriptor* descriptor = NULL;
	if(type == "input")
	{
		descriptor = nuiFrameworkManager::getInstance()->deleteInputEndpoint(pipeline,index);
	}
	else if(type == "output")
	{
		descriptor = nuiFrameworkManager::getInstance()->deleteOutputEndpoint(pipeline,index);
	}

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_delete_connection( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString(); 
	int srcIndex = root["params"]["source"].asInt(); 
	int srcPort = root["params"]["sourcePort"].asInt(); 
	int dstIndex = root["params"]["destination"].asInt(); 
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance()->
		deleteConnection(pipeline,srcIndex,dstIndex,srcPort,dstPort);
	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_current( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance()->getCurrentPipeline();
	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_pipeline( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString(); 
    nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance()->getPipeline(pipeline);
	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_module( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString();
	int index = root["params"]["moduleId"].asInt();
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance()->getModule(pipeline, index);
	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_module(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_get_connection( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	std::string pipeline = root["params"]["pipeline"].asString(); 
	int srcIndex = root["params"]["source"].asInt(); 
	int srcPort = root["params"]["sourcePort"].asInt(); 
	int dstIndex = root["params"]["destination"].asInt(); 
	int dstPort = root["params"]["destinationPort"].asInt();

	nuiDataStreamDescriptor* descriptor = nuiFrameworkManager::getInstance()->
		getConnection(pipeline, srcIndex,dstIndex,srcPort,dstPort);

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_connection(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_navigate_push( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	int index = root["params"]["index"].asInt();
	
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance()->navigatePush(index);

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_navigate_pop( const Json::Value& root, Json::Value& response )
{
	response["id"] = root["id"];
	nuiModuleDescriptor* descriptor = nuiFrameworkManager::getInstance()->navigatePop();

	if(descriptor == NULL)
	{
		setFailure(response);
		return false;
	}
	else
	{
		setSuccess(response);
		response["descriptor"] = serialize_pipeline(descriptor);
		return true;
	}
}

bool nuiJsonRpcApi::nui_save_pipeline( const Json::Value& root, Json::Value& response )
{
	// TODO : ? still needed ?
	return true;
}

Json::Value nuiJsonRpcApi::serialize_pipeline( nuiModuleDescriptor* descriptor )
{
	Json::Value jPipeline;
	jPipeline["name"] = descriptor->getName();
	jPipeline["author"] = descriptor->getAuthor();
	jPipeline["description"] = descriptor->getDescription();

	Json::Value* jModules = new Json::Value();
	for(int i = 0 ; i<descriptor->getChildModulesCount() ; i++)
		jModules->append(serialize_module(descriptor->getChildModuleDescriptor(i)));
	jPipeline["modules"] = *jModules;

	Json::Value* jInputEndpoints = new Json::Value();
	for(int i=0 ; i<descriptor->getInputEndpointsCount() ; i++)
		jInputEndpoints->append(serialize_endpoint(descriptor->getInputEndpointDescriptor(i)));
	jPipeline["inputEndpoints"] = *jInputEndpoints;

	Json::Value* jOutputEndpoints = new Json::Value();
	for(int i=0 ; i<descriptor->getOutputEndpointsCount() ; i++)
		jOutputEndpoints->append(serialize_endpoint(descriptor->getOutputEndpointDescriptor(i)));
	jPipeline["outputEndpoints"] = *jOutputEndpoints;

	Json::Value* connections = new Json::Value();
	for (int i = 0 ; i<descriptor->getDataStreamDescriptorCount() ; i++)
		connections->append(serialize_connection(descriptor->getDataStreamDescriptor(i)));
	jPipeline["connections"] = *connections;

	return jPipeline;
}

Json::Value nuiJsonRpcApi::serialize_module( nuiModuleDescriptor* descriptor )
{
	Json::Value jModule;
	jModule["name"] = descriptor->getName();
	jModule["author"] = descriptor->getAuthor();
	jModule["description"] = descriptor->getDescription();

	Json::Value* jInputEndpoints = new Json::Value();
	for(int i=0 ; i<descriptor->getInputEndpointsCount() ; i++)
		jInputEndpoints->append(serialize_endpoint(descriptor->getInputEndpointDescriptor(i)));
	jModule["inputEndpoints"] = *jInputEndpoints;

	Json::Value* jOutputEndpoints = new Json::Value();
	for(int i=0 ; i<descriptor->getOutputEndpointsCount() ; i++)
		jOutputEndpoints->append(serialize_endpoint(descriptor->getOutputEndpointDescriptor(i)));
	jModule["outputEndpoints"] = *jOutputEndpoints;

	Json::Value* connections = new Json::Value();
	for (int i = 0 ; i<descriptor->getDataStreamDescriptorCount() ; i++)
		connections->append(serialize_connection(descriptor->getDataStreamDescriptor(i)));
	jModule["connections"] = *connections;

	return jModule;
}

Json::Value nuiJsonRpcApi::serialize_endpoint( nuiEndpointDescriptor *descriptor )
{
	Json::Value jEndpoint;
	jEndpoint["index"] = descriptor->getIndex();
	jEndpoint["descriptor"] = descriptor->getDescriptor();

	return jEndpoint;
}

Json::Value nuiJsonRpcApi::serialize_connection( nuiDataStreamDescriptor *descriptor )
{
	Json::Value jConnection;
	jConnection["sourceModule"] = descriptor->sourceModuleID;
	jConnection["sourcePort"] = descriptor->sourcePort;
	jConnection["destinationModule"] = descriptor->destinationModuleID;
	jConnection["destinationPort"] = descriptor->destinationPort;

	jConnection["buffered"] = (int)descriptor->buffered;
	jConnection["bufferSize"] = (int)descriptor->bufferSize;
	jConnection["deepCopy"] = (int)descriptor->deepCopy;
	jConnection["asyncMode"] = (int)descriptor->asyncMode;
	jConnection["lastPacket"] = (int)descriptor->lastPacket;
	jConnection["overflow"] = (int)descriptor->overflow;

	return jConnection;
}

void nuiJsonRpcApi::setFailure( Json::Value &response )
{
	response["result"] = "failure";
}

void nuiJsonRpcApi::setSuccess( Json::Value &response )
{
	response["result"] = "success";
}

bool nuiJsonRpcApi::isFinished()
{
	return finished;
}
