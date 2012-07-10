#include "nuiFrameworkManager.h"
#include "nuiModule.h"
#include "nuiPipeline.h"
#include "nuiDataStream.h"
#include "nuiFactory.h"
#include "nuiUtils.h"

nuiFrameworkManager::nuiFrameworkManager()
{
	rootPipeline = NULL;
	currentModule = NULL;
	dataObjectTree = NULL;
}

nuiFrameworkManager::~nuiFrameworkManager()
{
	workflowStop();
	workflowQuit();
}

nuiFrameworkManager *nuiFrameworkManager::getInstance()
{ 
	static nuiFrameworkManager *instance = NULL;
	if (instance == NULL)
		instance = new nuiFrameworkManager();
	return instance;
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::initializeFrameworkManager(const char *fileName)
{
	nuiFrameworkManagerErrorCode returnCode = loadSettingsFromXml(fileName);
    this->rootPipeline = (nuiPipelineModule*)(nuiFactory::getInstance()->create("root"));
	if(rootPipeline != NULL) {
		nuiTreeNode<int, nuiModule*> *temp = new nuiTreeNode<int, nuiModule*>(rootPipeline->property("id").asInteger(), rootPipeline);
		for (int i=0; i<rootPipeline->getChildModuleCount(); i++) {
			temp->addChildNode(new nuiTreeNode<int, nuiModule*>(rootPipeline->getChildModuleAtIndex(i)->property("id").asInteger(), rootPipeline->getChildModuleAtIndex(i)));
		}
		dataObjectTree = new nuiTree<int, nuiModule*>(temp);
	}
	return (rootPipeline != NULL) ? NUI_FRAMEWORK_MANAGER_OK : NUI_FRAMEWORK_ROOT_INITIALIZATION_FAILED;
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::saveSettingsToXml(const char *fileName, std::list<nuiModuleDescriptor*>* descriptors)
{
    ofxXmlSettings* xmlSettings = new ofxXmlSettings();
    saveSettingsToXml(xmlSettings, descriptors);
    if(!xmlSettings->saveFile(fileName))
        return NUI_FRAMEWORK_ERROR_SAVING_FILE;
	return NUI_FRAMEWORK_MANAGER_OK;
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::loadSettingsFromXml(const char *fileName)
{
	ofxXmlSettings* xmlSettings = new ofxXmlSettings();
	if (!xmlSettings->loadFile(fileName))
		return NUI_FRAMEWORK_WRONG_FILE;
	return loadSettingsFromXml(xmlSettings);
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::saveSettingsToXml(ofxXmlSettings *xmlSettings,std::list<nuiModuleDescriptor*>* descriptors)
{
    std::list<nuiModuleDescriptor*> descriptions = *descriptors;

    xmlSettings->addTag("PIPELINES");
    xmlSettings->pushTag("PIPELINES");
    
    std::list<nuiModuleDescriptor*>::iterator it;
    int pipeline = 0;
    for (it = descriptions.begin() ; it != descriptions.end() ; it++)
    {
        xmlSettings->addTag("PIPELINE");
        xmlSettings->pushTag("PIPELINE");

        xmlSettings->addTag("TYPE");
        xmlSettings->setValue("TYPE",(*it)->getName(),pipeline);
        xmlSettings->addTag("AUTHOR");
        xmlSettings->setValue("AUTHOR",(*it)->getAuthor(),pipeline);
        xmlSettings->addTag("DESCRIPTION");
        xmlSettings->setValue("DESCRIPTION",(*it)->getDescription(),pipeline);
 
        xmlSettings->addTag("PROPERTIES");
        xmlSettings->pushTag("PROPERTIES");
        std::map<std::string, nuiProperty*> props = (*it)->getProperties();
        std::map<std::string, nuiProperty*>::iterator propIter;
        int pr = 0;
        for (propIter = props.begin() ; propIter != props.end() ; propIter++)
        {
            if(propIter->first == "id")
                continue;
            xmlSettings->addTag("PROPERTY");
            xmlSettings->pushTag("PROPERTY", pr++);

            xmlSettings->addTag("KEY");
            xmlSettings->setValue("KEY",propIter->first);
            
            xmlSettings->addTag("VALUE");
            xmlSettings->setValue("VALUE",propIter->second->asString());

            xmlSettings->popTag();
        }
        xmlSettings->popTag();
        
        xmlSettings->addTag("ENDPOINTS");
        xmlSettings->pushTag("ENDPOINTS",pipeline);
        xmlSettings->addTag("INPUT");
        xmlSettings->pushTag("INPUT",0);
        for (int i=0; i<(*it)->getInputEndpointsCount() ; i++)
        {
            xmlSettings->addTag("ENDPOINT");
            xmlSettings->pushTag("ENDPOINT",i);
            nuiEndpointDescriptor* descriptor = (*it)->getInputEndpointDescriptor(i);
            xmlSettings->addTag("ID");
            xmlSettings->setValue("ID",descriptor->getIndex(),i);
            xmlSettings->addTag("TYPE");
            xmlSettings->setValue("TYPE", descriptor->getDescriptor(), i);
            xmlSettings->popTag();
        }
        xmlSettings->popTag();

        xmlSettings->addTag("OUTPUT");
        xmlSettings->pushTag("OUTPUT",0);
        for (int i=0; i<(*it)->getOutputEndpointsCount() ; i++)
        {
            xmlSettings->addTag("ENDPOINT");
            xmlSettings->pushTag("ENDPOINT",i);
            nuiEndpointDescriptor* descriptor = (*it)->getOutputEndpointDescriptor(i);
            xmlSettings->addTag("ID");
            xmlSettings->setValue("ID",descriptor->getIndex(),i);
            xmlSettings->addTag("TYPE");
            xmlSettings->setValue("TYPE", descriptor->getDescriptor(), i);
            xmlSettings->popTag();
        }
        xmlSettings->popTag();
        xmlSettings->popTag();

        xmlSettings->addTag("MODULES");
        xmlSettings->pushTag("MODULES");
        
        for(int i=0 ; i<(*it)->getChildModulesCount() ; i++)
        {
            nuiModuleDescriptor* descriptor = (*it)->getChildModuleDescriptor(i);
            xmlSettings->addTag("MODULE");
            xmlSettings->pushTag("MODULE", i);

            xmlSettings->addTag("ID");
            xmlSettings->setValue("ID", descriptor->getProperties().find("id")->second->asInteger());
            xmlSettings->addTag("TYPE");
            xmlSettings->setValue("TYPE", descriptor->getName() );

            xmlSettings->addTag("PROPERTIES");
            xmlSettings->pushTag("PROPERTIES",0);
            std::map<std::string, nuiProperty*> props = descriptor->getProperties();
            std::map<std::string, nuiProperty*>::iterator propIter;
            int prop = 0;
            for( propIter = props.begin() ; propIter != props.end() ; propIter++)
            {
                if(propIter->first == "id")
                    continue;
                xmlSettings->addTag("PROPERTY");
                xmlSettings->pushTag("PROPERTY", prop++);
                xmlSettings->addTag("KEY");
                xmlSettings->setValue("KEY",propIter->first);
            
                xmlSettings->addTag("VALUE");
                xmlSettings->setValue("VALUE",propIter->second->asString());

                xmlSettings->popTag();
            }
            xmlSettings->popTag();
            
            xmlSettings->popTag();
        }

        xmlSettings->popTag();
        xmlSettings->addTag("CONNECTIONS");
        xmlSettings->pushTag("CONNECTIONS");
        
        for (int cnc = 0 ;cnc<(*it)->getDataStreamDescriptorCount() ; cnc++)
        {
            nuiDataStreamDescriptor* descriptor = (*it)->getDataStreamDescriptor(cnc);
            xmlSettings->addTag("CONNECTION");
            xmlSettings->pushTag("CONNECTION", cnc);
            xmlSettings->addTag("PROPERTIES");
            xmlSettings->pushTag("PROPERTIES");

            xmlSettings->addTag("ASYNC");
            xmlSettings->setValue("ASYNC",descriptor->asyncMode ? 1 : 0 );

            xmlSettings->addTag("BUFFERED");
            xmlSettings->setValue("BUFFERED",descriptor->buffered ? 1 : 0 );

            xmlSettings->addTag("BUFFERSIZE");
            xmlSettings->setValue("BUFFERSIZE",descriptor->bufferSize );

            xmlSettings->addTag("DEEPCOPY");
            xmlSettings->setValue("DEEPCOPY",descriptor->deepCopy ? 1 : 0 );

            xmlSettings->addTag("LASTPACKET");
            xmlSettings->setValue("LASTPACKET",descriptor->lastPacket ? 1 : 0 );

            xmlSettings->addTag("OVERFLOW");
            xmlSettings->setValue("OVERFLOW",descriptor->overflow ? 1 : 0 );
            xmlSettings->popTag();

            xmlSettings->addTag("SOURCE");
            xmlSettings->pushTag("SOURCE",0);
            xmlSettings->addTag("ID");
            xmlSettings->setValue("ID",descriptor->sourceModuleID);
            xmlSettings->addTag("PORT");
            xmlSettings->setValue("PORT",descriptor->sourcePort);
            xmlSettings->popTag();//pop source

			xmlSettings->addTag("DESTINATION");
            xmlSettings->pushTag("DESTINATION",0);
            xmlSettings->addTag("ID");
            xmlSettings->setValue("ID",descriptor->destinationModuleID);
            xmlSettings->addTag("PORT");
            xmlSettings->setValue("PORT",descriptor->destinationPort);
            xmlSettings->popTag();
            xmlSettings->popTag();
        }

        xmlSettings->popTag();
    }

    
	return NUI_FRAMEWORK_MANAGER_OK;
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::saveSettingsAsXml( const char* fileName, std::string& pipelineName )
{
    std::list<nuiModuleDescriptor*> descriptors;
    nuiModuleDescriptor *pipeline = nuiFrameworkManager::getInstance()->getPipeline(pipelineName);
    descriptors.push_back(pipeline);
    saveSettingsToXml(fileName, &descriptors);
	return NUI_FRAMEWORK_MANAGER_OK;
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::loadSettingsFromXml(ofxXmlSettings *xmlSettings)
{
 	xmlSettings->pushTag("PIPELINES",0);
	std::map<std::string,nuiModuleDescriptor*> pipelineDescriptionsMap;
	
	int numPipelines = xmlSettings->getNumTags("PIPELINE");

	for (int i=0;i<numPipelines;i++)
	{
		xmlSettings->pushTag("PIPELINE",i);
		nuiModuleDescriptor *parsedDescriptor = parseModuleDescriptor(xmlSettings);
		xmlSettings->popTag();
		if (parsedDescriptor != NULL)
		{
			std::map<std::string,nuiModuleDescriptor*>::iterator pipelineSearch = pipelineDescriptionsMap.find(parsedDescriptor->getName());
			//only first occurence of pipeline with specified is added to dictionary
			if (pipelineSearch == pipelineDescriptionsMap.end())
				pipelineDescriptionsMap[parsedDescriptor->getName()] = parsedDescriptor;
		}
	}	

	xmlSettings->popTag();
	for (std::map<std::string,nuiModuleDescriptor*>::iterator iter = pipelineDescriptionsMap.begin(); iter != pipelineDescriptionsMap.end(); iter++)
	{
		nuiFactory::getInstance()->pipelineDescriptors[iter->first] = iter->second;
        nuiModuleDescriptor* descr = nuiFactory::getInstance()->pipelineDescriptors[iter->first];
        int k = descr->getDataStreamDescriptorCount();
	}
	nuiFrameworkManagerErrorCode isGraphCorrect = NUI_FRAMEWORK_MANAGER_OK;// checkPipelineGraphForLoop(pipelineDescriptionsMap); todo
	return isGraphCorrect ? NUI_FRAMEWORK_MANAGER_OK : NUI_FRAMEWORK_PIPELINE_STRUCTURE_LOOP;
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::loadAddonsAtPath(const char *addonsPath)
{
	nuiFactory::getInstance()->loadDynamicModules((char*)addonsPath);
	return NUI_FRAMEWORK_MANAGER_OK;
}

nuiModuleDescriptor *nuiFrameworkManager::parseModuleDescriptor(ofxXmlSettings *xmlSettings)
{
	nuiModuleDescriptor* moduleDescriptor = new	nuiModuleDescriptor();
	moduleDescriptor->setName(xmlSettings->getValue("TYPE", "NULL"));
	moduleDescriptor->setAuthor(xmlSettings->getValue("AUTHOR", "NULL"));
	moduleDescriptor->setDescription(xmlSettings->getValue("DESCRIPTION", "NULL"));

	//load default pipeline settings. Default id of each pipeline is (0x0FFFFFFF).
	moduleDescriptor->property("id") = *(new nuiProperty(PIPELINE_ID));

	xmlSettings->pushTag("PROPERTIES", 0);
	parseModuleDescriptorParameters(*moduleDescriptor,xmlSettings);
	xmlSettings->popTag();

	xmlSettings->pushTag("MODULES", 0);
	int numModuleTags = xmlSettings->getNumTags("MODULE");
	for (int i=0;i<numModuleTags;i++)
	{
		xmlSettings->pushTag("MODULE", i);
		nuiModuleDescriptor* childDescriptor = new	nuiModuleDescriptor();
		childDescriptor->setName(xmlSettings->getValue("TYPE", "NULL"));
        int id = xmlSettings->getValue("ID", 0);
        nuiProperty* property = new nuiProperty(id);
		childDescriptor->property("id") = *property;
        xmlSettings->pushTag("PROPERTIES");
		parseModuleDescriptorParameters(*childDescriptor,xmlSettings);
		xmlSettings->popTag();
		moduleDescriptor->addChildModuleDescriptor(childDescriptor);
        xmlSettings->popTag();
	}
	xmlSettings->popTag();

	xmlSettings->pushTag("ENDPOINTS", 0);
    xmlSettings->pushTag("INPUT", 0);
	int numInputEndpointTags = xmlSettings->getNumTags("ENDPOINT");
    //moduleDescriptor->setInputEndpointsCount(numInputEndpointTags);
	for (int i=0;i<numInputEndpointTags;i++)
	{
		xmlSettings->pushTag("ENDPOINT", i);
		nuiEndpointDescriptor* endpointDescriptor = new	nuiEndpointDescriptor(xmlSettings->getValue("TYPE", "*"));
		endpointDescriptor->setIndex(xmlSettings->getValue("ID", 0));
		xmlSettings->popTag();
		moduleDescriptor->addInputEndpointDescriptor(endpointDescriptor,endpointDescriptor->getIndex());
	}
    xmlSettings->popTag();
	xmlSettings->popTag();

	xmlSettings->pushTag("ENDPOINTS", 0);
    xmlSettings->pushTag("OUTPUT", 0);
	int numOutputEndpointTags = xmlSettings->getNumTags("ENDPOINT");
    //moduleDescriptor->setOutputEndpointsCount(numOutputEndpointTags);
	for (int i=0;i<numOutputEndpointTags;i++)
	{
		xmlSettings->pushTag("ENDPOINT", i);
		nuiEndpointDescriptor* endpointDescriptor = new	nuiEndpointDescriptor(xmlSettings->getValue("TYPE", "*"));
		endpointDescriptor->setIndex(xmlSettings->getValue("ID", 0));
		xmlSettings->popTag();
		moduleDescriptor->addOutputEndpointDescriptor(endpointDescriptor,endpointDescriptor->getIndex());
	}
	xmlSettings->popTag();
    xmlSettings->popTag();

	xmlSettings->pushTag("CONNECTIONS",0);
	int numConnectionTags = xmlSettings->getNumTags("CONNECTION");

	for (int i=0;i<numConnectionTags;i++)
	{
		xmlSettings->pushTag("CONNECTION",i);
		
		nuiDataStreamDescriptor* datastreamDescriptor = new nuiDataStreamDescriptor();
		int sourceID = xmlSettings->getValue("SOURCE:ID", -1);
		int destinationID = xmlSettings->getValue("DESTINATION:ID", -1);	
		int sourcePort = xmlSettings->getValue("SOURCE:PORT", -1);
		int destinationPort = xmlSettings->getValue("DESTINATION:PORT", -1);

		if ((sourceID!=-1) && (destinationID!=-1) && (sourcePort!=-1) &&(destinationPort!=-1))
		{
			datastreamDescriptor->sourceModuleID = sourceID;
			datastreamDescriptor->sourcePort = sourcePort;
			datastreamDescriptor->destinationModuleID = destinationID;
			datastreamDescriptor->destinationPort = destinationPort;

			if (xmlSettings->getNumTags("PROPERTIES"))
			{
				xmlSettings->pushTag("PROPERTIES",0);
				datastreamDescriptor->asyncMode  = xmlSettings->getValue("ASYNC",0);
				datastreamDescriptor->buffered = xmlSettings->getValue("BUFFERED",0);
				datastreamDescriptor->bufferSize = xmlSettings->getValue("BUFFERSIZE",16);
				datastreamDescriptor->deepCopy = xmlSettings->getValue("DEEPCOPY",0);
				datastreamDescriptor->lastPacket = xmlSettings->getValue("LASTPACKET",0);
				datastreamDescriptor->overflow = xmlSettings->getValue("OVERFLOW",0);
				xmlSettings->popTag();
			}
			moduleDescriptor->addDataStreamDescriptor(datastreamDescriptor);
		}
		xmlSettings->popTag();
	}

	xmlSettings->popTag();

	return moduleDescriptor;
}

void nuiFrameworkManager::parseModuleDescriptorParameters(nuiModuleDescriptor &moduleDescriptor, ofxXmlSettings *xmlSettings)
{
	int numPropertyTags = xmlSettings->getNumTags("PROPERTY");
	for (int i=0;i<numPropertyTags;i++)
	{
		xmlSettings->pushTag("PROPERTY", i);
		std::string propertyID = xmlSettings->getValue("KEY", "none");
		std::string value = xmlSettings->getValue("VALUE", "none");
		if ((value != "none") && (propertyID != "none"))
		{
			nuiProperty *prop = new	nuiProperty(value);
			std::map<std::string, nuiProperty*>::iterator search = moduleDescriptor.getProperties().find(propertyID);
			if (search == moduleDescriptor.getProperties().end())
				moduleDescriptor.property(propertyID) = *prop;
		}
		xmlSettings->popTag();
	}
}

std::vector<std::string> *nuiFrameworkManager::listDynamicModules()
{
	return nuiFactory::getInstance()->listModuleNames();
}

std::vector<std::string> *nuiFrameworkManager::listPipelines(std::string &hostedPipelineName)
{
	std::vector<std::string>* pipelines = nuiFactory::getInstance()->listPipelineNames();
	std::vector<std::string> deniedPipelines;
	deniedPipelines.push_back("root");
	deniedPipelines.push_back(hostedPipelineName);
	while (deniedPipelines.size()>0)
	{
		std::string currentString = deniedPipelines.back();
		deniedPipelines.pop_back();
		for (int i=pipelines->size()-1;i>=0;i--)
		{
			bool needToBeDeleted = false;
			nuiModuleDescriptor* currentModuleDescriptor = nuiFactory::getInstance()->getDescriptor((*pipelines)[i]);
			if (currentModuleDescriptor==NULL)
				continue;
			for (int j=0;j<currentModuleDescriptor->getChildModulesCount();j++)
			{
				if (currentModuleDescriptor->getChildModuleDescriptor(j)->getName() == currentString)
				{
					needToBeDeleted = true;
					break;
				}
			}
			if (needToBeDeleted)
			{
				deniedPipelines.push_back((*pipelines)[i]);
				pipelines->erase(pipelines->begin()+i);
			}
		}
	}
	return pipelines;
}

nuiModuleDescriptor *nuiFrameworkManager::createPipeline(std::string &pipelineName)
{
	std::string newPipelineName = pipelineName;
	while (nuiFactory::getInstance()->getDescriptor(newPipelineName) != NULL)
		newPipelineName+='_';
	nuiModuleDescriptor* newModuleDescriptor = new nuiModuleDescriptor();
	newModuleDescriptor->setName(newPipelineName);
	newModuleDescriptor->setAuthor("native");
	newModuleDescriptor->setDescription("New Module");
	nuiModule* newPipeline = nuiFactory::getInstance()->createPipeline(newModuleDescriptor);
	if(newPipeline != NULL) {
		// do the treeness here
	}

	return newModuleDescriptor;
}

nuiModuleDescriptor *nuiFrameworkManager::createModule(std::string &pipelineName,std::string &moduleName)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	nuiModuleDescriptor* dafaultChildDescriptor = nuiFactory::getInstance()->getDescriptor(moduleName);
	if ((pipelineDescriptor == NULL) || (dafaultChildDescriptor == NULL))
		return NULL;
	
	nuiModuleDescriptor* childDescriptor = new nuiModuleDescriptor();
	childDescriptor->setAuthor(dafaultChildDescriptor->getAuthor());
	childDescriptor->setDescription(dafaultChildDescriptor->getDescription());
	childDescriptor->setName(dafaultChildDescriptor->getName());
	for (std::map<std::string, nuiProperty*>::iterator iter = dafaultChildDescriptor->getProperties().begin();iter != dafaultChildDescriptor->getProperties().end();iter++)
	{
		childDescriptor->property(iter->first) = iter->second;
	}
	childDescriptor->property("id") = new nuiProperty(nuiUtils::getRandomNumber());
	pipelineDescriptor->addChildModuleDescriptor(childDescriptor);

	for (nuiTree<int,nuiModule*>::iterator moduleObject = dataObjectTree->begin(); moduleObject != dataObjectTree->end(); moduleObject++)
	{
		nuiModule* currentModule = (*moduleObject)->getValue();
		if (currentModule == NULL)
			continue;
		if (currentModule->getName() == pipelineName)
		{
			nuiPipelineModule* pipelineObject = dynamic_cast<nuiPipelineModule*>(currentModule);
			if (pipelineObject == NULL)
				continue;
			nuiModule* moduleObjectChild = nuiFactory::getInstance()->create(moduleName);
			for (std::map<std::string, nuiProperty*>::iterator iter = childDescriptor->getProperties().begin();iter != childDescriptor->getProperties().end();iter++)
			{
				moduleObjectChild->property(iter->first) = iter->second;
			}
			pipelineObject->addChildModule(moduleObjectChild->property("id").asInteger(),moduleObjectChild);
			(*moduleObject)->addChildNode(new nuiTreeNode<int,nuiModule*>(moduleObjectChild->property("id").asInteger(),moduleObjectChild));
		}
	}

	return childDescriptor;
}

nuiDataStreamDescriptor *nuiFrameworkManager::createConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
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
	sourceModuleDescriptor = nuiFactory::getInstance()->getDescriptor(sourceModuleDescriptor->getName());
	destinationModuleDescriptor = nuiFactory::getInstance()->getDescriptor(destinationModuleDescriptor->getName());

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
				nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
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
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return 0;
	if (count == pipelineDescriptor->getInputEndpointsCount())
		return count;
	if (count < pipelineDescriptor->getInputEndpointsCount())
	{
		std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
		if (rootPipeline!=NULL)
			stack->push_back(rootPipeline);
		while (!stack->empty())
		{
			nuiPipelineModule* currentPipeline = stack->back();
			nuiModuleDescriptor* currentPipelineDescriptor = nuiFactory::getInstance()->getDescriptor(currentPipeline->getName());
			stack->pop_back();
			if (currentPipeline->getName() == pipelineName)
			{		
				currentPipeline->setInputEndpointCount(count);
			}
			else
			{
				for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
				{
					nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
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
		delete stack;

		for (std::map<std::string, nuiModuleDescriptor*>::iterator iter = nuiFactory::getInstance()->pipelineDescriptors.begin(); iter != nuiFactory::getInstance()->pipelineDescriptors.end(); iter++)
		{
			nuiModuleDescriptor *currentModuleDescriptor = iter->second;
			for (int i=currentModuleDescriptor->getChildModulesCount()-1;i>=0;i--)
			{
				if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
				{
					int id = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
					for (int j=currentModuleDescriptor->getDataStreamDescriptorCount()-1;i>=0;i--)
					{
						if (currentModuleDescriptor->getDataStreamDescriptor(j)->destinationModuleID == id) 
						{
							if (currentModuleDescriptor->getDataStreamDescriptor(j)->destinationPort >= count)
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
	pipelineDescriptor->setInputEndpointsCount(count);
	return pipelineDescriptor->getInputEndpointsCount();
}

int nuiFrameworkManager::setOutputEndpointCount(std::string &pipelineName,int count)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
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
			nuiModuleDescriptor* currentPipelineDescriptor = nuiFactory::getInstance()->getDescriptor(currentPipeline->getName());
			stack->pop_back();
			if (currentPipeline->getName() == pipelineName)
			{		
				currentPipeline->setOutputEndpointCount(count);
			}
			else
			{
				for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
				{
					nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
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

		for (std::map<std::string, nuiModuleDescriptor*>::iterator iter = nuiFactory::getInstance()->pipelineDescriptors.begin(); iter != nuiFactory::getInstance()->pipelineDescriptors.end(); iter++)
		{
			nuiModuleDescriptor *currentModuleDescriptor = iter->second;
			for (int i=currentModuleDescriptor->getChildModulesCount()-1;i>=0;i--)
			{
				if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
				{
					int id = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
					for (int j=currentModuleDescriptor->getDataStreamDescriptorCount()-1;i>=0;i--)
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
    return nuiFactory::getInstance()->getDescriptor(getCurrent()->getName());
}

nuiModuleDescriptor *nuiFrameworkManager::getPipeline(std::string &pipelineName)
{
	return nuiFactory::getInstance()->getDescriptor(pipelineName);
}

nuiModuleDescriptor *nuiFrameworkManager::getModule(std::string &pipelineName, int index)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	for (int i=0;i<pipelineDescriptor->getChildModulesCount();i++)
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
	return nuiFactory::getInstance()->getDescriptor(moduleName);
}

nuiEndpointDescriptor *nuiFrameworkManager::getInputEndpoint(std::string &pipelineName,int index)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	if (pipelineDescriptor->getInputEndpointsCount() <= index)
		return NULL;
	return pipelineDescriptor->getInputEndpointDescriptor(index);
}

nuiEndpointDescriptor *nuiFrameworkManager::getOutputEndpoint(std::string &pipelineName,int index)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	if (pipelineDescriptor->getOutputEndpointsCount() <= index)
		return NULL;
	return pipelineDescriptor->getOutputEndpointDescriptor(index);
}

nuiDataStreamDescriptor *nuiFrameworkManager::getConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	for (int i=0;i<pipelineDescriptor->getDataStreamDescriptorCount();i++)
	{
		if ((sourceModuleID == pipelineDescriptor->getDataStreamDescriptor(i)->sourceModuleID) && (destinationModuleID == pipelineDescriptor->getDataStreamDescriptor(i)->destinationModuleID) && (sourcePort == pipelineDescriptor->getDataStreamDescriptor(i)->sourcePort) && (sourcePort == pipelineDescriptor->getDataStreamDescriptor(i)->sourcePort))
			return pipelineDescriptor->getDataStreamDescriptor(i);
	}
	return NULL;
}

nuiFrameworkManagerErrorCode nuiFrameworkManager::deletePipeline(std::string &pipelineName)
{
	if (pipelineName == "root")
		return NUI_FRAMEWORK_ROOT_ACCESS_DENIED;
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NUI_FRAMEWORK_OBJECT_NOT_EXIST;

	std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
	if (rootPipeline!=NULL)
		stack->push_back(rootPipeline);
	while (!stack->empty())
	{
		nuiPipelineModule* currentPipeline = stack->back();
		nuiModuleDescriptor* currentPipelineDescriptor = nuiFactory::getInstance()->getDescriptor(currentPipeline->getName());
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
				nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor("nui" + iter->second->getName() + "Module");
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
	delete stack;

	for (std::map<std::string, nuiModuleDescriptor*>::iterator iter = nuiFactory::getInstance()->pipelineDescriptors.begin(); iter != nuiFactory::getInstance()->pipelineDescriptors.end(); iter++)
	{
		nuiModuleDescriptor *currentModuleDescriptor = iter->second;
		for (int i=currentModuleDescriptor->getChildModulesCount()-1;i>=0;i--)
		{
			if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
			{
				int childModuleID = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
				for (int j=currentModuleDescriptor->getDataStreamDescriptorCount()-1;j>=0;j--)
				{
					if ((currentModuleDescriptor->getDataStreamDescriptor(j)->destinationModuleID == childModuleID) ||
						(currentModuleDescriptor->getDataStreamDescriptor(j)->sourceModuleID == childModuleID))
					{
						currentModuleDescriptor->removeDataStreamDescriptor(currentModuleDescriptor->getDataStreamDescriptor(j));
					}
				}
				currentModuleDescriptor->removeChildModuleDescriptor(currentModuleDescriptor->getChildModuleDescriptor(i));
			}
		}
	}
	nuiFactory::getInstance()->pipelineDescriptors.erase(pipelineName);
	return NUI_FRAMEWORK_MANAGER_OK;
}

nuiModuleDescriptor *nuiFrameworkManager::deleteModule(std::string &pipelineName,int moduleIndex)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
	if (rootPipeline!=NULL)
		stack->push_back(rootPipeline);
	while (!stack->empty())
	{
		nuiPipelineModule* currentPipeline = stack->back();
		nuiModuleDescriptor* currentPipelineDescriptor = nuiFactory::getInstance()->getDescriptor(currentPipeline->getName());
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
				nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
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
//object level not implemented
nuiModuleDescriptor *nuiFrameworkManager::deleteInputEndpoint(std::string &pipelineName,int index)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	if (pipelineDescriptor->getInputEndpointsCount() <= index)
		return NULL;
	nuiEndpointDescriptor* endpointDescriptor = pipelineDescriptor->getInputEndpointDescriptor(index);
	if (endpointDescriptor == NULL)
		return NULL;

	for (std::map<std::string, nuiModuleDescriptor*>::iterator iter = nuiFactory::getInstance()->pipelineDescriptors.begin(); iter != nuiFactory::getInstance()->pipelineDescriptors.end(); iter++)
	{
		nuiModuleDescriptor *currentModuleDescriptor = iter->second;
		for (int i=currentModuleDescriptor->getChildModulesCount()-1;i>=0;i--)
		{
			if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
			{
				int id = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
				for (int j=currentModuleDescriptor->getDataStreamDescriptorCount()-1;i>=0;i--)
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
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	if (pipelineDescriptor->getOutputEndpointsCount() <= index)
		return NULL;
	nuiEndpointDescriptor* endpointDescriptor = pipelineDescriptor->getOutputEndpointDescriptor(index);
	if (endpointDescriptor == NULL)
		return NULL;

	for (std::map<std::string, nuiModuleDescriptor*>::iterator iter = nuiFactory::getInstance()->pipelineDescriptors.begin(); iter != nuiFactory::getInstance()->pipelineDescriptors.end(); iter++)
	{
		nuiModuleDescriptor *currentModuleDescriptor = iter->second;
		for (int i=currentModuleDescriptor->getChildModulesCount()-1;i>=0;i--)
		{
			if (currentModuleDescriptor->getChildModuleDescriptor(i)->getName() == pipelineName)
			{
				int id = currentModuleDescriptor->getChildModuleDescriptor(i)->property("id").asInteger();
				for (int j=currentModuleDescriptor->getDataStreamDescriptorCount()-1;i>=0;i--)
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
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
	if (pipelineDescriptor == NULL)
		return NULL;
	std::list<nuiPipelineModule*>* stack = new std::list<nuiPipelineModule*>();
	if (rootPipeline!=NULL)
		stack->push_back(rootPipeline);
	while (!stack->empty())
	{
		nuiPipelineModule* currentPipeline = stack->back();
		nuiModuleDescriptor* currentPipelineDescriptor = nuiFactory::getInstance()->getDescriptor(currentPipeline->getName());
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
				nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
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
	if (moduleDescriptor->getName() != pipelineName)
	{
		std::string newName = moduleDescriptor->getName();
		while ((nuiFactory::getInstance()->pipelineDescriptors.find(newName)!=nuiFactory::getInstance()->pipelineDescriptors.end()) || (nuiFactory::getInstance()->moduleDescriptors.find(newName)!=nuiFactory::getInstance()->moduleDescriptors.end()))
			newName+="_";
		for (std::map<std::string, nuiModuleDescriptor*>::iterator iter = nuiFactory::getInstance()->pipelineDescriptors.begin(); iter != nuiFactory::getInstance()->pipelineDescriptors.end(); iter++)
		{
			nuiModuleDescriptor *currentModuleDescriptor = iter->second;
			for (int i=currentModuleDescriptor->getChildModulesCount()-1;i>=0;i--)
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
					nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
					if (childDescriptor->getChildModulesCount() > 0)
					{
						stack->push_back((nuiPipelineModule*)iter->second);
					}
				}
			}
		}
		delete stack;
		nuiFactory::getInstance()->pipelineDescriptors.erase(pipelineName);
	}
	nuiFactory::getInstance()->pipelineDescriptors[moduleDescriptor->getName()] = moduleDescriptor;
	return moduleDescriptor;
}

nuiModuleDescriptor *nuiFrameworkManager::updateModule(std::string &pipelineName, int index, nuiModuleDescriptor* moduleDescriptor)
{
	if (moduleDescriptor == NULL)
		return NULL;
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
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
	for (std::map<std::string, nuiProperty*>::iterator iter = moduleDescriptor->getProperties().begin();iter!=moduleDescriptor->getProperties().begin();iter++)
	{
		if (iter->first != "id")
			oldModuleDescriptor->property(iter->first) = iter->second;
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
			for (std::map<std::string, nuiProperty*>::iterator iter = moduleDescriptor->getProperties().begin();iter!=moduleDescriptor->getProperties().begin();iter++)
			{
				if (iter->first != "id")
					updatedModule->property(iter->first) = iter->second;
			}
		}
		else
		{
			for (std::map<int,nuiModule*>::iterator iter = currentPipeline->modules.begin();  iter != currentPipeline->modules.end();iter++)
			{
				nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
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
//!note Changing name of endpoint descriptor type can cause deleting of iner and outer connection between modules.
nuiEndpointDescriptor *nuiFrameworkManager::updateInputEndpoint(std::string &pipelineName,int index, nuiEndpointDescriptor* endpointDescriptor)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
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
	std::vector<std::string>* pipelineNames = nuiFactory::getInstance()->listPipelineNames();
	for (int i = 0;i<pipelineNames->size();i++)
	{
		nuiModuleDescriptor* currentPipelineDescriptor = nuiFactory::getInstance()->getDescriptor((*pipelineNames)[i]);
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
								sourceModule = nuiFactory::getInstance()->getDescriptor(currentPipelineDescriptor->getChildModuleDescriptor(l)->getName());
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
			deleteConnection((*pipelineNames)[i],connectionsToBeDeleted[j]->sourceModuleID,connectionsToBeDeleted[j]->destinationModuleID,
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
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
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
	std::vector<std::string>* pipelineNames = nuiFactory::getInstance()->listPipelineNames();
	for (int i = 0;i<pipelineNames->size();i++)
	{
		nuiModuleDescriptor* currentPipelineDescriptor = nuiFactory::getInstance()->getDescriptor((*pipelineNames)[i]);
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
								sourceModule = nuiFactory::getInstance()->getDescriptor(currentPipelineDescriptor->getChildModuleDescriptor(l)->getName());
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
			deleteConnection((*pipelineNames)[i],connectionsToBeDeleted[j]->sourceModuleID,connectionsToBeDeleted[j]->destinationModuleID,
				connectionsToBeDeleted[j]->sourcePort,connectionsToBeDeleted[j]->destinationPort);
		}
	}
	oldEnpointDescriptor->setDescriptor(endpointDescriptor->getDescriptor());
	return oldEnpointDescriptor;
}

nuiDataStreamDescriptor *nuiFrameworkManager::updateConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort, nuiDataStreamDescriptor *connectionDescriptor)
{
	nuiModuleDescriptor* pipelineDescriptor = nuiFactory::getInstance()->getDescriptor(pipelineName);
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
				nuiModuleDescriptor* childDescriptor = nuiFactory::getInstance()->getDescriptor(iter->second->getName());
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

nuiFrameworkManagerErrorCode nuiFrameworkManager::workflowStart()
{

    nuiPipelineModule* current = getCurrent();
    if(!current->isStarted())
        current->start();

	return NUI_FRAMEWORK_MANAGER_OK;
}
nuiFrameworkManagerErrorCode nuiFrameworkManager::workflowStart(int moduleIndex)
{
    nuiPipelineModule* current = getCurrent();
    nuiModule* selected = current->getChildModuleAtIndex(moduleIndex);
    if(selected == NULL)
        return NUI_FRAMEWORK_OBJECT_NOT_EXIST;

    if(!selected->isStarted())
        selected->start();

    return NUI_FRAMEWORK_MANAGER_OK;
}
nuiFrameworkManagerErrorCode nuiFrameworkManager::workflowStop()
{
    nuiPipelineModule* current = getCurrent();
    if(current->isStarted())
        current->stop();

    return NUI_FRAMEWORK_MANAGER_OK;
}
nuiFrameworkManagerErrorCode nuiFrameworkManager::workflowStop(int moduleIndex)
{
    nuiPipelineModule* current = getCurrent();
    nuiModule* selected = current->getChildModuleAtIndex(moduleIndex);
    if(selected == NULL)
        return NUI_FRAMEWORK_OBJECT_NOT_EXIST;

    if(selected->isStarted())
        selected->start();

    return NUI_FRAMEWORK_MANAGER_OK;
}
nuiFrameworkManagerErrorCode nuiFrameworkManager::workflowQuit()
{
	return NUI_FRAMEWORK_MANAGER_OK;
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
    return nuiFactory::getInstance()->getDescriptor(newCurrent->getName());
}

nuiModuleDescriptor *nuiFrameworkManager::navigatePop( )
{
	if(pathToCurrent.empty())
        return NULL;
    pathToCurrent.pop_back();
    return nuiFactory::getInstance()->getDescriptor(getCurrent()->getName());
}

nuiPipelineModule *nuiFrameworkManager::getCurrent()
{
    nuiPipelineModule* current = rootPipeline;
    std::list<int>::iterator it;
    for (it = pathToCurrent.begin() ; it != pathToCurrent.end() ; it++)
        current = dynamic_cast<nuiPipelineModule*>(current->getChildModuleAtIndex(*it));
    return current;
}