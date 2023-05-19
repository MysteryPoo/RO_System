/*
  IOnConnect - Callback interface to respond to MQTT connections
*/

#ifndef _IONCONNECT_
#define _IONCONNECT_

class IOnConnect
{
public:
  virtual void OnConnect(const bool) const = 0;
};

#endif
