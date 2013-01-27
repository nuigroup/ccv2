/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiHighpassFilter.h
// Purpose:     Highpass filter
// Author:      Scott Halstvedt
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_HIGHPASS_FILTER_H
#define NUI_HIGHPASS_FILTER_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(HighpassFilter, IplImage*)

class nuiHighpassFilter : public nuiModule {
public:
	nuiHighpassFilter(); 
	virtual ~nuiHighpassFilter();

	void update();
	void start();

private:
    nuiEndpoint* input;
	nuiEndpoint* output;
	nuiDataPacket* outputDataPacket;
    
	IplImage* filterFrame;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiHighpassFilter)
IMPLEMENT_DEALLOCATOR(nuiHighpassFilter)

START_IMPLEMENT_DESCRIPTOR(nuiHighpassFilter,"native","Gaussian blurs a video stream")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(false);
END_IMPLEMENT_DESCRIPTOR(nuiHighpassFilter)	

START_EXPORT_MODULES()	  
REGISTER_MODULE(nuiHighpassFilter,"nuiHighpassFilter",1,0)
END_EXPORT_MODULES()

#endif

