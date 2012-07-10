#ifndef _NUI_PIPELINE_
#define _NUI_PIPELINE_

#include <vector>
#include <string>
#include <map>
#include "nuiModule.h"

#define PIPELINE_ID 0x0FFFFFFF

class nuiFactory;

class nuiPipelineModule : public nuiModule
{
public:
	nuiPipelineModule();
	~nuiPipelineModule();
public:
	void start();
	void stop();
	void update();
	void notifyDataReceived(nuiEndpoint *endpoint);
public:
	void setInputEndpoint(int n, nuiEndpoint *endpoint);
	void setOutputEndpoint(int n, nuiEndpoint *endpoint);
	void setInputEndpointCount(int n);
	void setOutputEndpointCount(int n);
	/*
public:
	void readSettingsFromXML(ofxXmlSettings *xmlSettings);
	void saveSettingsToXML(ofxXmlSettings *xmlSettings);	
private:
	void loadConnections(ofxXmlSettings *xmlSettings);
	*/
public:
	void setName(std::string name);
	void setAuthor(std::string author);
	void setDescription(std::string description);
	void addChildModule(int index, nuiModule* child);
	void removeChildModule(nuiModule* child);
	void removeChildModule(int index);
	int getChildModuleCount();
	nuiModule *getChildModuleAtIndex(int index);
	MODULE_INTERNALS();
protected:
	nuiEndpoint **inputInternalEndpoints;
	nuiEndpoint **outputInternalEndpoints;
	std::map<int,nuiModule*> modules;
	std::string name,description,author;
	friend class nuiFactory;
	friend class nuiFrameworkManager;
};

#endif//_NUI_PIPELINE_