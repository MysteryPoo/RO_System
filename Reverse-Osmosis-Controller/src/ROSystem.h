
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
#include "IHeartbeatReporter.h"
#include "IConfigurable.h"
#include "MQTT/ISubCallback.h"
#include <vector>

class ISensor;
class Relay;
class SystemLog;
class String;
class FloatSwitch;
class UltraSonic;
class MQTTClient;
class MqttQueue;
namespace spark
{
    class JSONValue;
}

class ROSystem : public IComponent, public IHeartbeatReporter, public IConfigurable, public ISubCallback {
public:
    enum State {
        BOOT,
        IDLE,
        FLUSH,
        FILL
    };
    
    ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger, MqttQueue& mqttQueue);
    ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger, MQTTClient* mqtt, MqttQueue& mqttQueue);

    void AddSensor(ISensor* sensor);

    // IComponent
    virtual void Update() override;
    // IHeartbeatReporter
    virtual void ReportHeartbeat(JSONBufferWriter& writer) const;
    // IConfigurable
    virtual void Configure(JSONValue json) override;
    // ISubCallback
    virtual void Callback(char* topic, uint8_t* payload, unsigned int length) override;
    virtual void OnConnect(bool connectSuccess, MQTTClient* mqtt) override;
    virtual String GetName() const override;

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
    MQTTClient* mqttClient = nullptr;
    MqttQueue& mqttQueue;
    bool flushedToday;
    unsigned long totalPumpTime;
    unsigned long totalPumpRuns;
    unsigned long lastPumpTime;
    unsigned long flushStartedTime;
    unsigned int flushDuration;
    unsigned long pumpRunTime;
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
