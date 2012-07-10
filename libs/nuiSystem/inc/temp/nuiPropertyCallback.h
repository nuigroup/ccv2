/* nuiPropertyCallback.h
*  
*  Created on 02/01/12.
*  Copyright 2012 NUI Group. All rights reserved.
*  Community Core Fusion
*  Author: Anatoly Churikov
*
*/
#ifndef _NUI_PROPERTY_CALLBACK_H_
#define _NUI_PROPERTY_CALLBACK_H_

enum nuiPropertyErrorCode;
class nuiProperty;

#define SET_VALUE_CONVERTER(sourceType,destinationType,callback)				\
	nuiProperty::addValueConverterCallback(sourceType,destinationType,callback);\

//******************************//
//		Converter callbacks		//
//******************************//
nuiPropertyErrorCode stringToBoolConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode integerToBoolConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode floatToBoolConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode charToBoolConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode boolToStringConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode integerToStringConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode floatToStringConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode charToStringConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode vector2DToStringConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode vector3DToStringConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode boolToFloatConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode integerToFloatConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode stringToFloatConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode boolToIntegerConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode stringToIntegerConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode floatToIntegerConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode boolToCharConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode stringToCharConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode stringToVector2DConverter(const void *oldValue, void *newValue);
nuiPropertyErrorCode stringToVector3DConverter(const void *oldValue, void *newValue);

#endif//_NUI_PROPERTY_CALLBACKS_H_