

#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

#include "InterfaceIP.h"

// #include "ARP.h"
// #include "ICMP.h"

namespace TCPIP
{
    class IPv4 : public InterfaceIP
    {
    private:
        static const uint8_t ADDRESS_SIZE = 4;
        static const uint8_t HEADER_SIZE = 20;

    public:
        struct AddressIP4Settings
        {
            uint8_t Address[ADDRESS_SIZE];
            uint8_t SubnetMask[ADDRESS_SIZE];
            uint8_t Gateway[ADDRESS_SIZE];
        };

        IPv4(){};
        // IPv4() = delete;
        // IPv4(IPv4 &) = delete;
        // IPv4(InterfaceMAC &mac, ARP &arp, ICMP &icmp);
        void ProcessRx(EthBuff *);
        // const EthBuff *GetTxBuffer(InterfaceMAC *);
        // void Transmit(const EthBuff *, uint8_t protocol, const uint8_t *targetIP, const uint8_t *sourceIP);
        // void Retransmit(const EthBuff *);
        // void Retry();

        // void FreeTxBuffer(const EthBuff *);
        // void FreeRxBuffer(const EthBuff *);
        uint8_t GetHeaderSize() const { return HEADER_SIZE; }
        uint8_t GetAddressSize() const { return ADDRESS_SIZE; }
        const uint8_t *GetUnicastAddress() const { return address_.Address; }
        const uint8_t *GetGatewayAddress() const { return address_.Gateway; }
        const uint8_t *GetSubnetMask() const { return address_.SubnetMask; }
        void SetAddressInfo(const AddressIP4Settings &info) { address_ = info; }

    private:
    // Version - 4 bits
    // Header Length - 4 bits
    // Type of Service - 8 bits
    // TotalLength - 16 bits
    // Identification - 16 bits
    // Flags - 3 bits
    // FragmentOffset - 13 bits
    // TTL - 8 bits
    // Protocol - 8 bits
    // HeaderChecksum - 16 bits

    struct blok {
        uint8_t Version:4;
        uint8_t IHL:4;

    };

        // bool IsLocal(const uint8_t *addr);

        // uint16_t PacketID;
        // void *TxBuffer[20];
        // osQueue UnresolvedQueue;

        AddressIP4Settings address_;
        // InterfaceMAC &mac_;
        // ARP &arp_;
        // ICMP &icmp_;
    };

}