#ifndef _BAROMETERPRESSUREMESSAGE_
#define _BAROMETERPRESSUREMESSAGE_

namespace BarometerMessage
{
  struct Pressure
  {
    Pressure(int32_t pressure)
      : pressure(pressure)
    {}

    int32_t pressure;
  };
}

#endif
