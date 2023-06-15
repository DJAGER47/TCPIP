

#pragma once

#include <cstdint>
#include <cstddef>
#include "InterfaceMAC.h"

namespace TCPIP
{
    class DataBuffer
    {
        static const uint16_t DATA_BUFFER_PAYLOAD_SIZE = 512;
    public:
        DataBuffer() = default;
        DataBuffer(DataBuffer &) = delete;

        uint8_t *Packet;
        uint16_t Length;
        uint16_t Remainder;
        bool Disposable;

        void Initialize(InterfaceMAC *);
        void Preallocate(size_t size);
        void ResetPreallocation(size_t size);

    private:
        InterfaceMAC *mac_;
        uint8_t Data[DATA_BUFFER_PAYLOAD_SIZE];
    };
}
