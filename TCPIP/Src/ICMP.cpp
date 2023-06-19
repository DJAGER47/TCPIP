#include "ICMP.h"
#include "Utility.h"

namespace TCPIP
{
  void ICMP::ProcessRx(const EthBuff *buffer, size_t offset, const uint8_t *sourceIP)
  {
    const uint8_t *packet = buffer->buff + offset;
    ICMPInfo info;
    info.type = packet[0];
    info.code = packet[1];
    info.checksum = 0;

    switch (info.type)
    {
    case Icmp_ECHO: // echo request
    {
      info.id = detail::Unpack16(packet, 4);
      info.seqnum = detail::Unpack16(packet, 6);

      EthBuff *txBuf = ip_.GetTxBuffer();
      if (txBuf == nullptr)
      {
        log_.print_log(InterfaceLogger::WARNING, "ICMP: ICMP failed to get tx buffer\n");
        return;
      }
      size_t offset = ip_.GetTxOffset();
      offset = detail::Pack8(txBuf->buff, offset, Icmp_ER);
      offset = detail::Pack8(txBuf->buff, offset, 0);
      offset = detail::Pack16(txBuf->buff, offset, detail::CalculateChecksum(txBuf->buff + ip_.GetTxOffset(), 2));
      txBuf->tot_len = txBuf->len = offset;

      ip_.Transmit(txBuf, InterfaceIP::Protocol::pICMP, sourceIP, ip_.GetUnicastAddress());
    }
    break;

    default:
      log_.print_log(InterfaceLogger::WARNING, "ICMP: Unsupported ICMP Protocol 0x%02X\n", info.type);
      break;
    }
  }
}