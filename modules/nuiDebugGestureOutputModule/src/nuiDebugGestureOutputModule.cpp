/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiDebugGestureOutputModule.h"
#include "nuiMultimodalSyntaxTree.h"

nuiDebugGestureOutputModuleDataPacket::~nuiDebugGestureOutputModuleDataPacket()
{
    if(this->isLocalCopy())
    {
        //! ToDo check
        delete this->data;
    }
};

nuiDataPacketError nuiDebugGestureOutputModuleDataPacket::packData(const void *_data)
{
    this->setLocalCopy(false);
    this->data = ( vector<client::unimodalLeafNode>*) _data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiDebugGestureOutputModuleDataPacket::unpackData(void* &_data)
{
    _data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiDebugGestureOutputModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDataPacket* newPacket = new nuiDebugGestureOutputModuleDataPacket();

    vector<client::unimodalLeafNode>* currentLeaves = this->data;
    vector<client::unimodalLeafNode>* newLeaves = new std::vector<client::unimodalLeafNode>(*currentLeaves);

    newPacket->packData(newLeaves);
    newPacket->setLocalCopy(true);
    errorCode = NUI_DATAPACKET_OK;
    return newPacket;
};

char* nuiDebugGestureOutputModuleDataPacket::getDataPacketType()
{
    return "tactiles";
};


#include "nuiDebugGestureOutputModule.h"

MODULE_DECLARE(DebugGestureOutput, "native", "Pushes a vector<unimodalLeaf> onto the stream for tactile GUI events");

nuiDebugGestureOutputModule::nuiDebugGestureOutputModule() : nuiModule() {
    MODULE_INIT();
    
//    this->stream = new nuiDataStream("vector<unimodalLeaf>");
//	this->declareOutput(0, &this->stream, new nuiDataStreamInfo(
//			"tactiles", "vector<unimodalLeaf>", "Vector of tactile GUI events"));
    this->output = new nuiEndpoint();
    this->output->setTypeDescriptor(std::string("tactiles"));
    this->setOutputEndpointCount(1);
    this->setOutputEndpoint(0,this->output);
    this->output->setModuleHoster(this);
    //! TODO no information transmited here. WTF?

    this->alreadySent = false;
}

nuiDebugGestureOutputModule::~nuiDebugGestureOutputModule() {
}

void nuiDebugGestureOutputModule::update() {
    LOG(NUI_DEBUG, "updating");
    /*std::vector<client::unimodalLeafNode> *gestureTree = new std::vector<client::unimodalLeafNode>();
    client::unimodalLeafNode* debugNode = new client::unimodalLeafNode;
    debugNode->type = "ball";
    debugNode->val = "0";
    gestureTree->push_back(*debugNode);
    debugNode = new client::unimodalLeafNode;
    debugNode->type = "point";
    debugNode->val = "(0,22)";
    gestureTree->push_back(*debugNode);
    this->stream->push(gestureTree);*/
}

void nuiDebugGestureOutputModule::start() {
    this->alreadySent = false;
    nuiModule::start();
}
/*
void nuiDebugGestureOutputModule::poll() {
    if(!this->alreadySent) {
        this->notifyUpdate();
        this->alreadySent = true;
    }
    nuiModule::poll();
}
*/

//! TODO needed?
/*void nuiDebugGestureOutputModule::trigger() {
    this->notifyUpdate();
}*/