/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiBackgroundSubtractModule.cpp
// Purpose:     Module to subtract the background
// Author:      Scott Halstvedt
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiBackgroundSubtractModule.h"

nuiBackgroundSubtractModuleDataPacket::~nuiBackgroundSubtractModuleDataPacket()
{
		cvReleaseImage(&data);
};

nuiDataPacketError nuiBackgroundSubtractModuleDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiBackgroundSubtractModuleDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiBackgroundSubtractModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiBackgroundSubtractModuleDataPacket* newDataPacket = new nuiBackgroundSubtractModuleDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = new IplImage(*(this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiBackgroundSubtractModuleDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(BackgroundSubtractModule, "native", "Filter out background");

nuiBackgroundSubtractModule::nuiBackgroundSubtractModule() : nuiModule() {
    MODULE_INIT();

	this->input = new nuiEndpoint(this);
    this->input->setTypeDescriptor(std::string("IplImage"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
	this->frameOutput = new nuiEndpoint(this);
	this->frameOutput->setTypeDescriptor(std::string("IplImage"));
	this->bgOutput = new nuiEndpoint(this);
	this->bgOutput->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(2);
	this->setOutputEndpoint(0,this->frameOutput);
	this->setOutputEndpoint(1,this->bgOutput);

	this->outputDataPacket = new nuiBackgroundSubtractModuleDataPacket();

}

nuiBackgroundSubtractModule::~nuiBackgroundSubtractModule() {
}

void nuiBackgroundSubtractModule::update() {  
	this->frameOutput->lock();
	this->frameOutput->clear();
	this->bgOutput->lock();
	this->bgOutput->clear();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if(packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);
	cv::Mat matFrame = cv::cvarrToMat(filterFrame, true);
	cv::Mat foreFrame;
	cv::Mat backImage;
	bg.operator ()(matFrame,foreFrame);
	//bg.getBackgroundImage(backImage);
	backImage = foreFrame;
	this->outputDataPacket->packData(new IplImage(foreFrame));
	this->frameOutput->setData(this->outputDataPacket);
	this->frameOutput->transmitData();
	this->frameOutput->unlock();
	this->outputDataPacket->packData(new IplImage(foreFrame));
	this->bgOutput->setData(this->outputDataPacket);
	this->bgOutput->transmitData();
	this->bgOutput->unlock();
	cvReleaseImage(&filterFrame);
}

void nuiBackgroundSubtractModule::start() {
	nuiModule::start();
	LOG(NUI_DEBUG,"starting gaussian filter");
}