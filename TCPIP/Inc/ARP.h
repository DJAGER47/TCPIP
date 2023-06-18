#pragma once

#include <cstdint>

#include "InterfaceMAC.h"
#include "InterfaceLogger.h"
#include "IPv4.h"

namespace TCPIP
{
  /// @brief Address Resolution Protocol
  class ARP
  {
  private:
    // https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
    static const uint8_t hardwareType = 1;

    enum
    {
      request = 0x0001,
      reply = 0x0002
    };

    struct ARPInfo
    {
      uint16_t HTYPE; // Hardware type
      uint16_t PTYPE; // Protocol type
      uint8_t HLEN;   // Hardware length
      uint8_t PLEN;   // Protocol length
      uint16_t OPER;  // Operation
      uint8_t SHA[6]; // Sender hardware address
      uint8_t SPA[4]; // Sender protocol address
      uint8_t THA[6]; // Target hardware address
      uint8_t TPA[4]; // Target protocol address
    };

  public:
    ARP() = delete;
    ARP(ARP &) = delete;
    ARP(InterfaceMAC &mac, IPv4 &ip, InterfaceLogger &log) : mac_(mac), ip_(ip), log_(log){};

    void ProcessRx(const EthBuff *buffer, size_t offset);
    void Add(const uint8_t *protocolAddress, const uint8_t *hardwareAddress);
    void SendRequest(const uint8_t *targetIP);

  private:
    InterfaceMAC &mac_;
    IPv4 &ip_;
    InterfaceLogger &log_;

    void SendReply(const ARPInfo &info);
  };
}