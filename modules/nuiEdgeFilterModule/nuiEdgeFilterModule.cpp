/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiEdgeFilterModule.cpp
// Purpose:     Module to filter edges
// Author:      Scott Halstvedt
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiEdgeFilterModule.h"

nuiEdgeFilterModuleDataPacket::~nuiEdgeFilterModuleDataPacket()
{
		cvReleaseImage(&data);
};

nuiDataPacketError nuiEdgeFilterModuleDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiEdgeFilterModuleDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiEdgeFilterModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiEdgeFilterModuleDataPacket* newDataPacket = new nuiEdgeFilterModuleDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = new IplImage(*(this->data));

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiEdgeFilterModuleDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(EdgeFilterModule, "native", "Filter edges");

nuiEdgeFilterModule::nuiEdgeFilterModule() : nuiModule() {
    MODULE_INIT();

	this->input = new nuiEndpoint(this);
    this->input->setTypeDescriptor(std::string("IplImage"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->output);

	this->outputDataPacket = new nuiEdgeFilterModuleDataPacket();
}

nuiEdgeFilterModule::~nuiEdgeFilterModule() {
}

void nuiEdgeFilterModule::update() { 
	this->output->lock();
	this->output->clear();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if(packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);
	cv::Mat newFrame = cv::cvarrToMat(filterFrame);
	cv::Mat edges = cv::cvarrToMat(filterFrame);
	cv::Canny(edges, edges, 0, 30, 3);
	IplImage* oldImage = new IplImage(edges);
	this->outputDataPacket->packData(oldImage);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
	cvReleaseImage(&filterFrame);
}

void nuiEdgeFilterModule::start() {
	nuiModule::start();
	this->timer->Start();
	LOG(NUI_DEBUG,"starting filter");
}