#include <string>
#include <iostream>
#include "logduration.h"
#include "test_framework.h"

#include "Logger.h"
#include "Mock.h"

#include "ARP.h"

void TestBase()
{
  Logger log;
  mockMAC mac;
  TCPIP::IPv4 ip;

  uint8_t mac_data[] = {MAC_UnicastAddress};
  memcpy(mac.macUnicastAddress, mac_data, sizeof mac_data);

  TCPIP::IPv4::AddressIP4Settings infoIp = {
    .Address = {IP_UnicastAddress},
    .SubnetMask = {255, 255, 255, 0},
    .Gateway = {192, 168, 1, 1}
  };
  ip.SetAddressInfo(infoIp);

  uint8_t arp_pkg[] = {
  0x00, 0x01,
  0x08, 0x00,
  0x06,
  0x04,
  0x00, 0x01,
  0x34, 0xce, 0x00, 0x64, 0x8a, 0x0b,
  0xc0, 0xa8, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  IP_UnicastAddress};

  TCPIP::ARP arp(mac, ip, log);

  mockDataBuf buf(arp_pkg);
  arp.ProcessRx(&buf);

  uint8_t arp_pkg_replay[] = {
    0x00, 0x01,
    0x08, 0x00,
    0x06,
    0x04,
    0x00, 0x02,
    MAC_UnicastAddress,
    IP_UnicastAddress,
    0x34, 0xce, 0x00, 0x64, 0x8a, 0x0b,
    0xc0, 0xa8, 0x01, 0x01
  };

  ASSERT(!memcmp(mac.data_, arp_pkg_replay, sizeof arp_pkg_replay));
}

void TestNullPtr()
{
  // Logger log;
  // mockMACnullptr mac;
  // TCPIP::IPv4 ip;

  // uint8_t mac_data[] = {MAC_UnicastAddress};
  // memcpy(mac.macUnicastAddress, mac_data, sizeof mac_data);

  // TCPIP::IPv4::AddressIP4Settings infoIp = {
  //   .Address = {IP_UnicastAddress},
  //   .SubnetMask = {255, 255, 255, 0},
  //   .Gateway = {192, 168, 1, 1}
  // };
  // ip.SetAddressInfo(infoIp);

  // uint8_t arp_pkg[] = {
  // 0x00, 0x01,
  // 0x08, 0x00,
  // 0x06,
  // 0x04,
  // 0x00, 0x01,
  // 0x34, 0xce, 0x00, 0x64, 0x8a, 0x0b,
  // 0xc0, 0xa8, 0x01, 0x01,
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // IP_UnicastAddress};

  // TCPIP::ARP arp(mac, ip, log);

  // mockDataBuf buf(arp_pkg);
  // arp.ProcessRx(&buf);

  // uint8_t arp_pkg_replay[] = {
  //   0x00, 0x01,
  //   0x08, 0x00,
  //   0x06,
  //   0x04,
  //   0x00, 0x02,
  //   MAC_UnicastAddress,
  //   IP_UnicastAddress,
  //   0x34, 0xce, 0x00, 0x64, 0x8a, 0x0b,
  //   0xc0, 0xa8, 0x01, 0x01
  // };

  // ASSERT(!memcmp(mac.data_, arp_pkg_replay, sizeof arp_pkg_replay));
}

void TestAll1()
{
  TestRunner tr;
  RUN_TEST(tr, TestBase);
}

////////////////////////////////////////////////////////////////////////////

int main()
{
  try
  {
    TestAll1();
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}