/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_DEBUG_GESTURE_OUTPUT_MODULE_H
#define NUI_DEBUG_GESTURE_OUTPUT_MODULE_H

#include "nuiModule.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiDebugLogger.h"
#include "nuiMultimodalSyntaxTree.h"
#include "nuiPlugin.h"
#include "nuiEndpoint.h"
#include "nuiDataPacket.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(DebugGestureOutputModule, vector<client::unimodalLeafNode>*)

class nuiDebugGestureOutputModule : public nuiModule {
public:
	nuiDebugGestureOutputModule(); 
	virtual ~nuiDebugGestureOutputModule();
	void update();
    //void trigger();
    void start();
private:
	//nuiDataStream *stream;
    nuiEndpoint* output;

    bool alreadySent;
	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiDebugGestureOutputModule)
IMPLEMENT_DEALLOCATOR(nuiDebugGestureOutputModule)

START_IMPLEMENT_DESCRIPTOR(nuiDebugGestureOutputModule,"native","Pushes a vector<unimodalLeaf> onto the stream for tactile GUI events")
descriptor->setInputEndpointsCount(0);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("tactiles");
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiDebugGestureOutputModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiDebugGestureOutputModule,"nuiDebugGestureOutputModule",1,0)
END_MODULE_REGISTRATION()

#endif

