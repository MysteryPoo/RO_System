
/*
    Main logic controller for the overall system
    
    Accepts sensor data from Float Switch and an Ultra-Sonic distance sensor.
    Controls a pump relay, inlet valve relay, and flush valve relay.
    
    Simple state machine to determine, based on water level inputs, whether to refill the resovoir or not.
*/

#ifndef _ROSYSTEM_
#define _ROSYSTEM_

#include "global-defines.h"
#include "ICloud.h"
#include "IComponent.h"
#include "Sensors/ISensor.h"
#include "IHeartbeatReporter.h"
#include "IConfigurable.h"
#include <vector>

class Relay;
class SystemLog;
class String;
class FloatSwitch;
class UltraSonic;
namespace spark
{
    class JSONValue;
}

class ROSystem : public ICloud, public IComponent, public IHeartbeatReporter, public IConfigurable {
public:
    enum State {
        BOOT,
        IDLE,
        FLUSH,
        FILL
    };
    
    ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger);

    void AddSensor(ISensor* sensor);

    // IComponent
    virtual void Update() override;
    // ICloud
    virtual void cloudSetup() override;
    // IHeartbeatReporter
    virtual void reportHeartbeat(JSONBufferWriter& writer) const;
    // IConfigurable
    virtual void Configure(JSONValue json) override;

    void shutdown();

    State getState() { return this->state; };
    String getStateString();

    bool getEnabled() const { return this->enabled; }
    
    
private:
    State state;
    Relay &pump;
    Relay &inlet;
    Relay &flush;
    SystemLog &logger;
    bool flushedToday;
    unsigned long totalPumpTime; // Deprecated
    unsigned long totalPumpRuns;
    unsigned long lastPumpTime;
    unsigned long flushStartedTime;
    unsigned int flushDuration;
    unsigned long pumpRunTime; // Deprecated
    bool firstPump;
    std::vector<ISensor*> sensors;

    // Configuration
    bool enabled;
    unsigned short fillStartDistance; // Centimeters
    unsigned short fillStopDistance; // Centimeters
    unsigned int pumpCooldown; // Time between turning off and then on again
    
    void update(bool tankFull);
    void requestState(ROSystem::State state, const char* requestReason);
    void requestState(ROSystem::State state, String requestReason);
    bool activatePump();
    bool deactivatePump();
    int setFillDistances(String csvFill);

#ifdef TESTING
    int configureState(String newState);
#endif
    int configureFillDistances(spark::JSONValue& distances);
    void configurePumpCooldown(int newCooldown);
    void configureFlushDuration(int duration);

    // IConfigurable
    virtual void fireConfigurationMessage() const override {};
    
};

#endif
