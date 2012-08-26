#include "nuiModule.h"
#include <windows.h> 

typedef ccxModule* (*GetCCXModule)();

int main()
{
	HINSTANCE lib = LoadLibrary( "addons/Plugin1.dll" );
	GetCCXModule func = (GetCCXModule)GetProcAddress(lib,"factory_create_module");
	ccxModule* a = func();
	a->declareInput(10,NULL,NULL);
	FreeLibrary(lib);
	return 0;
}