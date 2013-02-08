/** 
 * \file      nuiModule.cpp
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2011 NUI Group. All rights reserved.
 */

#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <errno.h>
#include <string.h>

#include "pasync.h"
#include "nuiModule.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "nuiThread.h"
#include "nuiUtils.h" 

LOG_DECLARE("Module");

//==============================================================================
// nuiModule methods
//==============================================================================

nuiModule::nuiModule() 
{
	this->is_started                = false;
	this->thread                    = NULL;
	this->use_thread                = false;
	this->need_update	              = false;
	this->timer                     = new nuiTimer();
	this->properties["use_thread"]  = new nuiProperty(false);
	this->inputEndpoints            = NULL;
	this->outputEndpoints           = NULL;
	this->inputDataReceived         = NULL;
	this->inputEndpointCount        = 0;
	this->outputEndpointCount       = 0;
	this->ocsillatorWait            = 15;
	this->mtx	                      = new pt::mutex();
}

nuiModule::~nuiModule()
{
	this->stop();

	mtx->lock();
	for (int i = 0; i<inputEndpointCount; i++)
	{
		delete inputEndpoints[i];
	}
	if (inputEndpoints != NULL)
	{
		free(inputEndpoints);
		inputEndpoints = NULL;
	}
	if (inputDataReceived != NULL)
	{
		free(inputDataReceived);
		inputDataReceived = NULL;
	}

	for (int i = 0; i<outputEndpointCount; i++)
	{
		delete outputEndpoints[i];
	}
	if (outputEndpoints != NULL)
	{
		free(outputEndpoints);
		outputEndpoints = NULL;
	}

	if ( this->properties.size() > 0 ) 
	{
		std::map<std::string, nuiProperty*>::iterator it;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) 
		{
			delete (*it).second;
			(*it).second = NULL;
		}
	}
	mtx->unlock();

	delete mtx;
}

bool nuiModule::needUpdate(/*bool isAsyncMode*/)
{
	if (this->isOscillatorMode()) // if constant periodic updates
	{
// 		if (isAsyncMode)
// 			this->thread->post(); // anyway allow update
    this->need_update = false;
		return true; // okay, we need update
	}
	else if (this->need_update) // in any other case if we have flag need_update set to true
	{
		this->need_update = false; // uncheck it
		return true; // okay, we need update
	}
// 	else if ( !isAsyncMode) // otherwise no need to update in non-async mode
//   {
// 		return false;
//   }
// 	else if ( isAsyncMode) // if in async mode then wait until something will unlock thread
//   {
		this->thread->wait();
    if ( this->need_update ) // if after wait will have need_update flag set
    {
      this->need_update = false;
      return true;
    }
//  }

	return false;
}

//==============================================================================
// THREAD PROCS
//==============================================================================
void nuiModule::thread_process(nuiThread *thread)
{
	nuiModule *module = (nuiModule *)thread->getUserData();
  // extract module from thread data

	module->timer->Start();
	while ( !thread->wantQuit()) 
	{
		if (!module->needUpdate(/*true*/))
			continue;
		module->timer->Wait(); // start stopwatch
		module->update();
		module->timer->Process(); // measure results
	}
}

void nuiModule::internal_oscillator(nuiThread *thread)
{
	nuiModule *module = (nuiModule*)thread->getUserData();
	while ( !thread->wantQuit())
	{
		module->trigger();
		pt::psleep(module->ocsillatorWait);
	}
}

void nuiModule::start() 
{
	this->use_thread = this->property("use_thread").asBool();
	this->oscillator_mode = this->property("oscillator_mode").asBool();
	this->ocsillatorWait = this->property("oscillator_wait").asInteger();
	this->is_synced_input = this->property("synced_input").asBool();
	timer->Start();
	if (( this->use_thread ) || ( this->oscillator_mode))
  // if uses oscillator or should be run in a separate thread
	{
		LOGM(NUI_TRACE, "start thread");
		this->thread = new nuiThread(this->oscillator_mode ? internal_oscillator : thread_process, this);
    // create thread based on property selected

		if ( this->thread == NULL )
		{
      //! \todo then how the update will be called?
			this->oscillator_mode = false;
			this->use_thread = false;
		}
		else
		{
			this->thread->start();
		}
	}
	this->is_started = true;
	LOGM(NUI_DEBUG, "start");
}

void nuiModule::stop() 
{
	if ((this->use_thread || this->oscillator_mode) && (this->thread != NULL)) 
	{
		this->thread->stop();
		this->thread->post();
		this->thread->waitfor();
		delete this->thread;
		this->thread = NULL;
		this->use_thread = false;
		this->oscillator_mode = false;
	}
	this->need_update = false;
	this->is_started = false;
	timer->Reset();
	mtx->unlock();
  //! \todo shouldn't we stop input datastreams?
	for(int i = 0; i < this->getOutputEndpointCount(); i++) 
  {
		nuiEndpoint* endpoint = this->getOutputEndpoint(i);
		for(int j = 0; j < endpoint->getConnectionCount(); j++) 
    {
			nuiDataStream* stream = 
        endpoint->getDataStreamForEndpoint(endpoint->getConnectedEndpointAtIndex(j));
			stream->stopStream();
		}
	}
	LOG(NUI_DEBUG, "stop <" << this->property("id").asString() << ">");
}

void nuiModule::trigger()
{
	if (!this->isStarted())
		return;
  
	this->need_update = true;

	if ( this->use_thread) // if use thread - allow thread do all the work
	{
		this->thread->post();
		return;
  }

	if (this->needUpdate(/*true*/))
	{
		timer->Wait();
		this->update();
		timer->Process();
	}
}

void nuiModule::notifyDataReceived(nuiEndpoint *endpoint)
{
	if (isSyncedInput())
	{
		int receivedCount = 0;
		for (int i = 0; i < inputEndpointCount; i++)
		{
			if (endpoint == inputEndpoints[i])
			{
				inputDataReceived[i] = 1;
			}
			receivedCount += inputDataReceived[i];
		}
		if (receivedCount != getInputEndpointCount())
			return;
		memset(inputDataReceived, 0, receivedCount * sizeof(char));
	}
	this->trigger();
}

nuiProperty &nuiModule::property(std::string str)
{
	std::map<std::string, nuiProperty*>::iterator it;
	it = this->properties.find(str);
	if ( it == this->properties.end() ) {
		this->properties[str] = new nuiProperty("", "?? auto created ??");
		return *(this->properties[str]);
	}
	return *it->second;
}

std::map<std::string, nuiProperty*> &nuiModule::getProperties()
{
	return this->properties;
}

void nuiModule::setInputEndpointCount(int n)
{
	if (n == inputEndpointCount)
		return;
	mtx->lock();
	nuiEndpoint **newInputEndpoints = (nuiEndpoint **)malloc(n * sizeof(nuiEndpoint*));
	memset((void*)newInputEndpoints, 0x00, n * sizeof(nuiEndpoint*));
	char* newInputDataReceived = (char*)malloc(n * sizeof(char));
	memset((void*)newInputDataReceived, 0x00, n * sizeof(char));
	if (n > inputEndpointCount)
	{
		memcpy(newInputEndpoints,inputEndpoints, inputEndpointCount * sizeof(nuiEndpoint*));
		memcpy(newInputDataReceived, inputDataReceived, inputEndpointCount * sizeof(char));
	}
	else
	{
		memcpy(newInputEndpoints,inputEndpoints, n * sizeof(nuiEndpoint*));
		for (int i = n; i<inputEndpointCount; i++)
		{
			delete inputEndpoints[i];
		}
	}
	free(inputEndpoints);
	free(inputDataReceived);
	inputEndpoints = newInputEndpoints;
	inputDataReceived = newInputDataReceived;
	inputEndpointCount = n;
	mtx->unlock();
}

void nuiModule::setOutputEndpointCount(int n)
{
	if (n == outputEndpointCount)
		return;
	mtx->lock();
	nuiEndpoint **newOutputEndpoints = (nuiEndpoint **)malloc(n * sizeof(nuiEndpoint*));
	memset((void*)newOutputEndpoints, 0x00, n * sizeof(nuiEndpoint*));
	if (n > outputEndpointCount)
		memcpy(newOutputEndpoints,outputEndpoints, outputEndpointCount * sizeof(nuiEndpoint*));
	else
	{
		memcpy(newOutputEndpoints,outputEndpoints, n * sizeof(nuiEndpoint*));
		for (int i = n; i<outputEndpointCount; i++)
		{
			delete outputEndpoints[i];
		}
	}
	free(outputEndpoints);
	outputEndpoints = newOutputEndpoints;
	outputEndpointCount = n;
	mtx->unlock();
}

void nuiModule::setInputEndpoint(int n, nuiEndpoint *endpoint)
{
	if ((n >= inputEndpointCount) || (n<0))
		return;
	mtx->lock();
	if (inputEndpoints[n] != NULL)
		delete inputEndpoints[n];
	inputEndpoints[n] = endpoint;
	mtx->unlock();
}
	
void nuiModule::setOutputEndpoint(int n, nuiEndpoint *endpoint)
{
	if ((n >= outputEndpointCount)  || (n<0))
		return;
	mtx->lock();
	if (outputEndpoints[n] != NULL)
		delete outputEndpoints[n];
	outputEndpoints[n] = endpoint;
	mtx->unlock();
}

int nuiModule::getInputEndpointCount()
{
	return inputEndpointCount;
}

int nuiModule::getOutputEndpointCount()
{
	return outputEndpointCount;
}

nuiEndpoint *nuiModule::getInputEndpoint(int n)
{
	if ((n < 0) || (n >= inputEndpointCount))
		return NULL;
	return inputEndpoints[n];
}

nuiEndpoint *nuiModule::getOutputEndpoint(int n)
{
	if ((n < 0) || (n >= outputEndpointCount))
		return NULL;
	return outputEndpoints[n];
}

int nuiModule::getInputEndpointIndex(nuiEndpoint *stream)
{
	int result = -1;
	for (int i=0;i<inputEndpointCount;i++)
	{
		if (inputEndpoints[i] == stream)
		{
			result = i;
			break;
		}
	}
	return result;
}
	
int nuiModule::getOutputEndpointIndex(nuiEndpoint *stream)
{
	int result = -1;
	for (int i=0;i<outputEndpointCount;i++)
	{
		if (outputEndpoints[i] == stream)
		{
			result = i;
			break;
		}
	}
	return result;
}

bool nuiModule::isStarted()
{
	return this->is_started;
}

bool nuiModule::isOscillatorMode()
{
	return oscillator_mode;
}

bool nuiModule::isSyncedInput()
{
	return is_synced_input;
}

//==============================================================================
// nuiModuleDescriptor methods
//==============================================================================

void nuiModuleDescriptor::addChildModuleDescriptor(nuiModuleDescriptor* moduleDescriptor)
{
	childrenModuleDescriptions.push_back(moduleDescriptor);
}

void nuiModuleDescriptor::removeChildModuleDescriptor(nuiModuleDescriptor* moduleDescriptor)
{
	if (moduleDescriptor != NULL)
	{
        std::vector<nuiModuleDescriptor*>::iterator iter;
		for ( iter = childrenModuleDescriptions.begin(); 
            iter != childrenModuleDescriptions.end(); iter++)
		{
			if (moduleDescriptor == *iter)
			{
				childrenModuleDescriptions.erase(iter);
				return;
			}
		}
	}
	delete moduleDescriptor;
}

nuiModuleDescriptor* nuiModuleDescriptor::getChildModuleDescriptor(int index)
{
	return childrenModuleDescriptions[index];
}

int nuiModuleDescriptor::getChildModulesCount()
{
	return childrenModuleDescriptions.size();
}

int nuiModuleDescriptor::getInputEndpointsCount()
{
	return inputDescriptions.size();
}

int nuiModuleDescriptor::getOutputEndpointsCount()
{
	return outputDescriptions.size();
}

void nuiModuleDescriptor::addInputEndpointDescriptor(nuiEndpointDescriptor* descriptor, int index)
{
	descriptor->setIndex(index);
	inputDescriptions.push_back(descriptor);
}

void nuiModuleDescriptor::addOutputEndpointDescriptor(nuiEndpointDescriptor* descriptor, int index)
{
	descriptor->setIndex(index);
	outputDescriptions.push_back(descriptor);
}

void nuiModuleDescriptor::removeInputEndpointDescriptor(nuiEndpointDescriptor* descriptor)
{
	int currentIndex = descriptor->getIndex();
    std::vector<nuiEndpointDescriptor*>::iterator iter;
	for (iter = inputDescriptions.begin(); iter != inputDescriptions.end(); iter++)
	{
		if ((*iter == descriptor))
		{
			inputDescriptions.erase(iter);
			break;
		}
	}

	for (iter = inputDescriptions.begin(); iter != inputDescriptions.end(); iter++)
	{
		if ((*iter)->getIndex() >= currentIndex)
			(*iter)->setIndex((*iter)->getIndex() - 1);
	}
}

void nuiModuleDescriptor::removeOutputEndpointDescriptor(nuiEndpointDescriptor* descriptor)
{	
	int currentIndex = descriptor->getIndex();
    std::vector<nuiEndpointDescriptor*>::iterator iter;
	for (iter = outputDescriptions.begin(); iter != outputDescriptions.end(); iter++)
	{
		if ((*iter == descriptor))
		{
			inputDescriptions.erase(iter);
			break;
		}
	}

	for (iter = outputDescriptions.begin(); iter != outputDescriptions.end(); iter++)
	{
		if ((*iter)->getIndex() >= currentIndex)
			(*iter)->setIndex((*iter)->getIndex() - 1);
	}
}

nuiEndpointDescriptor *nuiModuleDescriptor::getInputEndpointDescriptor(int index)
{
    std::vector<nuiEndpointDescriptor*>::iterator iter;
	for (iter = inputDescriptions.begin(); iter != inputDescriptions.end(); iter++)
	{
		if ((*iter)->getIndex() == index)
			return *iter;
	}
	return NULL;
}

nuiEndpointDescriptor *nuiModuleDescriptor::getOutputEndpointDescriptor(int index)
{
    std::vector<nuiEndpointDescriptor*>::iterator iter;
	for (iter = outputDescriptions.begin(); iter != outputDescriptions.end(); iter++)
	{
		if ((*iter)->getIndex() == index)
			return *iter;
	}
	return NULL;
}

void nuiModuleDescriptor::setName(std::string name)
{
	this->name = name;
}

void nuiModuleDescriptor::setDescription(std::string description)
{
	this->description = description;
}

void nuiModuleDescriptor::setAuthor(std::string author)
{
	this->author = author;
}

std::string nuiModuleDescriptor::getName()
{
	return name;
}

nuiProperty &nuiModuleDescriptor::property(std::string str)
{
	std::map<std::string, nuiProperty*>::iterator it;
	it = this->properties.find(str);
	if ( it == this->properties.end() ) 
	{
		this->properties[str] = new nuiProperty("", "?? auto created ??");
		return *(this->properties[str]);
	}
	return *it->second;
}

std::map<std::string, nuiProperty*> &nuiModuleDescriptor::getProperties()
{
	return this->properties;
}

void nuiModuleDescriptor::addDataStreamDescriptor(nuiDataStreamDescriptor* connection)
{
	connectionDescriptors.push_back(connection);
}

void nuiModuleDescriptor::removeDataStreamDescriptor(nuiDataStreamDescriptor* connection)
{
    std::vector<nuiDataStreamDescriptor*>::iterator iter;
	for (iter = connectionDescriptors.begin();
        iter != connectionDescriptors.end(); iter++)
	{
		if (connection == *iter)
		{
			connectionDescriptors.erase(iter);
			return;
		}
	}
}

int nuiModuleDescriptor::getDataStreamDescriptorCount()
{
	return connectionDescriptors.size();
}

nuiDataStreamDescriptor* nuiModuleDescriptor::getDataStreamDescriptor(int index)
{
	return connectionDescriptors[index];
}

void nuiModuleDescriptor::setInputEndpointsCount(int count)
{
	if (count == getInputEndpointsCount())
		return;
	if (count < getInputEndpointsCount())
	{
        std::vector<nuiEndpointDescriptor*>::iterator iter;
		for ( iter = inputDescriptions.end() - 1; iter != inputDescriptions.begin();)
		{
			if ((*iter)->getIndex() >= count)
				inputDescriptions.erase(iter);
			else
				iter++;
		}
	}
	else
	{
		for (int i = inputDescriptions.size(); i<count; i++)
		{
			nuiEndpointDescriptor* endpointDescription 
                = new nuiEndpointDescriptor("*");
			endpointDescription->setIndex(i);

			inputDescriptions.push_back(endpointDescription);
		}
	}
}

void nuiModuleDescriptor::setOutputEndpointsCount(int count)
{
	if (count == getOutputEndpointsCount())
		return;
	if (count < getOutputEndpointsCount())
	{
        std::vector<nuiEndpointDescriptor*>::iterator iter;
		for ( iter = outputDescriptions.end()-1; iter != outputDescriptions.begin();)
		{
			if ((*iter)->getIndex() >= count)
				outputDescriptions.erase(iter);
			else
				iter++;
		}
	}
	else
	{
		for (int i = outputDescriptions.size();i<count;i++)
		{
			nuiEndpointDescriptor* endpointDescription = new nuiEndpointDescriptor("*");
			endpointDescription->setIndex(i);
			outputDescriptions.push_back(endpointDescription);
		}
	}
}

std::string nuiModuleDescriptor::getDescription()
{
    return description;
}

std::string nuiModuleDescriptor::getAuthor()
{
    return author;
}