#pragma once

#include <cstdint>
#include "InterfaceBuffer.h"

namespace TCPIP
{
  class InterfaceMAC
  {
  public:
    enum EtherType
    {
      etIPv4 = 0x0800U,
      etARP = 0x0806U,
      etIPv6 = 0x86DDU,
    };

    virtual ~InterfaceMAC() {}

    virtual uint8_t GetHeaderSize() const = 0;
    virtual uint8_t GetAddressSize() const = 0;

    virtual const uint8_t *GetUnicastAddress() const = 0;
    virtual const uint8_t *GetBroadcastAddress() const = 0;

    typedef void (*DataTransmitHandler)(EthBuff *data);
    virtual void RegisterDataTransmitHandler(DataTransmitHandler) = 0;
    virtual void ProcessRx(const EthBuff *) = 0;
    virtual void Transmit(EthBuff *buffer, const uint8_t *targetMAC, uint16_t type) = 0;

    virtual EthBuff *GetTxBuffer() = 0;
    virtual EthBuff *GetRXBuffer() = 0;
    virtual void FreeTxBuffer(EthBuff *) = 0;
    virtual void FreeRxBuffer(EthBuff *) = 0;
  };
}