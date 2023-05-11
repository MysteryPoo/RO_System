
#include "system-log.h"
#include "application.h"
#include "Utility/JsonBuffer.h"
#include "MQTT/mqtt-manager.h"

SystemLog::SystemLog(MqttManager& mqtt)
: mqtt(mqtt)
{
}

void SystemLog::simpleMessage(String label, String message)
{
    JSONBufferWriter writer = JsonBuffer::createBuffer(256);
    writer.beginObject()
    .name("datetime").value(Particle.connected() ? Time.now() : 0)
    .name("message").value(message)
    .endObject();
    this->mqtt.PushPayload("log/" + label, writer.buffer());
    delete[] writer.buffer();
}

void SystemLog::trace(String message)
{
    simpleMessage("TRACE", message);
}

void SystemLog::information(String message)
{
    simpleMessage("INFO", message);
}

void SystemLog::warning(String message)
{
    simpleMessage("WARN", message);
}

void SystemLog::error(String message)
{
    simpleMessage("ERROR", message);
}
