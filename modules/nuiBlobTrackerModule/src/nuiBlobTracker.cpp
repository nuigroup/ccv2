//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiBlobTracker
// Purpose:		Track found contours/blobs
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "ofxNCore.h"
#include "nuiBlobTracker.h"
#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"

nuiBlobTrackerDataPacket::~nuiBlobTrackerDataPacket()
{ };

nuiDataPacketError nuiBlobTrackerDataPacket::packData(const void *_data)
{	
	this->setLocalCopy(false);
	this->data = (std::map<int, Blob>*)_data;
	return NUI_DATAPACKET_OK; 
};

nuiDataPacketError nuiBlobTrackerDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiBlobTrackerDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{	
	//! TODO : Not Implemented
	return NULL;
};

char* nuiBlobTrackerDataPacket::getDataPacketType()
{
	return "BlobTracker";
};

MODULE_DECLARE(BlobTrackerModule, "native", "Track found contours/blobs")

nuiBlobTrackerModule::nuiBlobTrackerModule() : nuiModule()
{
	MODULE_INIT();
};

nuiBlobTrackerModule::~nuiBlobTrackerModule()
{

};

void nuiBlobTrackerModule::update()
{
	this->_pOutput->lock();
	this->_pOutput->clear();

	void* data;
	nuiDataPacket* packet = this->_pInput->getData();
	if(packet == NULL) 
		return;
	packet->unpackData(data);
	ContourFinder* cf = (ContourFinder*)data;
	
	blobTracker.track(cf);

	this->_pOutputDataPacket->packData((void*)&blobTracker.getTrackedBlobs());
	this->_pOutput->setData(this->_pOutputDataPacket);
	this->_pOutput->transmitData();
	this->_pOutput->unlock();
};

void nuiBlobTrackerModule::start()
{
	nuiModule::start();
};

void nuiBlobTrackerModule::stop()
{
	nuiModule::stop();
};
