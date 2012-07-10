
#include <vector>
#include <queue>
#include <string>
#include <map>
#include "pasync.h"
#include "nuiThread.h"
#include "nuiUtils.h"
#include "nuiEndpoint.h"
#include "nuiDataStream.h"

int main()
{
	nuiEndpoint* sourceEndpoint = new nuiEndpoint();
	sourceEndpoint->setTypeDescriptor(std::string("string"));
	std::vector<nuiEndpoint*> destinationEndpoint;
	for (int i=0;i<4;i++)
	{
		destinationEndpoint.push_back(new nuiEndpoint());
		destinationEndpoint[i]->setTypeDescriptor(std::string("string2"));
		nuiDataStream *dataStream = sourceEndpoint->addConnection(destinationEndpoint[i]);
		if (dataStream!=NULL)
			dataStream->startStream();
	}
	printf("number of connections is %i\n",sourceEndpoint->getConnectionCount());
	int a = 12;
	//sourceEndpoint->setData((void*)&a,sizeof(int));
	sourceEndpoint->transmitData();
	for (int i=0;i<4;i++)
	{
		int size = 0;
		//printf("received data for %i is %i\n",i, *((int*)destinationEndpoint[i]->getData(size)));
	}
	return 0;
} 

