#include "nuiPluginManager.h"

int main()
{
  nuiPluginManager pm = nuiPluginManager::getInstance();
  pm.loadLibrary("modules\\nuiGaussianBlurFilter.dll");
  return 0;
}