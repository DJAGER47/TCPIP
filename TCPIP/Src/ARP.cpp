#include "ARP.h"
#include "Utility.h"

namespace TCPIP
{
  // public:

  /// @brief The function processes an ARP packet received by the device and takes appropriate actions based on
  /// the type of ARP operation.
  /// @param buffer A pointer to an EthBuff object, which contains the Ethernet frame buffer.
  /// @param offset The offset parameter is the starting position of the ARP packet within the Ethernet buffer.
  TErr ARP::ProcessRx(const EthBuff *buffer, size_t offset)
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
          AddEntry(info.SPA, info.SHA);
          SendReply(info);
          if (log_ != nullptr)
          {
            log_->print_log(InterfaceLogger::INFO, "ARP: This ARP is for me\n");
          }
        }
      }
    }
    else if (info.OPER == reply)
    {
      // Add mac <-> IP to cache
      AddEntry(info.SPA, info.SHA);
    }
    else
    {
      if (log_ != nullptr)
      {
        log_->print_log(InterfaceLogger::WARNING, "ARP: ARP OPER = %d\n", info.OPER);
      }
    }
    return TErr::eOk;
  }

  /// @brief The function adds an IP-MAC address pair to the ARP cache, updating an existing entry if it
  /// exists, and deleting the oldest entry if the cache is full.
  /// @param ip_address A pointer to an array of 4 bytes representing an IPv4 address in network byte order.
  /// @param mac_address A pointer to an array of 6 bytes representing the MAC address to be added to the ARP cache.
  void ARP::AddEntry(const uint8_t *ip_address, const uint8_t *mac_address)
  {
    if (log_ != nullptr)
    {
      log_->print_log(InterfaceLogger::INFO, "ARP: %03d.%03d.%03d.%03d <-> %02x:%02x:%02x:%02x:%02x:%02x to cache\n",
                     ip_address[0], ip_address[1], ip_address[2], ip_address[3],
                     mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
    }
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

  /// @brief The function returns the MAC address corresponding to a given IP address if it is present in the
  /// cache, otherwise it returns a null pointer.
  /// @param targetIP The targetIP parameter is a pointer to an array of 4 bytes representing the IP
  /// address of the device whose MAC address is being requested.
  /// @return The function `Ip2Mac` returns a pointer to a `uint8_t` array, which represents the MAC
  /// address of the device with the specified IP address. If the IP address is not found in the ARP
  /// cache, the function returns a null pointer.
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

  /// @brief The SendRequest function sends an ARP request to a target IP address using the Ethernet buffer and
  /// MAC address.
  /// @param targetIP A pointer to a uint8_t array representing the IP address of the target device to
  /// which an ARP request is being sent.
  TErr ARP::SendRequest(const uint8_t *targetIP)
  {
    EthBuff *txBuf = mac_.GetTxBuffer();
    if (txBuf == nullptr)
    {
      if (log_ != nullptr)
      {
        log_->print_log(InterfaceLogger::WARNING, "ARP: ARP failed to get tx buffer\n");
      }
      return TErr::eAlloc;  
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
    return TErr::eOk;
  }

  // private:

  /// @brief  The function sends an ARP reply message with the provided information.
  /// @param info A reference to an ARPInfo struct that contains information about the ARP request that
  /// this reply is in response to. This includes the target's hardware and protocol addresses (SHA and
  /// SPA), as well as the hardware and protocol types and sizes (HTYPE, PTYPE, HLEN, and PLEN).
  TErr ARP::SendReply(const ARPInfo &info)
  {
    EthBuff *txBuf = mac_.GetTxBuffer();
    if (txBuf == nullptr)
    {
      if (log_ != nullptr)
      {
        log_->print_log(InterfaceLogger::WARNING, "ARP: ARP failed to get tx buffer\n");
      }
      return TErr::eAlloc;
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
    return TErr::eOk;
  }

  /// @brief The function searches for an entry in an ARP cache based on an IP address and returns its index if
  /// found, or -1 if not found.
  /// @param ip_address A pointer to an array of uint8_t representing an IP address.
  /// @return an integer value, which is the index of the entry in the ARP cache that matches the given IP
  /// address. If no matching entry is found, the function returns -1.
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
