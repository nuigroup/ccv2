#include "nuiPluginEntity.h"
#include <memory>
namespace nuiPluginFramework
{
	nuiPluginEntity::nuiPluginEntity(const char* entityName,void* wrappedEntity, nuiReleaseInterface releaseInterface)
	{
		this->entity = wrappedEntity;
		this->releaseInterfaceFunc = releaseInterface;
		this->name = entityName;
		initialized = (wrappedEntity!=NULL);
	}
	nuiPluginEntity::~nuiPluginEntity()
	{
		if ((releaseInterfaceFunc!=NULL) && (initialized))
		{
			releaseInterfaceFunc(entity);
			initialized = false;
		}
	}
}