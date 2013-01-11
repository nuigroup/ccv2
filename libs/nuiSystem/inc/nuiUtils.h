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

  std::string guidToString(GUID guid) {
    std::array<char,40> output;
    snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(output.data());
  }

  GUID stringToGUID(const std::string& guid) {
    GUID output;
    const int ret = sscanf(guid.c_str(), "{%8X-%4hX-%4hX-%2hX%2hX-%2hX%2hX%2hX%2hX%2hX%2hX}", &output.Data1, &output.Data2, &output.Data3, &output.Data4[0], &output.Data4[1], &output.Data4[2], &output.Data4[3], &output.Data4[4], &output.Data4[5], &output.Data4[6], &output.Data4[7]);
    if (ret != 11)
      throw std::logic_error("Unvalid GUID, format should be {00000000-0000-0000-0000-000000000000}");
    return output;
  }
  //     static double degToRad(double deg);
  //     static double radToDeg(double rad);
  //    template<class Iterator, class Function> static Function for_each(Iterator first, Iterator last, Function f);
private:
  static bool isRandInitialized;
};

#endif
