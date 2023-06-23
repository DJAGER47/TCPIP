#pragma once

#include <cstddef>
#include <cstdint>

#define ETH_MAX_PACKETS_SIZE (1524U)

namespace TCPIP
{
  struct EthBuff
  {
    // next EthBuff in the chain
    EthBuff *next;
    // the full data length of the entire chain
    size_t tot_len;
    // length of this buffer
    size_t len;
    uint8_t buff[ETH_MAX_PACKETS_SIZE];
  };

  static_assert(ETH_MAX_PACKETS_SIZE > 255);

  class Allocator
  {
  public:
    virtual ~Allocator(){};
    virtual void release(EthBuff *ptr) = 0;
    virtual EthBuff *allocate() = 0;
  };
}
