#ifndef _MQTT_QUEUE_
#define _MQTT_QUEUE_

#include "global-defines.h"
#include "IComponent.h"
#include <queue>

class MQTTClient;
class MqttPayload;

class MqttQueue : public IComponent
{
public:
  MqttQueue(MQTTClient& mqttClient);
  void PushPayload(MqttPayload* payload);
  void PushPayload(String topic, String payload);
  // IComponent
  virtual void Update() override;

private:
  MQTTClient& mqttClient;
  std::queue<MqttPayload*> queue;
  void PublishQueue();
  void FlushQueue();
};

#endif
