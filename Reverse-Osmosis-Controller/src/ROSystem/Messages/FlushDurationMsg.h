#ifndef _FLUSHDURATIONMESSAGE_
#define _FLUSHDURATIONMESSAGE_

namespace RoSystemMessage
{
  struct FlushDuration
  {
    FlushDuration(int duration)
      : duration(duration)
    {}

    int duration;
  };
}

#endif
