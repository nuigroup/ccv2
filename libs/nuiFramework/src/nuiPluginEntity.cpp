#include "nuiPluginEntity.h"
#include <memory>

nuiPluginEntity::nuiPluginEntity(const char* entityName, void* wrappedEntity, nuiReleaseInterface releaseInterface)
{
	this->entity = wrappedEntity;
	this->releaseInterfaceFunc = releaseInterface;
	this->name = entityName;
    //! TODO : is this good check? 03.12.12 AnatolyL
	initialized = (wrappedEntity != NULL);
};

nuiPluginEntity::~nuiPluginEntity()
{
	if ((releaseInterfaceFunc!=NULL) && (initialized))
	{
		releaseInterfaceFunc(entity);
		initialized = false;
	}
};
