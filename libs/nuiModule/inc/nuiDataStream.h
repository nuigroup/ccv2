////////////////////////////////////////////////////////////////////////////
// Name:        nuiDataStream.h
// Author:      Anatoly Churikov
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef NUI_DATA_STREAM_H
#define NUI_DATA_STREAM_H

#include <string>
#include <vector>
#include <queue>
#include "pasync.h"
#include "nuiThread.h"
#include "nuiDataPacket.h"

#define MIN_NUI_STREAM_BUFFER_SIZE 16

typedef enum nuiDataStreamMetadata
{
	NUI_DATASTREAM_NONE						= 0x00000000,
	NUI_DATASTREAM_ASYNC					= 0x00000001,	//non-blocking called thread mode
	NUI_DATASTREAM_BUFFERED					= 0x00000002,	//allow buffer mode model
	NUI_DATASTREAM_DATA_DEEP_COPY			= 0x00000004,	//make a deep copy of transmitted data
	NUI_DATASTREAM_LAST_PACKET_PRIORITY		= 0x00000008,	//not usable with NUI_DATASTREAM_BUFFERED. If setted - send the latest packet, otherwise - only first
	NUI_DATASTREAM_OVERLOW					= 0x00000010,	//non blocking calle thread strategy when buffer overflowed
}nuiDataStreamMetadata;

typedef enum nuiDataStreamErrorCode
{
	NUI_DATASTREAM_OK						= 0x00000000,
	NUI_DATASTREAM_ENDPOINT_ERROR			= 0x00000001,
	NUI_DATASTREAM_ASYNC_INTERRUPTED		= 0x00000002,
	NUI_DATASTREAM_ASYNC_TIMEOUT			= 0x00000004,
	NUI_DATASTREAM_BUFFER_OVERFLOW			= 0x00000008,
	NUI_DATASTREAM_ENDPOINT_ERROR_NOT_EXIST = 0x00000010,
} nuiDataStreamErrorCode;

typedef void (*nuiDataSendCallback)(nuiDataStreamErrorCode returnCode, void *attachedData);

class nuiEndpointDescriptor;

struct nuiDataStreamDescriptor
{
public:
	int sourceModuleID,destinationModuleID,sourcePort,destinationPort;
	bool deepCopy,asyncMode,buffered,lastPacket,overflow;
	int bufferSize;
};

class nuiEndpoint;

class nuiDataStream
{
public:
	nuiDataStream(bool asyncMode = false, nuiDataSendCallback defaultCallback = NULL, bool deepCopy = false,  bool bufferedMode = false, int bufferSize = MIN_NUI_STREAM_BUFFER_SIZE, bool lastPacketProprity = true);
	~nuiDataStream();
public:
	inline bool isDeepCopy();
	inline bool isLastPacketPriority();
	inline bool isAsyncMode();
	inline bool isBuffered();
	inline bool isOverflow();
	inline int getBufferSize();
	inline bool isRunning();
	inline nuiEndpoint *getReceiver();
	void setIsOverflow(bool overflow);
	void setReceiver(nuiEndpoint &receiver);
	void setBufferedMode(bool buffered);
	void setAsyncMode(bool async);
	void setBufferSize(int bufferSize);
	void setDeepCopy(bool deepCopy);
	void setLastPacketPriority(bool lastPacketPriority);
public:
	void startStream();
	void stopStream();
	void sendData(nuiDataPacket *dataPacket, nuiDataSendCallback callback = NULL, int timelimit = 0);
private:
	void initStream();
	void cleanStream();
	bool hasDataToSent(bool isAsyncMode = false);
	void processData();
private:
	static void _thread_process(nuiThread *thread);
private:
	nuiThread* asyncThread;
	nuiDataSendCallback defaultCallback;
	std::queue<nuiDataSendCallback> callbackQueue;
	std::queue<nuiDataPacket*> packetData;
	pt::mutex *mtx;
	pt::semaphore *semaphore;
	nuiDataStreamMetadata streamMetadata;
	int bufferSize;
	bool running;
	nuiEndpoint* receiver;
};

#endif

