#include "MACEthernet.h"
#include "Utility.h"

namespace TCPIP
{

    MACEthernet::MACEthernet(ARP &arp, InterfaceIP &ipv4, InterfaceLogger &log) : TxHandler(nullptr), arp_(arp), ipv4_(ipv4), log_(log)
    {
        memset(BroadcastAddress, 0xFF, GetAddressSize());
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
            log_.print_log(InterfaceLogger::INFO, "MAC: Pocket size %d\n", buffer->pbuf.tot_len);

            uint16_t type = detail::Unpack16(buffer->buff, 12);
            switch (type)
            {
            case InterfaceMAC::EtherType::etIPv4:
                //IPv4.ProcessRx(buffer, GetHeaderSize());
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

    void MACEthernet::Transmit(const EthBuff *buffer, const uint8_t *targetMAC, uint16_t type)
    {
        // buffer->Packet -= GetHeaderSize();
        // buffer->Length += GetHeaderSize();

        // size_t offset = 0;
        // offset = PackBytes(buffer->Packet, offset, targetMAC, 6);
        // offset = PackBytes(buffer->Packet, offset, UnicastAddress, 6);
        // offset = Pack16(buffer->Packet, offset, type);

        // offset += buffer->Length;
        // while (buffer->Length < 60)
        // {
        //     buffer->Packet[buffer->Length++] = 0;
        // }

        // if (TxHandler)
        // {
        //     TxHandler(buffer->Packet, buffer->Length);
        // }

        // if (buffer->Disposable)
        // {
        //     TxBufferQueue.Put(buffer);
        // }
    }

    void MACEthernet::Retransmit(const EthBuff *buffer)
    {
        // if (TxHandler)
        // {
        //     TxHandler(buffer->Packet, buffer->Length);
        // }

        // if (buffer->Disposable)
        // {
        //     TxBufferQueue.Put(buffer);
        // }
    }

    // const EthBuff *MACEthernet::GetTxBuffer()
    // {
    //     const EthBuff *buffer = new const EthBuff;

    //     // while ((buffer = (const EthBuff *)TxBufferQueue.Get()) == nullptr)
    //     // {
    //     //     QueueEmptyEvent.Wait(__FILE__, __LINE__);
    //     // }
    //     // if (buffer != nullptr)
    //     // {
    //     //     buffer->Initialize(this);
    //     //     buffer->Packet += GetHeaderSize();
    //     //     buffer->Remainder -= GetHeaderSize();
    //     // }

    //     return buffer;
    // }

    // void MACEthernet::FreeTxBuffer(const EthBuff *buffer)
    // {
    //     // TxBufferQueue.Put(buffer);
    //     // QueueEmptyEvent.Notify();
    // }

    // void MACEthernet::FreeRxBuffer(const EthBuff *buffer)
    // {
    //     // RxBufferQueue.Put(buffer);
    // }

    // private:
    bool MACEthernet::IsThisMyAddress(const uint8_t *addr)
    {
        return detail::AddressCompare(UnicastAddress, addr, GetAddressSize()) ||
               detail::AddressCompare(BroadcastAddress, addr, GetAddressSize());
    }
}