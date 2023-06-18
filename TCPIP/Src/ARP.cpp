#include "ARP.h"
#include "Utility.h"

namespace TCPIP
{
  // public:

  /// @brief Parsing the package
  /// @param buffer picket data
  /// @param offset offset of ARP message data
  void ARP::ProcessRx(const EthBuff *buffer, size_t offset)
  {
    ARPInfo info;
    const uint8_t *packet = buffer->buff + offset;
    info.HTYPE = detail::Unpack16(packet, 0);
    info.PTYPE = detail::Unpack16(packet, 2);
    info.HLEN = detail::Unpack8(packet, 4);
    info.PLEN = detail::Unpack8(packet, 5);
    info.OPER = detail::Unpack16(packet, 6);
    auto index = 8;
    memcpy(info.SHA, &packet[index], sizeof(info.SHA));
    index += info.HLEN;
    memcpy(info.SPA, &packet[index], sizeof(info.SPA));
    index += info.PLEN;
    memcpy(info.THA, &packet[index], sizeof(info.THA));
    index += info.HLEN;
    memcpy(info.TPA, &packet[index], sizeof(info.TPA));

    if (info.OPER == request)
    {
      if (info.HLEN == mac_.GetAddressSize() &&
          info.PLEN == ip_.GetAddressSize())
      {
        // Compare iP addresses
        if (detail::AddressCompare(info.TPA, ip_.GetUnicastAddress(), ip_.GetAddressSize()))
        {
          // This ARP is for me
          SendReply(info);
          log_.print_log(InterfaceLogger::INFO, "ARP: This ARP is for me\n");
        }
      }
    }
    else if (info.OPER == reply)
    {
      // Add mac <-> IP to cache
      Add(info.SPA, info.SHA);
      log_.print_log(InterfaceLogger::INFO, "ARP: Add mac <-> IP to cache\n");
    }
    else
    {
      log_.print_log(InterfaceLogger::WARNING, "ARP: ARP OPER = %d\n", info.OPER);
    }
  }

  /// @brief Add to cache table
  /// @param protocolAddress IP address
  /// @param hardwareAddress MAC address
  void ARP::Add(const uint8_t *protocolAddress, const uint8_t *hardwareAddress)
  {
    UNUSED(protocolAddress);
    UNUSED(hardwareAddress);
    log_.print_log(InterfaceLogger::WARNING, "ARP: Realize a cache table\n");
    log_.print_log(InterfaceLogger::WARNING, "ARP: %03d.%03d.%03d.%03d <-> %02x:%02x:%02x:%02x:%02x:%02x\n",
                   protocolAddress[0], protocolAddress[1], protocolAddress[2], protocolAddress[3],
                   hardwareAddress[0], hardwareAddress[1], hardwareAddress[2], hardwareAddress[3], hardwareAddress[4], hardwareAddress[5]);
  }

  /// @brief Send a request for MAC address by IP
  /// @param protocolAddress target IP
  void ARP::SendRequest(const uint8_t *protocolAddress)
  {
    EthBuff *txBuffer = mac_.GetTxBuffer();
    if (txBuffer == nullptr)
    {
      log_.print_log(InterfaceLogger::WARNING, "ARP: ARP failed to get tx buffer\n");
      return;
    }
    size_t offset = mac_.GetHeaderSize();
    offset = detail::Pack16(txBuffer->buff, offset, hardwareType);                                       // Hardware Type
    offset = detail::Pack16(txBuffer->buff, offset, InterfaceMAC::EtherType::etIPv4);                    // Protocol Type
    offset = detail::Pack8(txBuffer->buff, offset, mac_.GetAddressSize());                               // Hardware Size
    offset = detail::Pack8(txBuffer->buff, offset, ip_.GetAddressSize());                                // Protocol Size
    offset = detail::Pack16(txBuffer->buff, offset, request);                                            // Op
    offset = detail::PackBytes(txBuffer->buff, offset, mac_.GetUnicastAddress(), mac_.GetAddressSize()); // Sender's Hardware Address
    offset = detail::PackBytes(txBuffer->buff, offset, ip_.GetUnicastAddress(), ip_.GetAddressSize());   // Sender's Protocol Address
    offset = detail::PackFill(txBuffer->buff, offset, 0, mac_.GetAddressSize());                         // Target's Hardware Address
    offset = detail::PackBytes(txBuffer->buff, offset, protocolAddress, 4);                              // Target's Protocol Address
    txBuffer->tot_len = txBuffer->len = offset;

    mac_.Transmit(txBuffer, mac_.GetBroadcastAddress(), InterfaceMAC::EtherType::etARP);
  }

  // private:

  /// @brief Reply to Mac Address Request
  /// @param info ARP packet
  void ARP::SendReply(const ARPInfo &info)
  {
    EthBuff *txBuffer = mac_.GetTxBuffer();
    if (txBuffer == nullptr)
    {
      log_.print_log(InterfaceLogger::WARNING, "ARP: ARP failed to get tx buffer\n");
      return;
    }
    size_t offset = mac_.GetHeaderSize();
    offset = detail::Pack16(txBuffer->buff, offset, info.HTYPE);
    offset = detail::Pack16(txBuffer->buff, offset, info.PTYPE);
    offset = detail::Pack8(txBuffer->buff, offset, info.HLEN);
    offset = detail::Pack8(txBuffer->buff, offset, info.PLEN);
    offset = detail::Pack16(txBuffer->buff, offset, reply);                                  // ARP Reply
    offset = detail::PackBytes(txBuffer->buff, offset, mac_.GetUnicastAddress(), info.HLEN); // My mac addres
    offset = detail::PackBytes(txBuffer->buff, offset, ip_.GetUnicastAddress(), info.PLEN);  // My ip  adress
    offset = detail::PackBytes(txBuffer->buff, offset, info.SHA, info.HLEN);
    offset = detail::PackBytes(txBuffer->buff, offset, info.SPA, info.PLEN);
    txBuffer->tot_len = txBuffer->len = mac_.GetHeaderSize();

    mac_.Transmit(txBuffer, info.SHA, InterfaceMAC::EtherType::etARP);
  }
}
