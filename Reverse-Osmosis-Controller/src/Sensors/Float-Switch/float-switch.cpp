
#include "global-defines.h"
#include "float-switch.h"
#include "spark_wiring_string.h"
#include "spark_wiring_ticks.h"
#include "system-log.h"

#define TIME_CONSIDERED_STABLE_MS 1000
#define TIME_CONSIDERED_UNSTABLE_MS 10000
#define COMPONENT_NAME "float-switch"

FloatSwitch::FloatSwitch(int pin, SystemLog &logger) :
    pin(pin),
    stableTimer(millis() + 1000),
    lastStatus(false),
    status(false),
    lastStable(millis()),
    stable(false),
    firedWarning(false),
    isReliable(true),
    logger(logger)
{
#ifndef TESTING
    pinMode(pin, INPUT_PULLDOWN);
#endif
}

void FloatSwitch::Update()
{
    this->sample();
    // Notify if the float switch was triggered.
    if(this->isFull() && !this->firedWarning)
    {
        logger.warning("Float Switch has been triggered!");
        this->firedWarning = true;
    }
    // Reset warning
    if (this->stable && !this->status)
    {
        this->firedWarning = false;
    }
}

bool FloatSwitch::isFull() const
{
    return (this->stable && this->status) || !isReliable;
}

void FloatSwitch::sample()
{
#ifndef TESTING
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
        if(curMillis - this->lastStable >= TIME_CONSIDERED_UNSTABLE_MS)
        {
            this->isReliable = false;
            this->logger.error("Float switch is highly unstable.");
        }
    }
    else if(curMillis > this->stableTimer)
    {
        this->stable = true;
        this->lastStable = curMillis;
    }
#else
    this->stable = true;
#endif
    this->Notify();
}

const String FloatSwitch::GetName() const
{
    return COMPONENT_NAME;
}
