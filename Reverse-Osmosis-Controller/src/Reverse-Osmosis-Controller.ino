
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

#define TESTING 1

#include "system-log.h"
#include "ultra-sonic.h"
#include "relay.h"
#include "float-switch.h"
#include "ultra-sonic.h"
#include "ROSystem.h"
#include "json.h"

#ifdef TESTING
#include "test-float-switch.h"
#include "test-ultra-sonic.h"
#endif

#define SECONDS_PER_DAY 86400
#define FULL_DISTANCE_CM 40

time32_t timeToRestart;

SystemLog syslog;
Relay pump(Relay::Name::COMPONENT_PUMP, syslog, D7);
Relay inlet(Relay::Name::COMPONENT_INLETVALVE, syslog, D6);
Relay flush(Relay::Name::COMPONENT_FLUSHVALVE, syslog, D5);

#ifdef TESTING
TestFloatSwitch fs(A0, syslog);
TestUltraSonic us(A3, A4, syslog);
#else
FloatSwitch fs(A0, syslog);
UltraSonic us(A3, A4, syslog);
#endif

ROSystem ro(pump, inlet, flush, syslog);

Timer restartSystem(10000, sysRestart_Helper, true);

#ifdef TESTING
bool testIsFull = false;
void simulateFull()
{
    fs.setStatus(testIsFull);
    us.setDistance(testIsFull ? 10 : 300);

    testIsFull = !testIsFull;
}
Timer runTestTimer(1000000, simulateFull, false);
Timer testTickTimer(30000, testTick, false);
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
#endif
}

void loop()
{
    if(Time.now() > timeToRestart)
    {
        sysRestart("Daily restart commencing.");
    }
    
    syslog.publishLog();
    
    us.sample();
    fs.sample();
    
    int distance = us.getDistance();
    
    // Use distance to top of tank as initial full check
    /*bool ultraSonicDetectsFull = false;
    if(distance < FULL_DISTANCE_CM)
    {
        ultraSonicDetectsFull = true;
    }
    */
    // Use float switch as a backup full check
    bool floatSwitchDetectsFull = fs.isStable() && fs.isActive();
    
    bool tankFull = /*ultraSonicDetectsFull || */floatSwitchDetectsFull;
    ro.update(tankFull, distance);
    // If float reads full but US reads not full, report critical error
    /*if(ultraSonicDetectsFull ^ floatSwitchDetectsFull)
    {
        String USFull = ultraSonicDetectsFull ? "full" : "not full";
        String FloatFull = floatSwitchDetectsFull ? "full" : "not full"; 
        syslog.error("Ultra Sonic and Float Switch disagree. US: [" + USFull + "]; Float: [" + FloatFull + "]");
    }*/
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

void testTick()
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
