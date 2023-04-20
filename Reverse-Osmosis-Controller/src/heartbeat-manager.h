
#ifndef _HEARTBEAT_MANAGER_
#define _HEARTBEAT_MANAGER_

#include "application.h"
#include "IComponent.h"
#include "IConfigurable.h"
#include "MQTT/ISubCallback.h"
#include <vector>

class SystemLog;
class IHeartbeatReporter;
class MQTTClient;
class MqttQueue;

class HeartbeatManager: public IComponent, public IConfigurable, public ISubCallback
{
public:
  HeartbeatManager(SystemLog& logger, MQTTClient* mqttClient, MqttQueue& mqttQueue);
  
  void RegisterReporter(String name, IHeartbeatReporter*);
  void SetPeriod(unsigned long newPeriod) { this->updatePeriod = newPeriod; }
  bool ForceHeartbeat() const; // TODO : Make this const

  // IComponent
  virtual void Update() override;
  // IConfigurable
  virtual void Configure(JSONValue json) override;
  // ISubCallback
  virtual void Callback(char* topic, uint8_t* payload, unsigned int length) override;
  virtual void OnConnect(bool connectSuccess, MQTTClient* mqtt) override;
  virtual String GetName() const override;

private:
  SystemLog& logger;
  MqttQueue& mqttQueue;
  unsigned long updatePeriod;
  unsigned long lastUpdate;

  std::vector<IHeartbeatReporter*> reporters;

  bool sendHeartbeat() const;

  // IConfigurable
  virtual void fireConfigurationMessage() const override {};
};

#endif
