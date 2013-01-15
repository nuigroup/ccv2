/** 
 * \file      nuiPipeline.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \author    Scott Halstvedt
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#ifndef NUI_PIPELINE_H
#define NUI_PIPELINE_H

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

#endif// NUI_PIPELINE_H