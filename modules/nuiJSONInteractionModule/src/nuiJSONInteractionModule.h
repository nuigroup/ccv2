/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiJSONInteractionModule.h
// Purpose:     Generates JSON from a mAST
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_JSON_INTERACTION_MODULE_H
#define NUI_JSON_INTERACTION_MODULE_H

#include "nuiMultimodalSyntaxTree.h"
#include "nuiModule.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiDebugLogger.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(JSONInteractionModule, cJSON*)

class nuiJSONInteractionModule : public nuiModule {
public:
	nuiJSONInteractionModule(); 
	virtual ~nuiJSONInteractionModule();

	//virtual void notifyData(nuiDataStream *stream);
	void update();

private:
	//nuiDataStream *input;
    //nuiDataStream *output;
    nuiEndpoint* input;
    nuiEndpoint* output;

    nuiDataPacket* inputDataPacket;
    nuiDataPacket* outputDataPacket;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiJSONInteractionModule)
IMPLEMENT_DEALLOCATOR(nuiJSONInteractionModule)

START_IMPLEMENT_DESCRIPTOR(nuiJSONInteractionModule,"native","Converts mAST to JSON for output")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("mAST");
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("cJSON");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiJSONInteractionModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiJSONInteractionModule,"nuiJSONInteractionModule",1,0)
END_MODULE_REGISTRATION()

#endif

