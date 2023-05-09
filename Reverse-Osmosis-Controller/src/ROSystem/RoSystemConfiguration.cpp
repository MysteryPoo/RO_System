#include "application.h"
#include "RoSystemConfiguration.h"
#include "Utility/JsonBuffer.h"
#include "ROSystem.h"
#include "RoSystemConstants.h"
#include "MQTT/mqtt-manager.h"
#include "Messages/EnabledMsg.h"
#include "Messages/PumpCooldownMsg.h"
#include "Messages/FlushDurationMsg.h"
#include "ObserverPattern/MessageType.h"

RoSystemConfiguration::RoSystemConfiguration(ROSystem& system, MqttManager& manager)
  : system(system)
  , mqtt(manager)
{
  manager.AttachConfiguration(this);
  manager.AttachOnConnect(this);
  manager.Subscribe(this, system.GetName() + "/configuration");
}

void RoSystemConfiguration::Update(const char* payload)
{
  JSONValue json = JSONValue::parseCopy(payload);
  this->configure(json);
}

const char* RoSystemConfiguration::GetName() const
{
  return this->system.GetName().c_str();
}

void RoSystemConfiguration::OnConnect(bool success)
{
  if (!success) return;
  reportOptions();
}

void RoSystemConfiguration::configure(JSONValue json)
{
  if (!json.isValid()) return; // TODO: Some kind of error handling
  JSONObjectIterator it(json);
  while(it.next())
  {
    if (it.name() == "enabled")
    {
      this->system.enabled = it.value().toBool();
      RoSystemMessage::Enabled msg(this->system.enabled);
      this->system.Notify(MessageType::ROSYSTEM_ENABLED_MSG, &msg);
    }
    if (it.name() == "Pump Cooldown")
    {
      this->system.pumpCooldown = (unsigned int)it.value().toInt();
      RoSystemMessage::PumpCooldown msg(this->system.pumpCooldown);
      this->system.Notify(MessageType::ROSYSTEM_PUMP_COOLDOWN_MSG, &msg);
    }
    if (it.name() == "Flush Duration")
    {
      this->system.flushDuration = (unsigned int)it.value().toInt();
      RoSystemMessage::FlushDuration msg(this->system.flushDuration);
      this->system.Notify(MessageType::ROSYSTEM_FLUSH_DURATION_MSG, &msg);
    }
  }
}

void RoSystemConfiguration::reportOptions() const
{
  JSONBufferWriter message = JsonBuffer::createBuffer(512);
  message.beginObject();
  message.name("display").value("RO System");
  message.name("description").value("The component responsible for maintaining water level.");
  message.name("options").beginArray()
  .beginObject()
  .name("name").value("enabled")
  .name("type").value("boolean")
  .name("default").value(true)
  .endObject()
  .beginObject()
  .name("name").value("Pump Cooldown")
  .name("type").value("number")
  .name("units").value("millisecond")
  .name("default").value(PUMP_FREQUENCY_MS)
  .endObject()
  .beginObject()
  .name("name").value("Flush Duration")
  .name("type").value("number")
  .name("units").value("millisecond")
  .name("default").value(FLUSH_TIMER_MS)
  .endObject()
  .endArray();
  message.endObject();
  this->mqtt.PushPayload(this->system.GetName() + "/configuration", message.buffer());
  delete[] message.buffer();
}
