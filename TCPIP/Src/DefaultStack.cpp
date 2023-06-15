

#include "DefaultStack.h"

namespace TCPIP
{

    DefaultStack::DefaultStack()
        : MAC(ARP, IP), IP(MAC, ARP, ICMP, TCP, UDP), ARP(MAC, IP), DHCP(MAC, IP, UDP), ICMP(IP), TCP(IP), UDP(IP, DHCP)
    {
    }

    void DefaultStack::RegisterDataTransmitHandler(InterfaceMAC::DataTransmitHandler handler)
    {
        MAC.RegisterDataTransmitHandler(handler);
    }

    void DefaultStack::SetMACAddress(uint8_t *addr)
    {
        MAC.SetUnicastAddress(addr);
    }

    void DefaultStack::StartDHCP()
    {
        DHCP.test();
    }

    void DefaultStack::Tick()
    {
        TCP.Tick();
    }

    void DefaultStack::ProcessRx(uint8_t *data, size_t length)
    {
        MAC.ProcessRx(data, length);
    }

}
