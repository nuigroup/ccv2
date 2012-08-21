//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiTUIOOutputModule
// Purpose:		Output TUIO
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#include "nuiTUIO.h"
#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"

nuiTUIOOutputDataPacket::~nuiTUIOOutputDataPacket()
{

};

nuiDataPacketError nuiTUIOOutputDataPacket::packData(const void *_data)
{
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiTUIOOutputDataPacket::unpackData(void* &_data)
{
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiTUIOOutputDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	return NULL;
};

char* nuiTUIOOutputDataPacket::getDataPacketType()
{
	return "*";
};

MODULE_DECLARE(TUIOOutputModule, "native", "Output TUIO")

nuiTUIOOutputModule::nuiTUIOOutputModule() : nuiModule()
{
	MODULE_INIT();

	this->_pInput = new nuiEndpoint(this);
	this->_pInput->setTypeDescriptor(std::string("*"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0,this->_pInput);

	this->_pOutput = new nuiEndpoint(this);
	this->_pOutput->setTypeDescriptor(std::string("*"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->_pOutput);

	this->_pInputDataPacket = new nuiTUIOInputDataPacket();
	this->_pOutputDataPacket = new nuiTUIOOutputDataPacket();
};

nuiTUIOOutputModule::~nuiTUIOOutputModule()
{};

void nuiTUIOOutputModule::update()
{

};

void nuiTUIOOutputModule::start()
{

};

void nuiTUIOOutputModule::stop()
{
	nuiModule::stop();
};
