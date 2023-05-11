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
enum MessageType : int;

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
  virtual void Update(const Subject* subject, const MessageType type, void* messagePtr) const override;
};

#endif
