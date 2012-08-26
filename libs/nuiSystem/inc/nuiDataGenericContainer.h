/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxDataGenericContainer.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef NUI_DATASTRUCT_H
#define NUI_DATASTRUCT_H

#include "nuiProperty.h"

#include <string>
#include <map>
#include <vector>

class nuiProperty;

class nuiDataGenericContainer 
{
public:
	nuiDataGenericContainer();
	virtual ~nuiDataGenericContainer();
	bool hasProperty(const std::string &name);
	std::map<std::string, nuiProperty*> properties;
    nuiDataGenericContainer* clone();
};

typedef std::vector<nuiDataGenericContainer *> nuiDataGenericList;

#endif

