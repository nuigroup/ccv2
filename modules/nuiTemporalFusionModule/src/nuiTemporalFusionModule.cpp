/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiTemporalFusionModule.cpp
// Purpose:     Temporal fusion module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiTemporalFusionModule.h"

nuiTemporalFusionModuleDataPacket::~nuiTemporalFusionModuleDataPacket()
{
    if (this->isLocalCopy())
    {
        delete this->data;
    }
};

nuiDataPacketError nuiTemporalFusionModuleDataPacket::packData(const void *_data)
{
    this->setLocalCopy(false);
    this->data = (client::multimodalSyntaxTree*)_data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiTemporalFusionModuleDataPacket::unpackData(void* &_data)
{
    _data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiTemporalFusionModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDataPacket* newDataPacket = new nuiTemporalFusionModuleDataPacket();
    
    client::multimodalSyntaxTree* currentTree = this->data;
    client::multimodalSyntaxTree* newTree = new client::multimodalSyntaxTree();
    *newTree = *currentTree;

    newDataPacket->packData(newTree);
    newDataPacket->setLocalCopy(true);
    errorCode = NUI_DATAPACKET_OK;
    return newDataPacket;
};

char* nuiTemporalFusionModuleDataPacket::getDataPacketType()
{
    return "client::multimodalSyntaxTree*";
};

MODULE_DECLARE(TemporalFusion, "native", "Temporal fusion module");

nuiTemporalFusionModule::nuiTemporalFusionModule() : nuiModule() {

	MODULE_INIT();

	/*this->audioInput = new nuiDataStream("mAST");
    this->tactileInput = new nuiDataStream("vector<unimodalLeaf>");
    this->output = new nuiDataStream("mAST");
            
    this->declareInput(0, &this->audioInput, new nuiDataStreamInfo("audio", "mAST", "Multimodal abstract syntax tree (from audio utterance)"));
    this->declareInput(1, &this->tactileInput, new nuiDataStreamInfo("tactile", "vector<unimodalLeaf>", "Series of unimodal actions from the GUI"));
    this->declareOutput(0, &this->output, new nuiDataStreamInfo("fused", "mAST", "Multimodal abstract syntax tree (fused)"));*/
    this->output = new nuiEndpoint();
    this->audioInput = new nuiEndpoint();
    this->tactileInput = new nuiEndpoint();

    this->setOutputEndpointCount(1);
    this->output = new nuiEndpoint();
    this->output->setTypeDescriptor(std::string("mAST"));
    this->setOutputEndpoint(0,this->output);
    this->output->setModuleHoster(this);

    this->setInputEndpointCount(2);
    this->audioInput = new nuiEndpoint();
    this->audioInput->setTypeDescriptor(std::string("mAST"));
    this->setInputEndpoint(0,this->audioInput);
    this->audioInput->setModuleHoster(this);

    this->tactileInput = new nuiEndpoint();
    this->tactileInput->setTypeDescriptor(std::string("tactiles"));
    this->setInputEndpoint(1,this->tactileInput);
    this->tactileInput->setModuleHoster(this);

    this->tactileInputDataPacket = NULL;
    this->audioInputDataPacket = NULL;
    this->outputDataPacket = new nuiTemporalFusionModuleDataPacket();
}

nuiTemporalFusionModule::~nuiTemporalFusionModule() {
}

//! TODO need this?
/*void nuiTemporalFusionModule::notifyData(nuiDataStream *audioInput) {
    //assert(audioInput != NULL);
    //assert(tactileInput != NULL);
    //assert(audioInput == this->audioInput);
    //assert(tactileInput == this->tactileInput);
    this->notifyUpdate();
}*/

struct multimodal_node_finder : boost::static_visitor<bool>
{
    bool operator()(client::multimodalLeafNode const& mmNode) const {
        return true;
    }
    bool operator()(client::unimodalLeafNode const& umNode) const {
        return false;
    }
    bool operator()(client::multimodalSyntaxTree const& mast) const {
        return false;
    }
};

void nuiTemporalFusionModule::update() {
    bool good = true;
    client::multimodalSyntaxTree* audioTree = (client::multimodalSyntaxTree *)this->audioInput->getData();
    std::vector<client::unimodalLeafNode>* tactileTree = (std::vector<client::unimodalLeafNode> *)this->tactileInput->getData();
    if(audioTree != NULL && tactileTree != NULL) {
        if(audioTree->type == "interaction" && tactileTree->size() > 0) {
            int childcount = 0;
            std::vector<client::unimodalLeafNode>::iterator gesIter = tactileTree->begin();
            BOOST_FOREACH(client::node const& nod, audioTree->children)
            {
                if(boost::apply_visitor(multimodal_node_finder(), nod) && gesIter != tactileTree->end()) {
                    client::multimodalLeafNode node = boost::get<client::multimodalLeafNode>(nod);
                    LOG(NUI_DEBUG, node.type << " needed");
                    if(node.type == gesIter->type) {
                        LOG(NUI_DEBUG, "success!!!!!!!!");
                        client::node newNode = *gesIter;
                        audioTree->children.at(childcount).swap(newNode);
                    }
                    else {
                        LOG(NUI_DEBUG, "mismatch");
                        good = false;
                    }
                    gesIter++;
                }
                childcount++;
            }
        }
        if(good) {
            //this->output->push(audioTree);
            this->outputDataPacket->packData(audioTree);
        }
        else {
            this->output->clear();
            //! TODO Errors?
            //this->setError("fusion error");
        }
    }
    else if(audioTree != NULL) {
        //this->output->push(audioTree);
        this->outputDataPacket->packData(audioTree);
    }
    else {
        this->output->clear();
    }
}

void nuiTemporalFusionModule::start() {
	
	nuiModule::start();
}

void nuiTemporalFusionModule::stop() {

	nuiModule::stop();
}