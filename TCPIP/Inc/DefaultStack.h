

#pragma once

#include "InterfaceMAC.h"
#include "ARP.h"
#include "ICMP.h"
#include "IPv4.h"
#include "MACEthernet.h"

namespace TCPIP
{
    class DefaultStack
    {
    public:
        DefaultStack();
        void RegisterDataTransmitHandler(InterfaceMAC::DataTransmitHandler);
        void SetMACAddress(uint8_t *addr);
        void StartDHCP();
        void Tick();

        void ProcessRx(uint8_t *data, size_t length);

        MACEthernet MAC;
        IPv4 IP;
        ARP ARP;
        ICMP ICMP;
    };
}
