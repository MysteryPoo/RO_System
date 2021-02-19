
#include "system-log.h"
#include "json.h"

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
            msg->datetime = Particle.connected() ? Time.now() : 0l;
        }
    }
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

void SystemLog::trace(String message)
{
    String data = JHelp::begin();
    data += JHelp::field("message", message);
    data += JHelp::end();
    pushMessage("TRACE", data);
}

void SystemLog::information(String message)
{
    String data = JHelp::begin();
    data += JHelp::field("message", message);
    data += JHelp::end();
    pushMessage("INFO", data);
}

void SystemLog::warning(String message)
{
    String data = JHelp::begin();
    data += JHelp::field("message", message);
    data += JHelp::end();
    pushMessage("WARN", data);
}

void SystemLog::error(String message)
{
    String data = JHelp::begin();
    data += JHelp::field("message", message);
    data += JHelp::end();
    pushMessage("ERROR", data);
}
