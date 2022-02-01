
#include "float-meter.h"
#include "system-log.h"

#define FLOAT_METER_RESOLUTION 4096.f
#define DEFAULT_FULL_VALUE 3.3f

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

bool FloatMeter::isFull()
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
  this->logger.pushMessage("float-meter", writer.buffer());
}

float FloatMeter::voltage() const
{
  return (float)input / FLOAT_METER_RESOLUTION * 3.3f;
}
