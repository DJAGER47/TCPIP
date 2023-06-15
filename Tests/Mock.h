#pragma once

#include <cstring>

#include "DataBuffer.h"
#include "InterfaceMAC.h"
#include "IPv4.h"

#define  IP_UnicastAddress 192, 168, 1, 20
#define MAC_UnicastAddress 0x68, 0x54, 0x5a, 0xf9, 0xb5, 0xeb

class mockDataBuf : public TCPIP::DataBuffer
{
private:
public:
    mockDataBuf(uint8_t *data)
    {
        Packet = data;
    }
};

class mockMAC : public TCPIP::InterfaceMAC
{
    public:

    TCPIP::DataBuffer dataBuf;
    uint8_t data_[1000];
    uint8_t macUnicastAddress[6];
    uint8_t macBroadcastAddress[6];

    void RegisterDataTransmitHandler(DataTransmitHandler)
    {
    }

    size_t GetAddressSize() const
    {
        return 6;
    }
    size_t HeaderSize() const
    {
        return 14;
    }

    const uint8_t *GetUnicastAddress() const
    {
        return macUnicastAddress;
    }
    const uint8_t *GetBroadcastAddress() const
    {
        return macBroadcastAddress;
    }

    TCPIP::DataBuffer *GetTxBuffer()
    {
        return &dataBuf;
    }
    void FreeTxBuffer(TCPIP::DataBuffer *)
    {
    }
    void FreeRxBuffer(TCPIP::DataBuffer *)
    {
    }

    void Transmit(TCPIP::DataBuffer *data, const uint8_t *targetMAC, uint16_t type)
    {
        memcpy(data_, data->Packet, data->Length);
    }
    void Retransmit(TCPIP::DataBuffer *buffer)
    {
    }
};

// class mockMACnullptr : public mockMAC{
// public:
// TCPIP::DataBuffer *GetTxBuffer()
//     {
//         return nullptr;
//     }
// };

