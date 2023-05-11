#include "MQTT/mqtt-manager.h"
#include "relay.h"
#include "RelayEnums.h"
#include "RelayReporter.h"
#include "Messages/StateChangeMessage.h"
#include "ObserverPattern/MessageType.h"

RelayReporter::RelayReporter(Relay* relay, MqttManager& manager)
  : AbstractReporter(manager)
{
  if (nullptr != relay) relay->Attach(this);
}

void RelayReporter::Update(const Subject* subject, const MessageType type, void* messagePtr) const
{
  const Relay* relay = static_cast<const Relay*>(subject);
  switch(type)
  {
    case MessageType::RELAY_STATE_MSG: {
      const RelayMessage::StateChange* message = static_cast<RelayMessage::StateChange*>(messagePtr);
      this->report("relay/" + RelayEnums::ToString(relay->GetName()) + "/state", RelayEnums::ToString(message->state).c_str());
      break;
    }
  }
}
