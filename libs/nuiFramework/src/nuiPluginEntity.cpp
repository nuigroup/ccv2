#include "nuiPluginEntity.h"
#include <memory>

nuiPluginEntity::nuiPluginEntity(const char* entityName, void* wrappedEntity, nuiDeallocateFunc releaseInterface)
{
	this->entity = wrappedEntity;
	this->deallocateFunc = releaseInterface;
	this->name = entityName;
  //! TODO : is this good check? 03.12.12 AnatolyL
	initialized = (wrappedEntity != NULL);
};

nuiPluginEntity::~nuiPluginEntity()
{
	if ((deallocateFunc!=NULL) && (initialized))
	{
		deallocateFunc(entity);
		initialized = false;
	}
};
