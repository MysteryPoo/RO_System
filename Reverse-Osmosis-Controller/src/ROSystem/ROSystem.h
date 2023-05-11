
/*
    Main logic controller for the overall system
    
    Accepts sensor data from Float Switch and an Ultra-Sonic distance sensor.
    Controls a pump relay, inlet valve relay, and flush valve relay.
    
    Simple state machine to determine, based on water level inputs, whether to refill the resovoir or not.
*/

#ifndef _ROSYSTEM_
#define _ROSYSTEM_

#include "IComponent.h"
#include "ObserverPattern/Subject.h"
#include <vector>

class AbstractSensor;
class Relay;
class SystemLog;
class String;
class MqttManager;
class RoSystemConfiguration;
namespace spark
{
    class JSONValue;
}
namespace RoSystemEnum
{
    enum State : uint8_t;
}

class ROSystem : public Subject, public IComponent {
    friend RoSystemConfiguration;
public:
    ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger);

    void AddSensor(AbstractSensor* sensor);
    void Shutdown();
    const bool GetEnabled() const { return this->enabled; }
    const bool GetFlushed() const { return this->flushedToday; }
    RoSystemEnum::State GetState() { return this->state; };
    
    // IComponent
    virtual const String GetName() const override;
    virtual void Update() override;
    
private:
    RoSystemEnum::State state;
    Relay &pump;
    Relay &inlet;
    Relay &flush;
    SystemLog &logger;
    bool flushedToday;
    unsigned long totalPumpTime;
    unsigned long totalPumpRuns;
    unsigned long lastPumpTime;
    unsigned long flushStartedTime;
    unsigned int flushDuration;
    unsigned long pumpRunTime;
    std::vector<AbstractSensor*> sensors;

    // Configuration
    bool enabled;
    unsigned short fillStartDistance; // Centimeters
    unsigned short fillStopDistance; // Centimeters
    unsigned int pumpCooldown; // Time between turning off and then on again
    
    bool activatePump();
    bool deactivatePump();
    bool getEnabled() const { return this->enabled; }
    bool isPumpReady() const;
    void requestState(RoSystemEnum::State state, const char* requestReason);
    void requestState(RoSystemEnum::State state, String requestReason);
    void update(bool tankFull);
    
};

#endif
