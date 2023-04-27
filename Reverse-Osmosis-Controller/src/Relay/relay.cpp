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

#include "MQTT/mqtt-manager.h"
#include "relay.h"
#include "RelayEnums.h"
#include "system-log.h"

Relay::Relay(RelayEnums::Name name, SystemLog& logger, MqttManager& manager, int pin, bool activeLow) : 
    name(name),
    logger(logger),
    mqtt(manager),
    pin(pin),
    activeLow(activeLow)
{
    pinMode(pin, OUTPUT);
    this->set(RelayEnums::State::OFF);
}

void Relay::set(const RelayEnums::State newState)
{
    if(this->pin < 0) // Bail out early if not configured
    {
        this->logger.error("Attempting to set an unconfigured relay.[" + RelayEnums::ToString(this->name) + "]");
        return;
    }
    if(newState == RelayEnums::State::ON)
    {
        digitalWrite(this->pin, this->activeLow ? LOW : HIGH);
    }
    else
    {
        digitalWrite(this->pin, this->activeLow ? HIGH : LOW);
    }
    this->state = newState;
    this->Notify();
}
