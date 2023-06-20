#pragma once

#include "InterfaceLogger.h"
#include <cstdio>
#include <cstdarg>

class Logger : public TCPIP::InterfaceLogger
{
public:
  int print_log(Type type, const char *sFormat, ...)
  {
    int r;
    va_list ParamList;
    va_start(ParamList, sFormat);
    if (type == InterfaceLogger::INFO)
      printf("INFO   : ");
    else if (type == InterfaceLogger::WARNING)
      printf("WARNING: ");
    else if (type == InterfaceLogger::ERROR)
      printf("ERROR  : ");

    r = vprintf(sFormat, ParamList);
    va_end(ParamList);
    return r;
  }
};