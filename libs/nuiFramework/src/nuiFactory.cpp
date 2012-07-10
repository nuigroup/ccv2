/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxFactory.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *** *******************************************************************/


#include "nuiFactory.h"
#include "nuiFileSystem.hpp"
#include "nuiPipeline.h"
#include "nuiFrameworkManager.h"

LOG_DECLARE("Factory");

static nuiFactory *instance = NULL;

nuiFactory::nuiFactory() 
{
}

nuiFactory::~nuiFactory() 
{
}

nuiFactory *nuiFactory::getInstance() 
{
	if ( instance == NULL )
		instance = new nuiFactory();
	return instance;
}

void nuiFactory::init()
{
}

void nuiFactory::cleanup() 
{
	delete instance;
	instance = NULL;
}

void nuiFactory::loadDynamicModules(char *pathLocation)
{
	//!add platform specific code for searching flies
	std::list<nuiFileSystem::nuiFile>* files = nuiFileSystem::nuiFilesystem::read_directory(pathLocation, "(.*\\.dll)");
	for (std::list<nuiFileSystem::nuiFile>::iterator iter = files->begin();iter!=files->end();iter++)
		nuiPluginManager::getInstance()->loadLibrary(std::string(iter->fullName));
	nuiPluginManager::getInstance()->loadPluginsFromLoadedLibraries();
	moduleDescriptors.clear();
	for (nuiRegisterPluginParamsMap::iterator iter = nuiPluginManager::getInstance()->registerPluginParamsMap.begin();iter!=nuiPluginManager::getInstance()->registerPluginParamsMap.end();iter++)
	{
		moduleDescriptors[iter->first] = iter->second.queryModuleDescriptorFunc();
	}
}

nuiModule *nuiFactory::create(const std::string &name)
{
	if (pipelineDescriptors.find(name) != pipelineDescriptors.end())
	{
		return createPipeline(pipelineDescriptors[name]);
	}
	if (moduleDescriptors.find(name) != moduleDescriptors.end())
	{
		nuiPluginFramework::nuiPluginEntity* module = NULL;
		if (nuiPluginManager::getInstance()->queryPluginObject((const nuiPluginFramework::nuiPluginEntity**)&module,name) == nuiPluginFrameworkOK)
			return (nuiModule*)module->getEntity();
		return NULL;
	}
	return NULL;
}

std::vector<std::string> *nuiFactory::listPipelineNames()
{
	std::vector<std::string>* l = new std::vector<std::string>();
	std::map<std::string, nuiModuleDescriptor*>::iterator it;
	for ( it = this->pipelineDescriptors.begin(); it != this->pipelineDescriptors.end(); it++ )
		l->push_back(it->first);
	return l;
}

std::vector<std::string> *nuiFactory::listModuleNames()
{
	std::vector<std::string>* l = new std::vector<std::string>();
	nuiPluginFramework::nuiRegisterPluginParamsMap* plugins = nuiPluginManager::getInstance()->getRigisteredPlugins();
	nuiPluginFramework::nuiRegisterPluginParamsMap::iterator it;
	for ( it = plugins->begin(); it != plugins->end(); it++ )
		l->push_back(it->first);
	return l;
}

std::vector<std::string>* nuiFactory::listAllNames()
{
	std::vector<std::string>* l;
	std::vector<std::string>* staticData = listPipelineNames();
	std::vector<std::string>* dynamicData = listModuleNames();
	for (std::vector<std::string>::iterator iter = staticData->begin();iter!=staticData->end();iter++)
	{
		l->push_back(*iter);
	}
	for (std::vector<std::string>::iterator iter = dynamicData->begin();iter!=dynamicData->end();iter++)
	{
		l->push_back(*iter);
	}
	delete dynamicData;
	delete staticData;
	return l;
}

nuiModuleDescriptor* nuiFactory::getDescriptor(const std::string &name)
{
	if (pipelineDescriptors.find(name)!=pipelineDescriptors.end())
		return pipelineDescriptors[name];
	if (moduleDescriptors.find(name)!=moduleDescriptors.end())
		return moduleDescriptors[name];
    return NULL;
}

nuiModuleDescriptor* nuiFactory::getDescriptor(const std::string &pipelineName,int id)
{
	if (pipelineDescriptors.find(pipelineName)==pipelineDescriptors.end())
		return NULL;
	nuiModuleDescriptor* pipelineDescriptor = pipelineDescriptors[pipelineName];
	nuiModuleDescriptor* tempChildDescriptor = pipelineDescriptor->getChildModuleDescriptor(id);
	if (tempChildDescriptor == NULL)
		return NULL;
	nuiModuleDescriptor* childDescriptor = getDescriptor(tempChildDescriptor->getName());
	if (childDescriptor == NULL)
		return NULL;
	childDescriptor->property("id") = new nuiProperty(id);
	for (std::map<std::string, nuiProperty*>::iterator iter = tempChildDescriptor->getProperties().begin();iter!= tempChildDescriptor->getProperties().end();iter++)
	{
		childDescriptor->property(iter->first) = iter->second;
	}
}

nuiModule *nuiFactory::createPipeline(nuiModuleDescriptor* descriptor)
{
	nuiPipelineModule* pipeline = new nuiPipelineModule();
	pipeline->setName(descriptor->getName());
	pipeline->setDescription(descriptor->getDescription());
	pipeline->setAuthor(descriptor->getAuthor());
	pipeline->property("id") = new nuiProperty(nuiUtils::getRandomNumber());
	for (int i=0;i<descriptor->getChildModulesCount();i++)
	{
		nuiModuleDescriptor* childDescriptor = descriptor->getChildModuleDescriptor(i);
		if (childDescriptor != NULL)
		{
			nuiModule* childModule = create(childDescriptor->getName());
			if (childModule == NULL)
				continue;
			loadSettings(childModule, childDescriptor);
			pipeline->addChildModule(childDescriptor->property("id").asInteger(),childModule);
		}
	} 
	//endpoints
	pipeline->setInputEndpointCount(descriptor->getInputEndpointsCount());
	for (int i=0;i<descriptor->getInputEndpointsCount();i++)
	{
		nuiEndpoint* newEndpoint = new nuiEndpoint();
		nuiEndpointDescriptor* endpointDescriptor = descriptor->getInputEndpointDescriptor(i);
		newEndpoint->setTypeDescriptor(endpointDescriptor->getDescriptor());
		newEndpoint->setModuleHoster(pipeline);
		pipeline->setInputEndpoint(endpointDescriptor->getIndex(),newEndpoint);
	}
	pipeline->setOutputEndpointCount(descriptor->getOutputEndpointsCount());
	for (int i=0;i<descriptor->getOutputEndpointsCount();i++)
	{
		nuiEndpoint* newEndpoint = new nuiEndpoint();
		nuiEndpointDescriptor* endpointDescriptor = descriptor->getOutputEndpointDescriptor(i);
		newEndpoint->setTypeDescriptor(endpointDescriptor->getDescriptor());
		newEndpoint->setModuleHoster(pipeline);
		pipeline->setOutputEndpoint(endpointDescriptor->getIndex(),newEndpoint);
	}
	printf("Processing pipeline %s connecitons:\n",descriptor->getName().c_str());
	//connections
	for (int i=descriptor->getDataStreamDescriptorCount() - 1;i >= 0;i--)
	{
		nuiDataStreamDescriptor* dataStreamDescriptor = descriptor->getDataStreamDescriptor(i);
		printf("Source module ID is %i, Destination module ID is %i, Source port is %i, Destination port is %i\n",dataStreamDescriptor->sourceModuleID,dataStreamDescriptor->destinationModuleID, dataStreamDescriptor->sourcePort, dataStreamDescriptor->destinationPort);
		nuiModuleDescriptor* sourceModuleDescriptor = NULL;
		nuiModuleDescriptor* destinationModuleDescriptor = NULL;
		if (dataStreamDescriptor->sourceModuleID == PIPELINE_ID)
			sourceModuleDescriptor = descriptor;
		if (dataStreamDescriptor->destinationModuleID == PIPELINE_ID)
			destinationModuleDescriptor = descriptor;
		for (int j=0;j<descriptor->getChildModulesCount();j++)
		{
			if (descriptor->getChildModuleDescriptor(j)->property("id").asInteger() == dataStreamDescriptor->sourceModuleID)
				sourceModuleDescriptor = descriptor->getChildModuleDescriptor(j);
			if (descriptor->getChildModuleDescriptor(j)->property("id").asInteger() == dataStreamDescriptor->destinationModuleID)
				destinationModuleDescriptor = descriptor->getChildModuleDescriptor(j);
		}
		if ((destinationModuleDescriptor == NULL) || (sourceModuleDescriptor == NULL))
		{
			descriptor->removeDataStreamDescriptor(descriptor->getDataStreamDescriptor(i));
			continue;
		}

		nuiModule* sourceModule = sourceModuleDescriptor == descriptor ? pipeline :  pipeline->getChildModuleAtIndex(dataStreamDescriptor->sourceModuleID);
		nuiModule* destinationModule = destinationModuleDescriptor == descriptor ? pipeline : pipeline->getChildModuleAtIndex(dataStreamDescriptor->destinationModuleID);

		nuiEndpoint* sourceEndpoint = NULL;
		nuiEndpoint* destinationEndpoint = NULL;
		if (sourceModule != pipeline) 
		{
			sourceEndpoint = sourceModule->getOutputEndpoint(dataStreamDescriptor->sourcePort);
		}
		else
		{
			sourceEndpoint = pipeline->outputInternalEndpoints[dataStreamDescriptor->sourcePort];
		}

		if (destinationModule != pipeline)
		{
			destinationEndpoint = destinationModule->getInputEndpoint(dataStreamDescriptor->destinationPort);
		}
		else
		{
			destinationEndpoint = pipeline->inputInternalEndpoints[dataStreamDescriptor->destinationPort];
		}

		printf("Source module %s is%s pipeline\nDestination module %s is%s pipeline\n", sourceModule->getName().c_str(),(sourceModule != pipeline) ? " not" : "",destinationModule->getName().c_str(),(destinationModule != pipeline) ? " not" : "");
		if ((sourceEndpoint!=NULL) && (destinationEndpoint!=NULL))
		{
			printf("Source Endpoint has %s type\nDestination Endpoint has %s type\n",sourceEndpoint->getTypeDescriptor().c_str(),destinationEndpoint->getTypeDescriptor().c_str());
			if (destinationModule == pipeline)
				pipeline->outputEndpoints[dataStreamDescriptor->destinationPort]->setTypeDescriptor(sourceEndpoint->getTypeDescriptor());
			if (sourceModule == pipeline)
				pipeline->inputEndpoints[dataStreamDescriptor->sourcePort]->setTypeDescriptor(destinationEndpoint->getTypeDescriptor());
			printf("Source Endpoint has %s type\nDestination Endpoint has %s type\n\n",sourceEndpoint->getTypeDescriptor().c_str(),destinationEndpoint->getTypeDescriptor().c_str());

			nuiDataStream* dataStream = sourceEndpoint->addConnection(destinationEndpoint);
	//		printf("!");
			if (dataStream!=NULL)
			{
		//	printf("*");

				dataStream->setAsyncMode(dataStreamDescriptor->asyncMode);
				dataStream->setBufferedMode(dataStreamDescriptor->buffered);
				dataStream->setBufferSize(dataStreamDescriptor->bufferSize);
				dataStream->setDeepCopy(dataStreamDescriptor->deepCopy);
				dataStream->setIsOverflow(dataStreamDescriptor->overflow);
				dataStream->setLastPacketPriority(dataStreamDescriptor->lastPacket);
			}
		}
		else
		{
			descriptor->removeDataStreamDescriptor(descriptor->getDataStreamDescriptor(i));
			continue;
		}
	}
	printf("---------------\n");

	loadSettings(pipeline, descriptor);
	pipelineDescriptors[descriptor->getName()] = descriptor;
	return pipeline;
}

void nuiFactory::loadSettings(nuiModule* module, nuiModuleDescriptor* descriptor)
{
	for (std::map<std::string, nuiProperty*>::iterator iter = descriptor->getProperties().begin();iter!= descriptor->getProperties().end();iter++)
	{
		module->property(iter->first) = iter->second;
	}
}

