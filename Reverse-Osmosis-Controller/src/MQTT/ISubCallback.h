
#ifndef __ISUBCALLBACK_
#define __ISUBCALLBACK_

class MQTTClient;

class ISubCallback
{
public:
  virtual void Callback(char*, uint8_t*, unsigned int) = 0;
  virtual void OnConnect(bool connected, MQTTClient* mqtt) {};
};


#endif
