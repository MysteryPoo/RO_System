/*
  IOnConnect - Callback interface to respond to MQTT connections
*/

#ifndef _IONCONNECT_
#define _IONCONNECT_

class IOnConnect
{
public:
  virtual void OnConnect(bool) = 0;
};

#endif
