#include "MACEthernet.h"
#include "Utility.h"

namespace TCPIP
{

    MACEthernet::MACEthernet(ARP &arp, InterfaceIP &ipv4, InterfaceLogger &log) : BroadcastAddress{0xFF},
                                                                                  TxHandler(nullptr), arp_(arp),
                                                                                  ipv4_(ipv4), log_(log)
    {
        // memset(BroadcastAddress, 0xFF, GetAddressSize());
    }

    void MACEthernet::SetUnicastAddress(const uint8_t *addr)
    {
        memcpy(UnicastAddress, addr, GetAddressSize());
    }

    void MACEthernet::ProcessRx(const EthBuff *buffer)
    {
        if (buffer == nullptr)
            return;

        // Check if the MAC Address is destined for me
        if (IsThisMyAddress(buffer->buff))
        {
            log_.print_log(InterfaceLogger::INFO, "MAC: Pocket size %d\n", buffer->tot_len);

            uint16_t type = detail::Unpack16(buffer->buff, 12);
            switch (type)
            {
            case InterfaceMAC::EtherType::etIPv4:
                // IPv4.ProcessRx(buffer, GetHeaderSize());
                break;
            case InterfaceMAC::EtherType::etARP:
                arp_.ProcessRx(buffer, GetHeaderSize());
                break;
            default:
                log_.print_log(InterfaceLogger::WARNING,
                               "MAC: Unsupported Unicast type 0x%04X\n", type);
                break;
            }
        }
    }

    void MACEthernet::Transmit(EthBuff *buffer, const uint8_t *targetMAC, uint16_t type)
    {
        size_t offset = 0;
        offset = detail::PackBytes(buffer->buff, offset, targetMAC, ADDRESS_SIZE);
        offset = detail::PackBytes(buffer->buff, offset, UnicastAddress, ADDRESS_SIZE);
        offset = detail::Pack16(buffer->buff, offset, type);

        if (buffer->tot_len < min_payload)
        {
            memset(buffer->buff + buffer->tot_len, 0, min_payload - buffer->tot_len);
        }

        if (TxHandler)
        {
            TxHandler(buffer);
        }
    }

    // private:
    bool MACEthernet::IsThisMyAddress(const uint8_t *addr)
    {
        return detail::AddressCompare(UnicastAddress, addr, GetAddressSize()) ||
               detail::AddressCompare(BroadcastAddress, addr, GetAddressSize());
    }
}