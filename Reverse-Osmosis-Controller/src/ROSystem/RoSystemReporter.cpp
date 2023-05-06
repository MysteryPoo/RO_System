#include "MQTT/mqtt-manager.h"
#include "RoSystemReporter.h"
#include "ROSystem.h"
#include "ObserverPattern/Subject.h"
#include "ObserverPattern/MessageType.h"

#include "RoSystemEnum.h"

RoSystemReporter::RoSystemReporter(ROSystem* system, MqttManager& manager)
  : AbstractReporter(manager)
  , _lastState(RoSystemEnum::State::BOOT)
  , _lastFlushed(false)
  , _lastEnabled(true)
{
  if (nullptr != system) system->Attach(this);
}

void RoSystemReporter::Update(Subject* subject)
{
  ROSystem* system = static_cast<ROSystem*>(subject);
  //RoSystemEnum::State state = system->GetState();
  const bool flushed = system->GetFlushed();
  const bool enabled = system->GetEnabled();
  if (flushed != this->_lastFlushed)
  {
    this->report(system->GetName() + "/flushed", flushed);
    this->_lastFlushed = flushed;
  }
  if (enabled != this->_lastEnabled)
  {
    this->report(system->GetName() + "/enabled", enabled);
    this->_lastEnabled = enabled;
  }
}

void RoSystemReporter::Update(Subject* subject, MessageType type, void* msg)
{
  ROSystem* system = static_cast<ROSystem*>(subject);
  switch(type)
  {
    case MessageType::ROSYSTEM_ENABLED_MSG: {
      break;
    }
    case MessageType::ROSYSTEM_FLUSHED_MSG: {
      break;
    }
  }
}
