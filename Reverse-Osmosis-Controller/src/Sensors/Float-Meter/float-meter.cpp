
#include "float-meter.h"
#include "system-log.h"

#define FLOAT_METER_RESOLUTION 4096.f
#define DEFAULT_FULL_VALUE 4096
#define COMPONENT_NAME "float-meter"

FloatMeter::FloatMeter(int pin, SystemLog &logger) :
  pin(pin),
  input(0),
  fullValue(DEFAULT_FULL_VALUE),
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
    if (jsonIt.name() == "voltage")
    {
      int newValue = (int)(jsonIt.value().toDouble() / 3.3 * 4096.);
      this->setValue(newValue);
    }
#endif
  }
}

bool FloatMeter::isFull() const
{
  return this->input >= this->fullValue;
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
  writer.name(COMPONENT_NAME).value((double)this->voltage());
}

#ifdef TESTING
void FloatMeter::setValue(int newValue)
{
  this->simulatedValue = newValue;
}
#endif
