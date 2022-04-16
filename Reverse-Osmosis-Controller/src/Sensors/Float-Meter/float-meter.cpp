
#include "float-meter.h"
#include "system-log.h"

#define FLOAT_METER_RESOLUTION 4096.f
#define DEFAULT_FULL_VALUE 4096
#define COMPONENT_NAME "float-meter"

FloatMeter::FloatMeter(int pin, SystemLog &logger) :
  pin(pin),
  input(0),
  fullValue(DEFAULT_FULL_VALUE),
  highIsFull(false),
#ifdef TESTING
  simulatedValue(0),
#endif
  logger(logger)
{
    this->fireConfigurationMessage();
}

void FloatMeter::Update()
{
#ifndef TESTING
  input = analogRead(pin);
#else
  input = simulatedValue;
#endif
}

void FloatMeter::Configure(JSONValue json)
{
  JSONObjectIterator jsonIt(json);
  while(jsonIt.next())
  {
#ifdef TESTING
    if (jsonIt.name() == "Current Voltage")
    {
      int newValue = (int)(jsonIt.value().toDouble() / 3.3 * 4096.);
      this->setValue(newValue);
    }
#endif
    if (jsonIt.name() == "Full Voltage")
    {
      int newValue = (int)(jsonIt.value().toDouble() / 3.3 * 4096.);
      this->fullValue = newValue;
    }
    if (jsonIt.name() == "Voltage High is Full")
    {
      this->highIsFull = jsonIt.value().toBool();
    }
  }
}

void FloatMeter::Callback(char* topic, uint8_t* payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = '\0';

    if (strcmp(topic, "to/" + System.deviceID() + "/configuration/float-meter"))
    {
        return;
    }

    JSONValue configuration = JSONValue::parseCopy(String(p));
    this->Configure(configuration);

}

void FloatMeter::OnConnect(bool success, MQTTClient* mqtt)
{
    if (nullptr != mqtt)
    {
        mqtt->Subscribe("configuration/float-meter/#", MQTT::EMQTT_QOS::QOS1);
        JSONBufferWriter message = SystemLog::createBuffer(512);
        message.beginObject();
        message.name("display").value("Float Meter");
        message.name("description").value("Measures volume within tank.");
        message.name("options").beginArray()
#ifdef TESTING
        .beginObject()
        .name("name").value("Current Voltage")
        .name("type").value("number")
        .name("units").value("Volts")
        .endObject()
#endif
        .name("name").value("Full Voltage")
        .name("type").value("number")
        .name("units").value("Volts")
        .name("default").value(this->fullValue)
        .name("name").value("Voltage High is Full")
        .name("type").value("boolean")
        .name("default").value(this->highIsFull)
        .endArray();
        message.endObject();
        mqtt->Publish("configuration/float-meter", message.buffer());
        delete[] message.buffer();
    }
}

bool FloatMeter::isFull() const
{
  if (this->highIsFull)
    return this->input >= this->fullValue;
  else
    return this->input <= this->fullValue;
}

void FloatMeter::fireConfigurationMessage() const
{
#ifndef TESTING
  int pinValue = this->pin;
#else
  String pinValue("Simulated");
#endif
  JSONBufferWriter writer = SystemLog::createBuffer(256);
  writer.beginObject();
  writer.name("event").value("configuration");
  writer.name("pin").value(pinValue);
  writer.endObject();
  this->logger.pushMessage(COMPONENT_NAME, writer.buffer());
}

float FloatMeter::voltage() const
{
  return (float)input / FLOAT_METER_RESOLUTION * 3.3f;
}

void FloatMeter::ReportHeartbeat(JSONBufferWriter& writer) const
{
  writer.name(COMPONENT_NAME).beginObject()
  .name("voltage").value((double)this->voltage())
  .endObject();
  this->logger.trace("F_Meter Voltage: " + String(this->voltage()));
}

#ifdef TESTING
void FloatMeter::setValue(int newValue)
{
  this->simulatedValue = newValue;
}
#endif
