
#include "ROSystem.h"
#include "application.h"
#include "relay.h"
#include "system-log.h"
#include "json.h"

#define FLUSH_TIMER_MS 300000
#define PUMP_INLET_DELAY_MS 5000
#define PUMP_FREQUENCY_MS 500000
#define FILL_START_DISTANCE_CM 80
#define FILL_STOP_DISTANCE_CM 20
#define PUMP_RUN_MIN_TO_FLUSH 2
#define WARNING_DELAY 60000

// TODO Make fill distances cloud configurable

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
    pumpRunTime(0),
    enabled(true),
    fillStartDistance(80),
    fillStopDistance(20)
{
    
}

void ROSystem::cloudSetup()
{
    Particle.variable("Pump-Runs", this->totalPumpRuns);
    Particle.variable("Pump-Time", this->totalPumpTime);
    Particle.function("requestState", &ROSystem::cloudRequestState, this);
    Particle.function("setEnable", &ROSystem::enable, this);
    Particle.function("setFillDistances", &ROSystem::setFillDistances, this);
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
            if(!tankFull && distance > this->fillStartDistance)
            {
                // Only flush if we've ran the pump a few times
                if(flushedToday || this->totalPumpRuns < PUMP_RUN_MIN_TO_FLUSH)
                {
                    this->requestState(ROSystem::State::FILL, "Tank is not full.");
                }
                else
                {
                    this->requestState(ROSystem::State::FLUSH, "Conditions met.");
                }
                lastAttempt = curMillis;
            }
            break;
        case ROSystem::FLUSH:
            // End flushing routine after designated time
            if(tankFull || curMillis > this->flushDelay)
            {
                this->flushedToday = true;
                this->requestState(ROSystem::State::IDLE, "Flush complete.");
                lastAttempt = curMillis;
            }
            break;
        case ROSystem::FILL:
            // Cutoff fill routine once full
            if(!this->enabled || tankFull || distance < this->fillStopDistance)
            {
                this->requestState(ROSystem::State::IDLE, this->enabled ? "Tank is full." : "System has been disabled.");
                lastAttempt = curMillis;
            }
            break;
        default:
            logger.error("ROSystem is in an invalid state!");
    }
}

void ROSystem::requestState(ROSystem::State state, const char* requestReason)
{
    String reason = String(requestReason);
    this->requestState(state, reason);
}

void ROSystem::requestState(ROSystem::State state, String requestReason)
{
    String error;
    String message;
    switch(state)
    {
        case ROSystem::IDLE:
            error = "";
            flush.set(Relay::State::OFF);
            if(deactivatePump())
            {
                this->state = ROSystem::State::IDLE;
            }
            else
            {
                error = "Failed to deactivate the pump.";
            }
            message = JHelp::begin();
            message += JHelp::field("event", "state-request");
            message += JHelp::next();
            message += JHelp::field("state", "IDLE");
            message += JHelp::next();
            message += JHelp::field("success", this->state == ROSystem::State::IDLE ? true : false);
            message += JHelp::next();
            message += JHelp::field("requestReason", requestReason);
            message += JHelp::next();
            message += JHelp::field("failureReason", error);
            message += JHelp::end();
            logger.pushMessage("system/state-request", message);
            break;
        case ROSystem::FLUSH:
            error = "";
            if(this->enabled)
            {
                if(activatePump())
                {
                    flush.set(Relay::State::ON);
                    this->flushDelay = millis() + FLUSH_TIMER_MS;
                    this->state = ROSystem::State::FLUSH;
                }
                else
                {
                    error = "Failed to activate the pump.";
                }
            }
            else
            {
                error = "System is currently disabled.";
            }
            message = JHelp::begin();
            message += JHelp::field("event", "state-request");
            message += JHelp::next();
            message += JHelp::field("state", "FLUSH");
            message += JHelp::next();
            message += JHelp::field("success", this->state == ROSystem::State::FLUSH ? true : false);
            message += JHelp::next();
            message += JHelp::field("requestReason", requestReason);
            message += JHelp::next();
            message += JHelp::field("failureReason", error);
            message += JHelp::end();
            logger.pushMessage("system/state-request", message);
            break;
        case ROSystem::FILL:
            error = "";
            if(this->enabled)
            {
                if(activatePump())
                {
                    this->state = ROSystem::State::FILL;
                }
                else
                {
                    error = "Failed to activate the pump.";
                }
            }
            else
            {
                error = "System is currently disabled.";
            }
            message = JHelp::begin();
            message += JHelp::field("event", "state-request");
            message += JHelp::next();
            message += JHelp::field("state", "FILL");
            message += JHelp::next();
            message += JHelp::field("success", this->state == ROSystem::State::FILL ? true : false);
            message += JHelp::next();
            message += JHelp::field("requestReason", requestReason);
            message += JHelp::next();
            message += JHelp::field("failureReason", error);
            message += JHelp::end();
            logger.pushMessage("system/state-request", message);
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
    this->requestState(ROSystem::State::IDLE, "Shutdown requested.");
}

int ROSystem::cloudRequestState(String newState)
{
    const String reasonCloud = "DEBUG Cloud Requested.";
    if(newState.toLowerCase() == "fill")
    {
        this->requestState(ROSystem::State::FILL, reasonCloud);
    }
    if(newState.toLowerCase() == "flush")
    {
        this->requestState(ROSystem::State::FLUSH, reasonCloud);
    }if(newState.toLowerCase() == "idle")
    {
        this->requestState(ROSystem::State::IDLE, reasonCloud);
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

int ROSystem::enable(String setEnabled)
{
    if(setEnabled.toUpperCase() == "TRUE")
    {
        this->enabled = true;
    }
    else if(setEnabled.toUpperCase() == "FALSE")
    {
        this->enabled = false;
    }
    else
    {
        return -1;
    }
    return 0;
}

int ROSystem::setFillDistances(String csvFill)
{
    int indexOfComma = csvFill.indexOf(',');
    if(indexOfComma != -1)
    {
        String fillStartString = csvFill.substring(0, indexOfComma);
        String fillStopString = csvFill.substring(indexOfComma + 1);
		int fillStart = fillStartString.toInt();
		int fillStop = fillStopString.toInt();
		if (fillStart == 0 || fillStop == 0)
		{
			return -1;
		}
		else
		{
			this->fillStartDistance = fillStart;
			this->fillStopDistance = fillStop;
			return 0;
		}
    }

    return -1;
}
