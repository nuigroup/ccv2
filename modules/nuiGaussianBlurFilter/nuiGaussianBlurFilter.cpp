/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiGaussianBlurFilter.cpp
// Purpose:     Module to filter edges
// Author:      Scott Halstvedt
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiGaussianBlurFilter.h"

nuiGaussianBlurFilterDataPacket::~nuiGaussianBlurFilterDataPacket()
{
		cvReleaseImage(&data);
};

nuiDataPacketError nuiGaussianBlurFilterDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiGaussianBlurFilterDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiGaussianBlurFilterDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiGaussianBlurFilterDataPacket* newDataPacket = new nuiGaussianBlurFilterDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = new IplImage(*(this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiGaussianBlurFilterDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(GaussianBlurFilter, "native", "Filter gaussian");

nuiGaussianBlurFilter::nuiGaussianBlurFilter() : nuiModule() {
    MODULE_INIT();

	this->input = new nuiEndpoint(this);
    this->input->setTypeDescriptor(std::string("IplImage"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->output);

	this->outputDataPacket = new nuiGaussianBlurFilterDataPacket();
}

nuiGaussianBlurFilter::~nuiGaussianBlurFilter() {
}

void nuiGaussianBlurFilter::update() {    
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if(packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);
	cv::Mat& newFrame = cv::cvarrToMat(filterFrame, true);
	cv::Mat& blur = cv::cvarrToMat(filterFrame);
	bool dev = true;
	if(dev) cv::cvtColor(newFrame, blur, CV_BGR2GRAY, 3);
	cv::GaussianBlur(blur, blur, cv::Size(9,9), 1.5, 1.5);
	IplImage* oldImage = new IplImage(blur);
	this->outputDataPacket->packData(oldImage);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
	cvReleaseImage(&filterFrame);
}

void nuiGaussianBlurFilter::start() {
	nuiModule::start();
	LOG(NUI_DEBUG,"starting gaussian filter");
}