
#include "system-log.h"

#define BURST_DELAY 2500
#define BURST_LIMIT 10

SystemLog::SystemLog()
: enabled(true)
, currentMessageIndex(0)
, lastBurst(millis() - BURST_DELAY)
{
    
}

void SystemLog::pushMessage(String component, String data)
{
    if(this->enabled)
    {
        if(this->currentMessageIndex < SYSTEMLOG_MESSAGEBUFFERSIZE)
        {
            Message* msg = &this->messages[this->currentMessageIndex++];
            msg->component = component;
            msg->data = data;
            msg->datetime = Particle.connected() ? Time.now() : 0;
        }
    }
}

void SystemLog::pushMessage(String component, char* data)
{
    this->pushMessage(component, String(data));
    delete[] data;
}
        
void SystemLog::publishLog()
{
    unsigned long curMillis = millis();
    if(Particle.connected() && curMillis > lastBurst + BURST_DELAY && !this->isEmpty())
    {
        if(this->isFull())
        {
            Particle.publish("romcon/logger", "Logger message queue full. Potential telemetry data loss.", 60, PRIVATE);
        }

        Message* msg = &this->messages[--currentMessageIndex];
        
        Particle.publish("romcon/" + msg->component, msg->toJSON(), 60, PRIVATE);
        
        this->lastBurst = curMillis;
    }
}

void SystemLog::simpleMessage(String label, String message)
{
    JSONBufferWriter writer = this->createBuffer(256);
    writer.beginObject();
    writer.name("message").value(message);
    writer.endObject();
    pushMessage(label, writer.buffer());
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
