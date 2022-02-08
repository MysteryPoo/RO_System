
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

class HeartbeatManager: public IComponent, public IConfigurable, public ISubCallback
{
public:
  HeartbeatManager(SystemLog& logger);
  
  void RegisterReporter(String name, IHeartbeatReporter*);
  void SetPeriod(unsigned long newPeriod) { this->updatePeriod = newPeriod; }
  void ForceHeartbeat(); // TODO : Make this const

  // IComponent
  virtual void Update() override;
  // IConfigurable
  virtual void Configure(JSONValue json) override;
  // ISubCallback
  virtual void Callback(char* topic, uint8_t* payload, unsigned int length) override;
  virtual void OnConnect(bool connectSuccess, MQTTClient* mqtt) override;

private:
  SystemLog& logger;
  unsigned long updatePeriod;

  std::vector<IHeartbeatReporter*> reporters;

  void sendHeartbeat();

  // IConfigurable
  virtual void fireConfigurationMessage() const override {};
};

#endif
