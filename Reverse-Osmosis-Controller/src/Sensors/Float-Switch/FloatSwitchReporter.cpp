#include "FloatSwitchReporter.h"
#include "float-switch.h"
#include "Messages/FloatSwitchStatusMessage.h"
#include "Messages/FloatSwitchReliableMessage.h"
#include "MQTT/mqtt-manager.h"
#include "ObserverPattern/MessageType.h"

using namespace FloatSwitchMessage;

FloatSwitchReporter::FloatSwitchReporter(FloatSwitch* sensor, MqttManager& manager)
  : AbstractReporter(manager)
{
  if (nullptr != sensor) sensor->Attach(this);
}

void FloatSwitchReporter::Update(const Subject* subject, const MessageType type, void* msg) const
{
  const FloatSwitch* floatSwitch = static_cast<const FloatSwitch*>(subject);
  switch(type)
  {
    case MessageType::FLOAT_SWITCH_STATUS_MSG: {
      const Status* message = static_cast<Status*>(msg);
      this->report(floatSwitch->GetName() + "/status", message->status);
      break;
    }
    case MessageType::FLOAT_SWITCH_RELIABLE_MSG: {
      const Reliable* message = static_cast<Reliable*>(msg);
      this->report(floatSwitch->GetName() + "/reliable", message->reliable);
      break;
    }
  }
}
