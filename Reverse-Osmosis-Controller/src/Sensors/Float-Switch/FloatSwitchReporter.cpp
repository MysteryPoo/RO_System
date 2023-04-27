#include "FloatSwitchReporter.h"
#include "float-switch.h"
#include "MQTT/mqtt-manager.h"

FloatSwitchReporter::FloatSwitchReporter(FloatSwitch* sensor, MqttManager& manager)
  : AbstractReporter(manager)
  , _lastStatus(false)
  , _lastReliable(false)
{
  if (nullptr != sensor) sensor->Attach(this);
}

void FloatSwitchReporter::Update(Subject* subject)
{
  FloatSwitch* floatSwitch = static_cast<FloatSwitch*>(subject);
  const bool status = floatSwitch->GetStatus();
  const bool reliable = floatSwitch->GetReliable();
  if (status != this->_lastStatus)
  {
    this->report(floatSwitch->GetName() + "/status", status);
    this->_lastStatus = status;
  }
  if (reliable != this->_lastReliable)
  {
    this->report(floatSwitch->GetName() + "/reliable", reliable);
    this->_lastReliable = reliable;
  }
}
