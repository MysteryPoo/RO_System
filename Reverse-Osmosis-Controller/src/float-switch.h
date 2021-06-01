
/*
    FloatSwitch
    
    Class that contains some stability logic for the float swtich. Float switch must read the same for X amount of time consistently to be considered stable.
    
    Readings should only be considered if stable is true.
*/

#ifndef _FLOAT_SWITCH_
#define _FLOAT_SWITCH_

#include "application.h"
#include "ICloud.h"
#include "IComponent.h"
#include "IConfigurable.h"

class SystemLog;

class FloatSwitch : public ICloud, public IComponent, IConfigurable {
public:
    FloatSwitch(int pin, SystemLog &logger);
    virtual bool isActive();

    virtual void cloudSetup() override;
    virtual void Update() override;
private:
    int pin;
    unsigned long stableTimer;
    bool lastStatus;
    bool status;
    unsigned long lastStable;
    bool stable;

    virtual void sample();
    virtual void fireConfigurationMessage() const override;

protected:
    SystemLog &logger;
    
};

#endif
