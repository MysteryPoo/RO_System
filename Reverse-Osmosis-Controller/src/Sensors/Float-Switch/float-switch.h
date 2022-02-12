
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
#include "Sensors/ISensor.h"
#include "IHeartbeatReporter.h"
#include "MQTT/ISubCallback.h"

class SystemLog;
class MQTTClient;

class FloatSwitch : public IComponent, public ISensor, public IHeartbeatReporter, public IConfigurable, public ISubCallback {
public:
    FloatSwitch(int pin, SystemLog &logger);
    FloatSwitch(int pin, SystemLog &logger, MQTTClient* mqtt);

    // IComponent
    virtual void Update() override;
    // ISensor
    virtual bool isFull() const override;
    // IHeartbeatReporter
    virtual void reportHeartbeat(JSONBufferWriter&) const override;
    // IConfigurable
    virtual void Configure(JSONValue json) override;
    // ISubCallback
    virtual void Callback(char* topic, uint8_t* payload, unsigned int length) override;
    virtual void OnConnect(bool connectSuccess, MQTTClient* mqtt) override;

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
#ifdef TESTING
    void setStatus(bool status);
#endif
    // IConfigurable
    virtual void fireConfigurationMessage() const override;

protected:
    SystemLog &logger;
    
};

#endif
