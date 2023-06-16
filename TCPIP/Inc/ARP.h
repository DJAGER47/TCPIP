#pragma once

#include <cstdint>

#include "InterfaceMAC.h"
#include "InterfaceLogger.h"
#include "IPv4.h"

namespace TCPIP
{
    class ARP
    {
    public:
        ARP() = delete;
        ARP(ARP &) = delete;
        ARP(InterfaceMAC &mac, IPv4 &ip, InterfaceLogger &log) : mac_(mac), ip_(ip), log_(log){};

        /// @brief Parsing the package
        /// @param buffer 
        void ProcessRx(const EthBuff *buffer, size_t offset);

        /// @brief Add to cache table
        void Add(const uint8_t *protocolAddress, const uint8_t *hardwareAddress);

    private:
        enum
        {
            request = 0x0001,
            reply = 0x0002
        };

        struct ARPInfo
        {
            uint16_t HTYPE; // Hardware type
            uint16_t PTYPE; // Protocol type
            uint8_t HLEN;   // Hardware length
            uint8_t PLEN;   // Protocol length
            uint16_t OPER;  // Operation
            uint8_t SHA[6]; // Sender hardware address
            uint8_t SPA[4]; // Sender protocol address
            uint8_t THA[6]; // Target hardware address
            uint8_t TPA[4]; // Target protocol address
        };

        /// @brief Reply to Mac Address Request
        /// @param info 
        void SendReply(const ARPInfo &info);

        InterfaceMAC &mac_;
        IPv4 &ip_;
        InterfaceLogger &log_;
    };

        // const uint8_t *Protocol2Hardware(const uint8_t *address);
        // bool IsLocal(const uint8_t *address);
        // bool IsBroadcast(const uint8_t *address);

        // void SendRequest(const uint8_t *targetIP);
        // int LocateProtocolAddress(const uint8_t *address);
}