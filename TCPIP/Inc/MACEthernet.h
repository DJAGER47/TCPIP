#pragma once

#include <cstdint>

#include "DataBuffer.h"
#include "InterfaceMAC.h"
#include "osEvent.h"
#include "osQueue.h"

namespace TCPIP
{

    class ARP;
    class IPv4;

    class MACEthernet : public InterfaceMAC
    {
    public:
        MACEthernet(ARP &, IPv4 &);
        void RegisterDataTransmitHandler(DataTransmitHandler);

        void ProcessRx(uint8_t *buffer, int length);

        void Transmit(DataBuffer *, const uint8_t *targetMAC, uint16_t type);
        void Retransmit(DataBuffer *buffer);

        DataBuffer *GetTxBuffer();
        void FreeTxBuffer(DataBuffer *);
        void FreeRxBuffer(DataBuffer *);

        size_t GetAddressSize() const;
        size_t HeaderSize() const;

        const uint8_t *GetUnicastAddress() const;
        const uint8_t *GetBroadcastAddress() const;

        void SetUnicastAddress(uint8_t *addr);
        static size_t GetHeaderSize() { return 14; }

    private:
        static const int ADDRESS_SIZE = 6;
        osQueue TxBufferQueue;
        osQueue RxBufferQueue;

        osEvent QueueEmptyEvent;

        uint8_t UnicastAddress[ADDRESS_SIZE];
        uint8_t BroadcastAddress[ADDRESS_SIZE];

        DataBuffer TxBuffer[TX_BUFFER_COUNT];
        DataBuffer RxBuffer[RX_BUFFER_COUNT];

        void *TxBufferBuffer[TX_BUFFER_COUNT];
        void *RxBufferBuffer[RX_BUFFER_COUNT];

        DataTransmitHandler TxHandler;
        ARP &ARP;
        IPv4 &IPv4;

        bool IsLocalAddress(const uint8_t *addr);

        MACEthernet(MACEthernet &);
        MACEthernet();
    };

}