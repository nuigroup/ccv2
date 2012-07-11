/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiSpiritParserModule.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiSpiritParserModule.h"
#include "nuiMultimodalSyntaxTree.h"

nuiSpiritParserModuleDataPacket::~nuiSpiritParserModuleDataPacket()
{
    if(this->isLocalCopy())
    {
        delete this->data;
    }
};

nuiDataPacketError nuiSpiritParserModuleDataPacket::packData(const void *_data)
{
    this->setLocalCopy(false);
    this->data = (client::multimodalSyntaxTree *) _data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiSpiritParserModuleDataPacket::unpackData(void* &_data)
{
    _data = (void*)this->data;
    return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiSpiritParserModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
    nuiDataPacket* newPacket = new nuiSpiritParserModuleDataPacket();

    client::multimodalSyntaxTree* currentSyntaxTree = this->data;
    client::multimodalSyntaxTree* newSyntaxTree = new client::multimodalSyntaxTree();

    *newSyntaxTree = *currentSyntaxTree;

    newPacket->packData(newSyntaxTree);
    newPacket->setLocalCopy(true);
    errorCode = NUI_DATAPACKET_OK;
    return newPacket;
};

char* nuiSpiritParserModuleDataPacket::getDataPacketType()
{
    return "mAST";
};


MODULE_DECLARE(SpiritParser, "native", "Parses attributes out of ASR text to fill in the semantic tree");

nuiSpiritParserModule::nuiSpiritParserModule() : nuiModule() {
    
    MODULE_INIT();
    
    //this->input = new nuiDataStream("CCAHypothesis");
    //this->output = new nuiDataStream("mAST");
            
    //this->declareInput(0, &this->input, new nuiDataStreamInfo("data", "CCAHypothesis", "CCA recognition hypothesis"));
    //this->declareOutput(0, &this->output, new nuiDataStreamInfo("data", "mAST", "Multimodal abstract syntax tree"));

    this->output = new nuiEndpoint();
    this->output->setTypeDescriptor(std::string("mAST"));
    this->setOutputEndpointCount(1);
    this->setOutputEndpoint(0,this->output);
    this->output->setModuleHoster(this);

    this->input = new nuiEndpoint();
    this->input->setTypeDescriptor(std::string("CCAHypothesis"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
    this->input->setModuleHoster(this);

    this->inputDataPacket = NULL;
    this->outputDataPacket = new nuiSpiritParserModuleDataPacket();
    
    this->properties["grammar"] = new nuiProperty("ballworld", "The grammar to use");

	this->mast = NULL;
                                                              
}


nuiSpiritParserModule::~nuiSpiritParserModule() {
    if(this->mast != NULL) delete(this->mast);
}


void nuiSpiritParserModule::start() {
    // initialize    
    this->mast = new client::multimodalSyntaxTree;
    nuiModule::start();
    LOG(NUI_DEBUG, "started!");
}

void nuiSpiritParserModule::stop() {
    LOG(NUI_DEBUG, "stopped!");
    nuiModule::stop();
}

//! TODO need this?
/*void nuiSpiritParserModule::notifyData(nuiDataStream *input) {
    assert(input != NULL);
    assert(input == this->input);
    //this->notifyUpdate();
}*/

void nuiSpiritParserModule::update() {
    this->input->lock();
    if(this->initializeFromString(std::string((char*)this->input->getData()))) {
        //this->output->push(mast);
        this->outputDataPacket->packData((void*)mast);
    }
    else {
        this->output->clear();
    }
    this->input->unlock();
}



bool nuiSpiritParserModule::initializeFromString(std::string input) {
        
    storage = input;
    
    delete(mast);
    mast = new client::multimodalSyntaxTree;
    
    std::string::iterator iter = storage.begin();
    std::string::iterator end = storage.end();
        
    bool r = boost::spirit::qi::phrase_parse(iter, end, grammar, boost::spirit::ascii::space, *mast);
    
    if(r && iter == end) {
        LOG(NUI_DEBUG, "success! sentence: \"" << storage << "\"");
        return(true);
    }
    else {
        LOG(NUI_DEBUG, "failure! sentence: \"" << storage << "\"");
        return(false);
    }
}
