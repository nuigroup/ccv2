/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxProperty.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef NUI_PROPERTY_H
#define NUI_PROPERTY_H

#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "nuiUtils.h"

typedef struct _nuiPoint {
	double x;
	double y;
} nuiPoint;

typedef std::vector<nuiPoint> nuiPointList;

typedef enum _nuiPropertyType {
	NUI_PROPERTY_NONE,
	NUI_PROPERTY_BOOL, 
	NUI_PROPERTY_STRING, 
	NUI_PROPERTY_INTEGER,
	NUI_PROPERTY_DOUBLE,
	NUI_PROPERTY_POINTLIST,
} nuiPropertyType;

class nuiProperty;

typedef void (*nuiPropertyCallback)(nuiProperty *property, void *userdata);

class nuiProperty {
public:
	nuiProperty(bool value, const std::string &description = "");
	nuiProperty(const char *value, const std::string &description = "");
	nuiProperty(std::string value, const std::string &description = "");
	nuiProperty(int value, const std::string &description = "");
	nuiProperty(double value, const std::string &description = "");
	nuiProperty(nuiPointList, const std::string &description = "");
	~nuiProperty();
	
	nuiPropertyType getType();

	void setDescription(const std::string& desc);
	std::string getDescription();

	void setReadOnly(bool);
	bool isReadOnly();
	
	bool asBool();
	std::string asString();
	double asDouble();
	int asInteger();
	nuiPointList asPointList();

	void set(bool value);
	void set(std::string value);
	void set(const char* value);
	void set(int value);
	void set(double value);
	void set(nuiPointList value);

	bool haveMin();
	bool haveMax();
	bool haveChoices();
	int getMin();
	int getMax();
	std::string getChoices();
	void setMin(int val);
	void setMax(int val);
	void setChoices(const std::string &val);

	void setText(bool is_text);
	bool isText();

	void addCallback(nuiPropertyCallback callback, void *userdata);
	void removeCallback(nuiPropertyCallback callback);
	
	friend std::ostream& operator<< (std::ostream& o, const nuiProperty& f);

	static std::string getPropertyTypeName(nuiPropertyType type);
	
private:
	nuiProperty(const nuiProperty& property);
	nuiPropertyType type;
	std::map<nuiPropertyCallback, void*> callbacks;
	std::string description;
	void* val;
	bool readonly;
	bool have_min;
	bool have_max;
	bool have_choices;
	bool is_text;
	int val_min;
	int val_max;
	std::string val_choices;
	
	void free();
	void init(const std::string& description);
	void fireCallback();
};

#endif

