
/*
    Reverse Osmosis Motor-and-valve CONtroller
    ROMCON
    
    Thornton Labs
    Copyright 2019
    
    Version: 1.0
    
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
        
*/

#include "global-defines.h"
#include "system-log.h"
#include "ultra-sonic.h"
#include "relay.h"
#include "float-switch.h"
#include "ultra-sonic.h"
#include "ROSystem.h"
#include <vector>

#define SECONDS_PER_DAY 86400
#define TEN_MINUTES_MS 600000
#define THIRTY_SECONDS_MS 30000
#define ONE_HOUR_MS 3600000
#define RESTART_DELAY 10000 // Delay before attempting to restart after manual request

time32_t timeToRestart;
bool lastFloatSwitch = false;

SystemLog syslog;
Relay pump(Relay::Name::COMPONENT_PUMP, syslog, D7, true);
Relay inlet(Relay::Name::COMPONENT_INLETVALVE, syslog, D6, true);
Relay flush(Relay::Name::COMPONENT_FLUSHVALVE, syslog, D5, true);

UltraSonic us(A3, A4, syslog);
FloatSwitch fs(A0, syslog);

ROSystem ro(pump, inlet, flush, fs, us, syslog);

std::vector<IComponent*> componentsToUpdate;

Timer restartSystem(RESTART_DELAY, sysRestart_Helper, true);

#ifdef TESTING
bool testIsFull = false;
void simulateFull()
{
    fs.setStatus(testIsFull);
    us.setDistance(testIsFull ? 10 : 150);

    testIsFull = !testIsFull;
}
Timer runTestTimer(TEN_MINUTES_MS, simulateFull, false);
Timer tickTimer(THIRTY_SECONDS_MS, sendTick, false);
#else
Timer tickTimer(TEN_MINUTES_MS, sendTick, false);
#endif


void setup()
{
    timeToRestart = Time.now() + SECONDS_PER_DAY;
    
    Particle.function("reset", sysRestart);
    Particle.function("configuration", Configuration);
    fs.cloudSetup();
    us.cloudSetup();
    ro.cloudSetup();

#ifdef TESTING
    syslog.information("TEST MODE ENABLED");
    runTestTimer.start();
#endif
    tickTimer.start();

    componentsToUpdate.push_back(&fs);
    componentsToUpdate.push_back(&us);
    componentsToUpdate.push_back(&ro);
}

void loop()
{
    if(Time.now() > timeToRestart)
    {
        sysRestart("Daily restart commencing.");
    }
    
    syslog.publishLog();

    for(IComponent* component : componentsToUpdate)
    {
        component->Update();
    }

    // Notify if the float switch was triggered.
    if(fs.isActive() && !lastFloatSwitch)
    {
        syslog.error("Float Switch has been triggered!");
    }
    lastFloatSwitch = fs.isActive();
}

int sysRestart(String data)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    JSONBufferWriter message(buffer, sizeof(buffer));
    message.beginObject();
    message.name("event").value("restart");
    message.name("reason").value(data);
    message.endObject();
    ro.shutdown();
    syslog.pushMessage("system/restart", String(message.buffer()));
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
                tickTimer.changePeriod((unsigned int)rawTickRate);
            }
        }
        if(iter.name() == "pumpCooldown")
        {
            ro.ConfigurePumpCooldown(iter.value().toInt());
        }
#ifdef TESTING
        if(iter.name() == "forceState")
        {
            String state = iter.value().toString().data();
            ro.cloudRequestState(state);
        }
        if(iter.name() == "float")
        {
            fs.setStatus(iter.value().toBool());
        }
        if(iter.name() == "ultraSonic")
        {
            us.setDistance(iter.value().toInt());
        }
        if(iter.name() == "flushDuration")
        {
            ro.ConfigureFlushDuration(iter.value().toInt());
        }
#endif
    }

    return error;
}

void sysRestart_Helper()
{
    if(syslog.isEmpty())
    {
        System.reset();
    }
    else
    {
        restartSystem.reset();
    }
}

void sendTick()
{
    JSONBufferWriter message = SystemLog::createBuffer(2048);
    message.beginObject();
    message.name("event").value("tick");
    message.name("messageQueueSize").value(syslog.messageQueueSize());
    message.name("floatSwitch").value(fs.isActive());
    message.name("ultra-sonic").value(us.getDistance());
    message.name("enabled").value(ro.getEnabled());
    message.endObject();

    syslog.pushMessage("system/tick", message.buffer());
}
