#include "nuiEndpoint.h"
#include "nuiModule.h"

nuiEndpoint::nuiEndpoint() 
{
	mtx = new pt::mutex();
	moduleHoster = NULL;
}

nuiEndpoint::~nuiEndpoint() 
{
	removeConnections();
	delete mtx;
}

nuiDataStreamErrorCode nuiEndpoint::writeData(nuiDataPacket* dataPacket)
{
	this->dataPacket = dataPacket;
	if (moduleHoster == NULL)
		moduleHoster->notifyDataReceived(this);
	return NUI_DATASTREAM_OK;
}

nuiEndpoint *nuiEndpoint::getConnectedEndpointOnIndex(int index)
{
	int i = 0;
	for (std::map<nuiEndpoint*,nuiDataStream*>::iterator iter = dataStreams.begin();iter != dataStreams.end(); iter++,i++)
	{
		if (i == index)
			return iter->first;
	}
	return NULL;
}


void nuiEndpoint::transmitData()
{
	mtx->lock();
	for (std::map<nuiEndpoint*,nuiDataStream*>::iterator iter = dataStreams.begin();iter != dataStreams.end(); iter++)
	{
		if (!iter->second->isRunning())
			iter->second->startStream();
		iter->second->sendData(dataPacket);
	}
	mtx->unlock();
}

nuiDataStream *nuiEndpoint::addConnection(nuiEndpoint *endpoint) 
{
	if ((endpoint == NULL) ||  (!canBePairedWithEndpoint(endpoint)))
		return NULL;
	std::map<nuiEndpoint*,nuiDataStream*>::iterator iter = dataStreams.find(endpoint);
	if (iter != dataStreams.end())
		return NULL;
	dataStreams[endpoint] = new nuiDataStream();
	dataStreams[endpoint]->setReceiver(*endpoint);
	return dataStreams[endpoint];
}

nuiDataStreamErrorCode nuiEndpoint::removeConnection(nuiEndpoint *endpoint)
{		
	std::map<nuiEndpoint*,nuiDataStream*>::iterator iter = dataStreams.find(endpoint);
	if (iter == dataStreams.end())
		return NUI_DATASTREAM_ENDPOINT_ERROR_NOT_EXIST;
	iter->second->stopStream();
	delete iter->second;
	dataStreams.erase(iter);
	return NUI_DATASTREAM_OK;
}

void nuiEndpoint::removeConnections()
{
	while (!dataStreams.empty())
	{
		std::map<nuiEndpoint*,nuiDataStream*>::iterator iter = dataStreams.begin();
		iter->second->stopStream();
		delete iter->second;
		dataStreams.erase(iter);
	}
}

nuiDataStream *nuiEndpoint::getDataStreamForEndpoint(nuiEndpoint *endpoint)
{
	std::map<nuiEndpoint*,nuiDataStream*>::iterator iter = dataStreams.find(endpoint);
	if (iter == dataStreams.end())
		return NULL;
	return iter->second;
}

unsigned int nuiEndpoint::getConnectionCount() 
{
	return dataStreams.size();
}

void nuiEndpoint::setTypeDescriptor(std::string typeDescriptor)
{
	this->typeDescriptor = typeDescriptor;
}

void nuiEndpoint::setData(nuiDataPacket *dataPacket)
{
	mtx->lock();
	if (canBeSettedData(dataPacket))
		this->dataPacket = dataPacket;	 
	mtx->unlock();
}

nuiDataPacket *nuiEndpoint::getData()
{
	return dataPacket;
}

std::string nuiEndpoint::getTypeDescriptor()
{
	return typeDescriptor;
}

bool nuiEndpoint::canBePairedWithEndpoint(nuiEndpoint *endpoint)
{
	if ( endpoint == NULL )
		return false;
	if (( endpoint->typeDescriptor == "*" ) || (this->typeDescriptor == "*"))
		return true;
	return nuiUtils::inList(endpoint->getTypeDescriptor(), this->getTypeDescriptor(), ",");
}

bool nuiEndpoint::canBeSettedData(nuiDataPacket *dataPacket)
{
	if ((dataPacket == NULL) || (dataPacket->getDataPacketType() == NULL) || (dataPacket->getDataPacketType() == "*"))
		return true;
	return nuiUtils::inList(std::string(dataPacket->getDataPacketType()), this->getTypeDescriptor(), ",");
}

void nuiEndpoint::lock()
{
	mtx->lock();
}

void nuiEndpoint::unlock()
{
	mtx->unlock();
}

void nuiEndpoint::clear()
{
	setData(NULL);
}

void nuiEndpoint::setModuleHoster(nuiModule *moduleHoster)
{
	this->moduleHoster = moduleHoster;
}

nuiModule *nuiEndpoint::getModuleHoster()
{
	return moduleHoster;
}

nuiEndpointDescriptor::nuiEndpointDescriptor(std::string typeDescriptor)
{
	this->typeDescriptor = typeDescriptor;
}

std::string &nuiEndpointDescriptor::getDescriptor()
{
	return typeDescriptor;
}

void nuiEndpointDescriptor::setDescriptor(std::string typeDescriptor)
{
	this->typeDescriptor = typeDescriptor;
}

void nuiEndpointDescriptor::setParentModuleDescriptor(nuiModuleDescriptor* parentModuleDescriptor)
{
	this->parentModuleDescriptor = parentModuleDescriptor;
}

nuiModuleDescriptor* nuiEndpointDescriptor::getParentModuleDescriptor()
{
	return this->parentModuleDescriptor;
}

int nuiEndpointDescriptor::getIndex()
{
	return index;
}
	
void nuiEndpointDescriptor::setIndex(int index)
{
	this->index = index;
}
