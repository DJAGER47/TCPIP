#pragma once

#include <cstdint>
#include "InterfaceBuffer.h"

namespace TCPIP
{
    class InterfaceMAC
    {
    public:

        enum EtherType{
            etIPv4 = 0x0800,
            etARP = 0x0806,
            etIPv6 = 0x86DD,
        };

        virtual ~InterfaceMAC() {}

        virtual size_t GetHeaderSize() const = 0;
        virtual size_t GetAddressSize() const = 0;

        virtual const uint8_t *GetUnicastAddress() const = 0;
        virtual const uint8_t *GetBroadcastAddress() const = 0;

        typedef void (*DataTransmitHandler)(void *data, size_t length);
        virtual void RegisterDataTransmitHandler(DataTransmitHandler) = 0;
        virtual void ProcessRx(const EthBuff *buffer) = 0;
        virtual void Transmit(const EthBuff *, const uint8_t *targetMAC, uint16_t type) = 0;
        virtual void Retransmit(const EthBuff *buffer) = 0;


        // virtual const EthBuff *GetTxBuffer() = 0;
        // virtual void FreeTxBuffer(const EthBuff *) = 0;
        // virtual void FreeRxBuffer(const EthBuff *) = 0;
    };
}