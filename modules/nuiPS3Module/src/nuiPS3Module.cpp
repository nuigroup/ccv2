//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiPS3Module
// Purpose:		Capture video from PS3 camera using PS3Eye SDK from Code Laboratories
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "nuiPS3Module.h"

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"

nuiPSModuleDataPacket::~nuiPSModuleDataPacket()
{
};

nuiDataPacketError nuiPSModuleDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiPSModuleDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiPSModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiPSModuleDataPacket* newDataPacket = new nuiPSModuleDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = new IplImage(*(this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiPSModuleDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(PS, "native", "Capture video from PS3 camera using Code Laborotories PS3 Eye SDK")

nuiPSModule::nuiPSModule() : nuiModule()
{
	MODULE_INIT();

	this->_pOutput = new nuiEndpoint(this);
	this->_pOutput->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->_pOutput);

	this->_pOutputDataPacket = new nuiPSModuleDataPacket();
};

nuiPSModule::~nuiPSModule()
{};

void nuiPSModule::update()
{
	LOG(NUI_DEBUG, "ps3 module update called");
	this->_pOutput->lock();

	this->_pOutput->clear();

	this->_pCam->getFrame(_pFrame);

	this->_pOutputDataPacket->packData((void*)_pFrame);
	this->_pOutput->setData(this->_pOutputDataPacket);
	this->_pOutput->transmitData();

	this->_pOutput->unlock();
};

void nuiPSModule::start()
{
	nuiModule::start();

	GUID guid = CLEyeGetCameraUUID(0);	// Get Camera GUID by index
	this->_pCam = new CLEyeCamera(guid, CLEYE_COLOR_PROCESSED, CLEYE_VGA, 120);

	this->_pFrame =  cvCreateImage( cvSize(_pCam->width(), _pCam->height()), IPL_DEPTH_8U, 4 );
};

void nuiPSModule::stop()
{
	nuiModule::stop();
};
