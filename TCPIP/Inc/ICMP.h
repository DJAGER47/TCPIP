

#pragma once

#include <cstdint>
#include "DataBuffer.h"
#include "IPv4.h"

namespace TCPIP
{
    class ICMP
    {
    public:
        ICMP() = delete;
        ICMP(ICMP &) = delete;
        ICMP(IPv4 &ip) : IP(ip) {};
        void ProcessRx(DataBuffer *, const uint8_t *sourceIP, const uint8_t *targetIP);

    private:
        enum
        {
            // https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
            Icmp_ER = 0,   /// Echo reply
            Icmp_DUR = 3,  /// Destination unreachable
            Icmp_SQ = 4,   /// Source quench
            Icmp_RD = 5,   /// Redirect
            Icmp_ECHO = 8, /// Echo
            Icmp_TE = 11,  /// Time exceeded
            Icmp_PP = 12,  /// Parameter problem
            Icmp_TS = 13,  /// Timestamp
            Icmp_TSR = 14, /// Timestamp reply
            Icmp_IRQ = 15, /// Information request
            Icmp_IR = 16,  /// Information reply
            Icmp_AM = 17,  /// Address mask request
            Icmp_AMR = 18, /// Address mask reply
        };

        struct ICMPInfo
        {
            uint8_t type;
            uint8_t code;
            uint16_t checksum;
            uint16_t id;
            uint16_t seqnum;
        };

        IPv4 &IP;
    };

}
