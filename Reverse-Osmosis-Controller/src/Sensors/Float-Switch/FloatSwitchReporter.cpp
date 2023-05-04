#include "FloatSwitchReporter.h"
#include "float-switch.h"
#include "Messages/FloatSwitchStatusMessage.h"
#include "Messages/FloatSwitchReliableMessage.h"
#include "MQTT/mqtt-manager.h"
#include "ObserverPattern/MessageType.h"

using namespace FloatSwitchMessage;

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

void FloatSwitchReporter::Update(Subject* subject, MessageType type, void* msg)
{
  FloatSwitch* floatSwitch = static_cast<FloatSwitch*>(subject);
  switch(type)
  {
    case MessageType::FLOAT_SWITCH_STATUS_MSG: {
      Status* message = static_cast<Status*>(msg);
      this->report(floatSwitch->GetName() + "/status", message->status);
      break;
    }
    case MessageType::FLOAT_SWITCH_RELIABLE_MSG: {
      Reliable* message = static_cast<Reliable*>(msg);
      this->report(floatSwitch->GetName() + "/reliable", message->reliable);
      break;
    }
  }
}
