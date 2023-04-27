#include "MQTT/mqtt-manager.h"
#include "relay.h"
#include "RelayEnums.h"
#include "RelayReporter.h"

RelayReporter::RelayReporter(Relay* relay, MqttManager& manager)
  : AbstractReporter(manager)
  , _lastState(RelayEnums::State::OFF)
{
  if (nullptr != relay) relay->Attach(this);
}

void RelayReporter::Update(Subject* subject)
{
  Relay* relay = static_cast<Relay*>(subject);
  RelayEnums::State state = relay->GetState();
  if (state != _lastState)
  {
    this->report(RelayEnums::ToString(relay->GetName()) + "/state", RelayEnums::ToString(state).c_str());
    _lastState = state;
  }
}
