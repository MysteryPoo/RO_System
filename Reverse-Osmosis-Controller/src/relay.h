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

#include "application.h"

class SystemLog;

class Relay
{
    public:
        enum Name
        {
            COMPONENT_PUMP,
            COMPONENT_INLETVALVE,
            COMPONENT_FLUSHVALVE
        };
        enum State
        {
            ON,
            OFF
        };
        String toString();
    private:
        Name name;
        int pin;
        bool activeLow;
        SystemLog &logger;
        State state;
        
    public:
        Relay(Name name, SystemLog &logger, int pin, bool activeLow = false);
        void set(Relay::State state);
        Relay::State get() {return this->state;}
        
};

#endif
