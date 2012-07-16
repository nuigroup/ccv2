/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiSpiritParserModule.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef NUI_SPIRIT_PARSER_MODULE_H
#define NUI_SPIRIT_PARSER_MODULE_H


#include "nuiMultimodalSyntaxTree.h"

#include "ballWorldGrammar.cpp" // do grammar loading more elegantly later

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiProperty.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndPoint.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(SpiritParserModule, client::multimodalSyntaxTree *)

class nuiSpiritParserModule : public nuiModule {
public:
    nuiSpiritParserModule();
    virtual ~nuiSpiritParserModule();
    
    //void notifyData(nuiDataStream*);
    void update();
    void stop();
    void start();
    
    bool initializeFromString(std::string input);
        
protected:
    //nuiEndPoint* input;
    nuiEndpoint* input;
    nuiEndpoint* output;

    nuiDataPacket* inputDataPacket;
    nuiDataPacket* outputDataPacket;
    
    std::string storage;
	
    client::multimodalSyntaxTree *mast;
    client::spiritGrammar<std::string::iterator> grammar;

    MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiSpiritParserModule)
IMPLEMENT_DEALLOCATOR(nuiSpiritParserModule)

START_IMPLEMENT_DESCRIPTOR(nuiSpiritParserModule,"native","Passes through vector<unimodalLeaf> onto the stream for tactile GUI events")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("CCAHypothesis");
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("mAST");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiSpiritParserModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiSpiritParserModule,"nuiSpiritParserModule",1,0)
END_MODULE_REGISTRATION()

#endif