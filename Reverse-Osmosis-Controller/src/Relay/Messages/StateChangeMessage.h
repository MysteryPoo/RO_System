#ifndef _RELAYSTATECHANGEMESSAGE_
#define _RELAYSTATECHANGEMESSAGE_

#include "Relay/RelayEnums.h"

namespace RelayMessage
{
  struct StateChange
  {
    StateChange(RelayEnums::State state)
      : state(state)
    {}

    RelayEnums::State state;
  };
}

#endif
