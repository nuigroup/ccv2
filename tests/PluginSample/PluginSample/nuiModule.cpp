#include "ccxDataStream.h"
#include "ccxDataStreamInfo.h"
#include "nuiModule.h"

	ccxModule::ccxModule(unsigned int capabilities)
	{
		t = capabilities;
	}
	void ccxModule::declareInput(int n, ccxDataStream **storage, ccxDataStreamInfo *info)
	{
	//	(*storage)->a = 0;
	//	info->b = 0;
		t--;
	}
	void ccxModule::declareOutput(int n, ccxDataStream **storage, ccxDataStreamInfo *info)
	{
	//	(*storage)->a = 0;
	//	info->b = 0;
		t++;
	}
