#pragma once

#include "InterfaceIP.h"
#include "InterfaceMAC.h"
#include "InterfaceLogger.h"
#include "ARP.h"
#include "ICMP.h"

namespace TCPIP
{

  class ARP;
  class ICMP;

  class IPv4 : public InterfaceIP // https://en.wikipedia.org/wiki/Internet_Protocol_version_4
  {
  private:
    static const uint8_t IpVersion = 4;
    static const uint8_t IpDefHeaderLength = 5;

    static const uint8_t ADDRESS_SIZE = 4;
    static const uint8_t HEADER_SIZE = 20;

  public:
    struct AddressIP4Settings
    {
      uint8_t Address[ADDRESS_SIZE];
      uint8_t SubnetMask[ADDRESS_SIZE];
      uint8_t Gateway[ADDRESS_SIZE];
    };

    typedef struct 
    {
      uint8_t Version : 4;        // Version - 4 bits
      uint8_t IHL : 4;            // Header Length - 4 bits
      uint8_t TypeOfService;      // Type of Service - 8 bits
      uint16_t TotalLength;       // TotalLength - 16 bits
      uint16_t Identification;    // Identification - 16 bits
      uint8_t Flags : 3;          // Flags - 3 bits
      uint16_t FragmentOffset : 13;// FragmentOffset - 13 bits
      uint8_t TTL;                // TTL - 8 bits
      uint8_t Protocol;           // Protocol - 8 bits
      uint16_t Crc;               // HeaderChecksum - 16 bits
      const uint8_t *sourceIP;    // 32 bits
      const uint8_t *targetIP;    // 32 bits
    }HeaderIPv4;

    IPv4() = delete;
    IPv4(IPv4 &) = delete;
    IPv4(InterfaceMAC &mac, ARP &arp, InterfaceLogger *log = nullptr) : PacketID(0), mac_(mac), arp_(arp), icmp_(nullptr), log_(log){};

    void SetIcmpClass(ICMP *icmp) { icmp_ = icmp; };

    TErr ProcessRx(const EthBuff *buffer, size_t offset) override;
    TErr Transmit(EthBuff *buffer, uint8_t protocol, const uint8_t *targetIP, const uint8_t *sourceIP) override;

    EthBuff *GetTxBuffer() override { return mac_.GetTxBuffer(); } ;
    size_t GetTxOffset() override { return mac_.GetTxOffset() + HEADER_SIZE; };
    uint8_t GetHeaderSize() const override { return HEADER_SIZE; }
    uint8_t GetAddressSize() const override { return ADDRESS_SIZE; }
    const uint8_t *GetUnicastAddress() const override { return address_.Address; }
    const uint8_t *GetBroadcastAddress() const override { return Broadcast; }
    const uint8_t *GetGatewayAddress() const override { return address_.Gateway; }
    const uint8_t *GetSubnetMask() const override { return address_.SubnetMask; }

    void SetAddressInfo(const AddressIP4Settings &info);

  private:
    AddressIP4Settings address_;
    uint8_t Broadcast[ADDRESS_SIZE];
    uint16_t PacketID;

    InterfaceMAC &mac_;
    ARP &arp_;
    ICMP *icmp_;
    InterfaceLogger *log_;

    bool IsThisMyAddress(const uint8_t *addr);
    void calcBroadcastAddress(void);
  };
}