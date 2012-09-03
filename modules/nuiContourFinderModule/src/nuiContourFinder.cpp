//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiContourFinder
// Purpose:		Find touch contours
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "ofxNCore.h"
#include "nuiContourFinder.h"
#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"

nuiContourFinderDataPacket::~nuiContourFinderDataPacket()
{ };

nuiDataPacketError nuiContourFinderDataPacket::packData(const void *_data)
{	
	this->setLocalCopy(false);
	this->data = (ContourFinder*)_data;
	return NUI_DATAPACKET_OK; 
};

nuiDataPacketError nuiContourFinderDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiContourFinderDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{	
	//! TODO : Not Implemented
	return NULL;
};

char* nuiContourFinderDataPacket::getDataPacketType()
{
	return "ContourFinder";
};

MODULE_DECLARE(ContourFinderModule, "native", "Find touch contours")

nuiContourFinderModule::nuiContourFinderModule() : nuiModule()
{
	MODULE_INIT();
};

nuiContourFinderModule::~nuiContourFinderModule()
{

};

void nuiContourFinderModule::update()
{
	this->_pOutput->lock();
	this->_pOutput->clear();

	void* data;
	nuiDataPacket* packet = this->_pInput->getData();
	if(packet == NULL) 
		return;
	packet->unpackData(data);
	CPUImageFilter* image = (CPUImageFilter*)data;
	
	contourFinder.findContours(*image, (minBlobSize * 2) + 1, 
		((camWidth * camHeight) * .4) * (maxBlobSize * .001), maxBlobs, false);

	this->_pOutputDataPacket->packData((void*)&contourFinder);
	this->_pOutput->setData(this->_pOutputDataPacket);
	this->_pOutput->transmitData();
	this->_pOutput->unlock();
};

void nuiContourFinderModule::start()
{
	camWidth = this->property("camWidth").asInteger();
	camHeight = this->property("camHeight").asInteger();
	minBlobSize = this->property("minBlobSize").asInteger();
	maxBlobSize = this->property("maxBlobSize").asInteger();
	maxBlobs = this->property("maxBlobs").asInteger();

	nuiModule::start();
};

void nuiContourFinderModule::stop()
{
	nuiModule::stop();
};
