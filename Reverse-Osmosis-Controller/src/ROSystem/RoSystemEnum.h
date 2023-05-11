
#ifndef _ROSYSTEMENUM_
#define _ROSYSTEMENUM_

#include "spark_wiring_string.h"
#include <stdint.h>

namespace RoSystemEnum
{
  enum State : uint8_t
  {
    BOOT,
    IDLE,
    FLUSH,
    FILL
  };

  inline String ToString(State state)
  {
    String stateString;
    switch(state)
    {
      case State::IDLE:
        stateString = "IDLE";
        break;
      case State::FILL:
        stateString = "FILL";
        break;
      case State::FLUSH:
        stateString = "FLUSH";
        break;
      default:
        stateString = "INVALID";
    }
    return stateString;
  }
}

#endif
