/*
    Relay
    
    A relay essentially has two properties. 
    
    Active:
        It can either by active-low (default 'on' when unpowered) or active-high (default 'on' when power applied).
    
    Pin:
        The pin that controls the relay. Whether the pin being on or off turns the relay on or off depends on the active setting.
        
    When using this class, define whether the relay is activeLow or not (look at the specs of the relay) and then it can
    be simply controlled using the set() method which will take into account the active setting.
*/

#include "relay.h"
#include "system-log.h"

Relay::Relay(Name name, SystemLog &logger, int pin, bool activeLow) : 
    name(name),
    pin(pin),
    activeLow(activeLow),
    logger(logger)
{
    pinMode(pin, OUTPUT);
    this->set(Relay::State::OFF);

    fireConfigurationMessage();
}

const String Relay::toString() const
{
    switch(this->name)
    {
        case COMPONENT_PUMP:
            return "COMPONENT_PUMP";
        case COMPONENT_INLETVALVE:
            return "COMPONENT_INLETVALVE";
        case COMPONENT_FLUSHVALVE:
            return "COMPONENT_FLUSHVALVE";
        default:
            return "COMPONENT_UNKNOWN";
    }
}

void Relay::set(const Relay::State newState)
{
    if(this->pin < 0) // Bail out early if not configured
    {
        this->logger.error("Attempting to set an unconfigured relay.[" + toString() + "]");
        return;
    }
    JSONBufferWriter writer = SystemLog::createBuffer(128);
    writer.beginObject();
    writer.name("event").value("set");
    writer.name("name").value(this->toString());
    if(newState == Relay::State::ON)
    {
        writer.name("value").value("ON");
        digitalWrite(this->pin, this->activeLow ? LOW : HIGH);
    }
    else
    {
        writer.name("value").value("OFF");
        digitalWrite(this->pin, this->activeLow ? HIGH : LOW);
    }
    writer.endObject();
    this->logger.pushMessage("relay/set", writer.buffer());
    this->state = newState;
}

void Relay::fireConfigurationMessage() const
{
    JSONBufferWriter writer = SystemLog::createBuffer(128);
    writer.beginObject();
    writer.name("event").value("configuration");
    writer.name("name").value(this->toString());
    writer.name("pin").value(this->pin);
    writer.name("activeLow").value(activeLow);
    writer.endObject();
    this->logger.pushMessage("relay/configuration", writer.buffer());
}
