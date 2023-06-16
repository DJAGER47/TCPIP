#pragma once

#include "InterfaceMAC.h"
#include "ARP.h"
#include "IPv4.h"
#include "ICMP.h"
#include "MACEthernet.h"
#include "Logger.h"

namespace TCPIP
{
    class DefaultStack
    {
    public:
        DefaultStack();

        Logger log_;
        MACEthernet mac_;
        IPv4 ip_;
        ARP arp_;
        // ICMP ICMP;
    };
}
