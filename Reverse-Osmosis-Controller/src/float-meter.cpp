
#include "float-meter.h"
#include "system-log.h"

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
  return input / 4096.f * 3.3f;
}
