/** 
 * \file      nuiEdgeFilterModule.h
 *            Filter to detect edges
 * \author    Scott Halstvedt
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#ifndef NUI_EDGE_FILTER_MODULE_H
#define NUI_EDGE_FILTER_MODULE_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(EdgeFilterModule, IplImage*)

class nuiEdgeFilterModule : public nuiModule {
public:
	nuiEdgeFilterModule(); 
	virtual ~nuiEdgeFilterModule();

	void update();
	void start();

private:
    nuiEndpoint* input;
	nuiEndpoint* output;
	nuiDataPacket* outputDataPacket;
    
	IplImage* filterFrame;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiEdgeFilterModule)
IMPLEMENT_DEALLOCATOR(nuiEdgeFilterModule)

START_IMPLEMENT_DESCRIPTOR(nuiEdgeFilterModule,"native","Filters edges from a video stream")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(false);
END_IMPLEMENT_DESCRIPTOR(nuiEdgeFilterModule)	

START_EXPORT_MODULES()	  
REGISTER_MODULE(nuiEdgeFilterModule,"nuiEdgeFilterModule",1,0)
END_EXPORT_MODULES()

#endif

