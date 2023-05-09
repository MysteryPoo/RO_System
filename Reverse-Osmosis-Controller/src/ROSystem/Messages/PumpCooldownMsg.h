#ifndef _PUMPCOOLDOWNMESSAGE_
#define _PUMPCOOLDOWNMESSAGE_

namespace RoSystemMessage
{
  struct PumpCooldown
  {
    PumpCooldown(int cooldown)
      : cooldown(cooldown)
    {}

    int cooldown;
  };
}

#endif
