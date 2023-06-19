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
      AddEntry(info.SPA, info.SHA);
      log_.print_log(InterfaceLogger::INFO, "ARP: Add mac <-> IP to cache\n");
      log_.print_log(InterfaceLogger::INFO, "ARP: %03d.%03d.%03d.%03d <-> %02x:%02x:%02x:%02x:%02x:%02x to cache\n",
                     info.SPA[0], info.SPA[1], info.SPA[2], info.SPA[3],
                     info.SHA[0], info.SHA[1], info.SHA[2], info.SHA[3], info.SHA[4], info.SHA[5]);
    }
    else
    {
      log_.print_log(InterfaceLogger::WARNING, "ARP: ARP OPER = %d\n", info.OPER);
    }
  }

  /// @brief Add to cache table
  /// @param ip_address IP address
  /// @param mac_address MAC address
  void ARP::AddEntry(const uint8_t *ip_address, const uint8_t *mac_address)
  {
    int index = FindEntry(ip_address);
    if (index != -1)
    {
      // update mac
      memcpy(cache_[index].mac_address, mac_address, MAC_ADDR_SIZE);
    }
    else if (num_entries_ < MAX_ENTRIES)
    {
      // Add new IP <-> MAC
      memcpy(cache_[num_entries_].ip_address, ip_address, IP_ADDR_SIZE);
      memcpy(cache_[num_entries_].mac_address, mac_address, MAC_ADDR_SIZE);
      ++num_entries_;
    }
    else
    {
      // the cache is full, let's delete the oldest entry by shifting the entire array
      memmove(cache_[0].ip_address, cache_[1].ip_address, (MAX_ENTRIES - 1) * IP_ADDR_SIZE);
      memmove(cache_[0].mac_address, cache_[1].mac_address, (MAX_ENTRIES - 1) * MAC_ADDR_SIZE);
      memcpy(cache_[MAX_ENTRIES - 1].ip_address, ip_address, IP_ADDR_SIZE);
      memcpy(cache_[MAX_ENTRIES - 1].mac_address, mac_address, MAC_ADDR_SIZE);
    }
  }

  /// @brief Match IP address and MAC address
  /// @param targetIP IPv4
  /// @return target MAC
  const uint8_t *ARP::Ip2Mac(const uint8_t *targetIP)
  {
    int index = FindEntry(targetIP);
    if (index != -1)
    {
      return cache_[index].mac_address;
    }
    else
    {
      return nullptr;
    }
  }

  /// @brief Send a request for MAC address by IP
  /// @param targetIP target IP
  void ARP::SendRequest(const uint8_t *targetIP)
  {
    EthBuff *txBuf = mac_.GetTxBuffer();
    if (txBuf == nullptr)
    {
      log_.print_log(InterfaceLogger::WARNING, "ARP: ARP failed to get tx buffer\n");
      return;
    }
    size_t offset = mac_.GetTxOffset();
    offset = detail::Pack16(txBuf->buff, offset, hardwareType);                                       // Hardware Type
    offset = detail::Pack16(txBuf->buff, offset, InterfaceMAC::EtherType::etIPv4);                    // Protocol Type
    offset = detail::Pack8(txBuf->buff, offset, mac_.GetAddressSize());                               // Hardware Size
    offset = detail::Pack8(txBuf->buff, offset, ip_.GetAddressSize());                                // Protocol Size
    offset = detail::Pack16(txBuf->buff, offset, request);                                            // Op
    offset = detail::PackBytes(txBuf->buff, offset, mac_.GetUnicastAddress(), mac_.GetAddressSize()); // Sender's Hardware Address
    offset = detail::PackBytes(txBuf->buff, offset, ip_.GetUnicastAddress(), ip_.GetAddressSize());   // Sender's Protocol Address
    offset = detail::PackFill(txBuf->buff, offset, 0, mac_.GetAddressSize());                         // Target's Hardware Address
    offset = detail::PackBytes(txBuf->buff, offset, targetIP, 4);                                     // Target's Protocol Address
    txBuf->tot_len = txBuf->len = offset;

    mac_.Transmit(txBuf, mac_.GetBroadcastAddress(), InterfaceMAC::EtherType::etARP);
  }

  // private:

  /// @brief Reply to Mac Address Request
  /// @param info ARP packet
  void ARP::SendReply(const ARPInfo &info)
  {
    EthBuff *txBuf = mac_.GetTxBuffer();
    if (txBuf == nullptr)
    {
      log_.print_log(InterfaceLogger::WARNING, "ARP: ARP failed to get tx buffer\n");
      return;
    }
    size_t offset = mac_.GetTxOffset();
    offset = detail::Pack16(txBuf->buff, offset, info.HTYPE);                             // Hardware Type
    offset = detail::Pack16(txBuf->buff, offset, info.PTYPE);                             // Protocol Type
    offset = detail::Pack8(txBuf->buff, offset, info.HLEN);                               // Hardware Size
    offset = detail::Pack8(txBuf->buff, offset, info.PLEN);                               // Protocol Size
    offset = detail::Pack16(txBuf->buff, offset, reply);                                  // Op - ARP Reply
    offset = detail::PackBytes(txBuf->buff, offset, mac_.GetUnicastAddress(), info.HLEN); // My mac addres
    offset = detail::PackBytes(txBuf->buff, offset, ip_.GetUnicastAddress(), info.PLEN);  // My ip  adress
    offset = detail::PackBytes(txBuf->buff, offset, info.SHA, info.HLEN);                 // Target's Hardware Address
    offset = detail::PackBytes(txBuf->buff, offset, info.SPA, info.PLEN);                 // Target's Protocol Address
    txBuf->tot_len = txBuf->len = offset;

    mac_.Transmit(txBuf, info.SHA, InterfaceMAC::EtherType::etARP);
  }

  /// @brief Search the cache entry by IP address
  /// @param ip_address
  /// @return >0 number in the table, -1 error
  int ARP::FindEntry(const uint8_t *ip_address) const
  {
    for (int i = 0; i < num_entries_; i++)
    {
      if (std::memcmp(cache_[i].ip_address, ip_address, IP_ADDR_SIZE) == 0)
      {
        return i;
      }
    }
    return -1;
  }
}
