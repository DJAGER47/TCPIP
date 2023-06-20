#include "MACEthernet.h"
#include "Utility.h"

namespace TCPIP
{

  // public:

  MACEthernet::MACEthernet(ARP &arp, InterfaceIP &ipv4, InterfaceLogger &log) : TxHandler(nullptr), arp_(arp),
                                                                                ipv4_(ipv4), log_(log)
  {
    memset(BroadcastAddress, 0xFF, GetAddressSize());
  }

  /// @brief This function sets the unicast address of the MACEthernet object.
  /// @param addr The parameter "addr" is a pointer to a constant array of 8-bit unsigned integers
  /// (uint8_t), which represents the MAC address that will be copied to the "UnicastAddress" array.
  void MACEthernet::SetUnicastAddress(const uint8_t *addr)
  {
    memcpy(UnicastAddress, addr, GetAddressSize());
  }

  /// @brief The function processes incoming Ethernet packets and checks if the MAC address is destined for the
  /// device, then forwards the packet to the appropriate protocol handler based on the packet's type.
  /// @param buffer A pointer to an EthBuff structure, which contains the received Ethernet packet data.
  void MACEthernet::ProcessRx(const EthBuff *buffer)
  {
    if (buffer == nullptr)
      return;

    // Check if the MAC Address is destined for me
    if (IsThisMyAddress(buffer->buff))
    {
      log_.print_log(InterfaceLogger::INFO, "MAC: Pocket size %d\n", buffer->tot_len);

      uint16_t type = detail::Unpack16(buffer->buff, 12);
      switch (type)
      {
      case InterfaceMAC::EtherType::etIPv4:
        ipv4_.ProcessRx(buffer, GetHeaderSize());
        break;
      case InterfaceMAC::EtherType::etARP:
        arp_.ProcessRx(buffer, GetHeaderSize());
        break;
      default:
        log_.print_log(InterfaceLogger::WARNING,
                       "MAC: Unsupported Unicast type 0x%04X\n", type);
        break;
      }
    }
  }

  /// @brief The Transmit function packs the target MAC address, source MAC address, and type into a buffer and
  /// calls the TxHandler function if it exists.
  /// @param buffer A pointer to an EthBuff struct, which contains the Ethernet frame data to be transmitted.
  /// @param targetMAC A pointer to an array of 6 bytes representing the destination MAC address of the
  /// Ethernet frame being transmitted.
  /// @param type The type parameter is a 16-bit unsigned integer that specifies the type of the Ethernet
  /// frame being transmitted. It is used to identify the protocol being used in the payload of the frame.
  void MACEthernet::Transmit(EthBuff *buffer, const uint8_t *targetMAC, uint16_t type)
  {
    size_t offset = 0;
    offset = detail::PackBytes(buffer->buff, offset, targetMAC, ADDRESS_SIZE);
    offset = detail::PackBytes(buffer->buff, offset, UnicastAddress, ADDRESS_SIZE);
    detail::Pack16(buffer->buff, offset, type);

    if (buffer->tot_len < min_payload)
    {
      memset(buffer->buff + buffer->tot_len, 0, min_payload - buffer->tot_len);
    }

    if (TxHandler)
    {
      TxHandler(buffer);
    }
  }

  // private:

  /// @brief The function checks if a given address matches the device's unicast or broadcast address.
  /// @param addr The parameter `addr` is a pointer to a uint8_t array that represents an Ethernet address.
  /// @return a boolean value.
  bool MACEthernet::IsThisMyAddress(const uint8_t *addr)
  {
    return detail::AddressCompare(UnicastAddress, addr, GetAddressSize()) ||
           detail::AddressCompare(BroadcastAddress, addr, GetAddressSize());
  }
}