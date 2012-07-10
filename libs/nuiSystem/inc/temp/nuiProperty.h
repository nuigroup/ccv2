/* nuiProperty.h
*  
*  Created on 02/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/

#ifndef _NUI_PROPERTY_H_
#define _NUI_PROPERTY_H_

#include <vector>
#include <map>
#include "nuiTimer.h"
#include "nuiPropertyCallback.h"


#define CHECK_ABILITY(type,ability)	\
	if (type == propertyType)		\
		return ability;				\

#define IMPLEMENT_CTOR(type,nuiType)																					\
nuiProperty(type &value, const std::string &description = "") : propertyType(nuiType)									\
{																														\
	init();																												\
	this->value = malloc(nuiPropertyInfo<nuiType>::nuiPropertySize());													\
	memcpy(this->value,(const void*) &value, nuiPropertyInfo<nuiType>::nuiPropertySize());								\
	this->description = description;																					\
	lastTimeSet = retreiveTicksCount();																					\
	propertyMetadata = (nuiPropertyMetadata)(propertyMetadata | NUI_METADATA_HAS_VALUE);								\
	valueSize = nuiPropertyInfo<nuiType>::nuiPropertySize();															\
}																														\

#define IMPLEMENT_SETTER(type,nuiType)																												\
nuiPropertyErrorCode setNewValue(type newValue, nuiValueConverterCallback converterCallback = NULL)													\
{																																					\
	return setNewValue((void*)&newValue,nuiType,converterCallback);																					\
}																																					\

#define IMPLEMENT_GETTER(type,nuiType)																												\
nuiPropertyErrorCode getValue(type** newValue, nuiValueConverterCallback converterCallback = NULL)													\
{																																					\
	return getValue((const void**)newValue,nuiType,converterCallback);																				\
}																																					\

typedef enum nuiPropertyErrorCode
{
	NUI_PROPERTY_OK	= 0,
	NUI_PROPERTY_READONLY,
	NUI_PROPERTY_NULL,
	NUI_PROPERTY_NOT_CONVERTED,
	NUI_PROPERTY_CALLBACK_ALREADY_DEFINED,
	NUI_PROPERTY_NULL_CALLBACK,
	NUI_PROPERTY_NOT_EXIST_CALLBACK,
	NUI_PROPERTY_OUT_OF_INTERVAL,
	NUI_PROPERTY_CANT_HAS_VALUE,
	NUI_PROPERTY_IS_READONLY,
	NUI_PROPERTY_CONVERTER_ALREADY_EXIST,
	NUI_PROPERTY_CONVERTER_DONT_EXIST,
	NUI_PROPERTY_HAS_NO_MIN_VALUE,
	NUI_PROPERTY_HAS_NO_MAX_VALUE,
	NUI_PROPERTY_CANT_HAVE_MIN,
	NUI_PROPERTY_CANT_HAVE_MAX,
}nuiPropertyErrorCode;

typedef enum nuiPropertyType
{
	NUI_PROP_VOID = 0,			
	NUI_PROP_INTEGER,			
	NUI_PROP_FLOAT,			
	NUI_PROP_BOOLEAN,			
	NUI_PROP_CHARACTER,		
	NUI_PROP_STRING,
	NUI_PROP_VECTOR2D,
	NUI_PROP_VECTOR3D,
} nuiPropertyType;

typedef struct nuiVector2D
{
public:
	double X,Y;
}nuiVector2D;

typedef struct nuiVector3D
{
public:
	double X,Y,Z;
}nuiVector3D;

template <int nuiPropertyType>
class nuiPropertyInfo
{
public:
	static int nuiPropertySize() { return 0; }
};

#define DECLARE_PROPERTY_INFO( propertyType, propertySize )					\
template<>																	\
class nuiPropertyInfo<propertyType>											\
{																			\
	public:																	\
		static int nuiPropertySize() 	{ return propertySize; }			\
};																			\

DECLARE_PROPERTY_INFO(NUI_PROP_INTEGER,sizeof(int))
DECLARE_PROPERTY_INFO(NUI_PROP_FLOAT,sizeof(double))
DECLARE_PROPERTY_INFO(NUI_PROP_BOOLEAN,sizeof(bool))
DECLARE_PROPERTY_INFO(NUI_PROP_CHARACTER,sizeof(char))
DECLARE_PROPERTY_INFO(NUI_PROP_STRING,sizeof(std::string))
DECLARE_PROPERTY_INFO(NUI_PROP_VECTOR2D,sizeof(nuiVector2D))
DECLARE_PROPERTY_INFO(NUI_PROP_VECTOR3D,sizeof(nuiVector3D))

typedef enum nuiPropertyMetadata
{
	NUI_METADATA_VOID			  = 0x00000000,
	NUI_METADATA_HAS_VALUE		  = 0x00000001,
	NUI_METADATA_READONLY		  = 0x00000002,
	NUI_METADATA_HAS_MINIMUM	  = 0x00000004,
	NUI_METADATA_HAS_MAXIMUM	  = 0x00000008,
} nuiPropertyMetadata;

class nuiProperty;

typedef void (*nuiValueChangedCallback)(const void *value,const nuiProperty *baseProperty);
typedef nuiPropertyErrorCode (*nuiValueConverterCallback)(const void *oldValue, void *newValue);

class nuiProperty
{
public:
	IMPLEMENT_CTOR(int,NUI_PROP_INTEGER)
	IMPLEMENT_CTOR(double,NUI_PROP_FLOAT)
	IMPLEMENT_CTOR(bool,NUI_PROP_BOOLEAN)
	IMPLEMENT_CTOR(char,NUI_PROP_CHARACTER)
	IMPLEMENT_CTOR(nuiVector2D,NUI_PROP_VECTOR2D)
	IMPLEMENT_CTOR(nuiVector3D,NUI_PROP_VECTOR3D)
	IMPLEMENT_CTOR(std::string,NUI_PROP_STRING)
	~nuiProperty();
private:
	nuiProperty(void *value, nuiPropertyType propertyType, const std::string &description = "");
public:
	inline bool getHasMinimumValue();
	inline bool getHasMaximumValue();
	inline nuiPropertyErrorCode setHasMinimumValue(bool hasMinimumValue);
	inline nuiPropertyErrorCode setHasMaximumValue(bool hasMaximumValue);
	inline bool getCanHasMinimumValue();
	inline bool getCanHasMaximumValue();
	IMPLEMENT_GETTER(int,NUI_PROP_INTEGER)
	IMPLEMENT_GETTER(double,NUI_PROP_FLOAT)
	IMPLEMENT_GETTER(bool,NUI_PROP_BOOLEAN)
	IMPLEMENT_GETTER(char,NUI_PROP_CHARACTER)
	IMPLEMENT_GETTER(std::string,NUI_PROP_STRING)
	IMPLEMENT_GETTER(nuiVector2D,NUI_PROP_VECTOR2D)
	IMPLEMENT_GETTER(nuiVector3D,NUI_PROP_VECTOR3D)
	IMPLEMENT_SETTER(int,NUI_PROP_INTEGER)
	IMPLEMENT_SETTER(double,NUI_PROP_FLOAT)
	IMPLEMENT_SETTER(bool,NUI_PROP_BOOLEAN)
	IMPLEMENT_SETTER(char,NUI_PROP_CHARACTER)
	IMPLEMENT_SETTER(std::string,NUI_PROP_STRING)
	IMPLEMENT_SETTER(nuiVector2D,NUI_PROP_VECTOR2D)
	IMPLEMENT_SETTER(nuiVector3D,NUI_PROP_VECTOR3D)
public:
	nuiPropertyErrorCode addValueChangedCallback(nuiValueChangedCallback callback);
	nuiPropertyErrorCode removeValueChangedCallback(nuiValueChangedCallback callback);
	nuiPropertyErrorCode removeAllValueChangedCallback();
	static nuiPropertyErrorCode addValueConverterCallback(nuiPropertyType sourceType, nuiPropertyType destinationType, nuiValueConverterCallback callback);
	static nuiPropertyErrorCode removeValueConverterCallback(nuiPropertyType sourceType, nuiPropertyType destinationType);
	static nuiPropertyErrorCode removeValueConverterCallback(nuiValueConverterCallback callback);
	static nuiPropertyErrorCode removeAllValueConverterCallback();
public:
	inline void setPropertyReadonly(bool isPropertyReadOnly);
	inline void setDescription(const std::string &description);
	inline bool getIsPropertyReadonly();
	unsigned long getPropertyValueMillisecondsAge();
	inline bool getHasValue();
	inline const std::string *getDescription();
private:
	void onValueChanged();
	nuiPropertyErrorCode setNewValue(void *newValue, nuiPropertyType newValueType, nuiValueConverterCallback converterCallback = NULL);
	nuiPropertyErrorCode getValue(const void **returnValue, nuiPropertyType returnValueType,  nuiValueConverterCallback converterCallback = NULL);
	static void init();
private:
	std::string description;
	void* value;
	int valueSize;
	unsigned long lastTimeSet;
	nuiPropertyMetadata propertyMetadata;
	nuiPropertyType propertyType;
	std::vector<nuiValueChangedCallback> valueChangedCallbacks;
	static bool isInitialized;
	static std::map<std::pair<nuiPropertyType,nuiPropertyType>,nuiValueConverterCallback> valueConverterCallback;
};

#endif//_NUI_PROPERTY_H_ 