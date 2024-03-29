#include "application.h"
#include "FloatSwitchConfiguration.h"
#include "float-switch.h"
#include "global-defines.h"
#include "Utility/JsonBuffer.h"
#include "MQTT/mqtt-manager.h"
#include "ObserverPattern/MessageType.h"
#include "Messages/FloatSwitchReliableMessage.h"
#include "Messages/FloatSwitchStatusMessage.h"

using namespace FloatSwitchMessage;

FloatSwitchConfiguration::FloatSwitchConfiguration(FloatSwitch& fs, MqttManager& manager /*, logger */)
  : floatSwitch(fs)
  , mqttManager(manager)
{
  manager.AttachConfiguration(this);
  manager.AttachOnConnect(this);
  manager.Subscribe(this, fs.GetName() + "/configuration");
}

void FloatSwitchConfiguration::Update(const char* payload)
{
  JSONValue json = JSONValue::parseCopy(payload);
  this->configure(json);
}

void FloatSwitchConfiguration::configure(JSONValue json)
{
  if (!json.isValid()) return; // TODO: Some kind of error handling
  JSONObjectIterator it(json);
  while(it.next())
  {
#ifdef TESTING
    if (it.name() == "float")
    {
        this->floatSwitch.status = it.value().toBool();
        Status msg(this->floatSwitch.status);
        this->floatSwitch.Notify(MessageType::FLOAT_SWITCH_STATUS_MSG, &msg);
    }
#endif
    if (it.name() == "Reset Reliable")
    {
        this->floatSwitch.isReliable = it.value().toBool();
        Reliable msg(this->floatSwitch.isReliable);
        this->floatSwitch.Notify(MessageType::FLOAT_SWITCH_RELIABLE_MSG, &msg);
    }
  }
}

const char* FloatSwitchConfiguration::GetName() const
{
  return this->floatSwitch.GetName().c_str();
}

void FloatSwitchConfiguration::OnConnect(const bool success) const
{
  if (!success) return;
  reportOptions();
  reportPins();
}

void FloatSwitchConfiguration::reportOptions() const
{
  JSONBufferWriter message = JsonBuffer::createBuffer(512);
  message.beginObject();
  message.name("display").value("Float Switch");
  message.name("description").value("On when the tank is full. Off when not.");
  message.name("options").beginArray()
#ifdef TESTING
  .beginObject()
  .name("name").value("float")
  .name("type").value("boolean")
  .endObject()
#endif
  .beginObject()
  .name("name").value("Reset Reliable")
  .name("type").value("trigger")
  .endObject()
  .endArray();
  message.endObject();
  this->mqttManager.PushPayload(this->floatSwitch.GetName() + "/configuration", message.buffer());
  delete[] message.buffer();
}

void FloatSwitchConfiguration::reportPins() const
{
#ifndef TESTING
  int pinValue = this->floatSwitch.pin;
#else
  int pinValue(-1);
#endif
  JSONBufferWriter writer = JsonBuffer::createBuffer(64);
  writer.beginObject();
  writer.name("pin").value(pinValue);
  writer.endObject();
  this->mqttManager.PushPayload(this->floatSwitch.GetName() + "/pin", writer.buffer());
  delete[] writer.buffer();
}
