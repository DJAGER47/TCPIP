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
    static const uint8_t sizeBuffers = 10; // the size of the reception/transmission buffers
    static const uint8_t min_payload = 46; // minimum Ethernet payload size

    static const uint8_t HEADER_SIZE = 14; // size MAC header
    static const uint8_t ADDRESS_SIZE = 6; // number of bytes in the mac address

  public:
    MACEthernet() = delete;
    MACEthernet(MACEthernet &) = delete;
    MACEthernet(InterfaceLogger *log = nullptr);

    void SetArpClass(ARP *arp) { arp_ = arp; };
    void SetIPv4Class(InterfaceIP *ipv4) { ipv4_ = ipv4; };

    uint8_t GetHeaderSize() const { return HEADER_SIZE; };
    uint8_t GetAddressSize() const { return ADDRESS_SIZE; };

    const uint8_t *GetUnicastAddress() const { return UnicastAddress; };
    const uint8_t *GetBroadcastAddress() const { return BroadcastAddress; };

    void RegisterDataTransmitHandler(DataTransmitHandler handler) { TxHandler = handler; };
    TErr ProcessRx(const EthBuff *buffer);
    TErr Transmit(EthBuff *buffer, const uint8_t *targetMAC, uint16_t type);

    EthBuff *GetTxBuffer() { return Tx_.allocate(); };
    size_t GetTxOffset() { return HEADER_SIZE; };
    EthBuff *GetRxBuffer() { return Rx_.allocate(); };
    void FreeTxBuffer(EthBuff *p) { return Tx_.release(p); };
    void FreeRxBuffer(EthBuff *p) { return Rx_.release(p); };

    void SetUnicastAddress(const uint8_t *addr);

  private:
    uint8_t UnicastAddress[ADDRESS_SIZE];
    uint8_t BroadcastAddress[ADDRESS_SIZE];

    DataTransmitHandler TxHandler;
    stupidAllocator<sizeBuffers> Rx_; // Memory allocator for reception
    stupidAllocator<sizeBuffers> Tx_; // Memory allocator for transmission

    ARP *arp_;
    InterfaceIP *ipv4_;
    InterfaceLogger *log_;

    bool IsThisMyAddress(const uint8_t *addr);
  };
}