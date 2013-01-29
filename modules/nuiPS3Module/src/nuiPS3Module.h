/** 
 * \file      nuiPS3Module.h
 *            Capture video from PS3 camera using PS3Eye SDK from Code Laboratories
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

//! \todo : Singleton?
//! \todo : Params to module instance ???
//! \todo: Initialize Module instance with camera number ???

#ifndef NUI_PS3MODULE
#define NUI_PS3MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include <CLEyeMulticam.h>

#include "CLEyeCamera.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(PSModule, IplImage*)

class nuiPSModule : public nuiModule
{
public:
	nuiPSModule();
	~nuiPSModule();

	void update();
	void start();
	void stop();

protected:
private:
	nuiEndpoint* _pOutput;

	nuiDataPacket* _pOutputDataPacket;

	CLEyeCamera* _pCam;
	IplImage* _pFrame;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiPSModule)
IMPLEMENT_DEALLOCATOR(nuiPSModule)

START_IMPLEMENT_DESCRIPTOR(nuiPSModule,"native","Capture video from PS3 camera")
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor, 0);
//descriptor->property("use_thread").set(true);
//descriptor->property("oscillator_mode").set(true);
//descriptor->property("oscillator_wait").set(50);
END_IMPLEMENT_DESCRIPTOR(nuiPSModule)

START_EXPORT_MODULES()	  
REGISTER_MODULE(nuiPSModule,"nuiPSModule", 1, 0)
END_EXPORT_MODULES()

#endif