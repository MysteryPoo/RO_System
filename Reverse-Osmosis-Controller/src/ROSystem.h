
/*
    Main logic controller for the overall system
    
    Accepts sensor data from Float Switch and an Ultra-Sonic distance sensor.
    Controls a pump relay, inlet valve relay, and flush valve relay.
    
    Simple state machine to determine, based on water level inputs, whether to refill the resovoir or not.
*/

#ifndef _ROSYSTEM_
#define _ROSYSTEM_

#include "ICloud.h"
#include "IComponent.h"

class Relay;
class SystemLog;
class String;
class FloatSwitch;
class UltraSonic;
namespace spark
{
    class JSONValue;
}

class ROSystem : public ICloud, public IComponent {
public:
    enum State {
        IDLE,
        FLUSH,
        FILL
    };
    
    ROSystem(Relay &pump, Relay &inlet, Relay &flush, FloatSwitch &floatSwitch, UltraSonic &ultraSonic, SystemLog &logger);

    virtual void Update() override;
    void update(bool tankFull, unsigned short distance);

    virtual void cloudSetup() override;

    void shutdown();

    State getState() { return this->state; };
    String getStateString();

    int enable(String setEnable); // Deprecated
    void setEnable(bool enable);
    int setFillDistances(String csvFill);
    bool getEnabled() { return this->enabled; }

    int cloudRequestState(String newState);

    int ConfigureFillDistances(spark::JSONValue& distances);
    void ConfigurePumpCooldown(int newCooldown);
    void ConfigureFlushDuration(int duration);
    
    
private:
    State state;
    Relay &pump;
    Relay &inlet;
    Relay &flush;
    FloatSwitch &floatSwitch;
    UltraSonic &ultraSonic;
    SystemLog &logger;
    bool flushedToday;
    unsigned long totalPumpTime;
    unsigned long totalPumpRuns;
    unsigned long lastPumpTime;
    unsigned long flushStartedTime;
    unsigned int flushDuration;
    unsigned long pumpRunTime;
    bool firstPump;

    // Configuration
    bool enabled;
    unsigned short fillStartDistance; // Centimeters
    unsigned short fillStopDistance; // Centimeters
    unsigned int pumpCooldown; // Time between turning off and then on again
    
    void requestState(ROSystem::State state, const char* requestReason);
    void requestState(ROSystem::State state, String requestReason);
    bool activatePump();
    bool deactivatePump();
    
};

#endif
