
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
#include "system-log.h"
#include "heartbeat-manager.h"
#include "relay.h"
#include "ROSystem.h"
#include <vector>
#ifdef FEATURE_ULTRASONIC
#include "Sensors/Ultra-Sonic/ultra-sonic.h"
#endif
#ifdef FEATURE_FLOATSWITCH
#include "Sensors/Float-Switch/float-switch.h"
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

time32_t timeToRestart;
Timer restartSystem(RESTART_DELAY, sysRestart_Helper, true);

std::vector<IComponent*> componentsToUpdate;
SystemLog syslog;
HeartbeatManager heartbeatManager(syslog);
Relay pump(Relay::Name::COMPONENT_PUMP, syslog, D7, true);
Relay inlet(Relay::Name::COMPONENT_INLETVALVE, syslog, D6, true);
Relay flush(Relay::Name::COMPONENT_FLUSHVALVE, syslog, D5, true);
ROSystem ro(pump, inlet, flush, syslog);

#ifdef FEATURE_ULTRASONIC
UltraSonic us(A3, A4, syslog);
#endif
#ifdef FEATURE_FLOATSWITCH
FloatSwitch fs(D4, syslog);
#endif
#ifdef FEATURE_FLOATMETER
FloatMeter fm(A5, syslog);
#endif

#ifdef TESTING
bool testIsFull = false;
void simulateFull()
{
#ifdef FEATURE_FLOATSWITCH
    fs.setStatus(testIsFull);
#endif
#ifdef FEATURE_ULTRASONIC
    us.setDistance(testIsFull ? 10 : 150);
#endif
    testIsFull = !testIsFull;
}
Timer runTestTimer(TEN_MINUTES_MS, simulateFull, false);
#endif


void setup()
{
    System.enableFeature(FEATURE_RESET_INFO);
    System.on(reset_pending, onResetPending);
    System.disableReset();
    watchDog = new ApplicationWatchdog(60000, watchDogHandler, 1536);
    timeToRestart = Time.now() + SECONDS_PER_DAY;
    
    Particle.function("reset", sysRestart);
    Particle.function("configuration", Configuration);
    ro.cloudSetup();

#ifdef FEATURE_FLOATSWITCH
    fs.cloudSetup();
    ro.AddSensor(&fs);
    componentsToUpdate.push_back(&fs);
#endif

#ifdef FEATURE_ULTRASONIC
    us.cloudSetup();
    componentsToUpdate.push_back(&us);
#endif

#ifdef FEATURE_FLOATMETER
    ro.AddSensor(&fm);
    componentsToUpdate.push_back(&fm);
#endif

#ifdef TESTING
    syslog.information("TEST MODE ENABLED");
    runTestTimer.start();
#endif

    componentsToUpdate.push_back(&syslog);
    componentsToUpdate.push_back(&heartbeatManager);
    componentsToUpdate.push_back(&ro);

    uint32_t resetData = System.resetReasonData();

    JSONBufferWriter message = SystemLog::createBuffer(2048);
    message.beginObject();
    message.name("event").value("restart");
    message.name("reason").value(getResetReason(resetData));
    message.name("data").value(resetData);
    message.endObject();

    syslog.pushMessage("system/restart", message.buffer());
}

void loop()
{
    if(Time.now() > timeToRestart)
    {
        sysRestart("");
    }

    for(IComponent* component : componentsToUpdate)
    {
        component->Update();
    }
}

int sysRestart(String data)
{
    syslog.enabled = false;
    restartSystem.start();
    timeToRestart = Time.now() + SECONDS_PER_DAY;
    return 0;
}

int Configuration(String config)
{
    int error = 0;
    JSONValue configuration = JSONValue::parseCopy(config);
    if(!configuration.isValid())
    {
        return 1;
    }

    syslog.pushMessage("system/configuration", config);

    JSONObjectIterator iter(configuration);
    while(iter.next())
    {
        if(iter.name() == "enabled")
        {
            ro.setEnable(iter.value().toBool());
        }
        if(iter.name() == "fillDistances")
        {
            JSONValue distances = iter.value();
            error += ro.ConfigureFillDistances(distances);
        }
        if(iter.name() == "tickRate")
        {
            int rawTickRate = iter.value().toInt();
            if(rawTickRate > 0)
            {
                //tickTimer.changePeriod((unsigned int)rawTickRate); // TODO: HeartbeatManager configurable
            }
        }
        if(iter.name() == "pumpCooldown")
        {
            ro.ConfigurePumpCooldown(iter.value().toInt());
        }
        if(iter.name() == "flushDuration")
        {
            ro.ConfigureFlushDuration(iter.value().toInt());
        }
#ifdef TESTING
        if(iter.name() == "forceState")
        {
            String state = iter.value().toString().data();
            ro.cloudRequestState(state);
        }
#ifdef FEATURE_FLOATSWITCH
        if(iter.name() == "float")
        {
            fs.setStatus(iter.value().toBool());
        }
#endif
#ifdef FEATURE_ULTRASONIC
        if(iter.name() == "ultraSonic")
        {
            us.setDistance(iter.value().toInt());
        }
#endif
#endif
    }

    return error;
}

void sysRestart_Helper()
{
    if(syslog.isEmpty() && ROSystem::State::IDLE == ro.getState())
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
    ro.shutdown();
    System.enableReset();
}
