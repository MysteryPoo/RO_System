/*
  MqttSubscriber
*/

#ifndef _MQTTSUBSCRIBER_
#define _MQTTSUBSCRIBER_

class MqttSubscriber
{
public:
  virtual void Update(const char* payload) = 0;
};

#endif
