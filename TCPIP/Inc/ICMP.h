#pragma once

#include <cstdint>
#include "IPv4.h"
#include "InterfaceBuffer.h"
#include "InterfaceLogger.h"

namespace TCPIP
{
  class IPv4;

  class ICMP // https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
  {
  public:
    ICMP() = delete;
    ICMP(ICMP &) = delete;
    ICMP(IPv4 &ip, InterfaceLogger *log = nullptr) : ip_(ip), log_(log){};
    TErr ProcessRx(const EthBuff *buffer, size_t offset, const uint8_t* sourceIP);

  private:
    enum
    {
      // https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
      Icmp_ER = 0,   // Echo reply
      Icmp_DUR = 3,  // Destination unreachable
      Icmp_SQ = 4,   // Source quench
      Icmp_RD = 5,   // Redirect
      Icmp_ECHO = 8, // Echo
      Icmp_TE = 11,  // Time exceeded
      Icmp_PP = 12,  // Parameter problem
      Icmp_TS = 13,  // Timestamp
      Icmp_TSR = 14, // Timestamp reply
      Icmp_IRQ = 15, // Information request
      Icmp_IR = 16,  // Information reply
      Icmp_AM = 17,  // Address mask request
      Icmp_AMR = 18, // Address mask reply
    };

    struct ICMPInfo
    {
      uint8_t type;
      uint8_t code;
      uint16_t checksum;
    };

    IPv4 &ip_;
    InterfaceLogger *log_;
  };
}
