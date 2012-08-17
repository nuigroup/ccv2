/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiDebugVideoSink.cpp
// Purpose:     Sink to display video
// Author:      Scott Halstvedt
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiDebugVideoSink.h"

MODULE_DECLARE(DebugVideoSink, "native", "Sink to display video");

nuiDebugVideoSink::nuiDebugVideoSink() : nuiModule() {
    MODULE_INIT();

	this->input = new nuiEndpoint(this);
    this->input->setTypeDescriptor(std::string("IplImage"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
}

nuiDebugVideoSink::~nuiDebugVideoSink() {
}

void nuiDebugVideoSink::update() {    
	//LOG(NUI_DEBUG, "module update called");
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if(packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	cv::Mat newFrame = cv::cvarrToMat(frame);
	cv::Mat edges;
	cv::Mat thr;
	cv::cvtColor(newFrame, edges, CV_BGR2GRAY);
	cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
	cv::Canny(edges, edges, 0, 30, 3);
	cv::imshow("edges", edges);
	//cvThreshold( frame, thr, 100, 255, CV_THRESH_BINARY );
	cv::threshold( newFrame, thr, 70, 255,3 );
	cvShowImage("ot", frame);
	cv::imshow("thr", thr);
	//cvShowImage("thr", thr);
	cv::waitKey(1);
}

void nuiDebugVideoSink::start() {
	nuiModule::start();
	LOG(NUI_DEBUG,"starting video sink");
}