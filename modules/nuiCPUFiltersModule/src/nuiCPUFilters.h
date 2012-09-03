//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiCPUFilters
// Purpose:		Filter image so ContoutFinder and Blobtracker will be able to work
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#ifndef NUI_CPUFILTERS_MODULE
#define NUI_CPUFILTERS_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

#include "Filters/CPUImageFilter.h"
#include "ofxCvColorImage.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvFloatImage.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(CPUFilters, void*)

class nuiCPUFiltersModule : public nuiModule
{
public:
	nuiCPUFiltersModule();
	~nuiCPUFiltersModule();

	void update();
	void start();
	void stop();

protected:
private:
	// Bunch of CPU filters to process image
	Filters*		filter;

	// GPU or CPU
	//bool			bUseGPU;
	nuiDataPacket*	_pOutputDataPacket;
	nuiEndpoint*	_pInput;
	nuiEndpoint*	_pOutput;

	MODULE_INTERNALS();
};

	IMPLEMENT_ALLOCATOR(nuiCPUFiltersModule)
	IMPLEMENT_DEALLOCATOR(nuiCPUFiltersModule)

START_IMPLEMENT_DESCRIPTOR(nuiCPUFiltersModule,"native","Filter image to further processing")

	descriptor->setInputEndpointsCount(1);
	nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("CPUImageFilter");
	descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

	descriptor->setOutputEndpointsCount(1);
	nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("CPUImageFilter");
	descriptor->addInputEndpointDescriptor(outputDescriptor, 0);

END_IMPLEMENT_DESCRIPTOR(nuiCPUFiltersModule)

	START_MODULE_EXIT()
	END_MODULE_EXIT()

START_MODULE_REGISTRATION()
	REGISTER_PLUGIN(nuiCPUFiltersModule,"nuiCPUFiltersModule", 1, 0)
END_MODULE_REGISTRATION()

#endif