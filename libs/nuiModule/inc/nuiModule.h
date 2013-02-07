/** 
 * \file      nuiModule.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2011 NUI Group. All rights reserved.
 */

#ifndef NUI_MODULE_H
#define NUI_MODULE_H

#include <string>
#include <map>
#include "nuiProperty.h"
#include "pasync.h"

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
	std::string nui##name##::getName() { return module_name; } \
	std::string nui##name##::getDescription() { return module_description; } \
	std::string nui##name##::getAuthor() { return module_author; } \

#define MODULE_INIT() \
	this->properties["id"] = new nuiProperty(nuiUtils::getRandomNumber()); \
	LOG(NUI_DEBUG, "create object <" << module_name << "> with id <" \
			   << this->property("id").asString() << ">"); \

#define MODULE_INTERNALS() \
	public: \
	virtual std::string getName(); \
	virtual std::string getDescription(); \
	virtual std::string getAuthor(); \

class nuiEndpointDescriptor;

//! holds information about module like children modules, endpoints, properties and so on
class nuiModuleDescriptor
{
public:
    //! no checks, just add this descriptor
	void addChildModuleDescriptor(nuiModuleDescriptor* moduleDescriptor);
    //! remove from list if exists
	void removeChildModuleDescriptor(nuiModuleDescriptor* moduleDescriptor);
    //! gets count of child modules
    int getChildModulesCount();
    //! gets child module at specified index
    nuiModuleDescriptor* getChildModuleDescriptor(int index);

    //! no checks, just add this connection
	void addDataStreamDescriptor(nuiDataStreamDescriptor* connection);
    //! remove from list if exists
	void removeDataStreamDescriptor(nuiDataStreamDescriptor* connection);
    //! gets count of attached connetion descriptors
    int getDataStreamDescriptorCount();
    //! return speicfied connection descriptor
    nuiDataStreamDescriptor* getDataStreamDescriptor(int index);
	
	int getInputEndpointsCount();
	int getOutputEndpointsCount();
    
    //! TODO : can we do this in module??? should be moved to pipeline.
	void setInputEndpointsCount(int count);
	void setOutputEndpointsCount(int count);

    //! TODO : wtf ?!?! set any provided index to endpoint descriptor, add endpoint to list
	void addInputEndpointDescriptor(nuiEndpointDescriptor* descriptor, int index);
    //! TODO : wtf ?!?! set any provided index to endpoint descriptor, add endpoint to list
	void addOutputEndpointDescriptor(nuiEndpointDescriptor* descriptor, int index);

	void removeInputEndpointDescriptor(nuiEndpointDescriptor* descriptor);
	void removeOutputEndpointDescriptor(nuiEndpointDescriptor* descriptor);
	nuiEndpointDescriptor *getInputEndpointDescriptor(int index);
	nuiEndpointDescriptor *getOutputEndpointDescriptor(int index);

    //! update module name
	void setName(std::string name);
    //! update module description
	void setDescription(std::string description);
    //! update module author
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
	std::string name, description, author;
	std::map<std::string, nuiProperty*> properties;
	bool isPipeline;
};

//! common interface for modules
class nuiModule
{
public:
  //! basic constructor, creates module with basic properties
	nuiModule();

  //! destructor
	virtual ~nuiModule();

  //! create thread based on properties and start it
	virtual void start();
  //! stop and delete worker thread, stop all associated datastreams
	virtual void stop();

  //! module update function (data processing)
	virtual void update() = 0;

  //! forced update triggered by internal_oscillator thread or notifyDataReceived
  //! sets need_update flag
	void trigger();

  //! notify that endpoint received some data. Force trigger.
	virtual void notifyDataReceived(nuiEndpoint *endpoint);

  //! check whether module requires update. If requires - uncheck need_update
	bool needUpdate(/*bool isAsyncMode = false*/);

	virtual void setInputEndpointCount(int n);
	virtual void setOutputEndpointCount(int n);
  virtual void setInputEndpoint(int n, nuiEndpoint *endpoint);
  virtual void setOutputEndpoint(int n, nuiEndpoint *endpoint);

	int getInputEndpointCount();
	int getOutputEndpointCount();

	nuiEndpoint *getInputEndpoint(int n);
	nuiEndpoint *getOutputEndpoint(int n);
	int getInputEndpointIndex(nuiEndpoint *stream);
	int getOutputEndpointIndex(nuiEndpoint *stream);

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
  //! module properties
  std::map<std::string, nuiProperty*> properties;

  //! started
	bool is_started;
  //! update should be called
  bool need_update;
  
  //! update type : should run in separate thread and update only on input received
  bool use_thread;
  //! update type : should provide constant periodical updates
  bool oscillator_mode;
  //! update type : update should happen only when all endpoints are filled with data
  bool is_synced_input;

  //! to calculate module processing time fps
	nuiTimer *timer;

  //! worker thread
	nuiThread *thread;

	pt::mutex *mtx;

	int inputEndpointCount;
  nuiEndpoint **inputEndpoints;
  
  int outputEndpointCount;
  nuiEndpoint **outputEndpoints;

  //! update period for oscillator_mode
	int ocsillatorWait;
	
  //! array, used to check whether all endpoints received data (when using is_synced_input).
  //! endpoint will have non-zero value when is filled with data.
	char* inputDataReceived;
};

#endif

