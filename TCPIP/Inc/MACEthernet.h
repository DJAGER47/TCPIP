#pragma once

#include <cstdint>

#include "InterfaceMAC.h"
#include "InterfaceIP.h"
#include "ARP.h"

namespace TCPIP
{
    class MACEthernet : public InterfaceMAC
    {
    private:
        static const int HEADER_SIZE = 14;
        static const int ADDRESS_SIZE = 6;

    public:

        MACEthernet() = delete;
        MACEthernet(MACEthernet &) = delete;
        MACEthernet(ARP &arp,InterfaceIP &ipv4, InterfaceLogger &log);

        size_t GetHeaderSize() const { return HEADER_SIZE; };
        size_t GetAddressSize() const { return ADDRESS_SIZE; };

        const uint8_t *GetUnicastAddress() const { return UnicastAddress; };
        const uint8_t *GetBroadcastAddress() const { return BroadcastAddress; };

        void RegisterDataTransmitHandler(DataTransmitHandler handler) { TxHandler = handler; };
        void ProcessRx(const EthBuff *buffer);
        void Transmit(const EthBuff *, const uint8_t *targetMAC, uint16_t type);
        void Retransmit(const EthBuff *buffer);

        // const EthBuff *GetTxBuffer();
        // void FreeTxBuffer(const EthBuff *);
        // void FreeRxBuffer(const EthBuff *);

        void SetUnicastAddress(const uint8_t *addr);

    private:
        uint8_t UnicastAddress[ADDRESS_SIZE];
        uint8_t BroadcastAddress[ADDRESS_SIZE];

        DataTransmitHandler TxHandler;
        ARP &arp_;
        InterfaceIP &ipv4_;
        InterfaceLogger &log_;

        bool IsThisMyAddress(const uint8_t *addr);

        // osQueue TxBufferQueue;
        // osQueue RxBufferQueue;
        // osEvent QueueEmptyEvent;

        // const EthBuff TxBuffer[20];
        // const EthBuff RxBuffer[20];

        // void *TxBufferBuffer[20];
        // void *RxBufferBuffer[20];
    };
}