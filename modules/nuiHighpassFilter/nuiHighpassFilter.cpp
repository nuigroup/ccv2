/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiHighpassFilter.cpp
// Purpose:     Highpass filter
// Author:      Scott Halstvedt
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiHighpassFilter.h"

nuiHighpassFilterDataPacket::~nuiHighpassFilterDataPacket()
{
		cvReleaseImage(&data);
};

nuiDataPacketError nuiHighpassFilterDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiHighpassFilterDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiHighpassFilterDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiHighpassFilterDataPacket* newDataPacket = new nuiHighpassFilterDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = new IplImage(*(this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiHighpassFilterDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(HighpassFilter, "native", "Filter gaussian");

nuiHighpassFilter::nuiHighpassFilter() : nuiModule() {
    MODULE_INIT();

	this->input = new nuiEndpoint(this);
    this->input->setTypeDescriptor(std::string("IplImage"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->output);

	this->outputDataPacket = new nuiHighpassFilterDataPacket();
}

nuiHighpassFilter::~nuiHighpassFilter() {
}

void nuiHighpassFilter::update() {    
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if(packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);

	CvMat* kernel=0;
	IplImage* dst = cvCloneImage(frame);

	kernel = cvCreateMat(9,9,CV_32FC1);
	cvSet2D( kernel, 0, 0, cvRealScalar(0) );
	cvSet2D( kernel, 0, 1, cvRealScalar(-1) );
	cvSet2D( kernel, 0, 2, cvRealScalar(0) );
	cvSet2D( kernel, 1, 0, cvRealScalar(-1) );
	cvSet2D( kernel, 1, 1, cvRealScalar(5) );
	cvSet2D( kernel, 1, 2, cvRealScalar(-1) );
	cvSet2D( kernel, 2, 0, cvRealScalar(0) );
	cvSet2D( kernel, 2, 1, cvRealScalar(-1) );
	cvSet2D( kernel, 2, 2, cvRealScalar(0) );

	cvFilter2D( filterFrame, dst, kernel, cvPoint(-1,-1) );
	IplImage* oldImage = cvCloneImage(dst);
	this->outputDataPacket->packData(oldImage);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
	cvReleaseImage(&filterFrame);
	cvReleaseImage(&dst);
}

void nuiHighpassFilter::start() {
	nuiModule::start();
	LOG(NUI_DEBUG,"starting highpass filter");
}