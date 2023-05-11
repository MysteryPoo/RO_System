#ifndef _MQTT_PAYLOAD_
#define _MQTT_PAYLOAD_

#include "application.h"

class MqttPayload
{
public:
  MqttPayload(String topic, String payload)
  : topic(topic)
  , payload(payload)
  {}
  String topic;
  String payload;
};

#endif
