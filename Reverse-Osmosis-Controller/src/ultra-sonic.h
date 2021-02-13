
/*
    UltraSonic
    
    An API to use the ultra sonic sensor.
    
    To use, assign the trigger and echo pins and then sample() at an appropriate rate (no faster than 50ms intervals).
    To retrieve the latest valid measurement, call getDistance() to retrieve the distance in centimeters.
*/

#ifndef _ULTRA_SONIC_
#define _ULTRA_SONIC_

#include "ICloud.h"

class SystemLog;

class UltraSonic : public ICloud {
public:
    UltraSonic(int trig, int echo, SystemLog &logger);
    
    virtual void sample();
    virtual int getDistance();

    virtual void cloudSetup() override;
    
private:
    int triggerPin;
    int echoPin;
    int distance;
    unsigned long cooldownPeriod;

protected:
    SystemLog &logger;
    
};
#endif
