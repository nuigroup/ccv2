#include "nuiFrameworkManager.h"
#include "nuiModule.h"
#include "nuiPipeline.h"
#include "nuiDataStream.h"
#include "nuiFactory.h"
#include "nuiUtils.h"
#include "nuiPluginManager.h"

LOG_DECLARE("FrameworkManager");

nuiFrameworkManager::nuiFrameworkManager()
{
  rootPipeline = NULL;
  currentModule = NULL;
  dataObjectTree = NULL;
};

nuiFrameworkManager::~nuiFrameworkManager()
{
  workflowStop();
};

nuiFrameworkManager& nuiFrameworkManager::getInstance()
{ 
  static nuiFrameworkManager instance;
  return instance;
};

nuiFrameworkManagerErrorCode::err nuiFrameworkManager::init()
{
  //this->rootPipeline = (nuiPipelineModule*)(nuiFactory::getInstance().create("root"));
  if(rootPipeline != NULL) {
    nuiTreeNode<int, nuiModule*> *temp = new nuiTreeNode<int, nuiModule*>(rootPipeline->property("id").asInteger(), rootPipeline);
    for (int i=0; i<rootPipeline->getChildModuleCount(); i++) {
      temp->addChildNode(new nuiTreeNode<int, nuiModule*>(rootPipeline->getChildModuleAtIndex(i)->property("id").asInteger(), rootPipeline->getChildModuleAtIndex(i)));
    }
    dataObjectTree = new nuiTree<int, nuiModule*>(temp);
  }
  return (rootPipeline != NULL) ? 
    nuiFrameworkManagerErrorCode::Success : nuiFrameworkManagerErrorCode::InitFailed;
};

std::vector<std::string>& nuiFrameworkManager::listModules()
{
  return nuiFactory::getInstance().listModules();
};

std::vector<std::string>& nuiFrameworkManager::listPipelines()
{
  return nuiFactory::getInstance().listPipelines();
};

nuiFrameworkManagerErrorCode::err nuiFrameworkManager::workflowStart()
{
  nuiPipelineModule* current = getCurrent();
  if(!current->isStarted())
    current->start();

  return nuiFrameworkManagerErrorCode::Success;
};

nuiFrameworkManagerErrorCode::err nuiFrameworkManager::workflowStart(int moduleIndex)
{
  nuiPipelineModule* current = getCurrent();
  nuiModule* selected = current->getChildModuleAtIndex(moduleIndex);
  if(selected == NULL)
    return nuiFrameworkManagerErrorCode::NonexistentModule;

  if(!selected->isStarted())
    selected->start();

  return nuiFrameworkManagerErrorCode::Success;
};

nuiFrameworkManagerErrorCode::err nuiFrameworkManager::workflowStop()
{
  nuiPipelineModule* current = getCurrent();
  if(!(current == NULL) && current->isStarted())
    current->stop();

  return nuiFrameworkManagerErrorCode::Success;
};

nuiFrameworkManagerErrorCode::err nuiFrameworkManager::workflowStop(int moduleIndex)
{
  nuiPipelineModule* current = getCurrent();
  nuiModule* selected = current->getChildModuleAtIndex(moduleIndex);
  if(selected == NULL)
    return nuiFrameworkManagerErrorCode::NonexistentModule;

  if(selected->isStarted())
    selected->start();

  return nuiFrameworkManagerErrorCode::Success;
};

nuiModuleDescriptor* nuiFrameworkManager::createNewPipelineTemplate(const std::string& pipelineName)
{
  std::string newPipelineName = pipelineName;
  while (nuiPluginManager::getInstance().getDescriptor(newPipelineName) != NULL)
    newPipelineName += '_';
  // choose new name if there's a pipeline with the same name

  nuiModuleDescriptor* newModuleDescriptor = new nuiModuleDescriptor();
  newModuleDescriptor->setName(newPipelineName);
  newModuleDescriptor->setAuthor("native");
  newModuleDescriptor->setDescription("New Pipeline Module");
  // create some new descriptor for pipeline

  nuiModule* newPipeline = nuiFactory::getInstance().create(newPipelineName);
  // create new pipeline with provided descriptor. in past - if there was no 
  // such desriptor in disctionary - we saved it. 

  //! \todo review here later
  if(newPipeline != NULL) {
    // do the treeness here
  }

  return newModuleDescriptor;
};

nuiModuleDescriptor *nuiFrameworkManager::createModule(std::string &pipelineName, std::string &moduleName)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  nuiModuleDescriptor* dafaultChildDescriptor = pm.getDescriptor(moduleName);
  if ((pipelineDescriptor == NULL) || (dafaultChildDescriptor == NULL))
    return NULL;
  // obtains pipeline and module descriptor

  nuiModuleDescriptor* childDescriptor = new nuiModuleDescriptor();
  childDescriptor->setAuthor(dafaultChildDescriptor->getAuthor());
  childDescriptor->setDescription(dafaultChildDescriptor->getDescription());
  childDescriptor->setName(dafaultChildDescriptor->getName());
  // copy default module descriptor

  std::map<std::string, nuiProperty*>& props  = dafaultChildDescriptor->getProperties();
  std::map<std::string, nuiProperty*>::iterator iter;
  for ( iter = props.begin();iter != props.end(); iter++)
  {
    childDescriptor->property(iter->first) = iter->second;
  }
  childDescriptor->property("id") = new nuiProperty(nuiUtils::getRandomNumber(), "identifier");
  // duplicate module descriptor, add random named as identifier

  pipelineDescriptor->addChildModuleDescriptor(childDescriptor);
  //add new descriptor as pipeline child
  
  for (nuiTree<int,nuiModule*>::iterator moduleObject = dataObjectTree->begin(); 
    moduleObject != dataObjectTree->end(); moduleObject++) 
    // iterate over all modules in Tree trying to find pipelines with specified names
  {
    nuiModule* currentModule = (*moduleObject)->getValue();
    if (currentModule == NULL)
      continue;
    else if (currentModule->getName() == pipelineName) // if this is required pipeline
    {
      nuiPipelineModule* pipelineObject = (nuiPipelineModule*)(currentModule);
      if (pipelineObject == NULL)
        continue;
      nuiModule* moduleObjectChild = nuiFactory::getInstance().create(moduleName);
      // create module instance

      nuiFactory::getInstance().applyDescriptorProps(moduleObjectChild, childDescriptor);

      pipelineObject->addChildModule(
        moduleObjectChild->property("id").asInteger(), moduleObjectChild);
      // add created instance as child for chosen pipeline

      (*moduleObject)->addChildNode(
        new nuiTreeNode<int,nuiModule*>(
          moduleObjectChild->property("id").asInteger(), moduleObjectChild));
      // for newly created instance add node to navigation tree
    }
  }

  return childDescriptor;
}

nuiDataStreamDescriptor *nuiFrameworkManager::createConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  nuiDataStreamDescriptor* existingDescriptor = getConnection(pipelineName, sourceModuleID, destinationModuleID, sourcePort, destinationPort);
  if (existingDescriptor!=NULL)
    return existingDescriptor;

  nuiModuleDescriptor* sourceModuleDescriptor = NULL;
  nuiModuleDescriptor* destinationModuleDescriptor = NULL;

  for (int i=0;i<pipelineDescriptor->getChildModulesCount();i++)
  {
    nuiModuleDescriptor* currentModuleDescriptor = pipelineDescriptor->getChildModuleDescriptor(i);
    if (currentModuleDescriptor->property("id").asInteger() == sourceModuleID)
      sourceModuleDescriptor = currentModuleDescriptor;
    if (currentModuleDescriptor->property("id").asInteger() == destinationModuleID)
      destinationModuleDescriptor = currentModuleDescriptor;
  }
  //!connection between module and pipeline
  if (sourceModuleID == PIPELINE_ID)
    sourceModuleDescriptor = pipelineDescriptor;
  if (destinationModuleID == PIPELINE_ID)
    destinationModuleDescriptor = pipelineDescriptor;
  //!we can't connect not existed modules and can't make self connection
  if (((sourceModuleDescriptor == NULL) || (destinationModuleDescriptor == NULL)) && (sourceModuleDescriptor!=destinationModuleDescriptor))
    return NULL;

  //we need to get information about structure of connected modules - so let's ask nuiFactory
  sourceModuleDescriptor = pm.getDescriptor(sourceModuleDescriptor->getName());
  destinationModuleDescriptor = pm.getDescriptor(destinationModuleDescriptor->getName());

  if ((sourceModuleDescriptor->getOutputEndpointsCount() <= sourcePort) || (destinationModuleDescriptor->getInputEndpointsCount() <= destinationPort))
    return NULL;
  nuiEndpointDescriptor* sourceEndpointDescriptor = sourceModuleDescriptor->getOutputEndpointDescriptor(sourcePort);
  nuiEndpointDescriptor* destinationEndpointDescriptor = destinationModuleDescriptor->getInputEndpointDescriptor(destinationPort);

  if ((sourceEndpointDescriptor->getDescriptor() == "*") || (destinationEndpointDescriptor->getDescriptor() == "*") || (destinationEndpointDescriptor->getDescriptor() != sourceEndpointDescriptor->getDescriptor()))
    return NULL;

  nuiDataStreamDescriptor* newDataStreamDescriptor = new nuiDataStreamDescriptor();
  newDataStreamDescriptor->asyncMode = false;
  newDataStreamDescriptor->buffered = false;
  newDataStreamDescriptor->bufferSize = false;
  newDataStreamDescriptor->overflow = false;
  newDataStreamDescriptor->lastPacket = true;
  newDataStreamDescriptor->sourceModuleID = sourceModuleID;
  newDataStreamDescriptor->destinationModuleID = destinationModuleID;
  newDataStreamDescriptor->sourcePort = sourcePort;
  newDataStreamDescriptor->destinationPort = destinationPort;
  pipelineDescriptor->addDataStreamDescriptor(newDataStreamDescriptor);

  std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
  if (rootPipeline!=NULL)
    stack->push_back(rootPipeline);

  while (!stack->empty())
  {
    nuiPipelineModule* currentPipeline = stack->back();
    stack->pop_back();
    if (currentPipeline->getName() == pipelineName)
    {		
      nuiModule* sourceModule = sourceModuleDescriptor == pipelineDescriptor ? currentPipeline : currentPipeline->getChildModuleAtIndex(newDataStreamDescriptor->sourceModuleID);
      nuiModule* destinationModule = destinationModuleDescriptor == pipelineDescriptor ? currentPipeline : currentPipeline->getChildModuleAtIndex(newDataStreamDescriptor->destinationModuleID);
      //???
      nuiEndpoint* sourceEndpoint = (sourceModule != currentPipeline) ? sourceModule->getOutputEndpoint(newDataStreamDescriptor->sourcePort) : currentPipeline->outputInternalEndpoints[newDataStreamDescriptor->sourcePort];
      nuiEndpoint* destinationEndpoint = (destinationModule != currentPipeline) ? destinationModule->getInputEndpoint(newDataStreamDescriptor->destinationPort) : currentPipeline->inputInternalEndpoints[newDataStreamDescriptor->destinationPort];
      if ((sourceEndpoint!=NULL) && (destinationEndpoint!=NULL))
      {
        nuiDataStream* dataStream = sourceEndpoint->addConnection(destinationEndpoint);
        if (dataStream!=NULL)
        {
          dataStream->setAsyncMode(newDataStreamDescriptor->asyncMode);
          dataStream->setBufferedMode(newDataStreamDescriptor->buffered);
          dataStream->setBufferSize(newDataStreamDescriptor->bufferSize);
          dataStream->setDeepCopy(newDataStreamDescriptor->deepCopy);
          dataStream->setIsOverflow(newDataStreamDescriptor->overflow);
          dataStream->setLastPacketPriority(newDataStreamDescriptor->lastPacket);
        }			
      }
    }
    else
    {
      for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
      {
        nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
        if (childDescriptor->getChildModulesCount() > 0)
        {
          stack->push_back((nuiPipelineModule*)iter->second);
        }
      }
    }
  }
  delete stack;

  return newDataStreamDescriptor;
}

int nuiFrameworkManager::setInputEndpointCount(std::string &pipelineName,int count)
{
  nuiFactory& factory = nuiFactory::getInstance();
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return 0;
  // find pipeline descriptor
  
  if (count == pipelineDescriptor->getInputEndpointsCount())
    return count;
  // do nothing if the same endpoint count

  if (count < pipelineDescriptor->getInputEndpointsCount()) // if less - remove
  {
    std::list<nuiPipelineModule*> stack;
    if (rootPipeline != NULL)
      stack.push_back(rootPipeline);
    while (!stack.empty()) // BFS over the pipelines?
    {
      nuiPipelineModule* currentPipeline = stack.back();
      nuiModuleDescriptor* currentPipelineDescriptor = pm.getDescriptor(currentPipeline->getName());
      stack.pop_back();
      if (currentPipeline->getName() == pipelineName)
      {		
        currentPipeline->setInputEndpointCount(count);
      }
      else
      {
        for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin(); 
          iter != currentPipeline->modules.end();iter++) // iterate over pipeline modules
        {
          nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
          if (childDescriptor->getChildModulesCount() > 0)
          {
            stack.push_back((nuiPipelineModule*)iter->second);
            if (childDescriptor->getName() == pipelineName)
            {
              //remove connections outside currentPipeline
              int pipelineModuleID = childDescriptor->property("id").asInteger();
              for (int i=0; i<currentPipelineDescriptor->getDataStreamDescriptorCount(); i++)
              {
                nuiDataStreamDescriptor* connectionDescriptor = currentPipelineDescriptor->getDataStreamDescriptor(i);
                if ((connectionDescriptor->destinationModuleID == pipelineModuleID) && (connectionDescriptor->destinationPort >= count))
                {
                  nuiModule* sourceModule = currentPipeline->modules[connectionDescriptor->sourceModuleID];
                  if (sourceModule == NULL)
                    continue;
                  nuiEndpoint* sourceEndpoint = sourceModule->getOutputEndpointCount() > connectionDescriptor->sourcePort ? sourceModule->getOutputEndpoint(connectionDescriptor->sourcePort) : NULL;
                  if (sourceEndpoint== NULL)
                    continue;
                  nuiModule* destinationModule = currentPipeline;
                  nuiEndpoint* destinationEndpoint = destinationModule->getInputEndpoint(connectionDescriptor->destinationPort);
                  if (destinationEndpoint == NULL)
                    continue;
                  sourceEndpoint->removeConnection(destinationEndpoint);
                }
              }
            }
          }
        }
      }
    }

    std::vector<nuiModuleDescriptor*>::iterator pipe;
    std::vector<nuiModuleDescriptor*>& pipes = pm.getPipelineDescriptors();
    for (pipe = pipes.begin(); pipe != pipes.end() ; pipe++) //iterate over all pipes
    {
      nuiModuleDescriptor *currentPipe = *pipe;
      for (int i = currentPipe->getChildModulesCount() - 1 ; i>=0 ; i--)
      {
        if (currentPipe->getChildModuleDescriptor(i)->getName() == pipelineName)
        {
          int id = currentPipe->getChildModuleDescriptor(i)->property("id").asInteger();
          for (int j = currentPipe->getDataStreamDescriptorCount() - 1 ; i>=0 ; i--)
          {
            if (currentPipe->getDataStreamDescriptor(j)->destinationModuleID == id) 
            {
              if (currentPipe->getDataStreamDescriptor(j)->destinationPort >= count)
              {
                currentPipe->removeDataStreamDescriptor(currentPipe->getDataStreamDescriptor(j));
                continue;
              }
            }
          }
        }
      }
    } // remove datastream descriptors to pipe if exist
  }

  pipelineDescriptor->setInputEndpointsCount(count);
  return pipelineDescriptor->getInputEndpointsCount();
}

int nuiFrameworkManager::setOutputEndpointCount(std::string &pipelineName,int count)
{
  nuiFactory& factory = nuiFactory::getInstance();
  nuiPluginManager& pm = nuiPluginManager::getInstance();

  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return 0;
  if (count == pipelineDescriptor->getOutputEndpointsCount())
    return count;
  if (count < pipelineDescriptor->getOutputEndpointsCount())
  {
    std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
    if (rootPipeline!=NULL)
      stack->push_back(rootPipeline);
    while (!stack->empty())
    {
      nuiPipelineModule* currentPipeline = stack->back();
      nuiModuleDescriptor* currentPipelineDescriptor = pm.getDescriptor(currentPipeline->getName());
      stack->pop_back();
      if (currentPipeline->getName() == pipelineName)
      {		
        currentPipeline->setOutputEndpointCount(count);
      }
      else
      {
        for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
        {
          nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
          if (childDescriptor->getChildModulesCount() > 0)
          {
            stack->push_back((nuiPipelineModule*)iter->second);
            if (childDescriptor->getName() == pipelineName)
            {
              //remove connections outside currentPipeline
              int pipelineModuleID = childDescriptor->property("id").asInteger();
              for (int i=0;i<currentPipelineDescriptor->getDataStreamDescriptorCount();i++)
              {
                nuiDataStreamDescriptor* connectionDescriptor = currentPipelineDescriptor->getDataStreamDescriptor(i);
                if ((connectionDescriptor->sourceModuleID == pipelineModuleID) && (connectionDescriptor->sourcePort >= count))
                {
                  nuiModule* sourceModule = currentPipeline->modules[connectionDescriptor->sourceModuleID];
                  if (sourceModule == NULL)
                    continue;
                  nuiEndpoint* sourceEndpoint = sourceModule->getOutputEndpointCount() > connectionDescriptor->sourcePort ? sourceModule->getOutputEndpoint(connectionDescriptor->sourcePort) : NULL;
                  if (sourceEndpoint== NULL)
                    continue;
                  sourceEndpoint->removeConnections();
                }
              }
            }
          }
        }
      }
    }
    delete stack;

    for (std::vector<nuiModuleDescriptor*>::iterator iter = pm.getPipelineDescriptors().begin(); iter != pm.getPipelineDescriptors().end(); iter++)
    {
      nuiModuleDescriptor *currentModuleDescriptor = *iter;
      for (int i=currentModuleDescriptor->getChildModulesCount() - 1 ; i>=0 ; i--)
      {
        if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
        {
          int id = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
          for (int j=currentModuleDescriptor->getDataStreamDescriptorCount()-1 ; i>=0 ; i--)
          {
            if (currentModuleDescriptor->getDataStreamDescriptor(j)->sourceModuleID == id) 
            {
              if (currentModuleDescriptor->getDataStreamDescriptor(j)->sourcePort >= count)
              {
                currentModuleDescriptor->removeDataStreamDescriptor(currentModuleDescriptor->getDataStreamDescriptor(j));
                continue;
              }
            }
          }
        }
      }
    }
  }

  pipelineDescriptor->setOutputEndpointsCount(count);
  return pipelineDescriptor->getOutputEndpointsCount();
}

nuiModuleDescriptor * nuiFrameworkManager::getCurrentPipeline()
{
  return nuiPluginManager::getInstance().getDescriptor(getCurrent()->getName());
}

nuiModuleDescriptor *nuiFrameworkManager::getWorkflowRoot()
{
  return nuiPluginManager::getInstance().getDescriptor(rootPipeline->getName());
}

nuiModuleDescriptor *nuiFrameworkManager::getPipeline(std::string &pipelineName)
{
  return nuiPluginManager::getInstance().getDescriptor(pipelineName);
}

nuiModuleDescriptor *nuiFrameworkManager::getModule(std::string &pipelineName, int index)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  for (int i=0; i<pipelineDescriptor->getChildModulesCount(); i++)
  {
    if (pipelineDescriptor->getChildModuleDescriptor(i)->property("id").asInteger() == index)
    {
      return pipelineDescriptor->getChildModuleDescriptor(i);
    }
  }
  return NULL;
}

nuiModuleDescriptor *nuiFrameworkManager::getModule(std::string &moduleName)
{
  return nuiPluginManager::getInstance().getDescriptor(moduleName);
}

nuiEndpointDescriptor *nuiFrameworkManager::getInputEndpoint(std::string &pipelineName,int index)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  if (pipelineDescriptor->getInputEndpointsCount() <= index)
    return NULL;
  return pipelineDescriptor->getInputEndpointDescriptor(index);
}

nuiEndpointDescriptor *nuiFrameworkManager::getOutputEndpoint(std::string &pipelineName,int index)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  if (pipelineDescriptor->getOutputEndpointsCount() <= index)
    return NULL;
  return pipelineDescriptor->getOutputEndpointDescriptor(index);
}

nuiDataStreamDescriptor *nuiFrameworkManager::getConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  for (int i=0;i<pipelineDescriptor->getDataStreamDescriptorCount();i++)
  {
    if ((sourceModuleID == pipelineDescriptor->getDataStreamDescriptor(i)->sourceModuleID) 
      && (destinationModuleID == pipelineDescriptor->getDataStreamDescriptor(i)->destinationModuleID) 
      && (sourcePort == pipelineDescriptor->getDataStreamDescriptor(i)->sourcePort)
      && (sourcePort == pipelineDescriptor->getDataStreamDescriptor(i)->sourcePort))
      return pipelineDescriptor->getDataStreamDescriptor(i);
  }
}

nuiFrameworkManagerErrorCode::err nuiFrameworkManager::deletePipeline(std::string &pipelineName)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiFactory& factory = nuiFactory::getInstance();

  if (pipelineName == "root")
    return nuiFrameworkManagerErrorCode::RootDeletionDeprecated;
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return nuiFrameworkManagerErrorCode::NonexistentModule;

  std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
  if (rootPipeline!=NULL)
    stack->push_back(rootPipeline);
  while (!stack->empty())
  {
    nuiPipelineModule* currentPipeline = stack->back();
    nuiModuleDescriptor* currentPipelineDescriptor = pm.getDescriptor(currentPipeline->getName());
    stack->pop_back();
    if (currentPipeline->getName() == pipelineName)
    {		
      //deletes internal connection and outer outputs from this pipeline
      delete currentPipeline;
    }
    else
    {
      for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
      {
        nuiModuleDescriptor* childDescriptor = pm.getDescriptor("nui" + iter->second->getName() + "Module");
        if (childDescriptor->getChildModulesCount() > 0)
        {
          stack->push_back((nuiPipelineModule*)iter->second);
          if (childDescriptor->getName() == pipelineName)
          {
            //remove connections outside currentPipeline
            int pipelineModuleID = childDescriptor->property("id").asInteger();
            for (int i=0;i<currentPipelineDescriptor->getDataStreamDescriptorCount();i++)
            {
              nuiDataStreamDescriptor* connectionDescriptor = currentPipelineDescriptor->getDataStreamDescriptor(i);
              if (connectionDescriptor->destinationModuleID == pipelineModuleID)
              {
                nuiModule* sourceModule = currentPipeline->modules[connectionDescriptor->sourceModuleID];
                if (sourceModule == NULL)
                  continue;
                nuiEndpoint* sourceEndpoint = 
                  (sourceModule->getOutputEndpointCount() > connectionDescriptor->sourcePort) ? 
                  sourceModule->getOutputEndpoint(connectionDescriptor->sourcePort) : NULL;
                if (sourceEndpoint== NULL)
                  continue;
                nuiModule* destinationModule = currentPipeline;
                nuiEndpoint* destinationEndpoint = 
                  destinationModule->getInputEndpoint(connectionDescriptor->destinationPort);
                if (destinationEndpoint == NULL)
                  continue;
                sourceEndpoint->removeConnection(destinationEndpoint);
              }
            }
          }
        }
      }
    }
  }
  delete stack;

  for (std::vector<nuiModuleDescriptor*>::iterator iter = pm.getPipelineDescriptors().begin(); 
    iter != pm.getPipelineDescriptors().end(); iter++)
  {
    nuiModuleDescriptor *currentModuleDescriptor = *iter;
    for (int i = currentModuleDescriptor->getChildModulesCount() - 1 ; i>=0 ; i--)
    {
      if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
      {
        int childModuleID = 
          currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
        for (int j=currentModuleDescriptor->getDataStreamDescriptorCount()-1 ; j>=0 ; j--)
        {
          if ((currentModuleDescriptor->getDataStreamDescriptor(j)->destinationModuleID == childModuleID) 
            ||(currentModuleDescriptor->getDataStreamDescriptor(j)->sourceModuleID == childModuleID))
          {
            currentModuleDescriptor->removeDataStreamDescriptor(
              currentModuleDescriptor->getDataStreamDescriptor(j));
          }
        }
        currentModuleDescriptor->removeChildModuleDescriptor(
          currentModuleDescriptor->getChildModuleDescriptor(i));
      }
    }
  }
  
  std::vector<nuiModuleDescriptor*>& pipes = pm.getPipelineDescriptors();
  for (int i = pipes.size()-1 ; i>=0 ; i--)
  {
    if(pipes[i]->getName() == pipelineName)
      pipes.erase(pipes.begin()+i);
  }

  return nuiFrameworkManagerErrorCode::Success;
}

nuiModuleDescriptor *nuiFrameworkManager::deleteModule(std::string &pipelineName,int moduleIndex)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiFactory& factory = nuiFactory::getInstance();

  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
  if (rootPipeline!=NULL)
    stack->push_back(rootPipeline);
  while (!stack->empty())
  {
    nuiPipelineModule* currentPipeline = stack->back();
    nuiModuleDescriptor* currentPipelineDescriptor = pm.getDescriptor(currentPipeline->getName());
    stack->pop_back();
    if (currentPipeline->getName() == pipelineName)
    {		
      nuiModule* deletedModule = currentPipeline->getChildModuleAtIndex(moduleIndex);
      if (deletedModule == NULL)
        continue;
      for (int i=0;i<pipelineDescriptor->getDataStreamDescriptorCount();i++)
      {
        nuiDataStreamDescriptor* dataStreamDescriptor = pipelineDescriptor->getDataStreamDescriptor(i);
        if (dataStreamDescriptor->destinationModuleID == moduleIndex)
        {
          nuiModule* sourceModule = currentPipeline->getChildModuleAtIndex(dataStreamDescriptor->sourceModuleID);
          if (sourceModule == NULL)
            continue;
          nuiEndpoint* sourceEndpoint = sourceModule->getOutputEndpoint(dataStreamDescriptor->sourcePort);
          nuiEndpoint* destinationEndpoint = deletedModule->getInputEndpoint(dataStreamDescriptor->destinationPort);
          sourceEndpoint->removeConnection(destinationEndpoint);
        }
      }
      delete deletedModule;
    }
    else
    {
      for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
      {
        nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
        if (childDescriptor->getChildModulesCount() > 0)
        {
          stack->push_back((nuiPipelineModule*)iter->second);
        }
      }
    }
  }
  delete stack;

  for (int i = pipelineDescriptor->getDataStreamDescriptorCount()-1;i>=0;i--)
  {
    if ((pipelineDescriptor->getDataStreamDescriptor(i)->sourceModuleID == moduleIndex) ||(pipelineDescriptor->getDataStreamDescriptor(i)->destinationModuleID == moduleIndex))
    {
      pipelineDescriptor->removeDataStreamDescriptor(pipelineDescriptor->getDataStreamDescriptor(i));
    }
  }
  for (int i=0;i<pipelineDescriptor->getChildModulesCount();i++)
  {
    if (pipelineDescriptor->getChildModuleDescriptor(i)->property("id").asInteger() == moduleIndex)
    {
      pipelineDescriptor->removeChildModuleDescriptor(pipelineDescriptor->getChildModuleDescriptor(i));
    }
  }

  return pipelineDescriptor;
}

//! \todo object level not implemented
nuiModuleDescriptor *nuiFrameworkManager::deleteInputEndpoint(std::string &pipelineName,int index)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();

  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  if (pipelineDescriptor->getInputEndpointsCount() <= index)
    return NULL;
  nuiEndpointDescriptor* endpointDescriptor = pipelineDescriptor->getInputEndpointDescriptor(index);
  if (endpointDescriptor == NULL)
    return NULL;

  for (std::vector<nuiModuleDescriptor*>::iterator iter = pm.getPipelineDescriptors().begin(); iter != pm.getPipelineDescriptors().end(); iter++)
  {
    nuiModuleDescriptor *currentModuleDescriptor = *iter;
    for (int i=currentModuleDescriptor->getChildModulesCount() - 1 ; i>=0 ; i--)
    {
      if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
      {
        int id = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
        for (int j=currentModuleDescriptor->getDataStreamDescriptorCount() - 1 ; i>=0 ; i--)
        {
          if (currentModuleDescriptor->getDataStreamDescriptor(j)->destinationModuleID == id) 
          {
            if (currentModuleDescriptor->getDataStreamDescriptor(j)->destinationPort == index)
            {
              currentModuleDescriptor->removeDataStreamDescriptor(currentModuleDescriptor->getDataStreamDescriptor(j));
              continue;
            }
            else
            {
              if (currentModuleDescriptor->getDataStreamDescriptor(j)->destinationPort > index)
                currentModuleDescriptor->getDataStreamDescriptor(j)->destinationPort--;
            }
          }
        }
        currentModuleDescriptor->removeChildModuleDescriptor(currentModuleDescriptor->getChildModuleDescriptor(i));
      }
    }
  }

  pipelineDescriptor->removeInputEndpointDescriptor(endpointDescriptor);
  return pipelineDescriptor;
}
//object level not implemented
nuiModuleDescriptor *nuiFrameworkManager::deleteOutputEndpoint(std::string &pipelineName,int index)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  if (pipelineDescriptor->getOutputEndpointsCount() <= index)
    return NULL;
  nuiEndpointDescriptor* endpointDescriptor = pipelineDescriptor->getOutputEndpointDescriptor(index);
  if (endpointDescriptor == NULL)
    return NULL;

  for (std::vector<nuiModuleDescriptor*>::iterator iter = pm.getPipelineDescriptors().begin(); iter != pm.getPipelineDescriptors().end(); iter++)
  {
    nuiModuleDescriptor *currentModuleDescriptor = *iter;
    for (int i=currentModuleDescriptor->getChildModulesCount()-1;i>=0;i--)
    {
      if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
      {
        int id = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
        for (int j=currentModuleDescriptor->getDataStreamDescriptorCount() - 1 ; i>=0 ; i--)
        {
          if (currentModuleDescriptor->getDataStreamDescriptor(j)->sourceModuleID == id) 
          {
            if (currentModuleDescriptor->getDataStreamDescriptor(j)->sourcePort == index)
            {
              currentModuleDescriptor->removeDataStreamDescriptor(currentModuleDescriptor->getDataStreamDescriptor(j));
              continue;
            }
            else
            {
              if (currentModuleDescriptor->getDataStreamDescriptor(j)->sourcePort > index)
                currentModuleDescriptor->getDataStreamDescriptor(j)->sourcePort--;
            }
          }
        }
        currentModuleDescriptor->removeChildModuleDescriptor(currentModuleDescriptor->getChildModuleDescriptor(i));
      }
    }
  }

  pipelineDescriptor->removeOutputEndpointDescriptor(endpointDescriptor);
  return pipelineDescriptor;
}

nuiModuleDescriptor *nuiFrameworkManager::deleteConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID, int sourcePort, int destinationPort)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
  if (rootPipeline!=NULL)
    stack->push_back(rootPipeline);
  while (!stack->empty())
  {
    nuiPipelineModule* currentPipeline = stack->back();
    nuiModuleDescriptor* currentPipelineDescriptor = pm.getDescriptor(currentPipeline->getName());
    stack->pop_back();
    if (currentPipeline->getName() == pipelineName)
    {	
      nuiModule* sourceModule = currentPipeline->getChildModuleAtIndex(sourceModuleID);
      if (sourceModule == NULL)
        continue;
      nuiEndpoint* sourceEndpoint = sourceModule->getOutputEndpoint(sourcePort);
      nuiModule* destinationModule = currentPipeline->getChildModuleAtIndex(destinationModuleID);
      if (destinationModule == NULL)
        continue;
      nuiEndpoint* destinationEndpoint = destinationModule->getInputEndpoint(destinationPort);
      sourceEndpoint->removeConnection(destinationEndpoint);
    }
    else
    {
      for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
      {
        nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
        if (childDescriptor->getChildModulesCount() > 0)
        {
          stack->push_back((nuiPipelineModule*)iter->second);
        }
      }
    }
  }
  delete stack;


  for (int i = pipelineDescriptor->getDataStreamDescriptorCount()-1;i>=0;i--)
  {
    if ((pipelineDescriptor->getDataStreamDescriptor(i)->sourceModuleID == sourceModuleID) && (pipelineDescriptor->getDataStreamDescriptor(i)->destinationModuleID == destinationModuleID) &&
      (pipelineDescriptor->getDataStreamDescriptor(i)->sourcePort == sourcePort) && (pipelineDescriptor->getDataStreamDescriptor(i)->destinationPort == destinationPort))
    {
      pipelineDescriptor->removeDataStreamDescriptor(pipelineDescriptor->getDataStreamDescriptor(i));
    }
  }
  return pipelineDescriptor;
}

nuiModuleDescriptor *nuiFrameworkManager::updatePipeline(std::string &pipelineName, nuiModuleDescriptor* moduleDescriptor)
{
  if (moduleDescriptor == NULL)
    return NULL;

  nuiPluginManager& pm = nuiPluginManager::getInstance();

  if (moduleDescriptor->getName() != pipelineName)
  {
    std::string newName = moduleDescriptor->getName();
    while ( pm.getDescriptor(newName) != NULL)
      newName += "_";
    for (std::vector<nuiModuleDescriptor*>::iterator iter = pm.getPipelineDescriptors().begin(); 
      iter != pm.getPipelineDescriptors().end(); iter++)
    {
      nuiModuleDescriptor *currentModuleDescriptor = *iter;
      for (int i=currentModuleDescriptor->getChildModulesCount() - 1 ; i>=0 ; i--)
      {
        if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
        {
          currentModuleDescriptor->getChildModuleDescriptor(i)->setName(newName);
        }
      }
    }
    std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
    if (rootPipeline!=NULL)
      stack->push_back(rootPipeline);
    while (!stack->empty())
    {
      nuiPipelineModule* currentPipeline = stack->back();
      stack->pop_back();
      if (currentPipeline->getName() == pipelineName)
      {	
        currentPipeline->setName(newName);
      }
      else
      {
        for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
        {
          nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
          if (childDescriptor->getChildModulesCount() > 0)
          {
            stack->push_back((nuiPipelineModule*)iter->second);
          }
        }
      }
    }
    delete stack;
    //\todo refactor search
    std::vector<nuiModuleDescriptor*>& pipelineDescriptors = pm.getPipelineDescriptors();
    for (int i=pipelineDescriptors.size() - 1 ; i>=0 ; i--)
    {
      if(pipelineDescriptors[i]->getName() == pipelineName)
        pipelineDescriptors.erase(pipelineDescriptors.begin() + i);
    }
  }
  std::vector<nuiModuleDescriptor*>& pipelineDescriptors = pm.getPipelineDescriptors();
  for (int i=pipelineDescriptors.size() - 1 ; i>=0 ; i--)
  {
    if(pipelineDescriptors[i]->getName() == pipelineName)
      pipelineDescriptors[i] = moduleDescriptor;
  }
  return moduleDescriptor;
}

nuiModuleDescriptor *nuiFrameworkManager::updateModule(std::string &pipelineName, int index, nuiModuleDescriptor* moduleDescriptor)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  if (moduleDescriptor == NULL)
    return NULL;
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  nuiModuleDescriptor* oldModuleDescriptor = NULL;
  for (int i = 0; i < pipelineDescriptor->getChildModulesCount();i++)
  {
    if (pipelineDescriptor->getChildModuleDescriptor(i)->property("id").asInteger() == index)
    {
      oldModuleDescriptor = pipelineDescriptor->getChildModuleDescriptor(i);
      break;
    }
  }
  if (oldModuleDescriptor == NULL)
    oldModuleDescriptor = createModule(pipelineName,moduleDescriptor->getName());
  if (oldModuleDescriptor == NULL)
    return NULL;
  for (std::map<std::string, nuiProperty*>::iterator iter = moduleDescriptor->getProperties().begin();iter!=moduleDescriptor->getProperties().end();iter++)
  {
    if (iter->first != "id")
      oldModuleDescriptor->property(iter->first).set(iter->second->asString()); // as int for other things!
  }
  std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
  if (rootPipeline!=NULL)
    stack->push_back(rootPipeline);
  while (!stack->empty())
  {
    nuiPipelineModule* currentPipeline = stack->back();
    stack->pop_back();
    if (currentPipeline->getName() == pipelineName)
    {		
      nuiModule* updatedModule = currentPipeline->getChildModuleAtIndex(index);
      if (updatedModule == NULL)
        continue;
      for (std::map<std::string, nuiProperty*>::iterator iter = moduleDescriptor->getProperties().begin();iter!=moduleDescriptor->getProperties().end();iter++)
      {
        if (iter->first != "id")
          updatedModule->property(iter->first).set(iter->second->asString());
      }
    }
    else
    {
      for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
      {
        nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
        if (childDescriptor->getChildModulesCount() > 0)
        {
          stack->push_back((nuiPipelineModule*)iter->second);
        }
      }
    }
  }
  delete stack;
  return oldModuleDescriptor;
}

//object level not implemented
//Changing name of endpoint descriptor type can cause deleting of iner and outer connection between modules.
nuiEndpointDescriptor *nuiFrameworkManager::updateInputEndpoint(std::string &pipelineName,int index, nuiEndpointDescriptor* endpointDescriptor)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();

  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  nuiEndpointDescriptor *oldEnpointDescriptor = getInputEndpoint(pipelineName, index);
  if (oldEnpointDescriptor == NULL)
    return NULL;
  if (oldEnpointDescriptor->getDescriptor() == endpointDescriptor->getDescriptor())
    return oldEnpointDescriptor;

  //check inside pipeline
  std::vector<nuiDataStreamDescriptor*> connectionsToBeDeleted;
  bool isChangeAccepted = true;
  for (int i=pipelineDescriptor->getDataStreamDescriptorCount()-1;i!=0;i--)
  {
    if ((pipelineDescriptor->getDataStreamDescriptor(i)->sourceModuleID == PIPELINE_ID) && (pipelineDescriptor->getDataStreamDescriptor(i)->sourcePort == index))
    {
      nuiModuleDescriptor* destinationModule = getModule(pipelineName,pipelineDescriptor->getDataStreamDescriptor(i)->destinationModuleID);
      if (destinationModule == NULL)
      {
        connectionsToBeDeleted.push_back(pipelineDescriptor->getDataStreamDescriptor(i));
        continue;
      }
      nuiEndpointDescriptor* destinationEndpointDescriptor = destinationModule->getInputEndpointDescriptor(pipelineDescriptor->getDataStreamDescriptor(i)->destinationPort);
      if (destinationEndpointDescriptor->getDescriptor() != endpointDescriptor->getDescriptor())
      {
        connectionsToBeDeleted.push_back(pipelineDescriptor->getDataStreamDescriptor(i));
      }
    }
  }
  for (int i=0;i<connectionsToBeDeleted.size();i++)
  {
    deleteConnection(pipelineName,connectionsToBeDeleted[i]->sourceModuleID,connectionsToBeDeleted[i]->destinationModuleID,
      connectionsToBeDeleted[i]->sourcePort,connectionsToBeDeleted[i]->destinationPort);
  }

  //check outside pipeline	
  std::vector<std::string>& pipelineNames = pm.listLoadedPipelines();
  for (int i = 0;i<pipelineNames.size();i++)
  {
    nuiModuleDescriptor* currentPipelineDescriptor = pm.getDescriptor(pipelineNames[i]);
    if (currentPipelineDescriptor == NULL)
      continue;
    connectionsToBeDeleted.clear();
    for (int j=0;j<currentPipelineDescriptor->getChildModulesCount();j++)
    {
      if (currentPipelineDescriptor->getChildModuleDescriptor(j)->getName() == pipelineName)
      {
        int currentID = currentPipelineDescriptor->getChildModuleDescriptor(j)->property("id").asInteger();
        for (int k=0;k<currentPipelineDescriptor->getDataStreamDescriptorCount();k++)
        {
          if (currentPipelineDescriptor->getDataStreamDescriptor(k)->destinationModuleID == currentID)
          {
            nuiModuleDescriptor* sourceModule = NULL;
            for (int l=0;l<currentPipelineDescriptor->getChildModulesCount();l++)
            {
              if (currentPipelineDescriptor->getChildModuleDescriptor(l)->property("id").asInteger() == currentPipelineDescriptor->getDataStreamDescriptor(k)->sourceModuleID)
              {
                sourceModule = pm.getDescriptor(currentPipelineDescriptor->getChildModuleDescriptor(l)->getName());
                break;
              }
            }
            if (sourceModule == NULL)
              continue;
            if (sourceModule->getOutputEndpointDescriptor(currentPipelineDescriptor->getDataStreamDescriptor(k)->destinationPort)->getDescriptor() != endpointDescriptor->getDescriptor())
            {
              connectionsToBeDeleted.push_back(currentPipelineDescriptor->getDataStreamDescriptor(k));
            }
          }
        }
      }
    }
    for (int j=0;j<connectionsToBeDeleted.size();j++)
    {
      deleteConnection(pipelineNames[i],connectionsToBeDeleted[j]->sourceModuleID,connectionsToBeDeleted[j]->destinationModuleID,
        connectionsToBeDeleted[j]->sourcePort,connectionsToBeDeleted[j]->destinationPort);
    }
  }
  oldEnpointDescriptor->setDescriptor(endpointDescriptor->getDescriptor());
  return oldEnpointDescriptor;
}

//object level not implemented
//need to check it again
nuiEndpointDescriptor *nuiFrameworkManager::updateOutputEndpoint(std::string &pipelineName,int index, nuiEndpointDescriptor* endpointDescriptor)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  nuiEndpointDescriptor *oldEnpointDescriptor = getInputEndpoint(pipelineName, index);
  if (oldEnpointDescriptor == NULL)
    return NULL;
  if (oldEnpointDescriptor->getDescriptor() == endpointDescriptor->getDescriptor())
    return oldEnpointDescriptor;

  //check inside pipeline
  std::vector<nuiDataStreamDescriptor*> connectionsToBeDeleted;
  bool isChangeAccepted = true;
  for (int i=pipelineDescriptor->getDataStreamDescriptorCount()-1;i!=0;i--)
  {
    if ((pipelineDescriptor->getDataStreamDescriptor(i)->destinationModuleID == PIPELINE_ID) && (pipelineDescriptor->getDataStreamDescriptor(i)->destinationPort == index))
    {
      nuiModuleDescriptor* sourceModule = getModule(pipelineName,pipelineDescriptor->getDataStreamDescriptor(i)->sourceModuleID);
      if (sourceModule == NULL)
      {
        connectionsToBeDeleted.push_back(pipelineDescriptor->getDataStreamDescriptor(i));
        continue;
      }
      nuiEndpointDescriptor* sourceEndpointDescriptor = sourceModule->getOutputEndpointDescriptor(pipelineDescriptor->getDataStreamDescriptor(i)->destinationPort);
      if (sourceEndpointDescriptor->getDescriptor() != endpointDescriptor->getDescriptor())
      {
        connectionsToBeDeleted.push_back(pipelineDescriptor->getDataStreamDescriptor(i));
      }
    }
  }
  for (int i=0;i<connectionsToBeDeleted.size();i++)
  {
    deleteConnection(pipelineName,connectionsToBeDeleted[i]->sourceModuleID,connectionsToBeDeleted[i]->destinationModuleID,
      connectionsToBeDeleted[i]->sourcePort,connectionsToBeDeleted[i]->destinationPort);
  }

  //check outside pipeline	
  std::vector<std::string>& pipelineNames = pm.listLoadedPipelines();
  for (int i=0; i<pipelineNames.size(); i++)
  {
    nuiModuleDescriptor* currentPipelineDescriptor = pm.getDescriptor(pipelineNames[i]);
    if (currentPipelineDescriptor == NULL)
      continue;
    connectionsToBeDeleted.clear();
    for (int j=0;j<currentPipelineDescriptor->getChildModulesCount();j++)
    {
      if (currentPipelineDescriptor->getChildModuleDescriptor(j)->getName() == pipelineName)
      {
        int currentID = currentPipelineDescriptor->getChildModuleDescriptor(j)->property("id").asInteger();
        for (int k=0;k<currentPipelineDescriptor->getDataStreamDescriptorCount();k++)
        {
          if (currentPipelineDescriptor->getDataStreamDescriptor(k)->destinationModuleID == currentID)
          {
            nuiModuleDescriptor* sourceModule = NULL;
            for (int l=0;l<currentPipelineDescriptor->getChildModulesCount();l++)
            {
              if (currentPipelineDescriptor->getChildModuleDescriptor(l)->property("id").asInteger() == currentPipelineDescriptor->getDataStreamDescriptor(k)->sourceModuleID)
              {
                sourceModule = pm.getDescriptor(currentPipelineDescriptor->getChildModuleDescriptor(l)->getName());
                break;
              }
            }
            if (sourceModule == NULL)
              continue;
            if (sourceModule->getOutputEndpointDescriptor(currentPipelineDescriptor->getDataStreamDescriptor(k)->destinationPort)->getDescriptor() != endpointDescriptor->getDescriptor())
            {
              connectionsToBeDeleted.push_back(currentPipelineDescriptor->getDataStreamDescriptor(k));
            }
          }
        }
      }
    }
    for (int j=0;j<connectionsToBeDeleted.size();j++)
    {
      deleteConnection(pipelineNames[i],connectionsToBeDeleted[j]->sourceModuleID,connectionsToBeDeleted[j]->destinationModuleID,
        connectionsToBeDeleted[j]->sourcePort,connectionsToBeDeleted[j]->destinationPort);
    }
  }
  oldEnpointDescriptor->setDescriptor(endpointDescriptor->getDescriptor());
  return oldEnpointDescriptor;
}

nuiDataStreamDescriptor *nuiFrameworkManager::updateConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort, nuiDataStreamDescriptor *connectionDescriptor)
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  nuiModuleDescriptor* pipelineDescriptor = pm.getDescriptor(pipelineName);
  if (pipelineDescriptor == NULL)
    return NULL;
  nuiDataStreamDescriptor* oldConnectionDescriptor = NULL;
  for (int i=0;i<pipelineDescriptor->getDataStreamDescriptorCount();i++)
  {
    if ((sourceModuleID == pipelineDescriptor->getDataStreamDescriptor(i)->sourceModuleID)
      && (destinationModuleID == pipelineDescriptor->getDataStreamDescriptor(i)->destinationModuleID) 
      && (destinationPort == pipelineDescriptor->getDataStreamDescriptor(i)->destinationPort) 
      && (sourcePort == pipelineDescriptor->getDataStreamDescriptor(i)->sourcePort))
    {
      oldConnectionDescriptor = pipelineDescriptor->getDataStreamDescriptor(i);
      break;
    }	
  }
  if (oldConnectionDescriptor == NULL)
    oldConnectionDescriptor = createConnection(pipelineName,sourceModuleID,destinationModuleID, sourcePort, destinationPort);
  if (oldConnectionDescriptor == NULL)
    return NULL;
  oldConnectionDescriptor->asyncMode = connectionDescriptor->asyncMode;
  oldConnectionDescriptor->buffered = connectionDescriptor->buffered;
  oldConnectionDescriptor->deepCopy = connectionDescriptor->deepCopy;
  oldConnectionDescriptor->bufferSize = connectionDescriptor->bufferSize;
  oldConnectionDescriptor->lastPacket = connectionDescriptor->lastPacket;
  oldConnectionDescriptor->overflow = connectionDescriptor->overflow;

  std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
  if (rootPipeline!=NULL)
    stack->push_back(rootPipeline);
  while (!stack->empty())
  {
    nuiPipelineModule* currentPipeline = stack->back();
    stack->pop_back();
    if (currentPipeline->getName() == pipelineName)
    {		
      nuiModule*  sourceModule = (sourceModuleID == PIPELINE_ID) ? currentPipeline : currentPipeline->getChildModuleAtIndex(sourceModuleID);
      nuiModule*  destinationModule = (destinationModuleID == PIPELINE_ID) ? currentPipeline : currentPipeline->getChildModuleAtIndex(destinationModuleID);
      if ((sourceModule!=NULL) && (destinationModule != NULL))
      {
        nuiEndpoint* sourceEndpoint = sourceModule == currentPipeline ? currentPipeline->inputInternalEndpoints[sourcePort] : sourceModule->getOutputEndpoint(sourcePort);
        nuiEndpoint* destinationEndpoint = destinationModule == currentPipeline ? currentPipeline->inputInternalEndpoints[destinationPort] : destinationModule->getInputEndpoint(destinationPort);
        if ((sourceEndpoint != NULL) && (destinationEndpoint != NULL))
        {
          nuiDataStream *dataStream = sourceEndpoint->getDataStreamForEndpoint(destinationEndpoint);
          dataStream->setAsyncMode(connectionDescriptor->asyncMode);
          dataStream->setBufferedMode(connectionDescriptor->buffered);
          dataStream->setDeepCopy(connectionDescriptor->deepCopy);
          dataStream->setBufferSize(connectionDescriptor->bufferSize);
          dataStream->setLastPacketPriority(connectionDescriptor->lastPacket);
          dataStream->setIsOverflow(connectionDescriptor->overflow);
        }
      }
    }
    else
    {
      for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
      {
        nuiModuleDescriptor* childDescriptor = pm.getDescriptor(iter->second->getName());
        if (childDescriptor->getChildModulesCount() > 0)
        {
          stack->push_back((nuiPipelineModule*)iter->second);
        }
      }
    }
  }
  delete stack;

  return oldConnectionDescriptor;
}

nuiModuleDescriptor *nuiFrameworkManager::navigatePush( int moduleIndex )
{
  nuiModule* newModule = getCurrent()->getChildModuleAtIndex(moduleIndex);
  if(newModule == NULL)
    return NULL;
  nuiPipelineModule* newCurrent = dynamic_cast<nuiPipelineModule*>(newModule);
  if(newCurrent == NULL)
    return NULL;
  pathToCurrent.push_back(moduleIndex);
  return nuiPluginManager::getInstance().getDescriptor(newCurrent->getName());
}

nuiModuleDescriptor *nuiFrameworkManager::navigatePop( )
{
  if(pathToCurrent.empty())
    return NULL;
  pathToCurrent.pop_back();
  return nuiPluginManager::getInstance().getDescriptor(getCurrent()->getName());
}

nuiPipelineModule *nuiFrameworkManager::getCurrent()
{
  nuiPipelineModule* current = rootPipeline;
  std::list<int>::iterator it;
  for (it = pathToCurrent.begin() ; it != pathToCurrent.end() ; it++)
    current = dynamic_cast<nuiPipelineModule*>(current->getChildModuleAtIndex(*it));

  return current;
}