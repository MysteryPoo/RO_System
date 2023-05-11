#ifndef _ROSYSTEMENABLEDMESSAGE_
#define _ROSYSTEMENABLEDMESSAGE_

namespace RoSystemMessage
{
  struct Enabled
  {
    Enabled(bool enabled)
      : enabled(enabled)
    {}

    bool enabled;
  };
}

#endif
