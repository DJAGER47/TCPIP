#pragma once

#include <stddef.h>
#include <stdint.h>
#include "stm32f7xx_hal.h"

namespace TCPIP
{
    class EthBuff
    {
    public:
        // next pbuf in the chain
        EthBuff *next;
        // the full data length of the entire chain
        size_t tot_len;
        // length of this buffer
        size_t len;
        uint8_t buff[ETH_MAX_PACKET_SIZE];
    };

    static_assert(ETH_MAX_PACKET_SIZE > 255);

    class Allocator
    {
    public:
        ~Allocator(){};
        virtual void release(EthBuff *ptr) = 0;
        virtual EthBuff *allocate() = 0;
    };
}
