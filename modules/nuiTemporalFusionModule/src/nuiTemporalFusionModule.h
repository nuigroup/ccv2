/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiTemporalFusionModule.h
// Purpose:     Temporal fusion module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_TEMPORAL_FUSION_MODULE_H
#define NUI_TEMPORAL_FUSION_MODULE_H

#include "nuiMultimodalSyntaxTree.h"

#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiProperty.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiPlugin.h"
#include "nuiEndPoint.h"
#include "nuiDataPacket.h"

#include <assert.h>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/foreach.hpp>

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(TemporalFusionModule, client::multimodalSyntaxTree*)

class nuiTemporalFusionModule : public nuiModule {
public:
	nuiTemporalFusionModule();
	virtual ~nuiTemporalFusionModule();

	void start();
    void update();
    void stop();

	void notifyData(nuiDataStream* data);
	
protected:
	//nuiDataStream* output;
	//nuiDataStream* audioInput;
    //nuiDataStream* tactileInput;

    nuiEndpoint* output;
    nuiDataPacket* outputDataPacket;
    nuiEndpoint* audioInput;
    nuiDataPacket* audioInputDataPacket;
    nuiEndpoint* tactileInput;
    nuiDataPacket* tactileInputDataPacket;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiTemporalFusionModule)
IMPLEMENT_DEALLOCATOR(nuiTemporalFusionModule)

START_IMPLEMENT_DESCRIPTOR(nuiTemporalFusionModule,"native","Temporal fusion module")
descriptor->setInputEndpointsCount(2);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor1 = new nuiEndpointDescriptor("mAST");
nuiEndpointDescriptor* inputDescriptor2 = new nuiEndpointDescriptor("tactiles");
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("mAST");
descriptor->addInputEndpointDescriptor(inputDescriptor1,0);
descriptor->addInputEndpointDescriptor(inputDescriptor2,1);
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiTemporalFusionModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiTemporalFusionModule,"nuiTemporalFusionModule",1,0)
END_MODULE_REGISTRATION()

#endif

