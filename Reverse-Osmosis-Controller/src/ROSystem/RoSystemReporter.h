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

class RoSystemReporter : AbstractReporter
{
public:
  RoSystemReporter(ROSystem* system, MqttManager& manager);
  // AbstractReporter
  virtual ~RoSystemReporter() override {};
  virtual void Update(Subject* subject) override;
  virtual void Update(Subject* subject, MessageType type, void* msg) override;
private:
  RoSystemEnum::State _lastState;
  bool _lastFlushed;
  bool _lastEnabled;
};

#endif
