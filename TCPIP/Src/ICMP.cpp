#include "ICMP.h"
#include "Utility.h"

namespace TCPIP
{
  /// @brief The function processes incoming ICMP packets and sends an ICMP echo reply if the packet is an echo request.
  /// @param buffer A pointer to an EthBuff object, which contains the received Ethernet frame.
  /// @param offset The offset parameter is the starting position of the ICMP packet within the Ethernet buffer. 
  /// It is used to locate the ICMP packet within the buffer.
  /// @param sourceIP The source IP address of the received ICMP packet.
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
      offset = detail::PackBytes(txBuf->buff, offset, packet + 4, buffer->tot_len - offset);
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