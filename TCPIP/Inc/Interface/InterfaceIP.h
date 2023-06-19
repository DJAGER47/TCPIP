#pragma once

#include "InterfaceBuffer.h"

namespace TCPIP
{
  class InterfaceIP
  {
  public:
    enum Protocol // https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers
    {
      pHOPOPT = 0, // IPv6 Hop-by-Hop Option RFC 8200
      pICMP = 1,   // Internet Control Message Protocol RFC 792
      pIGMP = 2,   // Internet Group Management Protocol RFC 1112
      pGGP = 3,    // Gateway-to-Gateway Protocol RFC 823
      pTCP = 6,    // Transmission Control Protocol
      pUDP = 17,   // User Datagram Protocol
      // etc.
    };

    virtual ~InterfaceIP(){};

    virtual void ProcessRx(const EthBuff *buffer, size_t offset) = 0;
    virtual void Transmit(EthBuff *, uint8_t protocol, const uint8_t *targetIP, const uint8_t *sourceIP) = 0;

    virtual EthBuff *GetTxBuffer() = 0;
    virtual size_t GetTxOffset() = 0;
    virtual uint8_t GetHeaderSize() const = 0;
    virtual uint8_t GetAddressSize() const = 0;
    virtual const uint8_t *GetUnicastAddress() const = 0;
    virtual const uint8_t *GetBroadcastAddress() const = 0;
    virtual const uint8_t *GetGatewayAddress() const = 0;
    virtual const uint8_t *GetSubnetMask() const = 0;
  };
}