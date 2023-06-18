#pragma once

namespace TCPIP
{
  class InterfaceLogger
  {
  public:
    enum Type
    {
      INFO,
      WARNING,
      ERROR
    };

    virtual ~InterfaceLogger() {}
    virtual int print_log(Type type, const char *sFormat, ...) = 0;
  };
}