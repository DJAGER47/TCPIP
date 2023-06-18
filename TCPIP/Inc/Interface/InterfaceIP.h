#pragma once

#include "InterfaceBuffer.h"

namespace TCPIP
{
    class InterfaceIP
    {
    public:
        virtual ~InterfaceIP() {}

        virtual void ProcessRx(EthBuff *) = 0;
        // const EthBuff *GetTxBuffer(InterfaceMAC *);
        // void Transmit(const EthBuff *, uint8_t protocol, const uint8_t *targetIP, const uint8_t *sourceIP);
        // void Retransmit(const EthBuff *);
        // void Retry();

        // void FreeTxBuffer(const EthBuff *);
        // void FreeRxBuffer(const EthBuff *);
        virtual uint8_t GetHeaderSize() const = 0;
        virtual uint8_t GetAddressSize() const = 0;
        virtual const uint8_t *GetUnicastAddress() const = 0;
        virtual const uint8_t *GetGatewayAddress() const  = 0;
        virtual const uint8_t *GetSubnetMask() const = 0;
        // virtual void SetAddressInfo(const AddressIP4Settings &info) = 0;
    };
}