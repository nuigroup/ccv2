/////////////////////////////////////////////////////////////////////////////
// Name:        nuiModule.h
// Author:      Anatoly Churikov
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_MODULE_H
#define NUI_MODULE_H

#include <string>
#include <map>
#include "nuiProperty.h"
#include "pasync.h"

#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif

#include "ofxXmlSettings.h"
#include "nuiTimer.h"
#include "nuiThread.h"
#include "nuiEndpoint.h"

class nuiDataStream;
class nuiEndpoint;
class nuiModule;

#define MODULE_DECLARE(name, author, description) \
	LOG_DECLARE(#name); \
	MODULE_DECLARE_EX(name, Module, author, description);

typedef nuiModule* (*nuiFactoryCreateCallback)();

#define MODULE_DECLARE_EX(name, module, author, description) \
	static std::string module_name = #name; \
	static std::string module_author = author; \
	static std::string module_description = description; \
	std::string nui##name##module::getName() { return module_name; } \
	std::string nui##name##module::getDescription() { return module_description; } \
	std::string nui##name##module::getAuthor() { return module_author; } \

#define MODULE_INIT() \
	this->properties["id"] = new nuiProperty(nuiUtils::getRandomNumber()); \
	LOG(NUI_DEBUG, "create object <" << module_name << "> with id <" \
			   << this->property("id").asString() << ">");

#define MODULE_INTERNALS() 					\
	public:									\
	virtual std::string getName(); 			\
	virtual std::string getDescription(); 	\
	virtual std::string getAuthor();

class nuiEndpointDescriptor;

class nuiModuleDescriptor
{
public:	
	void addChildModuleDescriptor(nuiModuleDescriptor* moduleDescriptor);
	void removeChildModuleDescriptor(nuiModuleDescriptor* moduleDescriptor);
	void addDataStreamDescriptor(nuiDataStreamDescriptor* connection);
	void removeDataStreamDescriptor(nuiDataStreamDescriptor* connection);
	int getDataStreamDescriptorCount();
	nuiDataStreamDescriptor* getDataStreamDescriptor(int index);

	nuiModuleDescriptor* getChildModuleDescriptor(int index);
	int getChildModulesCount();
	int getInputEndpointsCount();
	int getOutputEndpointsCount();
	void setInputEndpointsCount(int count);
	void setOutputEndpointsCount(int count);
	void addInputEndpointDescriptor(nuiEndpointDescriptor* descriptor,int index);
	void addOutputEndpointDescriptor(nuiEndpointDescriptor* descriptor,int index);
	void removeInputEndpointDescriptor(nuiEndpointDescriptor* descriptor);
	void removeOutputEndpointDescriptor(nuiEndpointDescriptor* descriptor);
	nuiEndpointDescriptor *getInputEndpointDescriptor(int index);
	nuiEndpointDescriptor *getOutputEndpointDescriptor(int index);
	//void setIsPipeline(bool isPipeline);
	//bool getIsPipeline(); do these belong here?
	void setName(std::string name);
	void setDescription(std::string description);
	void setAuthor(std::string author);
	std::string getName(); 
	std::string getDescription(); 
	std::string getAuthor();
    nuiProperty &property(std::string name);
	std::map<std::string, nuiProperty*> &getProperties();

private:
	std::vector<nuiEndpointDescriptor*> inputDescriptions;
	std::vector<nuiEndpointDescriptor*> outputDescriptions;
	std::vector<nuiModuleDescriptor*> childrenModuleDescriptions;
	std::vector<nuiDataStreamDescriptor*> connectionDescriptors;
	std::string name,description,author;
	std::map<std::string, nuiProperty*> properties;
	bool isPipeline;
};

class nuiModule
{
public:	
	nuiModule();
	virtual ~nuiModule();
public:
	virtual void start();
	virtual void stop();
	virtual void update() = 0;
	void trigger();
	virtual void notifyDataReceived(nuiEndpoint *endpoint);
	bool needUpdate(bool isAsyncMode = false);
public:
	bool hasInputEnpoints();
	bool hasOutputEnpoints();
	virtual void setInputEndpointCount(int n);
	virtual void setOutputEndpointCount(int n);
	int getInputEndpointCount();
	int getOutputEndpointCount();
	virtual void setInputEndpoint(int n, nuiEndpoint *endpoint);
	virtual void setOutputEndpoint(int n, nuiEndpoint *endpoint);
	nuiEndpoint *getInputEndpoint(int n=0);
	nuiEndpoint *getOutputEndpoint(int n=0);
	int getInputEndpointIndex(nuiEndpoint *stream);
	int getOutputEndpointIndex(nuiEndpoint *stream);
public:
	nuiProperty &property(std::string name);
	std::map<std::string, nuiProperty*> &getProperties();
	bool isStarted();
	bool isOscillatorMode();
	bool isSyncedInput();
	virtual std::string getName() = 0;
	virtual std::string getDescription() = 0;
	virtual std::string getAuthor() = 0;
private:
	static void thread_process(nuiThread *thread);
	static void internal_oscillator(nuiThread *thread);
protected:
	bool is_started, use_thread, need_update, oscillator_mode, is_synced_input;
	nuiTimer *timer;
	nuiThread *thread;
	pt::mutex *mtx;
	std::map<std::string, nuiProperty*> properties;
	int inputEndpointCount,outputEndpointCount;
	int ocsillatorWait;
	nuiEndpoint **inputEndpoints;
	char *inputDataReceived;
	nuiEndpoint **outputEndpoints;
	friend class nuiDataStream;
	friend class nuiEndpoint;
};

#endif

