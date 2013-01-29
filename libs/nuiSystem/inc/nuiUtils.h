/** 
 * \file      nuiUtils.h 
 * 
 * Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * \author    Scott Halstvedt
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#define _WINSOCKAPI_

#ifndef NUI_UTILS_H
#define NUI_UTILS_H

#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#else // UNIX
#include <sys/time.h>
#endif // WIN32

#include <array>

#define PI 3.14159265

class nuiUtils {
public:
  static std::vector<std::string> tokenize(const std::string& str, const std::string& delimiters);
  static double time();
  static bool inList(const std::string &pattern, const std::string &str, const std::string &delimiters = ",");
  static int getRandomNumber();

#ifdef _MSC_VER
#	define snprintf	_snprintf
#endif

  //     static double degToRad(double deg);
  //     static double radToDeg(double rad);
  //    template<class Iterator, class Function> static Function for_each(Iterator first, Iterator last, Function f);
private:
  static bool isRandInitialized;
};

#endif
