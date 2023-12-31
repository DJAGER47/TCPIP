#include "IPv4.h"
#include "Utility.h"

namespace TCPIP
{

  // public:

  /// @brief The function processes an incoming IPv4 packet, extracts its header information, checks its
  /// checksum, and forwards it to the appropriate protocol handler based on its protocol field.
  /// @param buffer A pointer to an EthBuff object, which contains the Ethernet frame buffer.
  /// @param offset The offset parameter is the starting position of the IPv4 header in the Ethernet buffer.
  TErr IPv4::ProcessRx(const EthBuff *buffer, size_t offset)
  {
    const uint8_t *packet = buffer->buff + offset;
    HeaderIPv4 head;
    head.Version = detail::Unpack8(packet, 0) & 0xF0 >> 4;
    head.IHL = detail::Unpack8(packet, 0) & 0x0F;
    head.TypeOfService = detail::Unpack8(packet, 1);
    head.TotalLength = detail::Unpack16(packet, 2);
    head.Identification = detail::Unpack16(packet, 4);
    head.Flags = detail::Unpack8(packet, 6) & 0xE0 >> 5;
    head.FragmentOffset = detail::Unpack16(packet, 6) & 0x1FFFU;
    head.TTL = detail::Unpack8(packet, 7);
    head.Protocol = detail::Unpack8(packet, 9);
    head.Crc = detail::Unpack16(packet, 10);
    head.sourceIP = &packet[12];
    head.targetIP = &packet[16];

    if (detail::CalculateChecksum(packet, HEADER_SIZE))
    {
      if (log_ != nullptr)
      {
        log_->print_log(InterfaceLogger::ERROR, "IPv4: CRC is not correct\n");
      }
      return TErr::eCRC;
    }

    if (IsThisMyAddress(head.targetIP))
    {
      switch (head.Protocol)
      {
      case InterfaceIP::Protocol::pICMP:
        if (log_ != nullptr)
        {
          log_->print_log(InterfaceLogger::INFO, "IPv4: ICMP\n");
        }
        if (icmp_ != nullptr)
        {
          icmp_->ProcessRx(buffer, offset + HEADER_SIZE, head.sourceIP);
        }
        break;

      default:
        if (log_ != nullptr)
        {
          log_->print_log(InterfaceLogger::WARNING, "IPv4: Unsupported IP Protocol 0x%02X\n", head.Protocol);
        }
        break;
      }
    }
    return TErr::eOk;
  }

  /// @brief The Transmit function sends an IPv4 packet with a specified protocol, target IP, and source IP using Ethernet.
  /// @param buffer A pointer to an EthBuff object, which contains the Ethernet frame buffer to be transmitted.
  /// @param protocol The protocol field in the IPv4 header, indicating the protocol used in the data
  /// portion of the IP datagram. Examples include TCP, UDP, ICMP, etc.
  /// @param targetIP A pointer to an array of 4 bytes representing the destination IPv4 address.
  /// @param sourceIP The source IP address of the IPv4 packet to be transmitted.
  TErr IPv4::Transmit(EthBuff *buffer, uint8_t protocol, const uint8_t *targetIP, const uint8_t *sourceIP)
  {
    size_t offset = mac_.GetTxOffset();
    uint8_t *packet = buffer->buff + offset;

    packet[0] = (IpVersion << 4) | (IpDefHeaderLength);                                       // Version and HeaderSize
    packet[1] = 0;                                                                            // Type of Service
    detail::Pack16(packet, 2, static_cast<uint16_t>(buffer->tot_len - mac_.GetHeaderSize())); // TotalLength
    detail::Pack16(packet, 4, ++PacketID);                                                    // Identification
    packet[6] = 0;                                                                            // Flags & FragmentOffset
    packet[7] = 0;                                                                            // FragmentOffset
    packet[8] = 32;                                                                           // TTL
    packet[9] = protocol;                                                                     // Protocol
    detail::Pack16(packet, 10, 0);                                                            // checksum
    detail::PackBytes(packet, 12, sourceIP, ADDRESS_SIZE);
    detail::PackBytes(packet, 16, targetIP, ADDRESS_SIZE);
    detail::Pack16(packet, 10, detail::CalculateChecksum(packet, HEADER_SIZE));

    const uint8_t *targetMAC = arp_.Ip2Mac(targetIP);
    if (targetMAC != nullptr)
    {
      mac_.Transmit(buffer, targetMAC, InterfaceMAC::EtherType::etIPv4);
    }
    else
    {
      if (log_ != nullptr)
      {
        log_->print_log(InterfaceLogger::WARNING, "IPv4: Can't send it, don't know MAC\n");
      }
    }
    return TErr::eOk;
  }

  /// @brief The function sets the IPv4 address information and calculates the broadcast address.
  /// @param info AddressIP4Settings object that contains information about the IPv4 address, such as the
  /// network address, subnet mask, and host address.
  void IPv4::SetAddressInfo(const AddressIP4Settings &info)
  {
    address_ = info;
    calcBroadcastAddress();
  }

  // private:

  /// @brief The function checks if a given IPv4 address matches the unicast or broadcast address of the current object.
  /// @param addr A pointer to an array of 4 bytes representing an IPv4 address.
  bool IPv4::IsThisMyAddress(const uint8_t *addr)
  {
    return detail::AddressCompare(addr, GetUnicastAddress(), GetAddressSize()) ||
           detail::AddressCompare(addr, GetBroadcastAddress(), GetAddressSize());
  }

  /// @brief The function calculates the broadcast address for an IPv4 address.
  void IPv4::calcBroadcastAddress(void)
  {
    uint32_t ipAddress = detail::Unpack32(address_.Address, 0);
    uint32_t subnetMask = detail::Unpack32(address_.SubnetMask, 0);
    uint32_t broadcastAddress = (ipAddress & subnetMask) | ~subnetMask;
    detail::Pack32(Broadcast, 0, broadcastAddress);
  }
}