#ifndef _TIMEHELPER_
#define _TIMEHELPER_

#include "spark_wiring_cloud.h"
#include "spark_wiring_time.h"

namespace TimeHelper
{
  static time32_t GetTimeIfAble()
  {
    return Particle.connected() ? Time.now() : 0;
  }
}

#endif
