//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiCPUFilters
// Purpose:		Filter image so ContoutFinder and Blobtracker will be able to work
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "ofxNCore.h"
#include "nuiCPUFilters.h"
#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"

nuiCPUFiltersDataPacket::~nuiCPUFiltersDataPacket()
{ };

nuiDataPacketError nuiCPUFiltersDataPacket::packData(const void *_data)
{	
	this->setLocalCopy(false);
	this->data = (CPUImageFilter*)_data;
	return NUI_DATAPACKET_OK; 
};

nuiDataPacketError nuiCPUFiltersDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiCPUFiltersDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{	
	//! TODO : Not Implemented
	return NULL;
};

char* nuiCPUFiltersDataPacket::getDataPacketType()
{
	return "CPUImageFilter";
};

MODULE_DECLARE(CPUFiltersModule, "native", "Filter image to further processing")

nuiCPUFiltersModule::nuiCPUFiltersModule() : nuiModule()
{
	MODULE_INIT();
	filter = new ProcessFilters();
};

nuiCPUFiltersModule::~nuiCPUFiltersModule()
{

};

void nuiCPUFiltersModule::update()
{
	this->_pOutput->lock();
	this->_pOutput->clear();


	void* data;
	nuiDataPacket* packet = this->_pInput->getData();
	if(packet == NULL) 
		return;
	packet->unpackData(data);
	CPUImageFilter* image = (CPUImageFilter*)data;
	
//	if(bUseGPU)
//		filter->applyGPUFilters(*image);
//	else
		filter->applyCPUFilters(*image);

	this->_pOutputDataPacket->packData((void*)image);
	this->_pOutput->setData(this->_pOutputDataPacket);
	this->_pOutput->transmitData();
	this->_pOutput->unlock();
};

void nuiCPUFiltersModule::start()
{
	filter->bLearnBackground = this->property("bLearnBackground").asBool();
	filter->bVerticalMirror = this->property("bVerticalMirror").asBool();
	filter->bTrackDark = this->property("bTrackDark").asBool();
	filter->bHighpass = this->property("bHightpass").asBool();
	filter->bAmplify = this->property("bAmplify").asBool();
	filter->bSmooth = this->property("bSmooth").asBool();
	filter->bDynamicBG = this->property("bDynamicBG").asBool();

	//bUseGPU = this->property("bUseGPU").asBool();

	nuiModule::start();
};

void nuiCPUFiltersModule::stop()
{
	nuiModule::stop();
};
