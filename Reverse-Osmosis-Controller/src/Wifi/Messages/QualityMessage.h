#ifndef _WIFIQUALITYMESSAGE_
#define _WIFIQUALITYMESSAGE_

namespace WifiMessage
{
  struct Quality
  {
    Quality(float quality)
      : quality(quality)
    {}

    float quality;
  };
}

#endif
