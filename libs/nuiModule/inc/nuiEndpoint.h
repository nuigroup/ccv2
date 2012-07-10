/////////////////////////////////////////////////////////////////////////////
// Name:        nuiEndpoint.h
// Author:      Anatoly Churikov
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef _NUI_ENDPOINT_H_
#define _NUI_ENDPOINT_H_

#include <string>
#include <vector>
#include <map>

#include "pasync.h"
#include "nuiUtils.h"
#include "nuiThread.h"
#include "nuiDataStream.h"
#include "nuiDataPacket.h"


class nuiModule;
class nuiModuleDescriptor;

class nuiEndpointDescriptor
{
public:
	nuiEndpointDescriptor(std::string typeDescriptor);
	std::string &getDescriptor();
	void setDescriptor(std::string typeDescriptor);
	int getIndex();
	void setIndex(int index);
	void setParentModuleDescriptor(nuiModuleDescriptor* parentModuleDescriptor);
	nuiModuleDescriptor* getParentModuleDescriptor();
private:
	std::string typeDescriptor;
	nuiModuleDescriptor* parentModuleDescriptor;
	int index;
};

class nuiEndpoint
{
public:
	nuiEndpoint();
	virtual ~nuiEndpoint();
public:
	void transmitData();
	nuiDataStream *addConnection(nuiEndpoint *endpoint);
	nuiDataStreamErrorCode removeConnection(nuiEndpoint *endpoint);
	void removeConnections();
	nuiDataStream *getDataStreamForEndpoint(nuiEndpoint *endpoint);
	unsigned int getConnectionCount();
public:
	void setTypeDescriptor(std::string typeDescriptor);
	void setData(nuiDataPacket *dataPacket);
	void setModuleHoster(nuiModule *moduleHoster);
	nuiModule *getModuleHoster();
	nuiDataPacket *getData();
	nuiEndpoint *getConnectedEndpointOnIndex(int index);
	inline std::string getTypeDescriptor();
	void lock();
	void unlock();
	void clear();
private:
	bool canBePairedWithEndpoint(nuiEndpoint *endpoint);
	bool canBeSettedData(nuiDataPacket *dataPacket);
	nuiDataStreamErrorCode writeData(nuiDataPacket *dataPacket);
private:
	std::map<nuiEndpoint*,nuiDataStream*> dataStreams;
	pt::mutex *mtx;
	std::string typeDescriptor;
	nuiDataPacket *dataPacket;
	nuiModule* moduleHoster;
	friend class nuiDataStream;
};

#endif//_NUI_ENDPOINT_H_