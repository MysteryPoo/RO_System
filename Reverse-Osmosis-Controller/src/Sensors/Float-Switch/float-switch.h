
/*
    FloatSwitch
    
    Class that contains some stability logic for the float swtich. Float switch must read the same for X amount of time consistently to be considered stable.
    
    Readings should only be considered if stable is true.
*/

#ifndef _FLOAT_SWITCH_
#define _FLOAT_SWITCH_

#include "global-defines.h"
#include "application.h"
#include "ICloud.h"
#include "IComponent.h"
#include "IConfigurable.h"
#include "Sensors/ISensor.h"
#include "IHeartbeatReporter.h"

class SystemLog;

class FloatSwitch : public ICloud, public IComponent, public ISensor, public IHeartbeatReporter, IConfigurable {
public:
    FloatSwitch(int pin, SystemLog &logger);
    void ResetReliableFlag();

    // ICloud
    virtual void cloudSetup() override;
    // IComponent
    virtual void Update() override;
    // ISensor
    virtual bool isFull() const override;
    // IHeartbeatReporter
    virtual void reportHeartbeat(JSONBufferWriter&) const override;
#ifdef TESTING
    void setStatus(bool status);
#endif
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
    // IConfigurable
    virtual void fireConfigurationMessage() const override;

protected:
    SystemLog &logger;
    
};

#endif
