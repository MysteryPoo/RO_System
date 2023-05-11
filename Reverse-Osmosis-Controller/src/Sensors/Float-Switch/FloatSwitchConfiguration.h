/*
  FloatSwitchConfiguration

  Responsible for managing the configuration settings for the Float Switch
*/
#ifndef _FLOATSWITCHCONFIGURATION_
#define _FLOATSWITCHCONFIGURATION_

#include "Configuration/IConfiguration.h"
#include "MQTT/MqttSubscriber.h"
#include "MQTT/IOnConnect.h"

class FloatSwitch;
class MqttManager;
namespace spark
{
  class JSONValue;
}

class FloatSwitchConfiguration : public MqttSubscriber, public IConfiguration, public IOnConnect
{
public:
  FloatSwitchConfiguration(FloatSwitch& fs, MqttManager& manager);

  // MqttSubscriber
  virtual void Update(const char* payload) override;
  // IConfiguration
  virtual const char* GetName() const override;
  // IOnConnect
  virtual void OnConnect(bool success) override;
private:
  FloatSwitch& floatSwitch;
  MqttManager& mqttManager;
  void configure(JSONValue json);
  void reportOptions() const;
  void reportPins() const;
};

#endif
