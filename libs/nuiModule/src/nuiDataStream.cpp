/** 
 * \file      nuiDataStream.cpp
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2011 NUI Group. All rights reserved.
 */

#include <assert.h>

#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiModule.h"
#include "nuiUtils.h"

nuiDataStream::nuiDataStream(bool asyncMode, nuiDataSendCallback defaultCallback, 
  bool deepCopy, bool bufferedMode, int bufferSize, bool lastPacketProprity)
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
  return streamMetadata & nuiDatastreamMode::DeepCopy;
}

bool nuiDataStream::isLastPacketPriority()
{
  return isBuffered() ? false : (streamMetadata & nuiDatastreamMode::LastPacketPriority);
}

bool nuiDataStream::isAsyncMode()
{
  return streamMetadata & nuiDatastreamMode::Async;
}

bool nuiDataStream::isBuffered()
{
  return streamMetadata & nuiDatastreamMode::Buffered;
}

bool nuiDataStream::isOverflow()
{
  return isBuffered() ? (streamMetadata & nuiDatastreamMode::Overflow) : false;
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
  streamMetadata = (nuiDatastreamMode::m)(streamMetadata ^ nuiDatastreamMode::Overflow);
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
  streamMetadata = (nuiDatastreamMode::m)(streamMetadata ^ nuiDatastreamMode::Buffered);
  if (isRunning())
    initStream();
}

void nuiDataStream::setAsyncMode(bool async)
{
  if (isAsyncMode() == async)
    return;
  streamMetadata = (nuiDatastreamMode::m)(streamMetadata ^ nuiDatastreamMode::Async);
  if (isRunning())
    initStream();
}

void nuiDataStream::setBufferSize(int bufferSize)
{
  this->bufferSize = (bufferSize < MIN_NUI_STREAM_BUFFER_SIZE) ? 
    MIN_NUI_STREAM_BUFFER_SIZE : bufferSize;
  if (isRunning())
    initStream();
}

void nuiDataStream::setDeepCopy(bool deepCopy)
{
  if (isDeepCopy() == deepCopy)
    return;
  streamMetadata = (nuiDatastreamMode::m)(streamMetadata ^ nuiDatastreamMode::DeepCopy);
  if (isRunning())
    initStream();
}

void nuiDataStream::setLastPacketPriority(bool lastPacketPriority)
{
  if (isLastPacketPriority() == lastPacketPriority)
    return;
  streamMetadata = (nuiDatastreamMode::m)(streamMetadata ^ nuiDatastreamMode::LastPacketPriority);
  if (isRunning())
    initStream();
}

void nuiDataStream::startStream()
{
  running = true;
  if (isAsyncMode())
  {
    if (asyncThread == NULL)
      asyncThread = new nuiThread(nuiDataStream::thread_process, this);
      asyncThread->start();
  }
}

void nuiDataStream::stopStream()
{
  mtx->lock(); //maybe cause null pointers?
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
    asyncThread = new nuiThread(nuiDataStream::thread_process, this);
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
    nuiDataPacketError::err errorCode;
    addingData = dataPacket->copyPacketData(errorCode);
    if (errorCode != nuiDataPacketError::NoError)
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
    nuiDataPacket *dataToSent = NULL;
    nuiDataSendCallback callback = NULL;

    //mtx->lock(); //this should be here but doesn't work
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
      if (callback != NULL)
        callback(nuiDatastreamError::EndpointError, dataToSent); //potential memory leak
    }
    else
    {
      nuiDatastreamError::err returnCode = receiver->writeData(dataToSent);// receiver->writeData(dataToSent);
      if (callback != NULL)
        callback(returnCode, NULL);
    }
  }
}

void nuiDataStream::thread_process(nuiThread *thread) 
{
  nuiDataStream* dataStream = (nuiDataStream*)thread->getUserData();
  while ( !thread->wantQuit()) 
  {
    if (!dataStream->hasDataToSent(true))
      continue;
    dataStream->processData();
  }
}