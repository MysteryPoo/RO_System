
#include "test-float-switch.h"

#define TIME_CONSIDERED_STABLE_MS 1000
#define TIME_CONSIDERED_UNSTABLE_MS 10000

TestFloatSwitch::TestFloatSwitch(int i, SystemLog& logger) : FloatSwitch::FloatSwitch(i, logger),
testValue(false)
{

}

void TestFloatSwitch::cloudSetup()
{
    Particle.variable("Float-Status", this->testValue);
}

void TestFloatSwitch::sample()
{
    
}

bool TestFloatSwitch::isActive()
{
    return this->testValue;
}

void TestFloatSwitch::setStatus(bool status)
{
    this->testValue = status;
    String message = JHelp::begin();
    message += JHelp::field("status", String(status ? "true" : "false"));
    message += JHelp::end();
    this->logger.pushMessage("float-switch", message);
}
