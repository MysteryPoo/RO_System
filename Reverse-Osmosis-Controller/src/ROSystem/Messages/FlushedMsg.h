#ifndef _ROSYSTEMFLUSHEDMESSAGE_
#define _ROSYSTEMFLUSHEDMESSAGE_

namespace RoSystemMessage
{
  struct Flushed
  {
    Flushed(bool flushed)
      : flushed(flushed)
    {}

    bool flushed;
  };
}

#endif
