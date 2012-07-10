#include "nuiModule.h"

class nuiPlugin1Module : public ccxModule
{
public:
	nuiPlugin1Module() : ccxModule(){ t = 10; }
	void trigger();
	MODULE_INTERNALS();
};