#ifndef _BAROMETERCONFIGURATION_
#define _BAROMETERCONFIGURATION_

#include "Configuration/IConfiguration.h"
#include "MQTT/MqttSubscriber.h"
#include "MQTT/IOnConnect.h"

class Barometer;
class MqttManager;

namespace spark
{
  class JSONValue;
}

class BarometerConfiguration : public MqttSubscriber, public IConfiguration, public IOnConnect
{
public:
  BarometerConfiguration(Barometer& sensor, MqttManager& manager);

  // MqttSubscriber
  virtual void Update(const char* payload) override;
  // IConfiguration
  virtual const char* GetName() const override;
  // IOnConnect
  virtual void OnConnect(const bool success) const override;
private:
  Barometer& barometer;
  MqttManager& mqttManager;
  void configure(spark::JSONValue json);
  void reportOptions() const;
};

#endif
