/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxDataGenericContainer.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#include "nuiDataGenericContainer.h"

nuiDataGenericContainer::nuiDataGenericContainer() {
}

nuiDataGenericContainer::~nuiDataGenericContainer() {
	std::map<std::string, nuiProperty*>::iterator it;
	for ( it = this->properties.begin(); it != this->properties.end(); it++ )
		delete (it->second);
	this->properties.clear();
}


nuiDataGenericContainer* nuiDataGenericContainer::clone(){
	std::map<std::string, nuiProperty*>::iterator it;
    nuiDataGenericContainer* clone = new nuiDataGenericContainer();
    for ( it = this->properties.begin(); it != this->properties.end(); it++ )
	{
		clone->properties[it->first] = new nuiProperty(it->second->asString());
	}
    return clone;
}

bool nuiDataGenericContainer::hasProperty(const std::string &name) {
	std::map<std::string, nuiProperty*>::iterator it;
	it = this->properties.find(name);
	return it == this->properties.end() ? false : true;
}

