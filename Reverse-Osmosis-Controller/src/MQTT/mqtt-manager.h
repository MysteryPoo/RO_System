/*
  MqttManager - 
  Responsible for wrapping the MQTT driver
    - Subscribing to topics
    - Publishing messages
  Maintaining connection to the mqtt broker
    - Discovery service
  Routing messages through the simple broker
  Managing the message queue

  TODO: Break down responsibilities at least into composable classes
*/
#ifndef _MQTT_CLIENT_
#define _MQTT_CLIENT_

#include "global-defines.h"
#include "IUpdateOnLoop.h"
#include "MQTT.h"
#include <vector>

class IConfiguration;
class IOnConnect;
class MQTT;
class MqttSubscriber;
class MqttQueue;
class SimpleBroker;
class String;

class MqttManager: public IUpdateOnLoop
{
public:
  MqttManager(SimpleBroker& broker, MqttQueue& queue);
  bool PublishQueue();
  bool isConnected() { return this->client.isConnected(); }
  void Subscribe(MqttSubscriber* subscriber, const char* topic, MQTT::EMQTT_QOS = MQTT::EMQTT_QOS::QOS1);
  void Unsubscribe(MqttSubscriber* subscriber, const char* topic);
  void OnMqttMessage(char* topic, uint8_t* buffer, unsigned int bufferLength);
  void AttachOnConnect(IOnConnect* listener);
  void AttachConfiguration(IConfiguration* configuration);
  void PushPayload(String topic, String payload);

  // IUpdateOnLoop
  virtual void Update() override;
private:
  MQTT client;
  uint8_t ipAddress[4] = {0, 0, 0, 0};
  std::vector<IOnConnect*> onConnectListeners;
  std::vector<IConfiguration*> configurations;
  std::vector<String> topicList;
  UDP udp;

  SimpleBroker& broker;
  MqttQueue& queue;
  bool connected;
  bool discovery;
  unsigned long lastUpdate;
  unsigned long lastDiscovery;

  void announceConfigurations();
  void discoverMQTT();
  void onConnect();
  void parseIpFromString(const char* cstring);
  void processQueue();
  void publish(const char* topic, const String payload);
};

#endif
