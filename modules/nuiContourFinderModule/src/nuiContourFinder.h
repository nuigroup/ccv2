//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiContourFinder
// Purpose:		Find touch contours
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#ifndef NUI_CONTOURFINDER_MODULE
#define NUI_CONTOURFINDER_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

#include "Filters/CPUImageFilter.h"
#include "ofxCvColorImage.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvFloatImage.h"
#include "Tracking/ContourFinder.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(ContourFinder, void*)

class nuiContourFinderModule : public nuiModule
{
public:
	nuiContourFinderModule();
	~nuiContourFinderModule();

	void update();
	void start();
	void stop();

protected:
private:
	ContourFinder	contourFinder;
	int				camWidth;
	int				camHeight;
	int				minBlobSize;
	int				maxBlobSize;
	int				maxBlobs;

	nuiDataPacket*	_pOutputDataPacket;
	nuiEndpoint*	_pInput;
	nuiEndpoint*	_pOutput;

	MODULE_INTERNALS();
};

	IMPLEMENT_ALLOCATOR(nuiContourFinderModule)
	IMPLEMENT_DEALLOCATOR(nuiContourFinderModule)

START_IMPLEMENT_DESCRIPTOR(nuiContourFinderModule,"native","Find touch contours")

	descriptor->setInputEndpointsCount(1);
	nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("CPUImageFilter");
	descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

	descriptor->setOutputEndpointsCount(1);
	nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("ContourFinder");
	descriptor->addInputEndpointDescriptor(outputDescriptor, 0);

END_IMPLEMENT_DESCRIPTOR(nuiContourFinderModule)

	START_MODULE_EXIT()
	END_MODULE_EXIT()

START_MODULE_REGISTRATION()
	REGISTER_PLUGIN(nuiContourFinderModule,"nuiContourFinderModule", 1, 0)
END_MODULE_REGISTRATION()

#endif