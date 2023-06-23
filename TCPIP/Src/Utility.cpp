#include "Utility.h"

namespace TCPIP
{
  namespace detail
  {
    static uint32_t Unpack(const uint8_t *p, size_t offset, size_t size)
    {
      uint32_t rc = 0;
      for (size_t i = 0; i < size; ++i)
      {
        rc <<= 8;
        rc |= p[offset++];
      }
      return rc;
    }

    uint8_t Unpack8(const uint8_t *p, size_t offset)
    {
      return static_cast<uint8_t>(Unpack(p, offset, 1));
    }

    uint16_t Unpack16(const uint8_t *p, size_t offset)
    {
      return static_cast<uint16_t>(Unpack(p, offset, 2));
    }

    uint32_t Unpack32(const uint8_t *p, size_t offset)
    {
      return Unpack(p, offset, 4);
    }

    size_t Pack8(uint8_t *p, size_t offset, uint8_t value)
    {
      p[offset++] = value;
      return offset;
    }

    size_t Pack16(uint8_t *p, size_t offset, uint16_t value)
    {
      p[offset++] = static_cast<uint8_t>((value >> 8) & 0xFF);
      p[offset++] = static_cast<uint8_t>(value & 0xFF);
      return offset;
    }

    size_t Pack32(uint8_t *p, size_t offset, uint32_t value)
    {
      p[offset++] = static_cast<uint8_t>((value >> 24) & 0xFF);
      p[offset++] = static_cast<uint8_t>((value >> 16) & 0xFF);
      p[offset++] = static_cast<uint8_t>((value >> 8) & 0xFF);
      p[offset++] = static_cast<uint8_t>(value & 0xFF);
      return offset;
    }

    size_t PackBytes(uint8_t *p, size_t offset, const uint8_t *value, size_t count)
    {
      memcpy(p + offset, value, count);
      return offset + count;
    }

    size_t PackFill(uint8_t *p, size_t offset, uint8_t value, size_t count)
    {
      memset(p + offset, value, count);
      return offset + count;
    }

    bool AddressCompare(const uint8_t *a1, const uint8_t *a2, int length)
    {
      return (memcmp(a1, a2, length) == 0);
    }

    /// @brief ICMP, IPv4
    /// @param buffer 
    /// @param size 
    /// @return 
    uint16_t CalculateChecksum(const uint8_t *buffer, int size)
    {
      uint32_t sum = 0;
      while (size > 1)
      {
        sum += *reinterpret_cast<const uint16_t *>(buffer);
        buffer += 2;
        size -= 2;
      }
      if (size == 1)
      {
        sum += *buffer;
      }
      while (sum >> 16)
      {
        sum = (sum & 0xFFFF) + (sum >> 16);
      }
      return static_cast<uint16_t>(~sum);
    }
  }
}