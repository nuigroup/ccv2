//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiTUIOOutputModule
// Purpose:		Output TUIO
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#ifndef NUI_TUIOOUTPUT_MODULE
#define NUI_TUIOOUTPUT_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(TUIOOutput, void*)

class nuiTUIOOutputModule : public nuiModule
{
public:
	nuiTUIOOutputModule();
	~nuiTUIOOutputModule();

	void update();
	void start();
	void stop();

protected:
private:
	nuiEndpoint*	_pInput;

	// TUIO broadcast params
	char*			host;
	int				port;
	int				flashport;

	// TUIO class for broadcasting
	TUIO			tuioOutput;

	MODULE_INTERNALS();
};

	IMPLEMENT_ALLOCATOR(nuiTUIOOutputModule)
	IMPLEMENT_DEALLOCATOR(nuiTUIOOutputModule)

START_IMPLEMENT_DESCRIPTOR(nuiTUIOOutputModule,"native","Output TUIO")

	descriptor->setInputEndpointsCount(1);
	nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("std::map<int, Blob>");
	descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

END_IMPLEMENT_DESCRIPTOR(nuiTUIOOutputModule)

	START_MODULE_EXIT()
	END_MODULE_EXIT()

START_MODULE_REGISTRATION()
	REGISTER_PLUGIN(nuiTUIOOutputModule,"nuiTUIOOutputModule", 1, 0)
END_MODULE_REGISTRATION()

#endif