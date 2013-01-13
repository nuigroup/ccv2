#include "nuiFramework.h"

int main()
{
  nuiPluginManager::getInstance().loadLibrary("modules\\nuiGaussianBlurFilter.dll");
  auto pm = nuiPluginManager::getInstance();
  return 0;
}