/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiBackgroundSubtractModule.h
// Purpose:     Filter to detect edges
// Author:      Scott Halstvedt
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_BACKGROUND_SUBTRACT_MODULE_H
#define NUI_BACKGROUND_SUBTRACT_MODULE_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(BackgroundSubtractModule, IplImage*)

class nuiBackgroundSubtractModule : public nuiModule {
public:
	nuiBackgroundSubtractModule(); 
	virtual ~nuiBackgroundSubtractModule();

	void update();
	void start();

private:
    nuiEndpoint* input;
	nuiEndpoint* frameOutput;
	nuiEndpoint* bgOutput;
	nuiDataPacket* outputDataPacket;
    
	IplImage* filterFrame;
	cv::BackgroundSubtractorMOG2 bg;
	std::vector<std::vector<cv::Point> > contours;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiBackgroundSubtractModule)
IMPLEMENT_DEALLOCATOR(nuiBackgroundSubtractModule)

START_IMPLEMENT_DESCRIPTOR(nuiBackgroundSubtractModule,"native","Subtracts the background from a video stream")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(false);
END_IMPLEMENT_DESCRIPTOR(nuiBackgroundSubtractModule)	

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiBackgroundSubtractModule,"nuiBackgroundSubtractModule",1,0)
END_MODULE_REGISTRATION()

#endif

