/*
  RelayReporter

  Reports changes on relays
*/

#ifndef _RELAYREPORTER_
#define _RELAYREPORTER_

#include "Reporters/AbstractReporter.h"
#include <stdint.h>

class MqttManager;
class Relay;
class Subject;

namespace RelayEnums
{
  enum Name : uint8_t;
  enum State : uint8_t;
}

class RelayReporter : AbstractReporter
{
public:
  RelayReporter(Relay* relay, MqttManager& manager);
  // AbstractReporter
  virtual ~RelayReporter() override {};
  virtual void Update(Subject* subject) override;
private:
  RelayEnums::State _lastState;
};

#endif
