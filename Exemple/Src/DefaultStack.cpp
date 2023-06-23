#include "DefaultStack.h"

namespace TCPIP
{

  DefaultStack::DefaultStack()
      : log_(),
        mac_(Rx_, Tx_, &log_),
        ip_(mac_, arp_, &log_),
        arp_(mac_, ip_, &log_),
        icmp_(ip_, &log_)
  {
    mac_.SetArpClass(&arp_);
    mac_.SetIPv4Class(&ip_);
    ip_.SetIcmpClass(&icmp_);
  }

}
