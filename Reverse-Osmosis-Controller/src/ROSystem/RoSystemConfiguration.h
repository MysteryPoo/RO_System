/*
  RoSystemConfiguration

  Responsible for managing the configuration settings for the System
*/

#ifndef _ROSYSTEMCONFIGURATION_
#define _ROSYSTEMCONFIGURATION_

#include "Configuration/IConfiguration.h"
#include "MQTT/MqttSubscriber.h"
#include "MQTT/IOnConnect.h"

class MqttManager;
class ROSystem;
namespace spark
{
  class JSONValue;
}

class RoSystemConfiguration : public MqttSubscriber, public IConfiguration, public IOnConnect
{
public:
  RoSystemConfiguration(ROSystem& system, MqttManager& manager);

  // MqttSubscriber
  virtual void Update(const char* payload) override;
  // IConfiguration
  virtual const char* GetName() const override;
  // IOnConnect
  virtual void OnConnect(const bool success) const override;
private:
  ROSystem& system;
  MqttManager& mqtt;
  void configure(spark::JSONValue json);
  void reportOptions() const;
};

#endif
