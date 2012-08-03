/////////////////////////////////////////////////////////////////////////////
// Name:        nui/nuiProperty.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#include "nuiProperty.h"

#ifdef _WIN32
	#define snprintf _snprintf
#endif

#define CASTEDGET(x) x value = *(static_cast<x*>(val));

#define AUTOCONVERT(typein, value) \
	switch ( this->type ) { \
		case NUI_PROPERTY_BOOL: \
			*(static_cast<bool*>(this->val)) = convertToBool(typein, &value); \
			break; \
		case NUI_PROPERTY_STRING: \
			*(static_cast<std::string*>(this->val)) = convertToString(typein, &value); \
			break; \
		case NUI_PROPERTY_INTEGER: \
			*(static_cast<int*>(this->val)) = convertToInteger(typein, &value); \
			break; \
		case NUI_PROPERTY_DOUBLE: \
			*(static_cast<double*>(this->val)) = convertToDouble(typein, &value); \
			break; \
		case NUI_PROPERTY_POINTLIST: \
			*(static_cast<nuiPointList*>(this->val)) = convertToPointList(typein, &value); \
			break; \
		default:; \
	}


static bool convertToBool(nuiPropertyType type, void *val) {
	switch ( type ) {
		case NUI_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value;
		}

		case NUI_PROPERTY_STRING: {
			CASTEDGET(std::string);
			if ( value == "True" || value == "true" || value == "TRUE" || value == "1" )
				return true;
			return false;
		}

		case NUI_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value == 0 ? false : true;
		}

		case NUI_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value == 0 ? false : true;
		}

		case NUI_PROPERTY_POINTLIST: {
			CASTEDGET(nuiPointList);
			return value.size() > 0;
		}

		default:
		assert(0);
	}

	assert(0);
	return false;
}

static std::string convertToString(nuiPropertyType type, void *val) {
	switch ( type ) {
		case NUI_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return value;
		}

		case NUI_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? "1" : "0";
		}

		case NUI_PROPERTY_INTEGER: {
			char buffer[64];
			CASTEDGET(int);
			snprintf(buffer, sizeof(buffer), "%d", value);
			return buffer;
		}

		case NUI_PROPERTY_DOUBLE: {
			char buffer[64];
			CASTEDGET(double);
			snprintf(buffer, sizeof(buffer), "%f", value);
			return buffer;
		}

		case NUI_PROPERTY_POINTLIST: {
			std::ostringstream oss;
			nuiPointList::iterator it;
			CASTEDGET(nuiPointList);
			for ( it = value.begin(); it != value.end(); it++ )
				oss << (*it).x << "," << (*it).y << ";";
			return oss.str();
		}

		default:;
		assert(0);
	}

	assert(0);
	return "";
}

static double convertToDouble(nuiPropertyType type, void *val) {
	switch ( type ) {
		case NUI_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atof(value.c_str());
		}

		case NUI_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1.0 : 0.0;
		}

		case NUI_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return (double)value;
		}

		case NUI_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value;
		}

		default:;
		assert(0);
	}

	assert(0);
	return 0.0;
}

static int convertToInteger(nuiPropertyType type, void *val) {
	switch ( type ) {
		case NUI_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atoi(value.c_str());
		}

		case NUI_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1 : 0;
		}

		case NUI_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value;
		}

		case NUI_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return (int)value;
		}

		case NUI_PROPERTY_POINTLIST: {
			CASTEDGET(nuiPointList);
			return value.size();
		}

		default:;
		assert(0);
	}

	assert(0);
	return 0;
}

static nuiPointList convertToPointList(nuiPropertyType type, void *val) {
	nuiPointList output = nuiPointList();
	switch ( type ) {
		case NUI_PROPERTY_STRING: {
			CASTEDGET(std::string);
			std::vector<std::string> points = nuiUtils::tokenize(value, ";");
			std::vector<std::string>::iterator it;
			for ( it = points.begin(); it != points.end(); it++ ) {
				std::vector<std::string> point = nuiUtils::tokenize((*it), ",");
				nuiPoint p;

				// it's an error, not 2 points. just forget it.
				if ( point.size() != 2 )
					continue;

				// push the point into the list
				p.x = atof(point[0].c_str());
				p.y = atof(point[1].c_str());
				output.push_back(p);
			}

			return output;
		}

		case NUI_PROPERTY_POINTLIST: {
			CASTEDGET(nuiPointList);
			return value;
		}

		// we can't do anything for other type.
		default:
			return output;
	}

	return output;
}



nuiProperty::nuiProperty(bool value, const std::string &description) {
	this->init(description);
	this->type = NUI_PROPERTY_BOOL;
	this->val = new bool();
	this->set(value);
}

nuiProperty::nuiProperty(const char *value, const std::string &description) {
	this->init(description);
	this->type = NUI_PROPERTY_STRING;
	this->val = new std::string();
	this->set(value);
}

nuiProperty::nuiProperty(std::string value, const std::string &description) {
	this->init(description);
	this->type = NUI_PROPERTY_STRING;
	this->val = new std::string();
	this->set(value);
}

nuiProperty::nuiProperty(int value, const std::string &description) {
	this->init(description);
	this->type = NUI_PROPERTY_INTEGER;
	this->val = new int();
	this->set(value);
}

nuiProperty::nuiProperty(double value, const std::string &description) {
	this->init(description);
	this->type = NUI_PROPERTY_DOUBLE;
	this->val = new double();
	this->set(value);
}

nuiProperty::nuiProperty(nuiPointList value, const std::string &description) {
	this->init(description);
	this->type = NUI_PROPERTY_POINTLIST;
	this->val = new nuiPointList();
	this->set(value);
}

void nuiProperty::init(const std::string &description) {
	this->is_text = false;
	this->readonly = false;
	this->val = NULL;
	this->val_min = 0;
	this->val_max = 0;
	this->val_choices = "";
	this->have_min = false;
	this->have_max = false;
	this->have_choices = false;
	this->callbacks = std::map<nuiPropertyCallback,void*>();
	this->setDescription(description);
}

void nuiProperty::set(bool value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(NUI_PROPERTY_BOOL, value);
	this->fireCallback();
}

void nuiProperty::set(std::string value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(NUI_PROPERTY_STRING, value);
	this->fireCallback();
}

void nuiProperty::set(const char *value) {
	if ( this->isReadOnly() )
		return;
	std::string s = std::string(value);
	AUTOCONVERT(NUI_PROPERTY_STRING, s);
	this->fireCallback();
}

void nuiProperty::set(int value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(NUI_PROPERTY_INTEGER, value);
	this->fireCallback();
}

void nuiProperty::set(double value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(NUI_PROPERTY_DOUBLE, value);
	this->fireCallback();
}

void nuiProperty::set(nuiPointList value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(NUI_PROPERTY_POINTLIST, value);
	this->fireCallback();
}

nuiProperty::~nuiProperty() {
	this->free();
}

nuiPropertyType nuiProperty::getType() {
	return this->type;
}

bool nuiProperty::asBool() {
	return convertToBool(this->type, this->val);
}

std::string nuiProperty::asString() {
	return convertToString(this->type, this->val);
}

double nuiProperty::asDouble() {
	return convertToDouble(this->type, this->val);
}

int nuiProperty::asInteger() {
	return convertToInteger(this->type, this->val);
}

nuiPointList nuiProperty::asPointList() {
	return convertToPointList(this->type, this->val);
}

void nuiProperty::free() {
	if ( this->val == NULL )
		return;

	switch ( this->type ) {
		case NUI_PROPERTY_STRING:
			delete (std::string *)(this->val);
			break;
		case NUI_PROPERTY_BOOL:
			delete static_cast<bool *>(this->val);
			break;
		case NUI_PROPERTY_INTEGER:
			delete static_cast<int *>(this->val);
			break;
		case NUI_PROPERTY_DOUBLE:
			delete static_cast<double *>(this->val);
			break;
		default:;
	}

	this->val = NULL;
}

std::string nuiProperty::getPropertyTypeName(nuiPropertyType type) {
	switch ( type ) {
		case NUI_PROPERTY_DOUBLE: return "double";
		case NUI_PROPERTY_INTEGER: return "integer";
		case NUI_PROPERTY_STRING: return "string";
		case NUI_PROPERTY_BOOL: return "bool";
		case NUI_PROPERTY_POINTLIST: return "pointlist";
		default:;
	}

	return "unknown";
}

std::ostream& operator<< (std::ostream& o, const nuiProperty& p) {

	// Bad bad ... :'(
	nuiProperty *f = (nuiProperty *)&p;

	switch ( f->getType() ) {
		case NUI_PROPERTY_STRING:	return o << f->asString();
		case NUI_PROPERTY_BOOL:		return o << f->asBool();
		case NUI_PROPERTY_INTEGER:	return o << f->asInteger();
		case NUI_PROPERTY_DOUBLE:	return o << f->asDouble();
		case NUI_PROPERTY_POINTLIST: return o << f->asPointList();
		default:;
	}

	return o;
}

std::string nuiProperty::getDescription() {
	return this->description;
}

void nuiProperty::setDescription(const std::string& description) {
	this->description = description;
}

bool nuiProperty::isReadOnly() {
	return this->readonly;
}

void nuiProperty::setReadOnly(bool ro) {
	this->readonly = ro;
}

bool nuiProperty::haveMin() {
	return this->have_min;
}

bool nuiProperty::haveMax() {
	return this->have_max;
}

bool nuiProperty::haveChoices() {
	return this->have_choices;
}

int nuiProperty::getMin() {
	return this->val_min;
}

int nuiProperty::getMax() {
	return this->val_max;
}

std::string nuiProperty::getChoices() {
	return this->val_choices;
}

void nuiProperty::setMin(int val) {
	this->val_min = val;
	this->have_min = true;
}

void nuiProperty::setMax(int val) {
	this->val_max = val;
	this->have_max = true;
}

void nuiProperty::setChoices(const std::string &val) {
	this->val_choices = val;
	this->have_choices = true;
}

void nuiProperty::addCallback(nuiPropertyCallback callback, void *userdata) {
	std::map<nuiPropertyCallback, void*>::iterator it;
	// ensure no callback already exist for this property
	assert( this->callbacks.find(callback) == this->callbacks.end() );
	this->callbacks[callback] = userdata;
}

void nuiProperty::removeCallback(nuiPropertyCallback callback) {
	std::map<nuiPropertyCallback, void*>::iterator it;
	for ( it = this->callbacks.begin(); it != this->callbacks.end(); it++ ) {
		if ( it->first == callback ) {
			this->callbacks.erase(it);
			return;
		}
	}
}

void nuiProperty::fireCallback() {
	std::map<nuiPropertyCallback, void*>::iterator it;
	for ( it = this->callbacks.begin(); it != this->callbacks.end(); it++ )
		it->first(this, it->second);
}

void nuiProperty::setText(bool is_text) {
	this->is_text = is_text;
}

bool nuiProperty::isText() {
	return this->is_text;
}

