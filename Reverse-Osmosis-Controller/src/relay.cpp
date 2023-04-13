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
#include "mqtt-queue.h"

Relay::Relay(Name name, SystemLog& logger, MqttQueue& mqtt, int pin, bool activeLow) : 
    name(name),
    logger(logger),
    mqtt(mqtt),
    pin(pin),
    activeLow(activeLow)
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
            return "pump";
        case COMPONENT_INLETVALVE:
            return "inlet-valve";
        case COMPONENT_FLUSHVALVE:
            return "flush-valve";
        default:
            return "unknown";
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
    writer.beginObject()
    .name("datetime").value(Particle.connected() ? Time.now() : 0);
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
    this->mqtt.PushPayload("relay/" + this->toString() + "/set", writer.buffer());
    delete[] writer.buffer();
    this->state = newState;
}

void Relay::fireConfigurationMessage() const
{
    JSONBufferWriter writer = SystemLog::createBuffer(128);
    writer.beginObject()
    .name("pin").value(this->pin)
    .name("activeLow").value(this->activeLow)
    .endObject();
    this->mqtt.PushPayload("relay/" + this->toString() + "/pin", writer.buffer());
    delete[] writer.buffer();
}
