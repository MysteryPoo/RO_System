
#include "float-meter.h"
#include "system-log.h"

#define FLOAT_METER_RESOLUTION 4096.f
#define DEFAULT_FULL_VALUE 3.3f
#define COMPONENT_NAME "float-meter"

FloatMeter::FloatMeter(int pin, SystemLog &logger) :
  pin(pin),
  input(0),
  logger(logger)
{
    this->fireConfigurationMessage();
}

void FloatMeter::Update()
{
  input = analogRead(pin);
}

bool FloatMeter::isFull() const
{
  return voltage() >= DEFAULT_FULL_VALUE;
}

void FloatMeter::fireConfigurationMessage() const
{
  JSONBufferWriter writer = SystemLog::createBuffer(256);
  writer.beginObject();
  writer.name("event").value("configuration");
  writer.name("pin").value(this->pin);
  writer.endObject();
  this->logger.pushMessage(COMPONENT_NAME, writer.buffer());
}

float FloatMeter::voltage() const
{
  return (float)input / FLOAT_METER_RESOLUTION * 3.3f;
}

void FloatMeter::reportHeartbeat(JSONBufferWriter& writer) const
{
  writer.name(COMPONENT_NAME).value((double)this->voltage());
}
