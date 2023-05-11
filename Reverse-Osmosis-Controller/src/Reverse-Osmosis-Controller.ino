
/*
    Reverse Osmosis Motor-and-valve CONtroller
    ROMCON
    
    Thornton Labs
    Copyright 2019
    
    Description:
    Control the RO pump and valves to sustain water reservoir.
    Run a flush cycle at most once every 24-hour cycle, only if the system has ran that day.
    Publish events for checking status remotely, including the water level.
    
    Inputs:
        Float switch: On/Off - Full/Not-Full
        Ultra Sonic Distance Sensor: Analog - Used to measure approximate current volume of water
    Outputs:
        Pump
        Inlet Valve: Must be opened for at lest 5 seconds prior to pump turning on
        Flush Valve: Open to flush system
        LED: Debugging status
        
    Pins
        D7: Output - Pump ( On: HIGH; Off: LOW )
        D6: Output - Inlet ( Open: LOW; Closed: HIGH )
        D5: Output - Flush ( Open: LOW; Closed: HIGH )
        D4: Input - Float Switch ( On: HIGH; Off; LOW )
        A3: Output - Distance Sensor Trigger
        A4: Input - Distance Sensor Echo
        A5: Input - Float Meter
        
*/

SYSTEM_THREAD(ENABLED)

#include "global-defines.h"
#include "IUpdateOnLoop.h"
#include "system-log.h"
// Wifi
#include "Wifi/WifiModule.h"
#include "Wifi/WifiReporter.h"
// Relay
#include "Relay/RelayEnums.h"
#include "Relay/relay.h"
#include "Relay/RelayConfiguration.h"
#include "Relay/RelayReporter.h"
// RoSystem
#include "ROSystem/RoSystemEnum.h"
#include "ROSystem/ROSystem.h"
#include "ROSystem/RoSystemConfiguration.h"
#include "ROSystem/RoSystemReporter.h"
// Mqtt
#include "MQTT/mqtt-manager.h"
#include "MQTT/mqtt-queue.h"
#include "MQTT/SimpleBroker.h"
// Utility
#include "Utility/JsonBuffer.h"
#include "Utility/TimeHelper.h"
// std
#include <vector>
#include <map>
#ifdef FEATURE_ULTRASONIC
#include "Sensors/Ultra-Sonic/ultra-sonic.h"
#endif
#ifdef FEATURE_FLOATSWITCH
#include "Sensors/Float-Switch/float-switch.h"
#include "Sensors/Float-Switch/FloatSwitchConfiguration.h"
#include "Sensors/Float-Switch/FloatSwitchReporter.h"
#endif
#ifdef FEATURE_FLOATMETER
#include "Sensors/Float-Meter/float-meter.h"
#endif

#define SECONDS_PER_DAY 86400
#define TEN_MINUTES_MS 600000
#define THIRTY_SECONDS_MS 30000
#define ONE_HOUR_MS 3600000
#define RESTART_DELAY 10000 // Delay before attempting to restart after manual request

enum UserReason
{
    DAILY,
};

ApplicationWatchdog *watchDog;
//SerialLogHandler logHandler;

unsigned long timeToRestart;
Timer restartSystem(RESTART_DELAY, sysRestart_Helper, true);

std::vector<IUpdateOnLoop*> thingsToUpdate;
SimpleBroker broker;
MqttQueue mqttQueue;
MqttManager mqttManager(broker, mqttQueue);
WifiModule wifi;
WifiReporter wifiReporter(&wifi, mqttManager);
SystemLog syslog(mqttManager);
Relay pump(RelayEnums::Name::COMPONENT_PUMP, syslog, mqttManager, D7, true);
Relay inlet(RelayEnums::Name::COMPONENT_INLETVALVE, syslog, mqttManager, D6, true);
Relay flush(RelayEnums::Name::COMPONENT_FLUSHVALVE, syslog, mqttManager, D5, true);
ROSystem ro(pump, inlet, flush, syslog);

RelayConfiguration pumpConfig(pump, mqttManager);
RelayReporter pumpReporter(&pump, mqttManager);
RelayConfiguration inletConfig(inlet, mqttManager);
RelayReporter inletReporter(&inlet, mqttManager);
RelayConfiguration flushConfig(flush, mqttManager);
RelayReporter flushReporter(&flush, mqttManager);
RoSystemConfiguration roConfig(ro, mqttManager);
RoSystemReporter roReporter(&ro, mqttManager);

#ifdef FEATURE_ULTRASONIC
UltraSonic us(A3, A4, syslog);
#endif
#ifdef FEATURE_FLOATSWITCH
FloatSwitch fs(D4, syslog);
FloatSwitchConfiguration fsConfig(fs, mqttManager);
FloatSwitchReporter fsReporter(&fs, mqttManager);
#endif
#ifdef FEATURE_FLOATMETER
FloatMeter fm(A5, syslog);
#endif

void setup()
{
    System.enableFeature(FEATURE_RESET_INFO);
    System.on(reset_pending, onResetPending);
    System.disableReset();
    //WiFi.selectAntenna(ANT_AUTO);
    watchDog = new ApplicationWatchdog(60000, watchDogHandler, 1536);
    timeToRestart = millis() + (SECONDS_PER_DAY * 1000ul);
    
    Particle.function("reset", sysRestart);

#ifdef FEATURE_FLOATSWITCH
    ro.AddSensor(&fs);
    thingsToUpdate.push_back(&fs);
#endif

#ifdef FEATURE_ULTRASONIC
    us.cloudSetup();
    thingsToUpdate.push_back(&us);
    heartbeatManager.RegisterReporter("ultra-sonic", &us);
#endif

#ifdef FEATURE_FLOATMETER
    ro.AddSensor(&fm);
    thingsToUpdate.push_back(&fm);
    heartbeatManager.RegisterReporter("float-meter", &fm);
#endif

#ifdef TESTING
    syslog.information("TEST MODE ENABLED");
#endif

    thingsToUpdate.push_back(&wifi);
    thingsToUpdate.push_back(&mqttManager);
    thingsToUpdate.push_back(&ro);

    reportRestartReason();
}

void loop()
{
    if(millis() > timeToRestart)
    {
        sysRestart("");
    }

    for(IUpdateOnLoop* thing : thingsToUpdate)
    {
        thing->Update();
    }
}

int sysRestart(String data)
{
    restartSystem.start();
    timeToRestart = Time.now() + SECONDS_PER_DAY;
    return 0;
}

void sysRestart_Helper()
{
    if(RoSystemEnum::State::IDLE == ro.GetState() && !mqttManager.isConnected())
    {
        System.reset(UserReason::DAILY);
    }
    else
    {
        restartSystem.reset();
    }
}

void watchDogHandler(void)
{
    System.reset(1, RESET_NO_WAIT);
}

String getResetReason(uint32_t data)
{
    int resetReason = System.resetReason();
    switch(resetReason)
    {
        case RESET_REASON_PIN_RESET:
            return "Pin Reset";
        case RESET_REASON_POWER_MANAGEMENT:
            return "Low Power Management";
        case RESET_REASON_POWER_DOWN:
            return "Power Down Reset";
        case RESET_REASON_POWER_BROWNOUT:
            return "Power Brownout";
        case RESET_REASON_WATCHDOG:
            return "Watchdog Reset";
        case RESET_REASON_UPDATE:
            return "Firmware Updated";
        case RESET_REASON_UPDATE_TIMEOUT:
            return "Firmware update timed out.";
        case RESET_REASON_FACTORY_RESET:
            return "Factory reset requested.";
        case RESET_REASON_SAFE_MODE:
            return "Safe mode requested.";
        case RESET_REASON_DFU_MODE:
            return "DFU mode requested.";
        case RESET_REASON_PANIC:
            return "System panicked!";
        case RESET_REASON_USER:
            return getUserReason(data);
        case RESET_REASON_UNKNOWN:
            return "Unknown reset reason.";
        case RESET_REASON_NONE:
            return "Reset information unavailable.";
        default:
            return "Reset reason fell-through switch.";
    }
}

String getUserReason(int code)
{
    switch(code)
    {
        case UserReason::DAILY:
            return "Daily restart.";
        default:
            return "Unknown reason.";
    }
}

void onResetPending()
{
    ro.Shutdown();
    mqttManager.PushPayload("status", "offline");
    System.enableReset();
}

void reportRestartReason()
{
    uint32_t resetData = System.resetReasonData();
    JSONBufferWriter message = JsonBuffer::createBuffer(2048);
    message.beginObject()
    .name("datetime").value(TimeHelper::GetTimeIfAble())
    .name("reason").value(getResetReason(resetData))
    .endObject();

    mqttQueue.PushPayload("restart", message.buffer());
    delete[] message.buffer();
}
