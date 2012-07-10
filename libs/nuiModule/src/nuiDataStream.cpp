/////////////////////////////////////////////////////////////////////////////
// Name:        nuiDataStream.cpp
// Author:      Anatoly Churikov
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiModule.h"
#include "nuiUtils.h"

nuiDataStream::nuiDataStream(bool asyncMode, nuiDataSendCallback defaultCallback, bool deepCopy,  bool bufferedMode, int bufferSize, bool lastPacketProprity)
{
	running = false;
	setAsyncMode(asyncMode);
	this->defaultCallback = defaultCallback;
	setDeepCopy(deepCopy);
	setBufferedMode(bufferedMode);
	setLastPacketPriority(lastPacketProprity);
	setBufferSize(bufferSize);
	mtx = new pt::mutex();
	asyncThread = NULL;
	semaphore = NULL;
}

nuiDataStream::~nuiDataStream()
{
	mtx->lock();
	cleanStream();
	mtx->unlock();
	delete mtx;
}

bool nuiDataStream::isDeepCopy()
{
	return streamMetadata & NUI_DATASTREAM_DATA_DEEP_COPY;
}

bool nuiDataStream::isLastPacketPriority()
{
	return isBuffered() ? false : streamMetadata & NUI_DATASTREAM_LAST_PACKET_PRIORITY;
}

bool nuiDataStream::isAsyncMode()
{
	return streamMetadata & NUI_DATASTREAM_ASYNC;
}

bool nuiDataStream::isBuffered()
{
	return streamMetadata & NUI_DATASTREAM_BUFFERED;
}

bool nuiDataStream::isOverflow()
{
	return isBuffered() ? NUI_DATASTREAM_OVERLOW & streamMetadata : false;
}

int nuiDataStream::getBufferSize() 
{ 
	return isBuffered() ? bufferSize : 0; 
}

bool nuiDataStream::isRunning()
{
	return running; 
}

nuiEndpoint *nuiDataStream::getReceiver()
{
	return receiver;
}
	
void nuiDataStream::setIsOverflow(bool overflow)
{
	if (isOverflow() == overflow)
		return;
	streamMetadata = (nuiDataStreamMetadata)(streamMetadata ^ NUI_DATASTREAM_BUFFER_OVERFLOW);
	if (isRunning())
		initStream();
}

void nuiDataStream::setReceiver(nuiEndpoint &receiver)
{
	this->receiver = &receiver;
}

void nuiDataStream::setBufferedMode(bool buffered)
{
	if (isBuffered() == buffered)
		return;
	streamMetadata = (nuiDataStreamMetadata)(streamMetadata ^ NUI_DATASTREAM_BUFFERED);
	if (isRunning())
		initStream();
}

void nuiDataStream::setAsyncMode(bool async)
{
	if (isAsyncMode() == async)
		return;
	streamMetadata = (nuiDataStreamMetadata)(streamMetadata ^ NUI_DATASTREAM_ASYNC);
	if (isRunning())
		initStream();
}

void nuiDataStream::setBufferSize(int bufferSize)
{
	this->bufferSize = bufferSize < MIN_NUI_STREAM_BUFFER_SIZE ? MIN_NUI_STREAM_BUFFER_SIZE : bufferSize;
	if (isRunning())
		initStream();
}

void nuiDataStream::setDeepCopy(bool deepCopy)
{
	if (isDeepCopy() == deepCopy)
		return;
	streamMetadata = (nuiDataStreamMetadata)(streamMetadata ^ NUI_DATASTREAM_DATA_DEEP_COPY);
	if (isRunning())
		initStream();
}

void nuiDataStream::setLastPacketPriority(bool lastPacketPriority)
{
	if (isLastPacketPriority() == lastPacketPriority)
		return;
	streamMetadata = (nuiDataStreamMetadata)(streamMetadata ^ NUI_DATASTREAM_LAST_PACKET_PRIORITY);
	if (isRunning())
		initStream();
}

void nuiDataStream::startStream()
{
	running = true;
	if (isAsyncMode())
	{
		if (asyncThread == NULL)
			asyncThread = new nuiThread(nuiDataStream::_thread_process, this);
		if (asyncThread == NULL)
			setAsyncMode(false);
		else
			asyncThread->start();
	}
}

void nuiDataStream::stopStream()
{
	mtx->lock();
	running = false;
	cleanStream();
	mtx->unlock();
}

void nuiDataStream::initStream()
{
	mtx->lock();
	cleanStream();
	if (isAsyncMode())
	{
		asyncThread = new nuiThread(nuiDataStream::_thread_process, this);
		if (asyncThread == NULL)
			setAsyncMode(false);
	}
	semaphore = new pt::semaphore(isBuffered() ? getBufferSize() : 1);
	mtx->unlock();
}

void nuiDataStream::cleanStream()
{
	if (asyncThread!=NULL)
	{
		asyncThread->stop();
		asyncThread->post();
		asyncThread->waitfor();
		delete asyncThread;
		asyncThread = NULL;
	}
	while (!callbackQueue.empty())
	{
		callbackQueue.pop();
	}
	while (!packetData.empty())
	{
		if (isDeepCopy())
			free(packetData.front());
		packetData.pop();
	}
	if (semaphore!=NULL)
	{
		delete semaphore;
		semaphore = NULL;
	}
}

//timelimit - ?
//callback - ?
void nuiDataStream::sendData(nuiDataPacket *dataPacket, nuiDataSendCallback callback, int timelimit)
{
	mtx->lock();

	nuiDataPacket *addingData = dataPacket;
	if (isDeepCopy())
	{
		nuiDataPacketError errorCode;
		addingData = dataPacket->copyPacketData(errorCode);
		if (errorCode != NUI_DATAPACKET_OK)
		{
			addingData = dataPacket;
			setDeepCopy(false);
		}
	}
	
	if (isBuffered())
	{
		semaphore->wait();
		
		packetData.push(addingData);
		callbackQueue.push(callback);

		if (isOverflow())
		{
			void *dataToBeDeleted = packetData.front();
			if (isDeepCopy())
				free(dataToBeDeleted);
			packetData.pop();
			callbackQueue.pop();
			packetData.push(addingData);
			callbackQueue.push(callback);
			semaphore->post();
		}

		if (isAsyncMode())			
		{							
			if (asyncThread!=NULL)	
				asyncThread->post();
		}	
	}
	else
	{
		if (isLastPacketPriority())
		{
			while (!packetData.empty())
			{
				if (isDeepCopy())
					delete packetData.front();
				packetData.pop();
			}
			while (!callbackQueue.empty())
			{
				callbackQueue.pop();
			}
			packetData.push(addingData);
			callbackQueue.push(callback);
			if (isAsyncMode())			
			{							
				if (asyncThread!=NULL)	
					asyncThread->post();
			}	
		}
		else
		{
			if (packetData.empty())
			{
				packetData.push(addingData);
				callbackQueue.push(callback);
				if (isAsyncMode())			
				{							
					if (asyncThread!=NULL)	
						asyncThread->post();
				}	
				return;
			}
			if (isDeepCopy())
				delete addingData;
		}
	}
	mtx->unlock();

	if (hasDataToSent() &&  (!isAsyncMode()))			
		processData();
}

bool nuiDataStream::hasDataToSent(bool isAsyncMode)
{
	if (!packetData.empty())
		return true;
	if (isAsyncMode)
	{
		if (asyncThread!=NULL)
			asyncThread->wait();
		return (!packetData.empty());
	}
	return false;
}
	
void nuiDataStream::processData()
{
	if (!packetData.empty())
	{
		void *dataToSent = NULL;
		nuiDataSendCallback callback = NULL;

		mtx->lock();
		dataToSent = packetData.front();
		callback = callbackQueue.front();
		packetData.pop();
		callbackQueue.pop();
		if (callback == NULL)
			callback = defaultCallback;
		//???????
		if (isBuffered() && (!isOverflow()) && (isAsyncMode()))
			semaphore->post();
		mtx->unlock();

		if (receiver == NULL)
		{
			if (callback!=NULL)
				callback(NUI_DATASTREAM_ENDPOINT_ERROR,dataToSent); //potential memory leak
		}
		else
		{
			nuiDataStreamErrorCode returnCode = NUI_DATASTREAM_OK;// receiver->writeData(dataToSent);
			if (callback!=NULL)
				callback(returnCode, NULL);
		}

	}
}

void nuiDataStream::_thread_process(nuiThread *thread) 
{
	nuiDataStream* dataStream = (nuiDataStream*)thread->getUserData();
	while ( !thread->wantQuit()) 
	{
		if (!dataStream->hasDataToSent(true))
			continue;
		dataStream->processData();
	}
}


