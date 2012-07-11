/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiDebugGestureModule.h
// Purpose:     Generate debug gestures
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef NUI_DEBUG_GESTURE_MODULE_H
#define NUI_DEBUG_GESTURE_MODULE_H

#include "nuiMultimodalSyntaxTree.h"
#include "nuiModule.h"
#include "nuiDataStream.h"
#include "nuiDataGenericContainer.h"
#include "nuiDebugLogger.h"
#include "nuiPlugin.h"
#include "nuiEndpoint.h"
#include "nuiDataPacket.h"

/*class nuiDebugGestureModuleDataPacket : public nuiDataPacket
{
public:
    ~nuiDebugGestureModuleDataPacket();
    nuiDataPacketError packData(const void *data);
    nuiDataPacketError unpackData(void* &data);
    nuiDataPacket*  copyPacketData(nuiDataPacketError &errorCode);
    char* getDataPacketType();
private:
    std::vector<client::unimodalLeafNode>* data;
};*/
NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(DebugGestureModule, std::vector<client::unimodalLeafNode>*)

class nuiDebugGestureModule : public nuiModule 
{
public:
	nuiDebugGestureModule(); 
	virtual ~nuiDebugGestureModule();

	void update();
    void start();

private:
	nuiEndpoint *input;
    nuiEndpoint *output;

    nuiDataPacket* inputDataPacket;
    nuiDataPacket* outputDataPacket;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiDebugGestureModule)
IMPLEMENT_DEALLOCATOR(nuiDebugGestureModule)

START_IMPLEMENT_DESCRIPTOR(nuiDebugGestureModule,"native","Passes through vector<unimodalLeaf> onto the stream for tactile GUI events")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("debugtactiles");
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("tactiles");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiDebugGestureModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiDebugGestureModule,"nuiDebugGestureModule",1,0)
END_MODULE_REGISTRATION()

#endif

