/*
  RelayConfiguration

  Sends the pin configuration for the relay
*/

#ifndef _RELAYCONFIGURATION_
#define _RELAYCONFIGURATION_

#include "MQTT/IOnConnect.h"

class MqttManager;
class Relay;

class RelayConfiguration: public IOnConnect
{
public:
  RelayConfiguration(Relay& relay, MqttManager& manager);
  // IOnConnect
  virtual void OnConnect(bool success) override;
private:
  Relay& relay;
  MqttManager& mqtt;
  void reportPins() const;
};

#endif
