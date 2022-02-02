
/*
    UltraSonic
    
    An API to use the ultra sonic sensor.
    
    To use, assign the trigger and echo pins and then sample() at an appropriate rate (no faster than 50ms intervals).
    To retrieve the latest valid measurement, call getDistance() to retrieve the distance in centimeters.
*/

#ifndef _ULTRA_SONIC_
#define _ULTRA_SONIC_

#include "application.h"
#include "global-defines.h"
#include "ICloud.h"
#include "IComponent.h"
#include "IConfigurable.h"
#include "IHeartbeatReporter.h"

class SystemLog;

class UltraSonic : public ICloud, public IComponent, public IHeartbeatReporter, public IConfigurable {
public:
    UltraSonic(int trig, int echo, SystemLog &logger);
    ~UltraSonic();
    
    virtual int getDistance();

    virtual void cloudSetup() override;
    virtual void Update() override;
    // IHeartbeatReporter
    virtual void reportHeartbeat(JSONBufferWriter& writer) const override;
    // IConfigurable
    virtual void Configure(JSONValue json) override;

#ifdef TESTING
    void setDistance(int distance);
#endif
    
private:
    int triggerPin;
    int echoPin;
    int distance;
    unsigned long cooldownPeriod;
    LEDStatus* samplingUltraSonicStatus;

    virtual void sample();

    // IConfigurable
    virtual void fireConfigurationMessage() const override;

protected:
    SystemLog &logger;
    
};
#endif
