#include <string>
#include <iostream>
#include "logduration.h"
#include "test_framework.h"

#include "Logger.h"
#include "InterfaceMAC.h"
#include "MACEthernet.h"
#include "ARP.h"
#include "IPv4.h"
#include "ICMP.h"

using namespace TCPIP;

static const uint8_t ADDR_MAC[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
static const TCPIP::IPv4::AddressIP4Settings ADDR_IP = {{192, 168, 0, 20},
                                                        {255, 255, 255, 0},
                                                        {192, 168, 0, 1}};

class DefaultStack
{
public:
  DefaultStack()
      : log_(),
        mac_(arp_, ip_, log_),
        ip_(mac_, arp_, icmp_, log_),
        arp_(mac_, ip_, log_),
        icmp_(ip_, log_)
  {
  }

  Logger log_;
  MACEthernet mac_;
  IPv4 ip_;
  ARP arp_;
  ICMP icmp_;
};

void writeMAC(TCPIP::EthBuff *data)
{
  uint8_t ret[] = {
      0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x08, 0x00,
      // 14
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0};

  ASSERT(!memcmp(data->buff, ret, data->tot_len));
}
void TestMAC()
{
  DefaultStack ds;
  ds.mac_.SetUnicastAddress(ADDR_MAC);
  ds.mac_.RegisterDataTransmitHandler(writeMAC);

  auto pack = ds.mac_.GetTxBuffer();
  memset(pack->buff, 0xFF, sizeof(pack->buff));
  pack->tot_len = pack->len = ds.mac_.GetTxOffset();
  const uint8_t mac[] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
  ds.mac_.Transmit(pack, mac, 0x0800);
}


void writeArp(TCPIP::EthBuff *data)
{
  uint8_t ret[] = {
      0x00, 0x01,
      0x08, 0x00,
      0x06,
      0x04,
      0x00, 0x02,
      0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
      192, 168, 0, 20,
      0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      192, 168, 0, 1,
      // 28
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  ASSERT(!memcmp(data->buff + 14, ret, data->tot_len - 14));
}
void TestARP()
{
  const uint8_t ip[] = {192, 168, 0, 1};
  const uint8_t mac[] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
  DefaultStack ds;
  ds.mac_.SetUnicastAddress(ADDR_MAC);
  ds.mac_.RegisterDataTransmitHandler(writeArp);
  ds.ip_.SetAddressInfo(ADDR_IP);

  auto ret1 = ds.arp_.Ip2Mac(ip);
  ASSERT(ret1 == nullptr);

  auto pack = ds.mac_.GetRxBuffer();
  memset(pack->buff, 0xFF, sizeof(pack->buff));
  uint8_t arp_pkg[] = {
      0x00, 0x01,
      0x08, 0x00,
      0x06,
      0x04,
      0x00, 0x01,
      0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
      192, 168, 0, 1,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      192, 168, 0, 20};

  memcpy(pack->buff + ds.mac_.GetHeaderSize(), arp_pkg, sizeof(arp_pkg));
  pack->len = pack->tot_len = ds.mac_.GetHeaderSize() + sizeof(arp_pkg);

  ds.arp_.ProcessRx(pack, ds.mac_.GetHeaderSize());

  auto ret2 = ds.arp_.Ip2Mac(ip);
  ASSERT(ret2 != nullptr);
  ASSERT(!memcmp(ret2, mac, 6));
}


void writeIP(TCPIP::EthBuff *data)
{
  uint8_t ret[] = {
      0x45, 0x00,
      0x00, 25, 
      0, 1,
      0, 0,
      32, 1,
      0x7E, 0x19,
      192, 168, 0, 20,
      192, 168, 0, 1,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
      };

  ASSERT(!memcmp(data->buff + 14, ret, data->tot_len - 14));
}
void TestIP()
{
  const uint8_t ip[] = {192, 168, 0, 1};
  const uint8_t mac[] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

  DefaultStack ds;
  ds.mac_.SetUnicastAddress(ADDR_MAC);
  ds.mac_.RegisterDataTransmitHandler(writeIP);
  ds.ip_.SetAddressInfo(ADDR_IP);
  ds.arp_.AddEntry(ip, mac);

  auto pack = ds.mac_.GetRxBuffer();
  memset(pack->buff, 0xFF, sizeof(pack->buff));
  pack->len = pack->tot_len = ds.mac_.GetHeaderSize() + ds.ip_.GetHeaderSize() + 5;
  ds.ip_.Transmit(pack, InterfaceIP::Protocol::pICMP, ip, ADDR_IP.Address);
}


void writeICMP(TCPIP::EthBuff *data)
{
  uint8_t ret[] = {
      0x00, 0x00,
      0x00, 0x00,
      // 4
      1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2};

  ASSERT(!memcmp(data->buff + 14 + 20, ret, data->tot_len - 14 - 20));
}
void TestICMP()
{
  const uint8_t ip[] = {192, 168, 0, 1};
  const uint8_t mac[] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
  DefaultStack ds;
  ds.mac_.SetUnicastAddress(ADDR_MAC);
  ds.mac_.RegisterDataTransmitHandler(writeICMP);
  ds.ip_.SetAddressInfo(ADDR_IP);
  ds.arp_.AddEntry(ip, mac);

  auto pack = ds.mac_.GetRxBuffer();
  memset(pack->buff, 0xFF, sizeof(pack->buff));
  uint8_t icmp_pkg[] = {
      0x08, 0x00,
      0xC4, 0xD5,
      // 4
      1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2};

  size_t offset = ds.mac_.GetHeaderSize() + ds.ip_.GetHeaderSize();
  memcpy(pack->buff + offset, icmp_pkg, sizeof(icmp_pkg));
  pack->len = pack->tot_len = offset + sizeof(icmp_pkg);
  ds.icmp_.ProcessRx(pack, offset, ip);
}

////////////////////////////////////////////////////////////////////////////

int main()
{
  try
  {
    TestRunner tr;
    RUN_TEST(tr, TestMAC);
    RUN_TEST(tr, TestARP);
    RUN_TEST(tr, TestIP);
    RUN_TEST(tr, TestICMP);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}