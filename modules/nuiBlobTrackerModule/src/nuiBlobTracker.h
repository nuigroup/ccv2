//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiBlobTracker
// Purpose:		Track found contours/blobs
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#ifndef NUI_BLOBTRACKER_MODULE
#define NUI_BLOBTRACKER_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

#include "ofxCvColorImage.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvFloatImage.h"
#include "Tracking/ContourFinder.h"
#include "Tracking/Tracking.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(BlobTracker, void*)

class nuiBlobTrackerModule : public nuiModule
{
public:
	nuiBlobTrackerModule();
	~nuiBlobTrackerModule();

	void update();
	void start();
	void stop();

protected:
private:
	BlobTracker		blobTracker;
	
	nuiDataPacket*	_pOutputDataPacket;
	nuiEndpoint*	_pInput;
	nuiEndpoint*	_pOutput;

	MODULE_INTERNALS();
};

	IMPLEMENT_ALLOCATOR(nuiBlobTrackerModule)
	IMPLEMENT_DEALLOCATOR(nuiBlobTrackerModule)

START_IMPLEMENT_DESCRIPTOR(nuiBlobTrackerModule,"native","Track found contours/blobs")

	descriptor->setInputEndpointsCount(1);
	nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("ContourFinder");
	descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

	descriptor->setOutputEndpointsCount(1);
	nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("std::map<int, Blob>");
	descriptor->addInputEndpointDescriptor(outputDescriptor, 0);

END_IMPLEMENT_DESCRIPTOR(nuiBlobTrackerModule)

	START_MODULE_EXIT()
	END_MODULE_EXIT()

START_MODULE_REGISTRATION()
	REGISTER_PLUGIN(nuiBlobTrackerModule,"nuiBlobTrackerModule", 1, 0)
END_MODULE_REGISTRATION()

#endif