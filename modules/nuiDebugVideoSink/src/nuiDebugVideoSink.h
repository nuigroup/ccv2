/** 
 * \file      nuiPluginManager.h
 *            Sink to display video
 * \author    Scott Halstvedt
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */
#ifndef NUI_DEBUG_VIDEO_SINK_H
#define NUI_DEBUG_VIDEO_SINK_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

class nuiDebugVideoSink : public nuiModule {
public:
	nuiDebugVideoSink(); 
	virtual ~nuiDebugVideoSink();

	void update();
	void start();

private:
  nuiEndpoint* input;

	IplImage *dispFrame;
    
	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiDebugVideoSink)
IMPLEMENT_DEALLOCATOR(nuiDebugVideoSink)

START_IMPLEMENT_DESCRIPTOR(nuiDebugVideoSink,"native","Print streams/structures to the debug console in text format")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(0);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiDebugVideoSink)	

START_EXPORT_MODULES()	  
REGISTER_MODULE(nuiDebugVideoSink,"nuiDebugVideoSink",1,0)
END_EXPORT_MODULES()

#endif

