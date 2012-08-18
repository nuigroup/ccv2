//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiVideoFileSource
// Purpose:		Get video from a file source
// Author:		Scott Halstvedt
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "nuiVideoFileSource.h"

nuiVideoFileSourceDataPacket::~nuiVideoFileSourceDataPacket()
{
	cvReleaseImage(&data);
};

nuiDataPacketError nuiVideoFileSourceDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiVideoFileSourceDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiVideoFileSourceDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiVideoFileSourceDataPacket* newDataPacket = new nuiVideoFileSourceDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = new IplImage(*(this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiVideoFileSourceDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(VideoFileSource, "native", "Get video from a file source")

nuiVideoFileSource::nuiVideoFileSource() : nuiModule()
{
	MODULE_INIT();

	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->output);

	this->outputDataPacket = new nuiVideoFileSourceDataPacket();
};

nuiVideoFileSource::~nuiVideoFileSource()
{};

void nuiVideoFileSource::update()
{
	//LOG(NUI_DEBUG, "ps3 module update called");
	this->output->lock();

	this->output->clear();

	frame = cvQueryFrame(cap);
	if(frame == NULL) {
		cvReleaseCapture(&cap);
		cvReleaseImage(&frame);
		cap = cvCaptureFromAVI("reardi.avi");
		frame = cvQueryFrame(cap);
	}

	this->outputDataPacket->packData((void*)frame);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
};

void nuiVideoFileSource::start()
{
	cap = cvCaptureFromAVI("out.avi");
	nuiModule::start();
};

void nuiVideoFileSource::stop()
{
	cvReleaseCapture(&cap);
	//cvReleaseImage(&frame);
	nuiModule::stop();
};
