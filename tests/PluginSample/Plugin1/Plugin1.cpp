#include "Plugin1.h"

MODULE_DECLARE(Plugin1,"Anatoly","Module One");

void nuiPlugin1Module::trigger()
{
	t++;
	printf("Hello form Plugin 1\n");
}