#include "MQTT/mqtt-manager.h"
#include "relay.h"
#include "RelayConfiguration.h"
#include "RelayEnums.h"
#include "Utility/JsonBuffer.h"

RelayConfiguration::RelayConfiguration(Relay& relay, MqttManager& manager)
  : relay(relay)
  , mqtt(manager)
{
  manager.AttachOnConnect(this);
}

void RelayConfiguration::OnConnect(bool success)
{
  if (!success) return;
  this->reportPins();
}

void RelayConfiguration::reportPins() const
{
  JSONBufferWriter writer = JsonBuffer::createBuffer(128);
  writer.beginObject()
  .name("pin").value(this->relay.pin)
  .name("activeLow").value(this->relay.activeLow)
  .endObject();
  this->mqtt.PushPayload("relay/" + RelayEnums::ToString(this->relay.name) + "/configuration", writer.buffer());
  delete[] writer.buffer();
}
