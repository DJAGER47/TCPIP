#include "DefaultStack.h"

namespace TCPIP
{

  DefaultStack::DefaultStack()
      : log_(), mac_(arp_, ip_, log_), ip_(), arp_(mac_, ip_, log_)
  {
  }

}
