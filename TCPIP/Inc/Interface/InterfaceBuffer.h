#pragma once

#include <stddef.h>
#include <stdint.h>
#include "stm32f7xx_hal.h"

namespace TCPIP
{
    struct pbuf
    {
        // next pbuf in the chain
        struct pbuf *next;
        // pointer to the actual data in the buffer
        uint8_t *payload;
        // the full data length of the entire chain
        uint16_t tot_len;
        // length of this buffer
        uint16_t len;
    };

    class EthBuff
    {
    public:
        struct pbuf pbuf;
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
