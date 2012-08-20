//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiPS3EyeModule
// Purpose:		Capture video from PS3Eye camera using PS3EyeEye SDK from Code Laboratories
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

//! TODO : Singleton
//! TODO : Params to module instance ???
//! TODO : Initialize Module instance with camera number ???

#ifndef NUI_PS3EyeMODULE
#define NUI_PS3EyeMODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include <CLEyeMulticam.h>

#include "CLEyeCamera.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(PSEye, IplImage*)

class nuiPSEye : public nuiModule
{
public:
	nuiPSEye();
	~nuiPSEye();

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

IMPLEMENT_ALLOCATOR(nuiPSEye)
IMPLEMENT_DEALLOCATOR(nuiPSEye)

START_IMPLEMENT_DESCRIPTOR(nuiPSEye,"native","Capture video from PS3Eye camera")
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor, 0);
//descriptor->property("use_thread").set(true);
//descriptor->property("oscillator_mode").set(true);
//descriptor->property("oscillator_wait").set(50);
END_IMPLEMENT_DESCRIPTOR(nuiPSEye)

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()	  
REGISTER_PLUGIN(nuiPSEye,"nuiPSEye", 1, 0)
END_MODULE_REGISTRATION()

#endif