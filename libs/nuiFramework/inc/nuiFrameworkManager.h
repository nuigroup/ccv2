#ifndef _NUI_FRAMEWORK_MANAGER_
#define _NUI_FRAMEWORK_MANAGER_

#include <string>
#include <vector>
#include <map>
#include <list>

#include "nuiProperty.h"
#include "pasync.h"
#include "ofxXmlSettings.h"
#include "nuiTimer.h"
#include "nuiThread.h"
#include "nuiEndpoint.h"
#include "nuiTree.h"



typedef enum nuiFrameworkManagerErrorCode
{
	NUI_FRAMEWORK_MANAGER_OK,
	NUI_FRAMEWORK_WRONG_FILE,
	NUI_FRAMEWORK_PIPELINE_STRUCTURE_LOOP,
    NUI_FRAMEWORK_ERROR_SAVING_FILE,
	NUI_FRAMEWORK_ROOT_ACCESS_DENIED,
	NUI_FRAMEWORK_OBJECT_NOT_EXIST,
	NUI_FRAMEWORK_ROOT_INITIALIZATION_FAILED,
} nuiFrameworkManagerErrorCode;

//typedef 

struct nuiModuleRuntimeDescriptor
{
public:
	double averageFPS;
	double averageWaitTime;
	double averageProcessTime;
	bool isRunning;
};

struct nuiDataStreamRuntimeDescriptor
{
public:
	bool isRunning;
};

class nuiModule;
class nuiModuleDescriptor;
class nuiPipelineModule;

class nuiFrameworkManager
{
public:
	~nuiFrameworkManager();
	static nuiFrameworkManager *getInstance();
private:
	nuiFrameworkManager();
public:
	nuiFrameworkManagerErrorCode initializeFrameworkManager(const char *fileName);
	nuiFrameworkManagerErrorCode loadAddonsAtPath(const char *addonsPath);
	std::vector<std::string> *listDynamicModules();
	std::vector<std::string> *listPipelines(std::string &hostedPipelineName);
    
    //! starts the current pipeline workflow
	nuiFrameworkManagerErrorCode workflowStart();

    //! starts module index workflow at the current pipeline
    nuiFrameworkManagerErrorCode workflowStart(int moduleIndex);
    
    //! stops the current pipeline workflow
	nuiFrameworkManagerErrorCode workflowStop();

    //! starts module index workflow at the current pipeline
    nuiFrameworkManagerErrorCode workflowStop(int moduleIndex);

	nuiFrameworkManagerErrorCode workflowQuit();


public:
	nuiModuleDescriptor *createPipeline(std::string &pipelineName);
	nuiModuleDescriptor *createModule(std::string &pipelineName,std::string &moduleName);
	nuiDataStreamDescriptor *createConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort);
	int setInputEndpointCount(std::string &pipelineName,int count);
	int setOutputEndpointCount(std::string &pipelineName,int count);
public:
    nuiModuleDescriptor *getCurrentPipeline();
	nuiModuleDescriptor *getPipeline(std::string &pipelineName);
	nuiModuleDescriptor *getModule(std::string &pipelineName, int index);
	nuiModuleDescriptor *getModule(std::string &moduleName);
	nuiEndpointDescriptor *getInputEndpoint(std::string &pipelineName,int index);
	nuiEndpointDescriptor *getOutputEndpoint(std::string &pipelineName,int index);
	nuiDataStreamDescriptor *getConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort);
public:
	nuiFrameworkManagerErrorCode deletePipeline(std::string &pipelineName);
	nuiModuleDescriptor *deleteModule(std::string &pipelineName,int moduleIndex);
	nuiModuleDescriptor *deleteInputEndpoint(std::string &pipelineName,int index);
	nuiModuleDescriptor *deleteOutputEndpoint(std::string &pipelineName,int index);
	nuiModuleDescriptor *deleteConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID, int sourcePort, int destinationPort);
public:
	nuiModuleDescriptor *updatePipeline(std::string &pipelineName, nuiModuleDescriptor* moduleDescriptor);
	nuiModuleDescriptor *updateModule(std::string &pipelineName, int index, nuiModuleDescriptor* moduleDescriptor);
	nuiEndpointDescriptor *updateInputEndpoint(std::string &pipelineName,int index, nuiEndpointDescriptor* endpointDescriptor);
	nuiEndpointDescriptor *updateOutputEndpoint(std::string &pipelineName,int index, nuiEndpointDescriptor* endpointDescriptor);
	nuiDataStreamDescriptor *updateConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID,int sourcePort, int destinationPort, nuiDataStreamDescriptor *connectionDescriptor);
//	nuiTreeNode<nuiModuleDescriptor,nuiModuleRuntimeDescriptor>* getRuntimeRootDescriptor();
public:
    //! dives into moduleIndex pipeline of the currect pipeline
    //! \returns new current pipeline descriptor, NULL if action failed
    nuiModuleDescriptor *navigatePush(int moduleIndex);
    //! returns back to upper pipeline
    //! \returns new current pipeline descriptor, NULL if action failed
    nuiModuleDescriptor *navigatePop( );
public:
    nuiFrameworkManagerErrorCode saveSettingsAsXml(const char* fileName, std::string& pipelineName);
private:
    //gets currently selected pipeline
    nuiPipelineModule *getCurrent();
    //list of child indexes to the current pipeline
    std::list<int> pathToCurrent;
private:
	nuiFrameworkManagerErrorCode saveSettingsToXml(const char *fileName, std::list<nuiModuleDescriptor*>* descriptors);
	nuiFrameworkManagerErrorCode loadSettingsFromXml(const char *fileName);
	nuiFrameworkManagerErrorCode saveSettingsToXml(ofxXmlSettings *xmlSettings, std::list<nuiModuleDescriptor*>* descriptors);
	nuiFrameworkManagerErrorCode loadSettingsFromXml(ofxXmlSettings *xmlSettings);
	nuiModuleDescriptor *parseModuleDescriptor(ofxXmlSettings *xmlSettings);
	void parseModuleDescriptorParameters(nuiModuleDescriptor &moduleDescriptor, ofxXmlSettings *xmlSettings);
private:
    nuiModule* currentModule;
	nuiPipelineModule *rootPipeline;
	nuiTree<int,nuiModule*> *dataObjectTree;
};

#endif//_NUI_FRAMEWORK_MANAGER_