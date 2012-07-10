#include "nuiPipeline.h"
#include "nuiDebugLogger.h"
#include "nuiFactory.h"


//MODULE_DECLARE(Pipeline, "native", "Handle object list");



nuiPipelineModule::nuiPipelineModule() : nuiModule()
{
	is_synced_input = true;
	this->properties["oscillator_mode"] = new nuiProperty(false);
	this->properties["synced_input"] = new nuiProperty(true);
	is_synced_input = true;
	inputInternalEndpoints = NULL;
	outputInternalEndpoints = NULL;
}

nuiPipelineModule::~nuiPipelineModule()
{
	for (int i = 0; i<inputEndpointCount; i++)
	{
		delete outputInternalEndpoints[i];
	}
	if (outputInternalEndpoints!=NULL)
	{
		free(outputInternalEndpoints);
		outputInternalEndpoints = NULL;
	}
	for (int i = 0; i<outputEndpointCount; i++)
	{
		delete inputInternalEndpoints[i];
	}
	if (inputInternalEndpoints!=NULL)
	{
		free(inputInternalEndpoints);
		inputInternalEndpoints = NULL;
	}
}

void nuiPipelineModule::start()
{
	for (std::map<int,nuiModule*>::iterator iter = modules.begin();iter!=modules.end();iter++)
		iter->second->start();
	nuiModule::start();
}

void nuiPipelineModule::stop()
{
	for (std::map<int,nuiModule*>::iterator iter = modules.begin();iter!=modules.end();iter++)
		iter->second->stop();
	nuiModule::stop();
}

void nuiPipelineModule::update()
{
	//what if was caused not by adding new value- 
	for (int i=0;i<inputEndpointCount;i++)
		outputInternalEndpoints[i]->setData(inputEndpoints[i]->getData());
	//should trigger other???
}

void nuiPipelineModule::notifyDataReceived(nuiEndpoint *endpoint)
{
	bool isInternalInput = false;
	for (int i=0;i<outputEndpointCount;i++)
	{
		if (endpoint == inputInternalEndpoints[i])
		{
			isInternalInput = true;
			outputEndpoints[i]->setData(endpoint->getData());
		}
	}
	if (!isInternalInput)
		nuiModule::notifyDataReceived(endpoint);
}

void nuiPipelineModule::setOutputEndpoint(int n, nuiEndpoint *endpoint)
{
	nuiModule::setOutputEndpoint(n,endpoint);
	if ((n >=outputEndpointCount) || (n<0))
		return;
	mtx->lock();
	if ((inputInternalEndpoints!=NULL) && (inputInternalEndpoints[n] != NULL))
		delete inputInternalEndpoints[n];
	inputInternalEndpoints[n] = new nuiEndpoint();
	inputInternalEndpoints[n]->setTypeDescriptor(endpoint->getTypeDescriptor());
	inputInternalEndpoints[n]->setModuleHoster(this);
	mtx->unlock();
}

void nuiPipelineModule::setInputEndpoint(int n, nuiEndpoint *endpoint)
{
	nuiModule::setInputEndpoint(n,endpoint);
	if ((n >= inputEndpointCount) || (n<0))
		return;
	mtx->lock();
	if ((inputInternalEndpoints!=NULL) && (inputInternalEndpoints[n] != NULL))
		delete outputInternalEndpoints[n];
	outputInternalEndpoints[n] = new nuiEndpoint();
	outputInternalEndpoints[n]->setTypeDescriptor(endpoint->getTypeDescriptor());
	outputInternalEndpoints[n]->setModuleHoster(this);
	mtx->unlock();
}

void nuiPipelineModule::setOutputEndpointCount(int n)
{
	int oldCount = outputEndpointCount;
	nuiModule::setOutputEndpointCount(n);
	if (n == oldCount)
		return;
	mtx->lock();
	nuiEndpoint **newInternalInputEndpoints = (nuiEndpoint **)malloc(n * sizeof(nuiEndpoint*));
	memset((void*)newInternalInputEndpoints, 0x00, n * sizeof(nuiEndpoint*));
	if (n > oldCount)
	{
		if ((oldCount > 0) && (inputInternalEndpoints!=NULL))
			memcpy(newInternalInputEndpoints,inputInternalEndpoints, oldCount * sizeof(nuiEndpoint*));
		for (int i=oldCount;i<n;i++)
		{
			newInternalInputEndpoints[i] = new nuiEndpoint();
			newInternalInputEndpoints[i]->setModuleHoster(this);
			newInternalInputEndpoints[i]->setTypeDescriptor("*");
		}
	}
	else
	{
		if (inputInternalEndpoints!=NULL)
			memcpy(newInternalInputEndpoints,inputInternalEndpoints, n * sizeof(nuiEndpoint*));
		for (int i = n; i<oldCount; i++)
			delete inputInternalEndpoints[i];
	}
	if (inputInternalEndpoints!=NULL)
		free(inputInternalEndpoints);
	inputInternalEndpoints = newInternalInputEndpoints;
	mtx->unlock();
}

void nuiPipelineModule::setInputEndpointCount(int n)
{
	int oldCount = inputEndpointCount;
	nuiModule::setInputEndpointCount(n);
	if (n == oldCount)
		return;
	mtx->lock();
	nuiEndpoint **newInternalOutputEndpoints = (nuiEndpoint **)malloc(n * sizeof(nuiEndpoint*));
	memset((void*)newInternalOutputEndpoints, 0x00, n * sizeof(nuiEndpoint*));
	if (n > oldCount)
	{
		if ((oldCount > 0) && (outputInternalEndpoints!=NULL))
			memcpy(newInternalOutputEndpoints,outputInternalEndpoints, oldCount * sizeof(nuiEndpoint*));
		for (int i=oldCount;i<n;i++)
		{
			newInternalOutputEndpoints[i] = new nuiEndpoint();
			newInternalOutputEndpoints[i]->setModuleHoster(this);
			newInternalOutputEndpoints[i]->setTypeDescriptor("*");
		}
	}
	else
	{
		if (outputInternalEndpoints!=NULL)
			memcpy(newInternalOutputEndpoints,outputInternalEndpoints, n * sizeof(nuiEndpoint*));
		for (int i = n; i<oldCount; i++)
			delete outputInternalEndpoints[i];
	}
	if (outputInternalEndpoints!=NULL)
		free(outputInternalEndpoints);
	outputInternalEndpoints = newInternalOutputEndpoints;
	mtx->unlock();
}

void nuiPipelineModule::setName(std::string name)
{
	this->name = name;
}

void nuiPipelineModule::setAuthor(std::string author)
{
	this->author = author;
}

void nuiPipelineModule::setDescription(std::string description)
{
	this->description = description;
}

std::string nuiPipelineModule::getName()
{
	return name;
}

std::string nuiPipelineModule::getDescription()
{
	return description;
}

std::string nuiPipelineModule::getAuthor()
{
	return author;
}

void nuiPipelineModule::addChildModule(int index, nuiModule* child)
{
	modules[index] = child;
}
	
void nuiPipelineModule::removeChildModule(nuiModule* child)
{
	for (std::map<int,nuiModule*>::iterator iter = 	modules.begin();iter!=modules.end();iter++)
	{
		if (iter->second == child)
		{
			modules.erase(iter);
			return;
		}
	}
}

void nuiPipelineModule::removeChildModule(int index)
{
	std::map<int,nuiModule*>::iterator find = modules.find(index);
	if (find!=modules.end())
		modules.erase(find);
}

int nuiPipelineModule::getChildModuleCount()
{
	return modules.size();
}

nuiModule *nuiPipelineModule::getChildModuleAtIndex(int index)
{
	return modules[index];
}
