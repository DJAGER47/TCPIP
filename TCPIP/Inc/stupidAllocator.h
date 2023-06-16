#pragma once

#include <stddef.h>
#include <stdint.h>
#include "InterfaceBuffer.h"

namespace TCPIP
{

    template <size_t length>
    class stupidAllocator : public Allocator
    {
    public:
        stupidAllocator() = default;

        void release(EthBuff *ptr)
        {
            memset(ptr, 0, sizeof(EthBuff));
        }

        EthBuff *allocate()
        {
            auto ret = &cache[index++];
            index = index >= length * 2 ? 0 : index;
            return ret;
        }

    private:
        EthBuff cache[length * 2];
        size_t index = 0;
    };
}
