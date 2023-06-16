#include "Utility.h"

namespace TCPIP
{
    namespace detail
    {
        uint8_t Unpack8(const uint8_t *p, size_t offset)
        {
            return p[offset];
        }

        uint16_t Unpack16(const uint8_t *p, size_t offset, size_t size)
        {
            uint16_t rc = 0;
            for (size_t i = 0; i < size; i++)
            {
                rc <<= 8;
                rc |= p[offset++];
            }
            return rc;
        }

        uint32_t Unpack32(const uint8_t *p, size_t offset, size_t size)
        {
            uint32_t rc = 0;
            for (size_t i = 0; i < size; i++)
            {
                rc <<= 8;
                rc |= p[offset++];
            }
            return rc;
        }

        size_t Pack8(uint8_t *p, size_t offset, uint8_t value)
        {
            p[offset++] = value;
            return offset;
        }

        size_t Pack16(uint8_t *p, size_t offset, uint16_t value)
        {
            p[offset++] = (value >> 8) & 0xFF;
            p[offset++] = value & 0xFF;
            return offset;
        }

        size_t Pack32(uint8_t *p, size_t offset, uint32_t value)
        {
            p[offset++] = (value >> 24) & 0xFFUL;
            p[offset++] = (value >> 16) & 0xFF;
            p[offset++] = (value >> 8) & 0xFF;
            p[offset++] = value & 0xFF;
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
            if (memcmp(a1, a2, length) == 0)
                return true;
            return false;
        }

        uint16_t CalculateIcmpChecksum(const uint8_t *buffer, int size)
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