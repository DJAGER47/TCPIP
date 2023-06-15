#include "ARP.h"

// #include "Config.h"
#include "DataBuffer.h"
#include "InterfaceMAC.h"
#include "IPv4.h"
#include "Utility.h"

namespace TCPIP
{
    void ARP::ProcessRx(const DataBuffer *buffer)
    {
        ARPInfo info;
        const uint8_t *packet = buffer->Packet;
        info.HTYPE = detail::Unpack16(packet, 0);
        info.PTYPE = detail::Unpack16(packet, 2);
        info.HLEN = detail::Unpack8(packet, 4);
        info.PLEN = detail::Unpack8(packet, 5);
        info.OPER = detail::Unpack16(packet, 6);
        auto index = 8;
        memcpy(info.SHA, &packet[index], sizeof(info.SHA));
        index += info.HLEN;
        memcpy(info.SPA, &packet[index], sizeof(info.SPA));
        index += info.PLEN;
        memcpy(info.THA, &packet[index], sizeof(info.THA));
        index += info.HLEN;
        memcpy(info.TPA, &packet[index], sizeof(info.TPA));

        if (info.OPER == request)
        {
            if (info.HLEN == mac_.GetAddressSize() &&
                info.PLEN == ip_.GetAddressSize())
            {
                // Compare iP addresses
                if (detail::AddressCompare(info.TPA, ip_.GetUnicastAddress(), ip_.GetAddressSize()))
                {
                    // This ARP is for me
                    SendReply(info);
                }
            }
        }
        else if (info.OPER == reply)
        {
            // Add mac <-> IP to cache
            Add(info.SPA, info.SHA);
        }
        else
        {
            log_._printf(InterfaceLogger::WARNING, "ARP OPER = %d\n ", info.OPER);
        }
    }

    void ARP::Add(const uint8_t *protocolAddress, const uint8_t *hardwareAddress)
    {
        log_._printf(InterfaceLogger::ERROR, "Realize a cache table");
    }

    void ARP::SendReply(const ARPInfo &info)
    {
        int offset = 0;
        DataBuffer *txBuffer = mac_.GetTxBuffer();
        if (txBuffer == nullptr)
        {
            log_._printf(InterfaceLogger::WARNING, "ARP failed to get tx buffer\n");
            return;
        }

        offset = detail::Pack16(txBuffer->Packet, offset, info.HTYPE);
        offset = detail::Pack16(txBuffer->Packet, offset, info.PTYPE);
        offset = detail::Pack8(txBuffer->Packet, offset, info.HLEN);
        offset = detail::Pack8(txBuffer->Packet, offset, info.PLEN);
        offset = detail::Pack16(txBuffer->Packet, offset, reply); // ARP Reply
        offset = detail::PackBytes(txBuffer->Packet, offset, mac_.GetUnicastAddress(), info.HLEN); // My mac addres
        offset = detail::PackBytes(txBuffer->Packet, offset, ip_.GetUnicastAddress(), info.PLEN);  // My ip  adress
        offset = detail::PackBytes(txBuffer->Packet, offset, info.SHA, info.HLEN);
        offset = detail::PackBytes(txBuffer->Packet, offset, info.SPA, info.PLEN);
        txBuffer->Length = offset;

        mac_.Transmit(txBuffer, info.SHA, EtherTypeARP);
    }

    // void ARP::SendRequest(const uint8_t *targetIP)
    // {
    //     ARPRequest.Initialize(&mac_);

    //     // This is normally done by the mac layer
    //     // but this buffer is reserved by arp and not allocated from the mac
    //     ARPRequest.Packet += mac_.HeaderSize();
    //     ARPRequest.Remainder -= mac_.HeaderSize();

    //     ARPRequest.Disposable = false;

    //     size_t offset = 0;
    //     offset = detail::Pack16(ARPRequest.Packet, offset, 0x0001); // Hardware Type
    //     offset = detail::Pack16(ARPRequest.Packet, offset, 0x0800); // Protocol Type
    //     offset = detail::Pack8(ARPRequest.Packet, offset, 6);       // Hardware Size
    //     offset = detail::Pack8(ARPRequest.Packet, offset, 4);       // Protocol Size
    //     offset = detail::Pack16(ARPRequest.Packet, offset, 0x0001); // Op

    //     // Sender's Hardware Address
    //     offset = detail::PackBytes(ARPRequest.Packet, offset, mac_.GetUnicastAddress(), 6);

    //     // Sender's Protocol Address
    //     offset = detail::PackBytes(ARPRequest.Packet, offset, ip_.GetUnicastAddress(), 4);

    //     // Target's Hardware Address
    //     offset = detail::PackFill(ARPRequest.Packet, offset, 0, 6);

    //     // Target's Protocol Address
    //     ARPRequest.Length = detail::PackBytes(ARPRequest.Packet, offset, targetIP, 4);

    //     mac_.Transmit(&ARPRequest, mac_.GetBroadcastAddress(), EtherTypeARP);
    // }

    // const uint8_t *ARP::Protocol2Hardware(const uint8_t *protocolAddress)
    // {
    //     int index;
    //     const uint8_t *rc = nullptr;

    //     if (IsBroadcast(protocolAddress))
    //     {
    //         rc = mac_.GetBroadcastAddress();
    //     }
    //     else
    //     {
    //         if (!IsLocal(protocolAddress))
    //         {
    //             protocolAddress = ip_.GetGatewayAddress();
    //         }
    //         index = LocateProtocolAddress(protocolAddress);

    //         if (index != -1)
    //         {
    //             // rc = Cache[index].MACAddress;
    //         }
    //         else
    //         {
    //             SendRequest(protocolAddress);
    //         }
    //     }
    //     return rc;
    // }

    // bool ARP::IsBroadcast(const uint8_t *address)
    // {
    //     for (auto i = 0; i < ip_.GetAddressSize(); ++i)
    //     {
    //         if (address[i] != 0xFF)
    //         {
    //             return false;
    //         }
    //     }
    //     return true;
    // }

    // bool ARP::IsLocal(const uint8_t *protocolAddress)
    // {
    //     int i;

    //     for (i = 0; i < ip_.GetAddressSize(); i++)
    //     {
    //         if ((protocolAddress[i] & ip_.GetSubnetMask()[i]) !=
    //             (ip_.GetUnicastAddress()[i] & ip_.GetSubnetMask()[i]))
    //         {
    //             break;
    //         }
    //     }

    //     return i == ip_.GetAddressSize();
    // }

    // int ARP::LocateProtocolAddress(const uint8_t *protocolAddress)
    // {
    //     int i;
    //     int j;

    //     for (i = 0; i < ARPCacheSize; i++)
    //     {
    //         // Go through the address backwards since least significant byte is most
    //         // likely to be unique
    //         for (j = ip_.GetAddressSize() - 1; j >= 0; j--)
    //         {
    //             if (Cache[i].IPv4Address[j] != protocolAddress[j])
    //             {
    //                 break;
    //             }
    //         }
    //         if (j == -1)
    //         {
    //             // found
    //             return i;
    //         }
    //     }

    //     return -1;
    // }

}