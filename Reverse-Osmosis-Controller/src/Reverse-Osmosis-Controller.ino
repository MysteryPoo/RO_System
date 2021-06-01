
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

#include "system-log.h"
#include "ultra-sonic.h"
#include "relay.h"
#include "float-switch.h"
#include "ultra-sonic.h"
#include "ROSystem.h"
#include "json.h"

#define TESTING 1

#ifdef TESTING
#include "test-float-switch.h"
#include "test-ultra-sonic.h"
#endif

#define SECONDS_PER_DAY 86400
#define TEN_MINUTES_MS 600000
#define THIRTY_SECONDS_MS 30000
#define ONE_HOUR_MS 3600000
#define FULL_DISTANCE_CM 40

time32_t timeToRestart;

SystemLog syslog;
Relay pump(Relay::Name::COMPONENT_PUMP, syslog, D7, true);
Relay inlet(Relay::Name::COMPONENT_INLETVALVE, syslog, D6, true);
Relay flush(Relay::Name::COMPONENT_FLUSHVALVE, syslog, D5, true);

#ifdef TESTING
TestFloatSwitch fs(A0, syslog);
TestUltraSonic us(A3, A4, syslog);
#else
FloatSwitch fs(A0, syslog);
UltraSonic us(A3, A4, syslog);
#endif

ROSystem ro(pump, inlet, flush, fs, us, syslog);

std::vector<IComponent*> componentsToUpdate;

Timer restartSystem(10000, sysRestart_Helper, true);

#ifdef TESTING
bool testIsFull = false;
void simulateFull()
{
    fs.setStatus(testIsFull);
    us.setDistance(testIsFull ? 10 : 150);

    testIsFull = !testIsFull;
}
Timer runTestTimer(TEN_MINUTES_MS, simulateFull, false);
Timer testTickTimer(THIRTY_SECONDS_MS, sendTick, false);
#else
Timer tickTimer(TEN_MINUTES_MS, sendTick, false);
#endif


void setup()
{
    timeToRestart = Time.now() + SECONDS_PER_DAY;
    
    Particle.function("reset", sysRestart);
    fs.cloudSetup();
    us.cloudSetup();
    ro.cloudSetup();

#ifdef TESTING
    syslog.information("TEST MODE ENABLED");
    runTestTimer.start();
    testTickTimer.start();
#else
    tickTimer.start();
#endif

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
    if(fs.isActive())
    {
        syslog.error("Float Switch has been triggered!");
    }
}

int sysRestart(String data)
{
    String message = JHelp::begin();
    message += JHelp::field("event", "restart");
    message += JHelp::next();
    message += JHelp::field("reason", data);
    message += JHelp::end();
    ro.shutdown();
    syslog.pushMessage("system/restart", message);
    syslog.enabled = false;
    restartSystem.start();
    timeToRestart = Time.now() + SECONDS_PER_DAY;
    return 0;
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
    String json = JHelp::begin();
    json += JHelp::field("event", "tick");
    json += JHelp::next();
    json += JHelp::field("messageQueueSize", syslog.messageQueueSize());
    json += JHelp::next();
    json += JHelp::field("floatSwitch", fs.isActive());
    json += JHelp::next();
    json += JHelp::field("ultra-sonic", us.getDistance());
    json += JHelp::next();
    json += JHelp::field("enabled", ro.getEnabled());
    json += JHelp::end();
    syslog.pushMessage("system/tick", json);
}
