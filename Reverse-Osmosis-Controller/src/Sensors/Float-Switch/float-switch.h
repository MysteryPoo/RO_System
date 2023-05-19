
/*
    FloatSwitch
    
    Class that contains some stability logic for the float swtich. Float switch must read the same for X amount of time consistently to be considered stable.
    
    Readings should only be considered if stable is true.
*/

#ifndef _FLOAT_SWITCH_
#define _FLOAT_SWITCH_

#include "Sensors/AbstractSensor.h"

class FloatSwitchConfiguration;
class String;
class SystemLog;

class FloatSwitch : public AbstractSensor {
    friend FloatSwitchConfiguration;
public:
    FloatSwitch(int pin, SystemLog &logger);
    virtual ~FloatSwitch() override {};

    // AbstractSensor
    virtual const bool isFull() const override;
    virtual void Update() override;
    virtual const String GetName() const override;

private:
    int pin;
    unsigned long stableTimer;
    bool lastStatus;
    bool status;
    unsigned long lastStable;
    bool stable;
    bool firedWarning;
    bool isReliable;

    virtual void sample();
};

#endif
