#ifndef _WIFISIGNALMESSAGE_
#define _WIFISIGNALMESSAGE_

namespace WifiMessage
{
  struct Signal
  {
    Signal(float signal)
      : signal(signal)
    {}

    float signal;
  };
}

#endif
