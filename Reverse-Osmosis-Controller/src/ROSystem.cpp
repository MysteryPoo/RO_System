
#include "ROSystem.h"
#include "application.h"
#include "relay.h"
#include "system-log.h"

#define FLUSH_TIMER_MS 300000
#define PUMP_INLET_DELAY_MS 5000
#define PUMP_FREQUENCY_MS 500000
#define REFILL_DISTANCE_CM 100
#define PUMP_RUN_MIN_TO_FLUSH 2
#define WARNING_DELAY 60000

ROSystem::ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger) :
    state(ROSystem::IDLE),
    pump(pump),
    inlet(inlet),
    flush(flush),
    logger(logger),
    flushedToday(false),
    totalPumpTime(0),
    totalPumpRuns(0),
    nextPumpTime(millis()),
    flushDelay(0),
    pumpRunTime(0)
{
    
}

void ROSystem::cloudSetup()
{
    Particle.variable("Pump-Runs", this->totalPumpRuns);
    Particle.variable("Pump-Time", this->totalPumpTime);
    Particle.function("requestState", &ROSystem::cloudRequestState, this);
}

void ROSystem::update(bool tankFull, unsigned short distance)
{
    unsigned long curMillis = millis();
    static unsigned long lastAttempt = curMillis;

    if(curMillis < lastAttempt + WARNING_DELAY)
    {
        return;
    }
    
    switch(this->state)
    {
        case ROSystem::IDLE:
            // Check if the tank is considered not-full and it has plenty of room to fill some
            if(!tankFull && distance > REFILL_DISTANCE_CM)
            {
                // Only flush if we've ran the pump a few times
                if(flushedToday || this->totalPumpRuns < PUMP_RUN_MIN_TO_FLUSH)
                {
                    this->requestState(ROSystem::State::FILL);
                }
                else
                {
                    this->requestState(ROSystem::State::FLUSH);
                }
                lastAttempt = curMillis;
            }
            break;
        case ROSystem::FLUSH:
            // End flushing routine after designated time
            if(curMillis > this->flushDelay)
            {
                this->flushedToday = true;
                this->requestState(ROSystem::State::IDLE);
                lastAttempt = curMillis;
            }
            break;
        case ROSystem::FILL:
            // Cutoff fill routine once full
            if(tankFull)
            {
                this->requestState(ROSystem::State::IDLE);
                lastAttempt = curMillis;
            }
            break;
        default:
            logger.error("ROSystem is in an invalid state!");
    }
}

void ROSystem::requestState(ROSystem::State state)
{
    switch(state)
    {
        case ROSystem::IDLE:
            flush.set(Relay::State::OFF);
            if(deactivatePump())
            {
                this->state = ROSystem::State::IDLE;
            }
            logger.pushMessage("system/state-request", "{\"event\":\"state-request\",\"state\":\"IDLE\", \"success\":" + String(this->state == ROSystem::State::IDLE ? "true" : "false") + "}");
            break;
        case ROSystem::FLUSH:
            if(activatePump())
            {
                flush.set(Relay::State::ON);
                this->flushDelay = millis() + FLUSH_TIMER_MS;
                this->state = ROSystem::State::FLUSH;
            }
            logger.pushMessage("system/state-request", "{\"event\":\"state-request\",\"state\":\"FLUSH\", \"success\":" + String(this->state == ROSystem::State::FLUSH ? "true" : "false") + "}");
            break;
        case ROSystem::FILL:
            if(activatePump())
            {
                this->state = ROSystem::State::FILL;
            }
            logger.pushMessage("system/state-request", "{\"event\":\"state-request\",\"state\":\"FILL\", \"success\":" + String(this->state == ROSystem::State::FILL ? "true" : "false") + "}");
            break;
        default:
            logger.error("ROSystem was requested to enter an invalid state!");
    }
}

bool ROSystem::activatePump()
{
    unsigned long curMillis = millis();
    static unsigned long lastWarning = curMillis;
    
    if(curMillis > this->nextPumpTime)
    {
        this->inlet.set(Relay::State::ON);
        delay(PUMP_INLET_DELAY_MS);

        curMillis = millis();
        this->pumpRunTime = curMillis;

        this->pump.set(Relay::State::ON);
        ++this->totalPumpRuns;
        return true;
    }
    else if(curMillis > lastWarning + WARNING_DELAY)
    {
        this->logger.warning("Attempting to activate the pump too frequently.");
        lastWarning = curMillis;
        return false;
    }
    else
    {
        return false;
    }
}

bool ROSystem::deactivatePump()
{
    unsigned long curMillis = millis();
    static unsigned long lastWarning = curMillis - WARNING_DELAY;

    if(Relay::State::ON == pump.get())
    {
        this->totalPumpTime += curMillis - this->pumpRunTime;
        this->nextPumpTime = curMillis + PUMP_FREQUENCY_MS;

        this->pump.set(Relay::State::OFF);
        delay(PUMP_INLET_DELAY_MS);
        this->inlet.set(Relay::State::OFF);
        
        return true;
    }
    else if(curMillis > lastWarning + WARNING_DELAY)
    {
        this->logger.warning("Attempting to deactivate the pump when it is already off.");
        lastWarning = curMillis;
        return false;
    }
    else
    {
        return false;
    }
}

void ROSystem::shutdown()
{
    this->requestState(ROSystem::State::IDLE);
}

int ROSystem::cloudRequestState(String newState)
{
    if(newState == "fill")
    {
        this->requestState(ROSystem::State::FILL);
    }
    if(newState == "flush")
    {
        this->requestState(ROSystem::State::FLUSH);
    }if(newState == "idle")
    {
        this->requestState(ROSystem::State::IDLE);
    }

    return 0;
}

String ROSystem::getStateString()
{
    String stateString;
    switch(this->state)
    {
        case ROSystem::State::IDLE:
            stateString = "IDLE";
            break;
        case ROSystem::State::FILL:
            stateString = "FILL";
            break;
        case ROSystem::State::FLUSH:
            stateString = "FLUSH";
            break;
        default:
            stateString = "INVALID";
    }
    return stateString;
}
