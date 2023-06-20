#pragma once

#include <cstdint>

#include "InterfaceMAC.h"
#include "InterfaceLogger.h"
#include "IPv4.h"

namespace TCPIP
{
  class IPv4;

  /// @brief Address Resolution Protocol
  class ARP
  {
  private:
    // https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
    static const uint8_t hardwareType = 1;

    static const int MAX_ENTRIES = 10;
    static const int IP_ADDR_SIZE = 4;
    static const int MAC_ADDR_SIZE = 6;
    struct CacheEntry
    {
      uint8_t ip_address[IP_ADDR_SIZE];
      uint8_t mac_address[MAC_ADDR_SIZE];
    };

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
    ARP(InterfaceMAC &mac, IPv4 &ip, InterfaceLogger &log) : num_entries_(0), mac_(mac), ip_(ip), log_(log){};

    void ProcessRx(const EthBuff *buffer, size_t offset);
    void AddEntry(const uint8_t *ip_address, const uint8_t *mac_address);
    const uint8_t *Ip2Mac(const uint8_t *targetIP);
    void SendRequest(const uint8_t *targetIP);

  private:
    CacheEntry cache_[MAX_ENTRIES]; // Array for storing records in the cache
    int num_entries_;               // Number of records in the cache

    InterfaceMAC &mac_;
    IPv4 &ip_;
    InterfaceLogger &log_;

    void SendReply(const ARPInfo &info);
    int FindEntry(const uint8_t *ip_address) const;
  };
}