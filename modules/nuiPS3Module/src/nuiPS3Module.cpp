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
	cvReleaseImage(&data);
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
	IplImage* newData = cvCloneImage((this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiPSModuleDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(PSModule, "native", "Capture video from PS3 camera using Code Laborotories PS3 Eye SDK")

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
	//LOG(NUI_DEBUG, "ps3 module update called");
	this->_pOutput->lock();

	this->_pOutput->clear();

	this->_pCam->getFrame(_pFrame);

	CvFont font;
    double hScale=0.5;
    double vScale=0.5;
    int    lineWidth=1;
    cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
	std::ostringstream oss;
	oss << "~ " << this->timer->getAverageFPS() << " FPS";
	cvRectangle(_pFrame, cvPoint(150,0), cvPoint(300,20), cvScalar(0,0,0), CV_FILLED, CV_AA);
	cvRectangle(_pFrame, cvPoint(150,0), cvPoint(300,20), cvScalar(100,100,255), 2, CV_AA);
	cvPutText (_pFrame, oss.str().c_str(), cvPoint(155,15), &font, cvScalar(255,255,255));

	this->_pOutputDataPacket->packData((void*)_pFrame);
	this->_pOutput->setData(this->_pOutputDataPacket);
	this->_pOutput->transmitData();
	this->_pOutput->unlock();
};

void nuiPSModule::start()
{
	if(CLEyeGetCameraCount() != 0) {
		GUID guid = CLEyeGetCameraUUID(0);	// Get first Camera GUID by index
		this->_pCam = new CLEyeCamera(guid, CLEYE_COLOR_PROCESSED, CLEYE_VGA, 120);
		this->_pFrame =  cvCreateImage( cvSize(_pCam->width(), _pCam->height()), IPL_DEPTH_8U, 4 );
		LOG(NUI_DEBUG, "Starting PS module");
		nuiModule::start();
	} else {
		LOG(NUI_CRITICAL, "No PS Eye detected");
		stop();
	}
};

void nuiPSModule::stop()
{
	nuiModule::stop();
};
