#include "nuiDefines.h"
#include <memory>

char * enumGetValue(int v, char *tmp)
{
	if (v < 0)
		return NULL;
	int index = 0;
	char *previous = tmp;
	char *pch = tmp;
	do 
	{
		if (pch!=NULL)
		{
			previous = pch;
			pch=strchr(pch,'|')+1;
			if (v == index)
				break;
			index++;
		}
	}
	while (pch != NULL);
 	if ((v == 0) && (pch-1 == NULL))
		return tmp;
	char* result = (char*)malloc((pch - previous) * sizeof(char));
	memset(result,0x00,(pch - previous) * sizeof(char));
	memcpy(result,previous,pch - previous - 1);
	return result;
}

