/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiDebugInputModule.h
// Purpose:     Prints data streams to the log for debugging
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_DEBUG_INPUT_MODULE_H
#define NUI_DEBUG_INPUT_MODULE_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"

//NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(DebugInputModule, datatype)

class nuiDebugInputModule : public nuiModule {
public:
	nuiDebugInputModule(); 
	virtual ~nuiDebugInputModule();

	//virtual void notifyData(nuiDataStream *stream);
	void update();

private:
	//nuiDataStream *stream;
    nuiEndpoint* input;
    
	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiDebugInputModule)
IMPLEMENT_DEALLOCATOR(nuiDebugInputModule)

START_IMPLEMENT_DESCRIPTOR(nuiDebugInputModule,"native","Print streams/structures to the debug console in text format")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(0);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("mAST");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiDebugInputModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiDebugInputModule,"nuiDebugInputModule",1,0)
END_MODULE_REGISTRATION()

#endif

