#ifndef _MQTT_QUEUE_
#define _MQTT_QUEUE_

#include "global-defines.h"
#include <queue>

class MqttPayload;
class String;

class MqttQueue
{
public:
  MqttQueue();
  void PushPayload(MqttPayload* payload);
  void PushPayload(String topic, String payload);
  MqttPayload* FrontPayload() const;
  void PopPayload();

private:
  std::queue<MqttPayload*> queue;
  void FlushQueue();
};

#endif
