#include "DefaultStack.h"

namespace TCPIP
{

  DefaultStack::DefaultStack()
      : log_(),
        mac_(arp_, ip_, log_),
        ip_(mac_, arp_, icmp_, log_),
        arp_(mac_, ip_, log_),
        icmp_(ip_, log_)
  {
  }

}
