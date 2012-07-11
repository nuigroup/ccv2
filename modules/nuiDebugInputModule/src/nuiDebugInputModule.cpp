/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiDebugInputModule.cpp
// Purpose:     Prints data streams to the log for debugging
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include "nuiDebugInputModule.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiMultimodalSyntaxTree.h"
#include "nuiPlugin.h"

/*nuiDebugInputModuleDataPacket::~nuiDebugInputModuleDataPacket()
{
    if(this->isLocalCopy())
    {
        //! ToDo check
        delete this->data;
    }
};

nuiDataPacketError nuiDebugInputModuleDataPacket::packData(const void *_data)
{
    this->data = () _data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiDebugInputModuleDataPacket::unpackData(void* &_data)
{
    _data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiDebugInputModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDataPacket* newPacket = new nuiDebugInputModuleDataPacket();


    newPacket->packData(newLeaves);
    newPacket->setLocalCopy(true);
    errorCode = NUI_DATAPACKET_OK;
    return newPacket;
};

char* nuiDebugInputModuleDataPacket::getDataPacketType()
{
    return "mAST";
};*/

MODULE_DECLARE(DebugInput, "native", "Print streams/structures to the debug console in text format");

nuiDebugInputModule::nuiDebugInputModule() : nuiModule() {
    MODULE_INIT();
    
//	this->stream = NULL;
//	this->declareInput(0, &this->stream, new nuiDataStreamInfo(
//			"data", "mAST", "Show any stream that conforms to GenericList in text format"));

    this->input = new nuiEndpoint();
    this->input->setTypeDescriptor(std::string("mAST"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
    this->input->setModuleHoster(this);
}

nuiDebugInputModule::~nuiDebugInputModule() {
}

void nuiDebugInputModule::update() {
//    LOG(NUI_DEBUG, "stream<" << stream << ">, type=" << stream->getFormat() << ", observers=" << stream->getObserverCount());
    
	if ( /*stream->getFormat() == "generic"*/ this->input->getTypeDescriptor() == "generic" ) {
		//nuiDataGenericList *list = static_cast<nuiDataGenericList*>(stream->getData());
        nuiDataGenericList *list;
        void* pList = (void*)list;
        this->input->getData()->unpackData(pList);
        list = (nuiDataGenericList*)pList;
//		LOG(NUI_DEBUG, " `- " << stream->getFormat() << " size=" << list->size());
	}
    
    //if( stream->getFormat() == "mAST" ) {
    if( this->input->getTypeDescriptor() == "mAST" ) {
//        LOG(NUI_DEBUG, mast_to_string(static_cast<client::multimodalSyntaxTree*>(stream->getData())));
    }
}

//! TODO needed?
/*void nuiDebugInputModule::notifyData(nuiDataStream *stream) {
    this->notifyUpdate();
}*/

