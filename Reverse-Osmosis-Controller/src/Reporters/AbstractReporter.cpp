#include "AbstractReporter.h"
#include "MQTT/mqtt-manager.h"
#include "Utility/JsonBuffer.h"
#include "Utility/TimeHelper.h"

AbstractReporter::AbstractReporter(MqttManager& manager)
  : mqtt(manager)
  { 
  }

void AbstractReporter::report(const char* topic, const bool value) const
{
  JSONBufferWriter writer = JsonBuffer::createBuffer(128);
  writer.beginObject()
  .name("datetime").value(TimeHelper::GetTimeIfAble())
  .name("value").value(value)
  .endObject();
  this->mqtt.PushPayload(topic, writer.buffer());
  delete[] writer.buffer();
}

void AbstractReporter::report(const char* topic, const char* value) const
{
  JSONBufferWriter writer = JsonBuffer::createBuffer(128);
  writer.beginObject()
  .name("datetime").value(TimeHelper::GetTimeIfAble())
  .name("value").value(value)
  .endObject();
  this->mqtt.PushPayload(topic, writer.buffer());
  delete[] writer.buffer();
}

void AbstractReporter::report(const char* topic, const float value) const
{
  JSONBufferWriter writer = JsonBuffer::createBuffer(128);
  writer.beginObject()
  .name("datetime").value(TimeHelper::GetTimeIfAble())
  .name("value").value(value)
  .endObject();
  this->mqtt.PushPayload(topic, writer.buffer());
  delete[] writer.buffer();
}

void AbstractReporter::report(const char* topic, const int value) const
{
  JSONBufferWriter writer = JsonBuffer::createBuffer(128);
  writer.beginObject()
  .name("datetime").value(TimeHelper::GetTimeIfAble())
  .name("value").value(value)
  .endObject();
  this->mqtt.PushPayload(topic, writer.buffer());
  delete[] writer.buffer();
}
