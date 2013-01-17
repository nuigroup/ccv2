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
#include "nuiPipeline.h"
#include "nuiFrameworkManager.h"

LOG_DECLARE("Factory");

nuiFactory::nuiFactory() 
{

};

nuiFactory::nuiFactory(const nuiFactory&)
{

};

nuiFactory& nuiFactory::getInstance() 
{
  static nuiFactory instance;
  return instance;
};

//! TODO : check this and the following method
std::vector<std::pair<GUID, std::string>>& nuiFactory::listPipelines()
{
  //   std::vector<std::string>* list = new std::vector<std::string>();
  //   std::map<std::string, nuiModuleDescriptor*>::iterator it;
  //   for ( it = this->pipelineDescriptors.begin(); it != this->pipelineDescriptors.end(); it++ )
  //     list->push_back(it->first);
  //   return list;
  return *(new std::vector<std::pair<GUID, std::string>>());
};

//! TODO : check this. Seems that one of them is collecting incorrect information
std::vector<std::pair<GUID, std::string>>& nuiFactory::listModules()
{
  //  std::vector<std::string>* list = new std::vector<std::string>();
  //   nuiRegisterPluginParamsMap* plugins = nuiPluginManager::getInstance()->getRegisteredPlugins();
  //   nuiRegisterPluginParamsMap::iterator it;
  //   for ( it = plugins->begin(); it != plugins->end(); it++ )
  //     list->push_back(it->first);
  //   return list;
  return *(new std::vector<std::pair<GUID, std::string>>());
};

nuiModule* nuiFactory::createModule(nuiModuleDescriptor* descriptor)
{
//   nuiPluginManager pm = nuiPluginManager::getInstance();
//   if (pipelineDescriptors.find(name) != pipelineDescriptors.end())
//   {
//     return createPipeline(pipelineDescriptors[name]);
//   }
//   if (moduleDescriptors.find(name) != moduleDescriptors.end())
//   {
//     nuiPluginEntity* module = NULL;
//     if (pm.queryPluginObject((const nuiPluginEntity**)&module,name) == nuiPluginFrameworkOK)
//       return (nuiModule*)module->getEntity();
//     return NULL;
//   }
//   return NULL;
  return NULL;
};

nuiModuleDescriptor* nuiFactory::getDescriptor(const GUID& guid)
{
//   if (pipelineDescriptors.find(name) != pipelineDescriptors.end())
//     return pipelineDescriptors[name];
//   if (moduleDescriptors.find(name) != moduleDescriptors.end())
//     return moduleDescriptors[name];
  return new nuiModuleDescriptor();
};

nuiModule *nuiFactory::createPipeline(nuiModuleDescriptor* descriptor)
{
//   nuiPipelineModule* pipeline = new nuiPipelineModule();
// 
//   pipeline->setName(descriptor->getName());
//   pipeline->setDescription(descriptor->getDescription());
//   pipeline->setAuthor(descriptor->getAuthor());
// 
//   //! TODO : consider something more uniq for id
//   nuiProperty *randId = new nuiProperty(nuiUtils::getRandomNumber());
//   pipeline->property("id") = *randId;
// 
//   // add modules to pipeline
//   for (int i=0; i < descriptor->getChildModulesCount(); i++)
//   {
//     nuiModuleDescriptor* childDescriptor = descriptor->getChildModuleDescriptor(i);
//     if (childDescriptor != NULL)
//     {
//       nuiModule* childModule = create(childDescriptor->getName());
//       if (childModule == NULL)
//         continue;
//       loadSettings(childModule, childDescriptor);
//       pipeline->addChildModule(childDescriptor->property("id").asInteger(), childModule);
//     }
//   } 
// 
//   //pipeline endpoints
//   pipeline->setInputEndpointCount(descriptor->getInputEndpointsCount());
//   for (int i=0; i < descriptor->getInputEndpointsCount(); i++)
//   {
//     nuiEndpoint* newEndpoint = new nuiEndpoint(pipeline);
//     nuiEndpointDescriptor* endpointDescriptor = descriptor->getInputEndpointDescriptor(i);
//     newEndpoint->setTypeDescriptor(endpointDescriptor->getDescriptor());
//     pipeline->setInputEndpoint(endpointDescriptor->getIndex(), newEndpoint);
//   }
// 
//   pipeline->setOutputEndpointCount(descriptor->getOutputEndpointsCount());
//   for (int i=0;i<descriptor->getOutputEndpointsCount();i++)
//   {
//     nuiEndpoint* newEndpoint = new nuiEndpoint(pipeline);
//     nuiEndpointDescriptor* endpointDescriptor = descriptor->getOutputEndpointDescriptor(i);
//     newEndpoint->setTypeDescriptor(endpointDescriptor->getDescriptor());
//     pipeline->setOutputEndpoint(endpointDescriptor->getIndex(),newEndpoint);
//   }
//   printf("Processing pipeline %s connecitons:\n",descriptor->getName().c_str());
// 
//   // connections between modules
//   for (int i=descriptor->getDataStreamDescriptorCount() - 1; i >= 0; i--)
//   {
//     nuiDataStreamDescriptor* dataStreamDescriptor = descriptor->getDataStreamDescriptor(i);
//     printf("Source module ID is %i, Destination module ID is %i, Source port is %i, Destination port is %i\n",dataStreamDescriptor->sourceModuleID,dataStreamDescriptor->destinationModuleID, dataStreamDescriptor->sourcePort, dataStreamDescriptor->destinationPort);
//     nuiModuleDescriptor* sourceModuleDescriptor = NULL;
//     nuiModuleDescriptor* destinationModuleDescriptor = NULL;
//     // check whether connection uses pipeline input/output endpoints
//     if (dataStreamDescriptor->sourceModuleID == PIPELINE_ID)
//       sourceModuleDescriptor = descriptor;
//     if (dataStreamDescriptor->destinationModuleID == PIPELINE_ID)
//       destinationModuleDescriptor = descriptor;
// 
//     for (int j=0; j<descriptor->getChildModulesCount(); j++)
//     {
//       if (descriptor->getChildModuleDescriptor(j)->property("id").asInteger() == dataStreamDescriptor->sourceModuleID)
//         sourceModuleDescriptor = descriptor->getChildModuleDescriptor(j);
//       if (descriptor->getChildModuleDescriptor(j)->property("id").asInteger() == dataStreamDescriptor->destinationModuleID)
//         destinationModuleDescriptor = descriptor->getChildModuleDescriptor(j);
//     }
// 
//     // if endpoint_module descriptors not found
//     if ((destinationModuleDescriptor == NULL) || (sourceModuleDescriptor == NULL))
//     {
//       descriptor->removeDataStreamDescriptor(descriptor->getDataStreamDescriptor(i));
//       continue;
//     }
// 
//     nuiModule* sourceModule = (sourceModuleDescriptor == descriptor) ? 
// pipeline :  pipeline->getChildModuleAtIndex(dataStreamDescriptor->sourceModuleID);
//     nuiModule* destinationModule = (destinationModuleDescriptor == descriptor) ? 
// pipeline : pipeline->getChildModuleAtIndex(dataStreamDescriptor->destinationModuleID);
// 
//     nuiEndpoint* sourceEndpoint = NULL;
//     nuiEndpoint* destinationEndpoint = NULL;
// 
//     if (sourceModule != pipeline) 
//     {
//       sourceEndpoint = sourceModule->getOutputEndpoint(dataStreamDescriptor->sourcePort);
//     }
//     else
//     {
//       sourceEndpoint = pipeline->outputInternalEndpoints[dataStreamDescriptor->sourcePort];
//     }
// 
//     if (destinationModule != pipeline)
//     {
//       destinationEndpoint = destinationModule->getInputEndpoint(dataStreamDescriptor->destinationPort);
//     }
//     else
//     {
//       destinationEndpoint = pipeline->inputInternalEndpoints[dataStreamDescriptor->destinationPort];
//     }
// 
//     printf("Source module %s is%s pipeline\nDestination module %s is%s pipeline\n", 
//       sourceModule->getName().c_str(), (sourceModule != pipeline) ? " not" : "",
//       destinationModule->getName().c_str(), (destinationModule != pipeline) ? " not" : "");
//     if ((sourceEndpoint != NULL) && (destinationEndpoint != NULL))
//     {
//       printf("Source Endpoint has %s type\nDestination Endpoint has %s type\n", 
//         sourceEndpoint->getTypeDescriptor().c_str(), destinationEndpoint->getTypeDescriptor().c_str());
// 
//       //! TODO : wtf
//       if (destinationModule == pipeline)
//         pipeline->outputEndpoints[dataStreamDescriptor->destinationPort]->setTypeDescriptor(sourceEndpoint->getTypeDescriptor());
//       if (sourceModule == pipeline)
//         pipeline->inputEndpoints[dataStreamDescriptor->sourcePort]->setTypeDescriptor(destinationEndpoint->getTypeDescriptor());
// 
//       printf("Source Endpoint has %s type\nDestination Endpoint has %s type\n\n",
//         sourceEndpoint->getTypeDescriptor().c_str(),destinationEndpoint->getTypeDescriptor().c_str());
// 
//       nuiDataStream* dataStream = sourceEndpoint->addConnection(destinationEndpoint);
//       if (dataStream != NULL)
//       {
//         dataStream->setAsyncMode(dataStreamDescriptor->asyncMode);
//         dataStream->setBufferedMode(dataStreamDescriptor->buffered);
//         dataStream->setBufferSize(dataStreamDescriptor->bufferSize);
//         dataStream->setDeepCopy(dataStreamDescriptor->deepCopy);
//         dataStream->setIsOverflow(dataStreamDescriptor->overflow);
//         dataStream->setLastPacketPriority(dataStreamDescriptor->lastPacket);
//       }
//     }
//     else
//     {
//       descriptor->removeDataStreamDescriptor(descriptor->getDataStreamDescriptor(i));
//       continue;
//     }
//   }
//   printf("---------------\n");
// 
//   loadSettings(pipeline, descriptor);
//   pipelineDescriptors[descriptor->getName()] = descriptor;
//   return pipeline;
  return NULL;
};

void nuiFactory::applyDescriptor(nuiModule* module, nuiModuleDescriptor* descriptor)
{
//   for (std::map<std::string, nuiProperty*>::iterator iter = descriptor->getProperties().begin();iter!= descriptor->getProperties().end();iter++)
//   {
//     module->property(iter->first).set(iter->second->asString());
//   }
};

nuiModule* nuiFactory::create( const GUID& guid )
{
  return NULL;
};

