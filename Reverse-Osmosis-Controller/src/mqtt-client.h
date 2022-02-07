#ifndef _MQTT_CLIENT_
#define _MQTT_CLIENT_

#include "global-defines.h"
#include "MQTT/MQTT.h"
#include "IComponent.h"
#include "MQTT/ISubCallback.h"
#include <vector>

class MQTTClient: public IComponent, public ISubCallback
{
public:
  MQTTClient();
  void Publish(const char* topic, const String payload);
  bool isConnected() { return this->client.isConnected(); }
  void RegisterCallbackListener(ISubCallback* listener);
  void Subscribe(const char* topic, MQTT::EMQTT_QOS);

  // IComponent
  virtual void Update() override;
  // ISubCallback
  virtual void Callback(char*, uint8_t*, unsigned int) override;
private:
  MQTT client;
  uint8_t ipAddress[4] = {0, 0, 0, 0};
  std::vector<ISubCallback*> listeners;
  UDP udp;
  bool discovery;

  void discoverMQTT();
};

#endif
