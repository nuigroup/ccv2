/** 
 * \file      nuiVideoFileSource.h 
 *            Provide sample video input
 * \author    Scott Halstvedt
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#ifndef NUI_VIDEOFILESOURCE_H
#define NUI_VIDEOFILESOURCE_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

#include "nuiDebugLogger.h"

#include <opencv\cv.h>
#include <opencv\highgui.h>

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(VideoFileSource, IplImage*)

class nuiVideoFileSource : public nuiModule
{
public:
	nuiVideoFileSource();
	~nuiVideoFileSource();

	void update();
	void start();
	void stop();

protected:
private:
	nuiEndpoint* output;

	nuiDataPacket* outputDataPacket;

	CvCapture* cap;
	IplImage* frame;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiVideoFileSource)
IMPLEMENT_DEALLOCATOR(nuiVideoFileSource)

START_IMPLEMENT_DESCRIPTOR(nuiVideoFileSource,"native","Get video data from file")
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor, 0);
//descriptor->property("use_thread").set(true);
//descriptor->property("oscillator_mode").set(true);
//descriptor->property("oscillator_wait").set(50);
END_IMPLEMENT_DESCRIPTOR(nuiVideoFileSource)

START_EXPORT_MODULES()	  
REGISTER_MODULE(nuiVideoFileSource,"nuiVideoFileSource", 1, 0)
END_EXPORT_MODULES()

#endif