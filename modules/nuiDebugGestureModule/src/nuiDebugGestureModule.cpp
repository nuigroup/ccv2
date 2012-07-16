#include "..\..\..\libs\nuiModule\inc\nuiMultimodalSyntaxTree.h"
#include "nuiDebugGestureModule.h"
/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiDebugGestureModule.cpp
// Purpose:     Generate debug gestures
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

nuiDebugGestureModuleDataPacket::~nuiDebugGestureModuleDataPacket()
{
    if(this->isLocalCopy())
    {
        //! ToDo check
        delete this->data;
    }
};

nuiDataPacketError nuiDebugGestureModuleDataPacket::packData(const void *_data)
{
    this->setLocalCopy(false);
    this->data = (std::vector<client::unimodalLeafNode>*) _data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiDebugGestureModuleDataPacket::unpackData(void* &_data)
{
    _data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiDebugGestureModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDebugGestureModuleDataPacket* newPacket = new nuiDebugGestureModuleDataPacket();

    std::vector<client::unimodalLeafNode>* newLeaves = new std::vector<client::unimodalLeafNode>();
    std::vector<client::unimodalLeafNode>* currentLeaves = this->data;
    newLeaves->resize(currentLeaves->size());

    std::vector<client::unimodalLeafNode>::iterator it;
    for( it=currentLeaves->begin() ; it < currentLeaves->end() ; it++)
    {
        client::unimodalLeafNode* leaf = new client::unimodalLeafNode();
        leaf->type = *(new string(it->type));
        leaf->val = *(new string(it->val));
        newLeaves->push_back(*leaf);
    }

    newPacket->packData(newLeaves);
    newPacket->setLocalCopy(true);
    errorCode = NUI_DATAPACKET_OK;
    return newPacket;
};

char* nuiDebugGestureModuleDataPacket::getDataPacketType()
{
    return "tactiles";
};

#include "nuiDebugGestureModule.h"

MODULE_DECLARE(DebugGesture, "native", "Passes through vector<unimodalLeaf> onto the stream for tactile GUI events");

nuiDebugGestureModule::nuiDebugGestureModule() : 
    nuiModule() 
{
    MODULE_INIT();
    
    //this->output = new nuiDataStream("vector<unimodalLeaf>");
	//this->declareOutput(0, &this->output, new nuiDataStreamInfo(
	//		"tactiles", "vector<unimodalLeaf>", "Vector of tactile GUI events"));
    this->output = new nuiEndpoint();
    this->output->setTypeDescriptor(std::string("tactiles"));
    this->setOutputEndpointCount(1);
    this->setOutputEndpoint(0,this->output);
    this->output->setModuleHoster(this);

    //this->input = new nuiDataStream("vector<unimodalLeaf");
    //this->declareInput(0, &this->input, new nuiDataStreamInfo("debugtactiles", "vector<unimodalLeaf>", "Vector of tactile GUI events"));
    this->input = new nuiEndpoint();
    this->input->setTypeDescriptor(std::string("debugtactiles"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
    this->input->setModuleHoster(this);

    inputDataPacket = NULL;
    outputDataPacket = new nuiDebugGestureModuleDataPacket();
}

nuiDebugGestureModule::~nuiDebugGestureModule() {
}

void nuiDebugGestureModule::update() {
    LOG(NUI_DEBUG, "passing through");
    std::vector<client::unimodalLeafNode> *gestureTree = NULL;
    if(this->input != NULL) 
        gestureTree = (std::vector<client::unimodalLeafNode>*)this->input->getData();
    if(gestureTree != NULL) 
        //this->output->push(gestureTree);
        this->outputDataPacket->packData((void*)gestureTree);
    else 
        this->output->clear();
}

void nuiDebugGestureModule::start() {
	LOG(NUI_DEBUG, "passing through");
    nuiModule::start();
}
