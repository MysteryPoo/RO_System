#ifndef _FLOATSWITCHRELIABLEMESSAGE_
#define _FLOATSWITCHRELIABLEMESSAGE_

namespace FloatSwitchMessage
{
  struct Reliable
  {
    Reliable(bool reliable)
      : reliable(reliable)
    {}

    bool reliable;
  };
}

#endif
