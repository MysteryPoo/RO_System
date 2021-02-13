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
    this->logger.pushMessage("relay/configuration", "{\"event\":\"configuration\",\"name\":\"" + this->toString() + "\",\"pin\":" + String(pin) + ",\"activeLow\":" + (activeLow ? "true" : "false") + "}");
    pinMode(pin, OUTPUT);
    
    this->set(Relay::State::OFF);
}

String Relay::toString()
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

void Relay::set(Relay::State newState)
{
    if(this->pin < 0) // Bail out early if not configured
    {
        this->logger.error("Attempting to set an unconfigured relay.[" + toString() + "]");
        return;
    }
    if(newState == Relay::State::ON)
    {
        this->logger.pushMessage("relay/set", "{\"event\":\"set\",\"name\":\"" + this->toString() + "\",\"value\":\"ON\"}");
        digitalWrite(this->pin, this->activeLow ? LOW : HIGH);
    }
    else
    {
        this->logger.pushMessage("relay/set", "{\"event\":\"set\",\"name\":\"" + this->toString() + "\",\"value\":\"OFF\"}");
        digitalWrite(this->pin, this->activeLow ? HIGH : LOW);
    }
    this->state = newState;
}
