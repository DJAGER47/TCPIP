#pragma once

namespace TCPIP
{
    class InterfaceIP
    {
    public:
        virtual ~InterfaceIP() {}

        virtual void ProcessRx(DataBuffer *) = 0;
        // DataBuffer *GetTxBuffer(InterfaceMAC *);
        // void Transmit(DataBuffer *, uint8_t protocol, const uint8_t *targetIP, const uint8_t *sourceIP);
        // void Retransmit(DataBuffer *);
        // void Retry();

        // void FreeTxBuffer(DataBuffer *);
        // void FreeRxBuffer(DataBuffer *);
        // static size_t GetHeaderSize() { return HEADER_SIZE; }
        virtual size_t GetAddressSize() const = 0;
        virtual const uint8_t *GetUnicastAddress() const = 0;
        virtual const uint8_t *GetGatewayAddress() const  = 0;
        virtual const uint8_t *GetSubnetMask() const = 0;
        virtual void SetAddressInfo(const AddressIP4Settings &info) = 0;
    };
}