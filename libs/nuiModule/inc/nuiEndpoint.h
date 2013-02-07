/** 
 * \file      nuiEndpoint.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2011 NUI Group. All rights reserved.
 */

#ifndef NUI_ENDPOINT_H
#define NUI_ENDPOINT_H

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

//! endpoint class, used to hold input and output data for module.
class nuiEndpoint
{
public:
	nuiEndpoint(nuiModule *hostModule);
	virtual ~nuiEndpoint();

  //! lock data and send it to all outgoing streams
	void transmitData();

	nuiDataStream* addConnection(nuiEndpoint *endpoint);
	nuiDatastreamError::err removeConnection(nuiEndpoint *endpoint);

  //! erase all connections at once
	void removeConnections();

	nuiDataStream* getDataStreamForEndpoint(nuiEndpoint *endpoint);
	unsigned int getConnectionCount();
  
  //! \todo setTypeDescriptor marked as public?
	void setTypeDescriptor(std::string typeDescriptor);

  //! set endpoint to hold provided data
	void setData(nuiDataPacket *dataPacket);
  //! obtain stored data
	nuiDataPacket* getData();

  //! assign parent module
	void setModuleHoster(nuiModule *moduleHoster);
  //! get parent module
	nuiModule* getModuleHoster();

	nuiEndpoint* getConnectedEndpointOnIndex(int index);
	inline std::string getTypeDescriptor();
	void lock();
	void unlock();
	void clear();

private:
	bool canBePairedWithEndpoint(nuiEndpoint *endpoint);
	bool canBeSettedData(nuiDataPacket *dataPacket);
	nuiDatastreamError::err writeData(nuiDataPacket *dataPacket);

  //! add endpoint and datastream, that connects this and that endpoints
	std::map<nuiEndpoint*,nuiDataStream*> dataStreams;
	pt::mutex *mtx;
	std::string typeDescriptor;
	nuiDataPacket *dataPacket;
	nuiModule* moduleHoster;

	friend class nuiDataStream;
};

#endif//NUI_ENDPOINT_H