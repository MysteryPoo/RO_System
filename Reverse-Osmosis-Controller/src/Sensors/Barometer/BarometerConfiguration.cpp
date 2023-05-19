#include "BarometerConfiguration.h"
#include "BarometerConstants.h"
#include "barometer.h"
#include "global-defines.h"
#include "spark_wiring_json.h"
#include "Utility/JsonBuffer.h"
#include "MQTT/mqtt-manager.h"
#include "Messages/BarometerPressureMessage.h"
#include "ObserverPattern/MessageType.h"

using namespace spark;

BarometerConfiguration::BarometerConfiguration(Barometer& sensor, MqttManager& manager)
  : barometer(sensor)
  , mqttManager(manager)
{
  manager.AttachConfiguration(this);
  manager.AttachOnConnect(this);
  manager.Subscribe(this, sensor.GetName() + "/configuration");
}

void BarometerConfiguration::Update(const char* payload)
{
  JSONValue json = JSONValue::parseCopy(payload);
  this->configure(json);
}

void BarometerConfiguration::configure(JSONValue json)
{
  if (!json.isValid()) return; // TODO: Some kind of error handling
  JSONObjectIterator it(json);
  while(it.next())
  {
    if (it.name() == OPTION_FULL_PRESSURE)
    {
        this->barometer.fullPressure = it.value().toInt();
    }
#ifdef TESTING
    if (it.name() == OPTION_PRESSURE)
    {
      this->barometer.pressure = it.value().toInt();
      BarometerMessage::Pressure msg(this->barometer.pressure);
      this->barometer.Notify(MessageType::BAROMETER_PRESSURE_MSG, &msg);
    }
#endif
  }
}

const char* BarometerConfiguration::GetName() const
{
  return this->barometer.GetName().c_str();
}

void BarometerConfiguration::OnConnect(const bool isSuccess) const
{
  if (!isSuccess) return;
}

void BarometerConfiguration::reportOptions() const
{
  JSONBufferWriter message = JsonBuffer::createBuffer(512);
  message.beginObject();
  message.name("display").value(DISPLAY_NAME);
  message.name("description").value("Utilizes barometric pressure readings to determine tank level.");
  message.name("options").beginArray()
  .beginObject()
  .name("name").value(OPTION_FULL_PRESSURE)
  .name("type").value("number")
  .name("units").value("Pascal")
  .name("default").value(10000UL)
  .endObject()
#ifdef TESTING
  .beginObject()
  .name("name").value(OPTION_PRESSURE)
  .name("type").value("number")
  .name("units").value("Pascal")
  .name("default").value(0UL)
#endif
  .endArray();
  message.endObject();
  this->mqttManager.PushPayload(this->barometer.GetName() + "/configuration", message.buffer());
  delete[] message.buffer();
}
