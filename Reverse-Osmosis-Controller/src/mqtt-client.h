#ifndef _MQTT_CLIENT_
#define _MQTT_CLIENT_

#include "global-defines.h"
#include "MQTT/MQTT.h"
#include "IComponent.h"
#include "MQTT/ISubCallback.h"
#include "system-log.h"

class MQTTClient: public IComponent, public ISubCallback
{
public:
  MQTTClient(SystemLog& logger);
  void Publish();

  // IComponent
  virtual void Update() override;
  // ISubCallback
  virtual void Callback(char*, uint8_t*, unsigned int) override;
private:
  MQTT client;
  const uint8_t ipAddress[4] = {192, 168, 0, 165};
  const short port = 1883;

  SystemLog& logger;
};

#endif
