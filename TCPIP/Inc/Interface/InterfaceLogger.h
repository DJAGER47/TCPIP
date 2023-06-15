#pragma once

namespace TCPIP
{
    class InterfaceLogger
    {
    public:
        enum Type
        {
            WARNING,
            ERROR
        };

        virtual ~InterfaceLogger() {}
        virtual int _printf(Type type, const char *sFormat, ...) = 0;
    };
}