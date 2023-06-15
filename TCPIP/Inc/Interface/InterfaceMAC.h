#pragma once

#include <cstdint>
#include "DataBuffer.h"

namespace TCPIP
{
    class DataBuffer;

    class InterfaceMAC
    {
    public:
        virtual ~InterfaceMAC() {}
        typedef void (*DataTransmitHandler)(void *data, size_t length);

        virtual void RegisterDataTransmitHandler(DataTransmitHandler) = 0;

        virtual size_t GetAddressSize() const = 0;
        virtual size_t HeaderSize() const = 0;

        virtual const uint8_t *GetUnicastAddress() const = 0;
        virtual const uint8_t *GetBroadcastAddress() const = 0;

        virtual DataBuffer *GetTxBuffer() = 0;
        virtual void FreeTxBuffer(DataBuffer *) = 0;
        virtual void FreeRxBuffer(DataBuffer *) = 0;

        virtual void Transmit(DataBuffer *, const uint8_t *targetMAC, uint16_t type) = 0;
        virtual void Retransmit(DataBuffer *buffer) = 0;
    };
}