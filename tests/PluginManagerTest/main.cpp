#include "nuiFramework.h"

int main()
{
  nuiPluginManager& pm = nuiPluginManager::getInstance();
  pm.loadLibrary("modules\\nuiGaussianBlurFilter.dll");
  pm.unloadLibrary("modules\\nuiGaussianBlurFilter.dll");
  pm.loadDefaultConfiguration();
  return 0;
}