/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxUtils.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef NUI_UTILS_H
#define NUI_UTILS_H

#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#else // WIN32
#include <sys/time.h>
#endif // WIN32

#define PI 3.14159265

class nuiUtils {
public:
	static std::vector<std::string> tokenize(const std::string& str, const std::string& delimiters);
	static double time();
	static bool inList(const std::string &pattern, const std::string &str, const std::string &delimiters = ",");
	static double degToRad(double deg);
	static double radToDeg(double rad);
	static int getRandomNumber();
private:
	static bool isRandInitialized;
};

#endif
