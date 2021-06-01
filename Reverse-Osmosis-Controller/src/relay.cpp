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
#include "json.h"
#include "system-log.h"

Relay::Relay(Name name, SystemLog &logger, int pin, bool activeLow) : 
    name(name),
    pin(pin),
    activeLow(activeLow),
    logger(logger)
{
    String configMsg = JHelp::begin();
    configMsg += JHelp::field("event", "configuration");
    configMsg += JHelp::next();
    configMsg += JHelp::field("name", this->toString());
    configMsg += JHelp::next();
    configMsg += JHelp::field("pin", String(pin));
    configMsg += JHelp::next();
    configMsg += JHelp::field("activeLow", activeLow ? "true" : "false");
    configMsg += JHelp::end();
    this->logger.pushMessage("relay/configuration", configMsg);
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
    String message = JHelp::begin();
    message += JHelp::field("event", "set");
    message += JHelp::next();
    message += JHelp::field("name", this->toString());
    message += JHelp::next();
    if(newState == Relay::State::ON)
    {
        message += JHelp::field("value", "ON");
        digitalWrite(this->pin, this->activeLow ? LOW : HIGH);
    }
    else
    {
        message += JHelp::field("value", "OFF");
        digitalWrite(this->pin, this->activeLow ? HIGH : LOW);
    }
    message += JHelp::end();
    this->logger.pushMessage("relay/set", message);
    this->state = newState;
}
