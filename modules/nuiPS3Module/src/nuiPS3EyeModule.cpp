//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiPS3EyeModule
// Purpose:		Capture video from PS3Eye camera using PS3EyeEye SDK from Code Laboratories
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "nuiPS3EyeModule.h"

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"

std::list<GUID> nuiPSEye::usedCameras;

nuiPSEyeDataPacket::~nuiPSEyeDataPacket()
{
	cvReleaseImage(&data);
};

nuiDataPacketError nuiPSEyeDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiPSEyeDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiPSEyeDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiPSEyeDataPacket* newDataPacket = new nuiPSEyeDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = cvCloneImage((this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiPSEyeDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(PSEye, "native", "Capture video from PS3Eye camera using Code Laborotories PS3Eye Eye SDK")

nuiPSEye::nuiPSEye() : nuiModule()
{
	MODULE_INIT();

	this->_pOutput = new nuiEndpoint(this);
	this->_pOutput->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->_pOutput);

	this->_pOutputDataPacket = new nuiPSEyeDataPacket();
};

nuiPSEye::~nuiPSEye()
{};

void nuiPSEye::update()
{
	//LOG(NUI_DEBUG, "PS3Eye module update called");
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

void nuiPSEye::start()
{
	if(CLEyeGetCameraCount() != 0) {
		GUID guid = CLEyeGetCameraUUID(this->property("camera_number").asInteger());
		if(nuiPSEye::isCameraInUse(guid))
		{
			LOG(NUI_DEBUG, "Camera already in use");
			stop();
			return;
		}
		nuiPSEye::usedCameras.push_back(guid);
		this->_pCam = new CLEyeCamera(guid, CLEYE_COLOR_PROCESSED, CLEYE_VGA, 120);
		this->_pFrame =  cvCreateImage( cvSize(_pCam->width(), _pCam->height()), IPL_DEPTH_8U, 4 );
		LOG(NUI_DEBUG, "Starting PS module");
		nuiModule::start();
	} else {
		LOG(NUI_CRITICAL, "No PS Eye detected");
		stop();
	}
};

void nuiPSEye::stop()
{
	nuiModule::stop();
};

bool nuiPSEye::isCameraInUse(GUID cameraGUID)
{
	for(std::list<GUID>::iterator it = usedCameras.begin() ; it != usedCameras.end() ; it++)
		if( IsEqualGUID(*it, cameraGUID) )
			return true;

	return false;
}
