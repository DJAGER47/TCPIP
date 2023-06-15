

#pragma once

#include <cstdint>
#include <cstring>

namespace TCPIP
{
    namespace detail
    {
        uint8_t Unpack8(const uint8_t *p, size_t offset, size_t size = 1);
        uint16_t Unpack16(const uint8_t *p, size_t offset, size_t size = 2);
        uint32_t Unpack32(const uint8_t *p, size_t offset, size_t size = 4);

        size_t Pack8(uint8_t *p, size_t offset, uint8_t value);
        size_t Pack16(uint8_t *p, size_t offset, uint16_t value);
        size_t Pack32(uint8_t *p, size_t offset, uint32_t value);

        size_t PackBytes(uint8_t *p, size_t offset, const uint8_t *value, size_t count);
        size_t PackFill(uint8_t *p, size_t offset, uint8_t value, size_t count);

        bool AddressCompare(const uint8_t *a1, const uint8_t *a2, int length);

        uint16_t CalculateIcmpChecksum(const uint8_t *buffer, int size);

    } // namespace detail
}