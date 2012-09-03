//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiTUIOOutputModule
// Purpose:		Output TUIO
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "ofxNCore.h"
#include "nuiTUIO.h"
#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"

nuiTUIOOutputDataPacket::~nuiTUIOOutputDataPacket()
{ };

// never used as TUIO Output is output module
nuiDataPacketError nuiTUIOOutputDataPacket::packData(const void *_data)
{	return NUI_DATAPACKET_OK; };

nuiDataPacketError nuiTUIOOutputDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

// never used as TUIO Output is output module
nuiDataPacket* nuiTUIOOutputDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{	return NULL; };

char* nuiTUIOOutputDataPacket::getDataPacketType()
{
	return "std::map<int, Blob>";
};

MODULE_DECLARE(TUIOOutputModule, "native", "Output TUIO")

nuiTUIOOutputModule::nuiTUIOOutputModule() : nuiModule()
{
	MODULE_INIT();

	this->_pInput = new nuiEndpoint(this);
	this->_pInput->setTypeDescriptor(std::string("std::map<int, Blob>"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0,this->_pInput);
};

nuiTUIOOutputModule::~nuiTUIOOutputModule()
{

};

void nuiTUIOOutputModule::update()
{
	std::map<int, Blob>* data;
	void* dataPacket = (void*)data;

	nuiDataPacket* packet = this->_pInput->getData();
	if(packet == NULL) 
		return;
	packet->unpackData(dataPacket);

	tuioOutput.sendTUIO(data);
};

void nuiTUIOOutputModule::start()
{
	host = (char*)(this->property("host").asString().c_str());
	port = this->property("port").asInteger();
	flashport = this->property("flashport").asInteger();
	
	tuioOutput.setup(host, port, flashport);

	nuiModule::start();
};

void nuiTUIOOutputModule::stop()
{
	nuiModule::stop();
};
