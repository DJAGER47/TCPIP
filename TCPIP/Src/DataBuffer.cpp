#include "DataBuffer.h"

namespace TCPIP
{
    void DataBuffer::Initialize(InterfaceMAC *mac)
    {
        Packet = Data;
        Length = 0;
        Remainder = DATA_BUFFER_PAYLOAD_SIZE;
        Disposable = true;
        mac_ = mac;
    }

    void DataBuffer::Preallocate(size_t size)
    {
        Packet += size;
        Remainder -= size;
    }

    void DataBuffer::ResetPreallocation(size_t size)
    {
        Packet -= size;
        Remainder += size;
    }
}