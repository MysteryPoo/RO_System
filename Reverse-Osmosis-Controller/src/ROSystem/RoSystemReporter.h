/*
  RoSystemReporter
  Responsible for tracking and reporting values that are of interest to the cloud
*/

#ifndef _ROSYSTEMREPORTER_
#define _ROSYSTEMREPORTER_

#include "Reporters/AbstractReporter.h"

typedef byte uin8_t;

class MqttManager;
class ROSystem;
class Subject;
enum MessageType : int;

namespace RoSystemEnum
{
  enum State : uint8_t;
}

namespace RoSystemMessage
{
  struct StateChange;
}

class RoSystemReporter : AbstractReporter
{
public:
  RoSystemReporter(ROSystem* system, MqttManager& manager);
  // AbstractReporter
  virtual ~RoSystemReporter() override {};
  virtual void Update(const Subject* subject, const MessageType type, void* msg) const override;
private:
  using AbstractReporter::report;
  void report(const char* topic, const RoSystemMessage::StateChange* value) const;
};

#endif
