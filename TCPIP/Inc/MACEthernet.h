#pragma once

#include <cstdint>

#include "InterfaceMAC.h"
#include "InterfaceIP.h"
#include "ARP.h"
#include "stupidAllocator.h"

namespace TCPIP
{
  class MACEthernet : public InterfaceMAC // https://en.wikipedia.org/wiki/Ethernet
  {
  private:
    static const uint8_t min_payload = 46; // minimum Ethernet payload size

    static const uint8_t HEADER_SIZE = 14; // size MAC header
    static const uint8_t ADDRESS_SIZE = 6; // number of bytes in the mac address

  public:
    MACEthernet() = delete;
    MACEthernet(MACEthernet &) = delete;
    MACEthernet(Allocator &Rx, Allocator &Tx, InterfaceLogger *log = nullptr);

    void SetArpClass(ARP *arp) { arp_ = arp; };
    void SetIPv4Class(InterfaceIP *ipv4) { ipv4_ = ipv4; };

    uint8_t GetHeaderSize() const override { return HEADER_SIZE; };
    uint8_t GetAddressSize() const override { return ADDRESS_SIZE; };

    const uint8_t *GetUnicastAddress() const override { return UnicastAddress; };
    const uint8_t *GetBroadcastAddress() const override { return BroadcastAddress; };

    void RegisterDataTransmitHandler(DataTransmitHandler handler) override { TxHandler = handler; };
    TErr ProcessRx(const EthBuff *buffer) override;
    TErr Transmit(EthBuff *buffer, const uint8_t *targetMAC, uint16_t type) override;

    EthBuff *GetTxBuffer() override { return Tx_.allocate(); };
    size_t GetTxOffset() override { return HEADER_SIZE; };
    EthBuff *GetRxBuffer() override { return Rx_.allocate(); };
    void FreeTxBuffer(EthBuff *p) override { return Tx_.release(p); };
    void FreeRxBuffer(EthBuff *p) override { return Rx_.release(p); };

    void SetUnicastAddress(const uint8_t *addr);

  private:
    uint8_t UnicastAddress[ADDRESS_SIZE];
    uint8_t BroadcastAddress[ADDRESS_SIZE];

    DataTransmitHandler TxHandler;
    Allocator &Rx_; // Memory allocator for reception
    Allocator &Tx_; // Memory allocator for transmission

    ARP *arp_;
    InterfaceIP *ipv4_;
    InterfaceLogger *log_;

    bool IsThisMyAddress(const uint8_t *addr);
  };
}