#pragma once

#include "InterfaceLogger.h"
#include "debug.h"
#include "cmsis_os.h"

class Logger : public TCPIP::InterfaceLogger
{
  osMutexDef(mutex);
  osMutexId mutex_id;

public:
  Logger() : mutex_id(osMutexCreate(osMutex(mutex)))
  {
    if (mutex_id == NULL)
      print_log(InterfaceLogger::ERROR, "osMutexCreate error\n");
  }

  int print_log(Type type, const char *sFormat, ...)
  {
    int r;
    osMutexWait(mutex_id, osWaitForever);

    if (type == InterfaceLogger::INFO)
      _printf("%sINFO   : %s", RTT_CTRL_TEXT_GREEN, RTT_CTRL_RESET);
    else if (type == InterfaceLogger::WARNING)
      _printf("%sWARNING: %s", RTT_CTRL_TEXT_YELLOW, RTT_CTRL_RESET);
    else if (type == InterfaceLogger::ERROR)
      _printf("%sERROR  : %s", RTT_CTRL_TEXT_RED, RTT_CTRL_RESET);

    va_list ParamList;
    va_start(ParamList, sFormat);
    r = SEGGER_RTT_vprintf(0, sFormat, &ParamList);
    va_end(ParamList);

    osMutexRelease(mutex_id);
    return r;
  }
};