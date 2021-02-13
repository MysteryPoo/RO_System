
#include "float-switch.h"
#include "system-log.h"

#define TIME_CONSIDERED_STABLE_MS 1000
#define TIME_CONSIDERED_UNSTABLE_MS 10000

FloatSwitch::FloatSwitch(int pin, SystemLog &logger) :
    pin(pin),
    stableTimer(millis() + 1000),
    lastStatus(false),
    status(false),
    lastStable(millis()),
    stable(false),
    logger(logger)
{
    pinMode(pin, INPUT_PULLDOWN);
    this->logger.pushMessage("float-switch", "{\"event\":\"configuration\",\"pin\":" + String(pin) + "}");
}

void FloatSwitch::cloudSetup()
{
    Particle.variable("Float-Status", this->status);
}

bool FloatSwitch::isActive()
{
    return this->status;
}

void FloatSwitch::sample()
{
    unsigned long curMillis = millis();
    this->status = digitalRead(this->pin) == HIGH ? true : false;
    // Check if the current status reading is the same as last time.
    if(this->status != this->lastStatus)
    {
        // If it's not, reset stability flag
        this->lastStatus = this->status;
        this->stableTimer = curMillis + TIME_CONSIDERED_STABLE_MS;
        this->stable = false;
        // If unstable for a long period of time (constant flip/flop), we have a problem.
        if(curMillis > this->lastStable + TIME_CONSIDERED_UNSTABLE_MS)
        {
            this->logger.error("Float switch is highly unstable.");
        }
    }
    else if(curMillis > this->stableTimer)
    {
        this->stable = true;
        this->lastStable = curMillis;
    }
}

bool FloatSwitch::isStable()
{
    return this->stable;
}
