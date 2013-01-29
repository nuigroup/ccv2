/** 
 * \file      nuiGaussianBlurFilter.h 
 *            Filter to gaussian blur
 * \author    Scott Halstvedt
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#ifndef NUI_GAUSSIAN_BLUR_FILTER_H
#define NUI_GAUSSIAN_BLUR_FILTER_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(GaussianBlurFilter, IplImage*)

class nuiGaussianBlurFilter : public nuiModule {
public:
	nuiGaussianBlurFilter(); 
	virtual ~nuiGaussianBlurFilter();

	void update();
	void start();

private:
    nuiEndpoint* input;
	nuiEndpoint* output;
	nuiDataPacket* outputDataPacket;
    
	IplImage* filterFrame;

	bool dev;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiGaussianBlurFilter)
IMPLEMENT_DEALLOCATOR(nuiGaussianBlurFilter)

START_IMPLEMENT_DESCRIPTOR(nuiGaussianBlurFilter,"native","Gaussian blurs a video stream")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(false);
END_IMPLEMENT_DESCRIPTOR(nuiGaussianBlurFilter)	

START_EXPORT_MODULES()	  
REGISTER_MODULE(nuiGaussianBlurFilter,"nuiGaussianBlurFilter", 1, 0)
END_EXPORT_MODULES()

#endif

