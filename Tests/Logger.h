#pragma once

#include "InterfaceLogger.h"
#include <cstdio>
#include <cstdarg>

class Logger : public TCPIP::InterfaceLogger
{
public:
  int _printf(Type type, const char *sFormat, ...)
  {
    int r;
    va_list ParamList;
    va_start(ParamList, sFormat);
    if (type == InterfaceLogger::WARNING)
      printf("WARNING ");
    else
      printf("ERROR   ");

    r = vprintf(sFormat, ParamList);
    va_end(ParamList);
    return r;
  }
};