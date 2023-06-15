#include <stdio.h>

#include "ARP.h"
#include "IPv4.h"
#include "MACEthernet.h"
#include "Utility.h"

namespace TCPIP
{

    // Destination - 6 bytes
    // Source - 6 bytes
    // FrameType - 2 bytes

    MACEthernet::MACEthernet(ARP &arp, IPv4 &ipv4)
        : TxBufferQueue("Tx", TX_BUFFER_COUNT, TxBufferBuffer), RxBufferQueue("Rx", RX_BUFFER_COUNT, RxBufferBuffer), QueueEmptyEvent("MACEthernet"), TxHandler(nullptr), ARP(arp), IPv4(ipv4)
    {
        int i;

        BroadcastAddress[0] = 0xFF;
        BroadcastAddress[1] = 0xFF;
        BroadcastAddress[2] = 0xFF;
        BroadcastAddress[3] = 0xFF;
        BroadcastAddress[4] = 0xFF;
        BroadcastAddress[5] = 0xFF;

        for (i = 0; i < TX_BUFFER_COUNT; i++)
        {
            TxBufferQueue.Put(&TxBuffer[i]);
        }
        for (i = 0; i < RX_BUFFER_COUNT; i++)
        {
            RxBufferQueue.Put(&RxBuffer[i]);
        }
    }

    void MACEthernet::RegisterDataTransmitHandler(DataTransmitHandler handler)
    {
        TxHandler = handler;
    }

    bool MACEthernet::IsLocalAddress(const uint8_t *addr)
    {
        return AddressCompare(UnicastAddress, addr, 6) || AddressCompare(BroadcastAddress, addr, 6);
    }

    void MACEthernet::ProcessRx(uint8_t *buffer, int actualLength)
    {
        uint16_t type;
        DataBuffer *packet = (DataBuffer *)RxBufferQueue.Get();
        int i;
        int length =
            (DataBuffer::DATA_BUFFER_PAYLOAD_SIZE < actualLength ? DataBuffer::DATA_BUFFER_PAYLOAD_SIZE : actualLength);

        if (packet == nullptr)
        {
            log_._printf(InterfaceLogger::WARNING, "MACEthernet::ProcessRx Out of receive buffers\n");
            return;
        }

        if (length > DataBuffer::DATA_BUFFER_PAYLOAD_SIZE)
        {
            // log_._printf(InterfaceLogger::WARNING, "MACEthernet::ProcessRx Rx data overrun %d, %d\n",
            //        length,
            //        DataBuffer::DATA_BUFFER_PAYLOAD_SIZE);
            RxBufferQueue.Put(packet);
            return;
        }

        packet->Initialize(this);

        for (i = 0; i < length; i++)
        {
            packet->Packet[i] = buffer[i];
        }
        packet->Length = length;

        type = Unpack16(packet->Packet, 12);

        // Check if the MAC Address is destined for me
        if (IsLocalAddress(packet->Packet))
        {
            // DumpData( buffer, length, printf );
            if (actualLength > length)
            {
                // log_._printf(InterfaceLogger::WARNING, "MACEthernet::ProcessRx Rx data overrun %d, %d\n",
                //        length,
                //        DataBuffer::DATA_BUFFER_PAYLOAD_SIZE);
                // log_._printf(InterfaceLogger::WARNING, "Unicast type 0x%04X\n", type);
                RxBufferQueue.Put(packet);
                return;
            }
            // Unicast
            packet->Packet += GetHeaderSize();
            packet->Length -= GetHeaderSize();

            switch (type)
            {
            case 0x0800: // IP
                IPv4.ProcessRx(packet);
                break;
            case ARP::0x0806: // ARP
                ARP.ProcessRx(packet);
                break;
            default:
                // log_._printf(InterfaceLogger::WARNING,  "Unsupported Unicast type 0x%04X\n", type );
                break;
            }
        }

        if (packet->Disposable)
        {
            RxBufferQueue.Put(packet);
        }
    }

    DataBuffer *MACEthernet::GetTxBuffer()
    {
        DataBuffer *buffer;

        while ((buffer = (DataBuffer *)TxBufferQueue.Get()) == nullptr)
        {
            QueueEmptyEvent.Wait(__FILE__, __LINE__);
        }
        if (buffer != nullptr)
        {
            buffer->Initialize(this);
            buffer->Packet += GetHeaderSize();
            buffer->Remainder -= GetHeaderSize();
        }

        return buffer;
    }

    void MACEthernet::FreeTxBuffer(DataBuffer *buffer)
    {
        TxBufferQueue.Put(buffer);
        QueueEmptyEvent.Notify();
    }

    void MACEthernet::FreeRxBuffer(DataBuffer *buffer)
    {
        RxBufferQueue.Put(buffer);
    }

    void MACEthernet::Transmit(DataBuffer *buffer, const uint8_t *targetMAC, uint16_t type)
    {
        buffer->Packet -= GetHeaderSize();
        buffer->Length += GetHeaderSize();

        size_t offset = 0;
        offset = PackBytes(buffer->Packet, offset, targetMAC, 6);
        offset = PackBytes(buffer->Packet, offset, UnicastAddress, 6);
        offset = Pack16(buffer->Packet, offset, type);

        offset += buffer->Length;
        while (buffer->Length < 60)
        {
            buffer->Packet[buffer->Length++] = 0;
        }

        if (TxHandler)
        {
            TxHandler(buffer->Packet, buffer->Length);
        }

        if (buffer->Disposable)
        {
            TxBufferQueue.Put(buffer);
        }
    }

    void MACEthernet::Retransmit(DataBuffer *buffer)
    {
        if (TxHandler)
        {
            TxHandler(buffer->Packet, buffer->Length);
        }

        if (buffer->Disposable)
        {
            TxBufferQueue.Put(buffer);
        }
    }

    size_t MACEthernet::GetAddressSize() const
    {
        return ADDRESS_SIZE;
    }

    size_t MACEthernet::HeaderSize() const
    {
        return GetHeaderSize();
    }

    const uint8_t *MACEthernet::GetUnicastAddress() const
    {
        return UnicastAddress;
    }

    const uint8_t *MACEthernet::GetBroadcastAddress() const
    {
        return BroadcastAddress;
    }

    void MACEthernet::SetUnicastAddress(uint8_t *addr)
    {
        for (int i = 0; i < 6; i++)
        {
            UnicastAddress[i] = addr[i];
        }
    }
}