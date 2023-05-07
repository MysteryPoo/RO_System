#ifndef _ROSYSTEMSTATECHANGEMESSAGE_
#define _ROSYSTEMSTATECHANGEMESSAGE_

#include "ROSystem/RoSystemEnum.h"

namespace RoSystemMessage
{
  struct StateChange
  {
    StateChange(RoSystemEnum::State state, bool success, const char* requestReason, const char* failureReason)
      : state(state)
      , success(success)
      , requestReason(requestReason)
      , failureReason(failureReason)
    {}

    RoSystemEnum::State state;
    bool success;
    const char* requestReason;
    const char* failureReason;
  };
}

#endif
