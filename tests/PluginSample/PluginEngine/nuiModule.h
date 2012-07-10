#ifndef NUI_MODULE_H
#define NUI_MODULE_H

#include <string>
#include <map>
#include <vector>

class ccxDataStream;
class ccxDataStreamInfo;

typedef struct 
{
	double average_fps;
	double average_process_time;
	double average_wait_time;
	double total_process_time;
	double total_wait_time;
	unsigned long long total_process_frame;
	unsigned long long _process_frame;
	double _process_time;
	double _wait_time;
	double _last_time;
} ccx_module_stats_t;


#define MODULE_DECLARE(name, author, description) \
	MODULE_DECLARE_EX(name, Module, author, description);

#define MODULE_DECLARE_EX(name, module, author, description) \
	static std::string module_name = #name; \
	static std::string module_author = author; \
	static std::string module_description = description; \
	std::string nui##name##module::getName() { return module_name; } \
	std::string nui##name##module::getDescription() { return module_description; } \
	std::string nui##name##module::getAuthor() { return module_author; } \
	extern "C" __declspec( dllexport )	ccxModule *factory_create_module() { return new nui##name##module(); }

#define MODULE_INIT() ;

#define MODULE_INTERNALS() 					\
	public:									\
	virtual std::string getName(); 			\
	virtual std::string getDescription(); 	\
	virtual std::string getAuthor();

enum {
	CCX_MODULE_NONE		= 0x00000000,		/*< Module have no input/output */
	CCX_MODULE_INPUT	= 0x00000001,		/*< Module have inputs */
	CCX_MODULE_OUTPUT	= 0x00000002,		/*< Module have outputs */
	CCX_MODULE_GUI      = 0x00000004,		/*< Module have a GUI */
};

class ccxModule 
{
public:	
	ccxModule() {}
	ccxModule(unsigned int capabilities);
    virtual void trigger() = 0;
	void declareInput(int n, ccxDataStream **storage, ccxDataStreamInfo *info);
	void declareOutput(int n, ccxDataStream **storage, ccxDataStreamInfo *info);
	virtual std::string getName() = 0;
	virtual std::string getDescription() = 0;
	virtual std::string getAuthor() = 0;
	int t;
protected:
	friend class ccxDataStream;
};

#endif

