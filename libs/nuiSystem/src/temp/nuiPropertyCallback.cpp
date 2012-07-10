#include "nuiPropertyCallback.h"
#include "nuiProperty.h"
#include <string>
#include <sstream>

template <class T>
bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}

nuiPropertyErrorCode stringToBoolConverter(const void *oldValue, void *newValue)
{
	std::string *value = (std::string*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	if ( *value == "True" || *value == "true" || *value == "TRUE" || *value == "1" )
	{
		*((bool*)newValue) = true;
		return NUI_PROPERTY_OK;
	}
	else if ( *value == "False" || *value == "false" || *value == "FALSE" || *value == "0" )
	{
		*((bool*)newValue) = false;
		return NUI_PROPERTY_OK;
	}
	return NUI_PROPERTY_NOT_CONVERTED;
}

nuiPropertyErrorCode integerToBoolConverter(const void *oldValue, void *newValue)
{
	int* value = (int*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((bool*)newValue) = (*value!=0);
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode floatToBoolConverter(const void *oldValue, void *newValue)
{
	double* value = (double*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((bool*)newValue) = (*value!=0);
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode charToBoolConverter(const void *oldValue, void *newValue)
{
	char* value = (char*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	if ((*value == 'Y') || (*value == 'y'))
	{
		*((bool*)newValue) = true;
		return NUI_PROPERTY_OK;
	}
	else if ((*value == 'N') || (*value == 'n'))
	{
		*((bool*)newValue) = false;
		return NUI_PROPERTY_OK;
	}
	return NUI_PROPERTY_NOT_CONVERTED;
}

nuiPropertyErrorCode boolToStringConverter(const void *oldValue, void *newValue)
{
	bool* value = (bool*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((std::string*)newValue) = *value ? "True" : "False";
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode integerToStringConverter(const void *oldValue, void *newValue)
{
	int* value = (int*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	std::stringstream out;
	out << *value;
	*((std::string*)newValue) = std::string(out.str());
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode floatToStringConverter(const void *oldValue, void *newValue)
{
	double* value = (double*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	std::stringstream out;
	out.precision(6);
	out << *value;
	*((std::string*)newValue) = std::string(out.str());
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode charToStringConverter(const void *oldValue, void *newValue)
{
	char* value = (char*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	std::stringstream out;
	out << *value;
	*((std::string*)newValue) = std::string(out.str());
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode vector2DToStringConverter(const void *oldValue, void *newValue)
{
	nuiVector2D* value = (nuiVector2D*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	std::stringstream out;
	out.precision(6);
	out << '[' << value->X << ';' << value->Y << ']';
	*((std::string*)newValue) = std::string(out.str());
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode vector3DToStringConverter(const void *oldValue, void *newValue)
{
	nuiVector3D* value = (nuiVector3D*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	std::stringstream out;
	out.precision(6);
	out << '[' << value->X << ';' << value->Y << ';' << value->Z << ']';
	*((std::string*)newValue) = std::string(out.str());
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode boolToFloatConverter(const void *oldValue, void *newValue)
{
	bool* value = (bool*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((double*)newValue) = (*value) ? 1.0 : 0.0;
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode integerToFloatConverter(const void *oldValue, void *newValue)
{
	int* value = (int*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((double*)newValue) = (*value);
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode stringToFloatConverter(const void *oldValue, void *newValue)
{
	std::string* value = (std::string*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	double ret;
	if(from_string<double>(ret, *value, std::dec))
	{
		*((double*)newValue) = ret;
		return NUI_PROPERTY_OK;
	}
	return NUI_PROPERTY_NOT_CONVERTED;
}

nuiPropertyErrorCode boolToIntegerConverter(const void *oldValue, void *newValue)
{
	bool* value = (bool*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((int*)newValue) = (*value) ? 1 : 0;
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode stringToIntegerConverter(const void *oldValue, void *newValue)
{
	std::string* value = (std::string*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	int ret;
	if(from_string<int>(ret, *value, std::dec))
	{
		*((int*)newValue) = ret;
		return NUI_PROPERTY_OK;
	}
	return NUI_PROPERTY_NOT_CONVERTED;
}

nuiPropertyErrorCode floatToIntegerConverter(const void *oldValue, void *newValue)
{
	double* value = (double*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((int*)newValue) = (int)(*value);
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode boolToCharConverter(const void *oldValue, void *newValue)
{
	bool* value = (bool*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	*((char*)newValue) = (*value) ? 'Y' : 'N';
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode stringToCharConverter(const void *oldValue, void *newValue)
{
	std::string* value = (std::string*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	if (value->length() <= 0)
		return NUI_PROPERTY_NOT_CONVERTED;
	*((char*)newValue) = (*value)[0];
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode stringToVector2DConverter(const void *oldValue, void *newValue)
{
	std::string* value = (std::string*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	value->erase('[');
	value->erase(']');
	int position = value->find(';');
	if ((position < 0) || (position >= value->length()))
		return NUI_PROPERTY_NOT_CONVERTED;
	double ret;
	if(from_string<double>(ret, value->substr(0,position), std::dec))
	{
		(*((nuiVector2D*)newValue)).X = ret;
	}
	else
		return NUI_PROPERTY_NOT_CONVERTED;

	if(from_string<double>(ret, value->substr(position+1,value->length() - position - 1), std::dec))
	{
		(*((nuiVector2D*)newValue)).Y = ret;
	}
	else
		return NUI_PROPERTY_NOT_CONVERTED;
	return NUI_PROPERTY_OK;
}

nuiPropertyErrorCode stringToVector3DConverter(const void *oldValue, void *newValue)
{
	std::string* value = (std::string*)oldValue;
	if (value == NULL)
		return NUI_PROPERTY_NULL;
	value->erase('[');
	value->erase(']');
	int lPosition = value->find(';');
	int rPosition = value->rfind(';');
	if (((lPosition < 0) || (lPosition >= value->length())) || ((rPosition < 0) || (rPosition >= value->length())) || (lPosition == rPosition))
		return NUI_PROPERTY_NOT_CONVERTED;
	double ret;
	if(from_string<double>(ret, value->substr(0,lPosition), std::dec))
	{
		(*((nuiVector3D*)newValue)).X = ret;
	}
	else
		return NUI_PROPERTY_NOT_CONVERTED;
	if(from_string<double>(ret, value->substr(lPosition+1,rPosition - lPosition - 2), std::dec))
	{
		(*((nuiVector3D*)newValue)).Y = ret;
	}
	else
		return NUI_PROPERTY_NOT_CONVERTED;
	if(from_string<double>(ret, value->substr(rPosition+1,value->length() - rPosition - 1), std::dec))
	{
		(*((nuiVector3D*)newValue)).Z = ret;
	}
	else
		return NUI_PROPERTY_NOT_CONVERTED;
	return NUI_PROPERTY_OK;
}


