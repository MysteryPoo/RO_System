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

#ifndef _RELAY_
#define _RELAY_

#include "ObserverPattern/Subject.h"

class MqttManager;
class RelayConfiguration;
class SystemLog;
namespace RelayEnums
{
    enum Name : uint8_t;
    enum State : uint8_t;
}

class Relay : public Subject
{
    friend RelayConfiguration;
public:
    Relay(RelayEnums::Name name, SystemLog& logger, MqttManager& manager, int pin, bool activeLow = false);
    void set(const RelayEnums::State state);
    const RelayEnums::State GetState() const { return this->state; }
    const RelayEnums::Name GetName() const { return this->name; }

private:
    RelayEnums::Name name;
    SystemLog& logger;
    MqttManager& mqtt;
    int pin;
    bool activeLow;
    RelayEnums::State state;
};

#endif
