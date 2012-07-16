/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiJSONInteractionModule.cpp
// Purpose:     Generates JSON from a mAST
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include "nuiJSONInteractionModule.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiMultimodalSyntaxTree.h"
#include "nuiDataPacket.h"

MODULE_DECLARE(JSONInteraction, "native", "Converts mAST to JSON for output");

nuiJSONInteractionModuleDataPacket::~nuiJSONInteractionModuleDataPacket()
{
    if (this->isLocalCopy())
    {
        delete this->data;
    }
};

nuiDataPacketError nuiJSONInteractionModuleDataPacket::packData(const void *_data)
{
    this->setLocalCopy(false);
    this->data = (cJSON*)_data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiJSONInteractionModuleDataPacket::unpackData(void* &_data)
{
    _data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiJSONInteractionModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDataPacket* newDataPacket = new nuiJSONInteractionModuleDataPacket();
    
    cJSON* newData = new cJSON();
    cJSON* currentData = this->data;
    newData->next = currentData->next;
    newData->prev = currentData->prev;
    newData->child = currentData->child;
    newData->type = currentData->type;

    newData->valuestring = (char*)malloc(strlen(currentData->valuestring));
    strcpy(newData->valuestring, currentData->valuestring);

    newData->valueint = currentData->valueint;
    newData->valuedouble = currentData->valuedouble;

    newData->string = (char*)malloc(strlen(currentData->string));
    strcpy(newData->string, currentData->string);

    newDataPacket->packData(newData);
    newDataPacket->setLocalCopy(true);
    return newDataPacket;
};

char* nuiJSONInteractionModuleDataPacket::getDataPacketType()
{
    return "cJSON";
};

nuiJSONInteractionModule::nuiJSONInteractionModule() : nuiModule() {
    MODULE_INIT();
    
	/*this->input = new nuiDataStream("mAST");
	this->declareInput(0, &this->input, new nuiDataStreamInfo(
			"data", "mAST", "Multimodal abstract syntax tree"));
    this->output = new nuiDataStream("JSON");
    this->declareOutput(0, &this->output, new nuiDataStreamInfo("cJSON", "JSON", "JSON interaction"));*/
    this->output = new nuiEndpoint();
    this->output->setTypeDescriptor(std::string("cJSON"));
    this->setOutputEndpointCount(1);
    this->setOutputEndpoint(0,this->output);
    this->output->setModuleHoster(this);

    this->input = new nuiEndpoint();
    this->input->setTypeDescriptor(std::string("mAST"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
    this->input->setModuleHoster(this);

    this->inputDataPacket = NULL;
    this->outputDataPacket = new nuiJSONInteractionModuleDataPacket();
}

nuiJSONInteractionModule::~nuiJSONInteractionModule() {
}

void nuiJSONInteractionModule::update() {
    cJSON* outputJSON;
    if(this->input->getData() != NULL) 
        outputJSON = mast_to_json((client::multimodalSyntaxTree *)this->input->getData());
    if(outputJSON != NULL) 
        //this->output->push(outputJSON);
        this->outputDataPacket->packData((void*)outputJSON);
    else 
        this->output->clear();
}

//! ToDo What this bunch of code was originated to do?

/*void nuiJSONInteractionModule::notifyData(nuiDataStream *stream) {
    this->notifyUpdate();
}*/

