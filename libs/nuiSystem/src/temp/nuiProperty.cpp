#include "nuiProperty.h"
#include <memory>
#include <string>

char * MyGetValue(int v, char *tmp)
{
	if (v < 0)
		return NULL;
	int index = 0;
	char *previous = tmp;
	char *pch = tmp;
	do 
	{
		if (pch!=NULL)
		{
			previous = pch;
			pch=strchr(pch,'|')+1;
			if (v == index)
				break;
			index++;
		}
	}
	while (pch != NULL);
 	if ((v == 0) && (pch-1 == NULL))
		return tmp;
	char* result = (char*)malloc((pch - previous) * sizeof(char));
	memset(result,0x00,(pch - previous) * sizeof(char));
	memcpy(result,previous,pch - previous - 1);
	return result;
}

std::map<std::pair<nuiPropertyType,nuiPropertyType>,nuiValueConverterCallback> nuiProperty::valueConverterCallback;
bool nuiProperty::isInitialized = false; 

void nuiProperty::init()
{
	if (isInitialized)
		return;
	SET_VALUE_CONVERTER(NUI_PROP_STRING,NUI_PROP_BOOLEAN,stringToBoolConverter)
	SET_VALUE_CONVERTER(NUI_PROP_INTEGER,NUI_PROP_BOOLEAN,integerToBoolConverter)
	SET_VALUE_CONVERTER(NUI_PROP_FLOAT,NUI_PROP_BOOLEAN,floatToBoolConverter)
	SET_VALUE_CONVERTER(NUI_PROP_CHARACTER,NUI_PROP_BOOLEAN,charToBoolConverter)
	SET_VALUE_CONVERTER(NUI_PROP_BOOLEAN,NUI_PROP_STRING,boolToStringConverter)
	SET_VALUE_CONVERTER(NUI_PROP_INTEGER,NUI_PROP_STRING,integerToStringConverter)
	SET_VALUE_CONVERTER(NUI_PROP_FLOAT,NUI_PROP_STRING,floatToStringConverter)
	SET_VALUE_CONVERTER(NUI_PROP_CHARACTER,NUI_PROP_STRING,charToStringConverter)
	SET_VALUE_CONVERTER(NUI_PROP_VECTOR2D,NUI_PROP_STRING,vector2DToStringConverter)
	SET_VALUE_CONVERTER(NUI_PROP_VECTOR3D,NUI_PROP_STRING,vector3DToStringConverter)
	SET_VALUE_CONVERTER(NUI_PROP_BOOLEAN,NUI_PROP_FLOAT,boolToFloatConverter)
	SET_VALUE_CONVERTER(NUI_PROP_INTEGER,NUI_PROP_FLOAT,integerToFloatConverter)
	SET_VALUE_CONVERTER(NUI_PROP_STRING,NUI_PROP_FLOAT,stringToFloatConverter)
	SET_VALUE_CONVERTER(NUI_PROP_BOOLEAN,NUI_PROP_INTEGER,boolToIntegerConverter)
	SET_VALUE_CONVERTER(NUI_PROP_STRING,NUI_PROP_INTEGER,stringToIntegerConverter)
	SET_VALUE_CONVERTER(NUI_PROP_FLOAT,NUI_PROP_INTEGER,floatToIntegerConverter)
	SET_VALUE_CONVERTER(NUI_PROP_BOOLEAN,NUI_PROP_CHARACTER,boolToCharConverter)
	SET_VALUE_CONVERTER(NUI_PROP_STRING,NUI_PROP_CHARACTER,stringToCharConverter)
	SET_VALUE_CONVERTER(NUI_PROP_STRING,NUI_PROP_VECTOR2D,stringToVector2DConverter)
	SET_VALUE_CONVERTER(NUI_PROP_STRING,NUI_PROP_VECTOR3D,stringToVector3DConverter)
	isInitialized = true;
}

nuiProperty::nuiProperty(void *value, nuiPropertyType propertyType, const std::string &description)
{
	this->value = value;
	this->propertyType = propertyType;
	this->description = description;
	lastTimeSet = retreiveTicksCount();
	if (value!=NULL)
		propertyMetadata = (nuiPropertyMetadata)(propertyMetadata | NUI_METADATA_HAS_VALUE);
}

nuiProperty::~nuiProperty()
{
}

inline bool nuiProperty::getHasMinimumValue()
{
	if (getCanHasMinimumValue())
		return propertyMetadata & NUI_METADATA_HAS_MINIMUM;
	return false;
}

inline bool nuiProperty::getHasMaximumValue()
{
	if (getCanHasMaximumValue())
		return propertyMetadata & NUI_METADATA_HAS_MAXIMUM;
	return false;
}

inline bool nuiProperty::getCanHasMinimumValue()
{
	CHECK_ABILITY(NUI_PROP_VOID,false)
	CHECK_ABILITY(NUI_PROP_INTEGER,true)
	CHECK_ABILITY(NUI_PROP_FLOAT,true)
	CHECK_ABILITY(NUI_PROP_BOOLEAN,true)
	CHECK_ABILITY(NUI_PROP_CHARACTER,true)
	CHECK_ABILITY(NUI_PROP_STRING,false)
	CHECK_ABILITY(NUI_PROP_VECTOR2D,false)
	CHECK_ABILITY(NUI_PROP_VECTOR3D,false)
	return false;
}

inline bool nuiProperty::getCanHasMaximumValue()
{
	CHECK_ABILITY(NUI_PROP_VOID,false)
	CHECK_ABILITY(NUI_PROP_INTEGER,true)
	CHECK_ABILITY(NUI_PROP_FLOAT,true)
	CHECK_ABILITY(NUI_PROP_BOOLEAN,true)
	CHECK_ABILITY(NUI_PROP_CHARACTER,true)
	CHECK_ABILITY(NUI_PROP_STRING,false)
	CHECK_ABILITY(NUI_PROP_VECTOR2D,false)
	CHECK_ABILITY(NUI_PROP_VECTOR3D,false)
	return false;
}

inline nuiPropertyErrorCode nuiProperty::setHasMinimumValue(bool hasMinimumValue)
{
	if (getCanHasMinimumValue())
	{
		if (getHasMinimumValue() != hasMinimumValue)
			propertyMetadata = (nuiPropertyMetadata)(propertyMetadata ^ NUI_METADATA_HAS_MINIMUM);
		return NUI_PROPERTY_OK;
	}
	return NUI_PROPERTY_CANT_HAVE_MIN;
}

inline nuiPropertyErrorCode nuiProperty::setHasMaximumValue(bool hasMaximumValue)
{
	if (getCanHasMaximumValue())
	{
		if (getHasMaximumValue() != hasMaximumValue)
			propertyMetadata = (nuiPropertyMetadata)(propertyMetadata ^ NUI_METADATA_HAS_MAXIMUM);
		return NUI_PROPERTY_OK;
	}
	return NUI_PROPERTY_CANT_HAVE_MAX;
}

nuiPropertyErrorCode nuiProperty::addValueChangedCallback(nuiValueChangedCallback callback)
{
	for (std::vector<nuiValueChangedCallback>::iterator iter = valueChangedCallbacks.begin(); iter != valueChangedCallbacks.end(); iter++)
	{
		if (callback == *iter)
			return NUI_PROPERTY_CALLBACK_ALREADY_DEFINED;
	}
	valueChangedCallbacks.push_back(callback);
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode nuiProperty::removeValueChangedCallback(nuiValueChangedCallback callback)
{
	for (std::vector<nuiValueChangedCallback>::iterator iter = valueChangedCallbacks.begin(); iter != valueChangedCallbacks.end(); iter++)
	{
		if (callback == *iter)
		{
			valueChangedCallbacks.erase(iter);
			return NUI_PROPERTY_OK;
		}
	}
	return NUI_PROPERTY_NOT_EXIST_CALLBACK;
}

nuiPropertyErrorCode nuiProperty::removeAllValueChangedCallback()
{
	valueChangedCallbacks.clear();
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode nuiProperty::addValueConverterCallback(nuiPropertyType sourceType, nuiPropertyType destinationType, nuiValueConverterCallback callback)
{
	for (std::map<std::pair<nuiPropertyType,nuiPropertyType>,nuiValueConverterCallback>::iterator iter = valueConverterCallback.begin(); iter != valueConverterCallback.end(); iter++)
	{
		if ((sourceType == iter->first.first) && (destinationType == iter->first.second))
			return NUI_PROPERTY_CALLBACK_ALREADY_DEFINED;
	}
	valueConverterCallback[std::pair<nuiPropertyType,nuiPropertyType>(sourceType,destinationType)] = callback;
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode nuiProperty::removeValueConverterCallback(nuiPropertyType sourceType, nuiPropertyType destinationType)
{
	for (std::map<std::pair<nuiPropertyType,nuiPropertyType>,nuiValueConverterCallback>::iterator iter = valueConverterCallback.begin(); iter != valueConverterCallback.end(); iter++)
	{
		if ((sourceType == iter->first.first) && (destinationType == iter->first.second))
		{
			valueConverterCallback.erase(iter);
			return NUI_PROPERTY_OK;
		}
	}
	return NUI_PROPERTY_NOT_EXIST_CALLBACK;
}

nuiPropertyErrorCode nuiProperty::removeValueConverterCallback(nuiValueConverterCallback callback)
{
	for (std::map<std::pair<nuiPropertyType,nuiPropertyType>,nuiValueConverterCallback>::iterator iter = valueConverterCallback.begin(); iter != valueConverterCallback.end(); iter++)
	{
		if (callback == iter->second)
		{
			valueConverterCallback.erase(iter);
			return NUI_PROPERTY_OK;
		}
	}
	return NUI_PROPERTY_NOT_EXIST_CALLBACK;
}

nuiPropertyErrorCode nuiProperty::removeAllValueConverterCallback()
{
	valueConverterCallback.clear();
	return NUI_PROPERTY_OK;
}

inline void nuiProperty::setPropertyReadonly(bool isPropertyReadOnly)
{
	if (getIsPropertyReadonly() != isPropertyReadOnly)
		propertyMetadata = (nuiPropertyMetadata)(propertyMetadata ^ NUI_METADATA_READONLY);
}

inline void nuiProperty::setDescription(const std::string &description) 
{ 
	this->description = description;
}

inline bool nuiProperty::getIsPropertyReadonly() 
{ 
	return propertyMetadata & NUI_METADATA_READONLY; 
}

unsigned long nuiProperty::getPropertyValueMillisecondsAge() 
{ 
	if (propertyMetadata & NUI_METADATA_HAS_VALUE) 
	{
		if (lastTimeSet == 0)
		{
			lastTimeSet = retreiveTicksCount();
			return 0;
		}
		return (retreiveTicksCount() - lastTimeSet);
	}
}

void nuiProperty::onValueChanged()
{
	for (std::vector<nuiValueChangedCallback>::iterator iter = valueChangedCallbacks.begin(); iter != valueChangedCallbacks.end(); iter++)
		 (*iter)(value, (const nuiProperty*) this);
}


inline bool nuiProperty::getHasValue()
{ 
	return NUI_METADATA_HAS_VALUE & propertyMetadata; 
}

inline const std::string *nuiProperty::getDescription() 
{ 
	return &description; 
}

nuiPropertyErrorCode nuiProperty::setNewValue(void *newValue, nuiPropertyType newValueType, nuiValueConverterCallback converterCallback)
{
	if (getIsPropertyReadonly())
		return NUI_PROPERTY_IS_READONLY;
	nuiValueConverterCallback converter = converterCallback;
	if (converter == NULL)
	{
		for (std::map<std::pair<nuiPropertyType,nuiPropertyType>,nuiValueConverterCallback>::iterator iter = valueConverterCallback.begin(); iter != valueConverterCallback.end(); iter++)
		{
			if ((iter->first.first == newValueType) && (iter->first.second == propertyType))
			{
				converter = iter->second;
			}
		}
	}
 	if (converter == NULL)
	{
		if (newValueType == propertyType)
		{
			onValueChanged();
			return NUI_PROPERTY_OK;
		}
		return NUI_PROPERTY_CONVERTER_DONT_EXIST;
	} 
	else
	{
		nuiPropertyErrorCode errorCode = converter((const void*)newValue,value);
		if (errorCode == NUI_PROPERTY_OK)
		{
			onValueChanged();
			return NUI_PROPERTY_OK;
		}
		return errorCode;
	}
}

nuiPropertyErrorCode nuiProperty::getValue(const void **returnValue, nuiPropertyType returnValueType,  nuiValueConverterCallback converterCallback)
{
	nuiValueConverterCallback converter = converterCallback;
	if (converter == NULL)
	{
		for (std::map<std::pair<nuiPropertyType,nuiPropertyType>,nuiValueConverterCallback>::iterator iter = valueConverterCallback.begin(); iter != valueConverterCallback.end(); iter++)
		{
			if ((iter->first.first == propertyType) && (iter->first.second == returnValueType))
			{
				converter = iter->second;
			}
		}
	}
	if (converter == NULL)
	{
		if (returnValueType == propertyType)
		{
			*returnValue = value;
			return NUI_PROPERTY_OK;
		}
		return NUI_PROPERTY_CONVERTER_DONT_EXIST;
	} 
	else
	{
		void* convertedValue = NULL;
		nuiPropertyErrorCode errorCode = converter((const void*)value,convertedValue);
		if (errorCode == NUI_PROPERTY_OK)
		{
			*returnValue = convertedValue;
			return NUI_PROPERTY_OK;
		}
		return errorCode;
	}
}




