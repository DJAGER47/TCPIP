#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "InterfaceBuffer.h"

namespace TCPIP
{

  template <size_t length>
  class stupidAllocator : public Allocator
  {
  public:
    stupidAllocator()
    {
      for (size_t i = 0; i < sizeof(isFree); ++i)
        isFree[i] = true;
    };

    void release(EthBuff *ptr)
    {
      if (ptr->next != nullptr)
        release(ptr->next);

      for (size_t i = 0; i < length; ++i)
      {
        if (ptr == &cache[i])
        {
          isFree[i] = true;
          return;
        }
      }
    }

    EthBuff *allocate()
    {
      for (size_t i = 0; i < length; ++i)
      {
        if (isFree[i])
        {
          isFree[i] = false;
          return &cache[i];
        }
      }
      return nullptr;
    }

  private:
    EthBuff cache[length];
    bool isFree[length];
  };
}
