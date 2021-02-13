
#include "test-ultra-sonic.h"
#include "system-log.h"

#define US_TRIGGER_TIME 11
#define US_INTERVAL_TIME 51
#define CM_PER_MICROSECOND 0.0343

TestUltraSonic::TestUltraSonic(int trig, int echo, SystemLog &logger) : UltraSonic::UltraSonic(trig, echo, logger),
testDistance(100)
{

}

void TestUltraSonic::sample()
{
    
}

int TestUltraSonic::getDistance()
{
    return this->testDistance;
}

void TestUltraSonic::cloudSetup()
{
    Particle.variable("Ultra-Sonic-Distance", this->testDistance);
}

void TestUltraSonic::setDistance(int distance)
{
    this->testDistance = distance;
    this->logger.pushMessage("ultra-sonic", "{\"distance\":" + String(distance) + "}");
}
