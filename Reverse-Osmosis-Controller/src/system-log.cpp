
#include "system-log.h"
#include "mqtt-client.h"
#include "mqtt-queue.h"

#define BURST_DELAY 500
#define BURST_LIMIT 10

SystemLog::SystemLog(MQTTClient& mqtt, MqttQueue& mqttQueue)
: enabled(true)
, mqttClient(mqtt)
, mqttQueue(mqttQueue)
, lastBurst(millis() - BURST_DELAY)
{
    
}

void SystemLog::pushMessage(String component, String data)
{
    if(this->enabled)
    {
        if(this->messageQueue.size() < SYSTEMLOG_MESSAGEBUFFERSIZE)
        {
            Message* msg = new Message();
            msg->component = component;
            msg->data = data;
            msg->datetime = Particle.connected() ? Time.now() : 0;
            this->messageQueue.push(msg);
        }
    }
}

void SystemLog::pushMessage(String component, char* data)
{
    this->pushMessage(component, String(data));
    delete[] data;
}

void SystemLog::Update()
{
    this->publishLog();
}
        
void SystemLog::publishLog()
{
    unsigned long curMillis = millis();
    if(mqttClient.isConnected() && curMillis > lastBurst + BURST_DELAY && !this->isEmpty())
    {
        if(this->isFull())
        {
            mqttClient.Publish("romcon/logger", "Logger message queue full. Potential telemetry data loss.");
        }

        Message* msg = this->messageQueue.front();
        
        mqttClient.Publish("romcon/" + msg->component, msg->toJSON());

        this->messageQueue.pop();
        delete msg;
        
        this->lastBurst = curMillis;
    }
}

void SystemLog::simpleMessage(String label, String message)
{
    JSONBufferWriter writer = this->createBuffer(256);
    writer.beginObject()
    .name("datetime").value(Particle.connected() ? Time.now() : 0)
    .name("message").value(message)
    .endObject();
    mqttQueue.PushPayload("log/" + label, writer.buffer());
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

JSONBufferWriter SystemLog::createBuffer(int size)
{
    char* buffer = new char[size];
    memset(buffer, 0, size * sizeof(char));
    return JSONBufferWriter(buffer, size * sizeof(char));
}
