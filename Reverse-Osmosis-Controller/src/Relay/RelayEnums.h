
#ifndef _RELAYENUMS_
#define _RELAYENUMS_

#include <stdint.h>

namespace RelayEnums
{
  enum Name : uint8_t
  {
    COMPONENT_PUMP,
    COMPONENT_INLETVALVE,
    COMPONENT_FLUSHVALVE
  };
  enum State : uint8_t
  {
    ON,
    OFF
  };

  inline const String ToString(Name name)
  {
    switch(name)
    {
      case COMPONENT_PUMP:
        return "pump";
      case COMPONENT_INLETVALVE:
        return "inlet-valve";
      case COMPONENT_FLUSHVALVE:
        return "flush-valve";
      default:
        return "unknown";
    }
  }

  inline const String ToString(State state)
  {
    switch(state)
    {
      case ON:
        return "on";
      case OFF:
        return "off";
      default:
        return "unknown";
    }
  }
}

#endif
