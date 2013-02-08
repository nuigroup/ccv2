/** 
 * \file      nuiDataStream.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2011 NUI Group. All rights reserved.
 */

#ifndef NUI_DATA_STREAM_H
#define NUI_DATA_STREAM_H

#include <string>
#include <vector>
#include <queue>
#include "pasync.h"
#include "nuiThread.h"
#include "nuiDataPacket.h"

#define MIN_NUI_STREAM_BUFFER_SIZE 16

class nuiEndpoint;

//! namespaced enum of datastream errors
struct nuiDatastreamError
{
  enum err
  {
    Success,
    EndpointError,
    NonexistentEndpoint,
  };
};

//! namespaced enum of datastream running modes
struct nuiDatastreamMode
{
  enum m
  {
    None                = 0x00000000,
    Async               = 0x00000001,
    Buffered            = 0x00000002,
    DeepCopy            = 0x00000004,
    LastPacketPriority  = 0x00000008,
    Overflow            = 0x00000010,
  };
};

typedef void (*nuiDataSendCallback)(nuiDatastreamError::err returnCode, void *attachedData);

//! datastream descriptor
struct nuiDataStreamDescriptor
{
public:
	int sourceModuleID;
  int destinationModuleID;
  int sourcePort;
  int destinationPort;

	bool deepCopy;
  bool asyncMode;
  bool buffered;
  bool lastPacket;
  bool overflow;

	int bufferSize;
};

//! datastream class, used to transfer data from one endpoint to another
class nuiDataStream
{
public:
	nuiDataStream(bool asyncMode = false, bool deepCopy = true, bool bufferedMode = false, 
    int bufferSize = MIN_NUI_STREAM_BUFFER_SIZE, bool lastPacketProprity = true);

	~nuiDataStream();
  
  inline bool isRunning();

  // getters
	inline bool isDeepCopy();
	inline bool isLastPacketPriority();
	inline bool isAsyncMode();
	inline bool isBuffered();
	inline bool isOverflow();
	inline int getBufferSize();
	inline nuiEndpoint* getReceiver();

  // setters
  void setDeepCopy(bool deepCopy);
  void setLastPacketPriority(bool lastPacketPriority);
  void setAsyncMode(bool async);
  void setBufferedMode(bool buffered);
	void setIsOverflow(bool overflow);
  void setBufferSize(int bufferSize);
  void setReceiver(nuiEndpoint &receiver);
	
  // stream control
	void startStream();
	void stopStream();
	void sendData(nuiDataPacket *dataPacket);

private:
  static void thread_process(nuiThread *thread);

	void initStream();
	void cleanStream();
	bool hasDataToSend(bool isAsyncMode = false);
	void processData();

	nuiThread* asyncThread;
  pt::mutex *mtx;
  pt::semaphore *semaphore;

	std::queue<nuiDataPacket*> packetData;
	
	nuiDatastreamMode::m streamMetadata;
	int bufferSize;
	bool running;
	nuiEndpoint* receiver;
};

#endif

