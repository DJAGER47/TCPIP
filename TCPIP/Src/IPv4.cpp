
// #include <stdio.h>
#include "IPv4.h"

// #include "InterfaceMAC.h"
// #include "ARP.h"
// #include "ICMP.h"
// #include "Utility.h"

namespace TCPIP
{

// public:

    // IPv4::IPv4(InterfaceMAC &mac, ARP &arp, ICMP &icmp)
    //     : PacketID(0)
    //       // , UnresolvedQueue("IP", TX_BUFFER_COUNT, TxBuffer)
    //       ,
    //       address_(), mac_(mac), arp_(arp), icmp_(icmp)
    // {
    //     address_.DataValid = false;
    // }

    void IPv4::ProcessRx(DataBuffer *buffer)
    {
        // uint8_t headerLength;
        // uint8_t protocol;
        // uint8_t* sourceIP;
        // uint8_t* targetIP;
        // uint8_t* packet = buffer->Packet;
        // uint16_t length = buffer->Length;
        // uint16_t dataLength;

        // headerLength = (packet[0] & 0x0F) * 4;
        // dataLength = Unpack16(packet, 2);
        // protocol = packet[9];
        // sourceIP = &packet[12];
        // targetIP = &packet[16];

        // if (IsLocal(targetIP))
        // {
        //     buffer->Packet += headerLength;
        //     dataLength -= headerLength;
        //     buffer->Length = dataLength;

        //     switch (protocol)
        //     {
        //     case 0x01: // icmp_
        //         icmp_.ProcessRx(buffer, sourceIP, targetIP);
        //         break;
        //     default: printf("Unsupported IP Protocol 0x%02X\n", protocol); break;
        //     }
        // }
    }

    // DataBuffer *IPv4::GetTxBuffer(InterfaceMAC *mac)
    // {
    //     DataBuffer *buffer;

    //     // buffer = mac->GetTxBuffer();
    //     // if (buffer != nullptr)
    //     // {
    //     //     buffer->Packet += GetHeaderSize();
    //     //     buffer->Remainder -= GetHeaderSize();
    //     // }

    //     return buffer;
    // }

    // void IPv4::Transmit(DataBuffer *buffer,
    //                     uint8_t protocol,
    //                     const uint8_t *targetIP,
    //                     const uint8_t *sourceIP)
    // {
    //     // uint16_t checksum;
    //     // const uint8_t* targetMAC;
    //     // uint8_t* packet;

    //     // buffer->Packet -= GetHeaderSize();
    //     // buffer->Length += GetHeaderSize();
    //     // packet = buffer->Packet;

    //     // packet[0] = 0x45; // Version and HeaderSize
    //     // packet[1] = 0;    // ToS
    //     // Pack16(packet, 2, buffer->Length);

    //     // PacketID++;
    //     // Pack16(packet, 4, PacketID);
    //     // packet[6] = 0; // Flags & FragmentOffset
    //     // packet[7] = 0; // rest of FragmentOffset

    //     // packet[8] = 32; // TTL
    //     // packet[9] = protocol;
    //     // Pack16(packet, 10, 0); // checksum placeholder
    //     // PackBytes(packet, 12, sourceIP, 4);
    //     // PackBytes(packet, 16, targetIP, 4);

    //     // checksum = FCS::Checksum(packet, 20);
    //     // Pack16(packet, 10, checksum);

    //     // targetMAC = arp_.Protocol2Hardware(targetIP);
    //     // if (targetMAC != nullptr)
    //     // {
    //     //     mac_.Transmit(buffer, targetMAC, 0x0800);
    //     // }
    //     // else
    //     // {
    //     //     // Could not find mac_ address, arp_ for it
    //     //     UnresolvedQueue.Put(buffer);
    //     // }
    // }

    // void IPv4::Retransmit(DataBuffer *buffer)
    // {
    //     // mac_.Retransmit(buffer);
    // }

    // void IPv4::Retry()
    //{
        // int count;
        // DataBuffer* buffer;
        // const uint8_t* targetMAC;

        // count = UnresolvedQueue.GetCount();
        // for (int i = 0; i < count; i++)
        // {
        //     buffer = (DataBuffer*)UnresolvedQueue.Get();

        //     targetMAC = arp_.Protocol2Hardware(&buffer->Packet[16]);
        //     if (targetMAC != nullptr)
        //     {
        //         mac_.Transmit(buffer, targetMAC, 0x0800);
        //     }
        //     else
        //     {
        //         log_._printf("Still could not find mac_ for IP\n");
        //         UnresolvedQueue.Put(buffer);
        //     }
        // }
    //}

    // void IPv4::FreeTxBuffer(DataBuffer *buffer)
    // {
    //     // mac_.FreeTxBuffer(buffer);
    // }

    // void IPv4::FreeRxBuffer(DataBuffer *buffer)
    // {
    //     // mac_.FreeRxBuffer(buffer);
    // }


    


// private:

    // bool IPv4::IsLocal(const uint8_t *addr)
    // {
    //     bool rc;
    //     // uint8_t broadcast[] = {0xFF, 0xFF, 0xFF, 0xFF};
    //     // if (address_.DataValid)
    //     // {
    //     //     rc = AddressCompare(addr, broadcast, GetAddressSize()) ||
    //     //          AddressCompare(addr, GetUnicastAddress(), GetAddressSize()) ||
    //     //          AddressCompare(addr, GetBroadcastAddress(), GetAddressSize());
    //     // }
    //     // else
    //     // {
    //     //     rc = AddressCompare(addr, broadcast, GetAddressSize());
    //     // }
    //     return rc;
    // }
}