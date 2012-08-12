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
    this->input->setTypeDescriptor(std::string("cv::Mat"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
}

nuiDebugVideoSink::~nuiDebugVideoSink() {
}

void nuiDebugVideoSink::update() {    
    /*cv::Mat *frame;
	cv::Mat edges;
    void* pFrame = (void*)frame;
    this->input->getData()->unpackData(pFrame);
    frame = (cv::Mat*)pFrame;

	//cv::namedWindow("edges",1);
	cv::cvtColor(*frame, edges, CV_BGR2GRAY);
	cv::GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
	cv::Canny(edges, edges, 0, 30, 3);
	cv::imshow("edges", edges);*/
}

void nuiDebugVideoSink::start() {
	nuiModule::start();
	cv::Mat edges;
	cv::namedWindow("edges",1);
	cv::Mat frame = cvLoadImageM("thing.png");
	cv::cvtColor(frame, edges, CV_BGR2GRAY);
    GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
    Canny(edges, edges, 0, 30, 3);
    imshow("edges", edges);
	LOG(NUI_DEBUG,"starting video sink");
	if(cv::waitKey(30) >= 0) LOG(NUI_DEBUG,"bad");
}