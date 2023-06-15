#ifndef stupidAllocator_H_
#define stupidAllocator_H_

#include <stddef.h>
#include <stdint.h>


struct pbuf {
  // next pbuf in the chain
  struct pbuf *next;
  // pointer to the actual data in the buffer
  uint8_t *payload;
  // the full data length of the entire chain
  uint16_t tot_len;
  // length of this buffer
  uint16_t len;
};

typedef struct 
{
  struct pbuf pbuf;
  uint8_t buff[ETH_MAX_PACKET_SIZE];
}RxBuff_t;

template <size_t length>
class stupidAllocator
{
public:
  stupidAllocator() = default;

  void release(RxBuff_t* ptr)
  {
    (void)ptr;
  }

  RxBuff_t* allocate()
  {
    auto ret = &cache[index++];
    index = index > length*2 ? 0 : index;
    return ret;
  }


private:
  RxBuff_t cache[length*2];
  size_t index = 0;
};

#endif
