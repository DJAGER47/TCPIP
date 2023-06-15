

#include "ICMP.h"

// #include <stdio.h>
// #include "FCS.h"
#include "IPv4.h"
#include "Utility.h"


namespace TCPIP
{
    void ICMP::ProcessRx(DataBuffer *buffer, const uint8_t *remoteIP, const uint8_t *)
    {
        uint8_t type;
        uint8_t code;
        DataBuffer *txBuffer;
        uint16_t i;

        type = buffer->Packet[0];
        code = buffer->Packet[1];

        switch (type)
        {
        case 8: // echo request
            // txBuffer = IP.GetTxBuffer(buffer->MAC);
            // if (txBuffer && buffer->Length <= txBuffer->Remainder)
            // {
            //     for (i = 0; i < buffer->Length; i++)
            //     {
            //         txBuffer->Packet[i] = buffer->Packet[i];
            //     }
            //     txBuffer->Packet[0] = 0;
            //     Pack16(txBuffer->Packet, 2, 0); // clear the checksum
            //     i = detail::CalculateIcmpChecksum(txBuffer->Packet, buffer->Length);
            //     Pack16(txBuffer->Packet, 2, i); // set the checksum
            //     txBuffer->Length = buffer->Length;
            //     IP.Transmit(txBuffer, 0x01, remoteIP, IP.GetUnicastAddress());
            // }
            break;
        default:
            break;
        }
    }

}