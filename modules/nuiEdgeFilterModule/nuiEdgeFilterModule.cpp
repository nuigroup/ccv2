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
	IplImage* newData = cvCloneImage(this->data);

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
	try {
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
		cv::Mat grad;
		if(!this->property("disable").asBool()) {
			if(this->property("mode").asString() == "canny") {
				cv::Canny(edges, grad, 0, 30, 3);
			} else if(this->property("mode").asString() == "sobel") {

				cv::Mat src, src_gray;
				int scale = 1;
				int delta = 0;
				int ddepth = CV_16S;

				cv::Mat grad_x, grad_y;
				cv::Mat abs_grad_x, abs_grad_y;

				/// Gradient X
				//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
				cv::Sobel( edges, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
				cv::convertScaleAbs( grad_x, abs_grad_x );

				/// Gradient Y
				//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
				cv::Sobel( edges, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
				convertScaleAbs( grad_y, abs_grad_y );

				/// Total Gradient (approximate)
				cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
			} else {
				grad = edges;
			}
		}
		IplImage* oldImage = new IplImage(grad);
		this->outputDataPacket->packData(oldImage);
		this->output->setData(this->outputDataPacket);
		this->output->transmitData();
		this->output->unlock();
		cvReleaseImage(&filterFrame);
		delete packet;
	} catch (cv::Exception& e) {
		LOG(NUI_CRITICAL, "OpenCV exception");
	}
}

void nuiEdgeFilterModule::start() {
	nuiModule::start();
	this->timer->Start();
	LOG(NUI_DEBUG,"starting filter");
}