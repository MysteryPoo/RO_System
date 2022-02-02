
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
#include "Sensors/ISensor.h"
#include "IHeartbeatReporter.h"
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

class ROSystem : public ICloud, public IComponent, public IHeartbeatReporter {
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

    void shutdown();

    State getState() { return this->state; };
    String getStateString();

    void setEnable(bool enable) { this->enabled = enable; };
    int setFillDistances(String csvFill);
    bool getEnabled() const { return this->enabled; }

    int cloudRequestState(String newState);

    int ConfigureFillDistances(spark::JSONValue& distances);
    void ConfigurePumpCooldown(int newCooldown);
    void ConfigureFlushDuration(int duration);
    
    
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
    
};

#endif
