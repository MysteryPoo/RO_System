#include "spark_wiring_string.h"
#include "spark_wiring_ticks.h"
#include "spark_wiring_thread.h"
#include "ROSystem.h"
#include "Relay/relay.h"
#include "system-log.h"
#include "Sensors/AbstractSensor.h"
#include "ObserverPattern/MessageType.h"
#include "Messages/FlushedMsg.h"
#include "Messages/StateChangeMessage.h"

#include "RoSystemEnum.h"
#include "Relay/RelayEnums.h"
#include "RoSystemConstants.h"

ROSystem::ROSystem(Relay& pump, Relay& inlet, Relay& flush, SystemLog& logger) :
    state(RoSystemEnum::State::BOOT),
    pump(pump),
    inlet(inlet),
    flush(flush),
    logger(logger),
    flushedToday(false),
    totalPumpTime(0),
    totalPumpRuns(0),
    lastPumpTime(-PUMP_FREQUENCY_MS),
    flushStartedTime(0),
    flushDuration(FLUSH_TIMER_MS),
    pumpRunTime(0),
    enabled(true),
    fillStartDistance(FILL_START_DISTANCE_CM),
    fillStopDistance(FILL_STOP_DISTANCE_CM),
    pumpCooldown(PUMP_FREQUENCY_MS)
{ 
}

void ROSystem::AddSensor(AbstractSensor* sensor)
{
    sensors.push_back(sensor);
}

void ROSystem::Update()
{
    bool tankIsFull = sensors.size() == 0;
    for (AbstractSensor* sensor : sensors)
    {
        tankIsFull |= sensor->isFull();
    }
    this->update(tankIsFull);
}

const String ROSystem::GetName() const
{
    return COMPONENT_NAME;
}

void ROSystem::update(bool tankFull)
{
    unsigned long curMillis = millis();

    switch(this->state)
    {
        case RoSystemEnum::State::BOOT:
            requestState(RoSystemEnum::State::IDLE, "Initial boot.");
            break;
        case RoSystemEnum::State::IDLE:
            if(!this->isPumpReady())
            {
                return; // Bail out early if we're checking inside Idle too often.
            }
            // Check if the tank is considered not-full
            if(!tankFull)
            {
                // Only flush if we've ran the pump a few times
                if(flushedToday || this->totalPumpRuns < PUMP_RUN_MIN_TO_FLUSH)
                {
                    this->requestState(RoSystemEnum::State::FILL, "Tank is not full.");
                }
                else
                {
                    this->requestState(RoSystemEnum::State::FLUSH, "Conditions met.");
                }
            }
            break;
        case RoSystemEnum::State::FLUSH:
            // End flushing routine after designated time
            if(!this->enabled || tankFull || curMillis - this->flushStartedTime >= this->flushDuration)
            {
                this->flushedToday = true;
                if (!tankFull)
                {
                    this->requestState(RoSystemEnum::State::FILL, this->enabled ? "Flush complete. Completing fill." : "System has been disabled.");
                }
                else
                {
                    this->requestState(RoSystemEnum::State::IDLE, this->enabled ? "Flush complete." : "System has been disabled.");
                }
            }
            break;
        case RoSystemEnum::State::FILL:
            // Cutoff fill routine once full
            if(!this->enabled || tankFull)
            {
                this->requestState(RoSystemEnum::State::IDLE, this->enabled ? "Tank is full." : "System has been disabled.");
            }
            break;
        default:
            logger.error("ROSystem is in an invalid state!");
    }
}

void ROSystem::requestState(RoSystemEnum::State newState, const char* requestReason)
{
    String reason = String(requestReason);
    this->requestState(newState, reason);
}

void ROSystem::requestState(RoSystemEnum::State newState, String requestReason)
{
    String error;

    bool isAlreadyInState = newState == this->state;

    switch(newState)
    {
        case RoSystemEnum::State::IDLE:
            error = "";
            flush.set(RelayEnums::State::OFF);
            if(deactivatePump())
            {
                this->state = RoSystemEnum::State::IDLE;
            }
            else
            {
                error = "Failed to deactivate the pump.";
            }
            break;
        case RoSystemEnum::State::FLUSH:
            error = "";
            if(this->enabled)
            {
                if(activatePump())
                {
                    this->flush.set(RelayEnums::State::ON);
                    this->flushStartedTime = millis();
                    this->state = RoSystemEnum::State::FLUSH;
                    RoSystemMessage::Flushed message(true);
                    this->Notify(MessageType::ROSYSTEM_FLUSHED_MSG, &message);
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
            break;
        case RoSystemEnum::State::FILL:
            error = "";
            if(this->enabled)
            {
                if(activatePump())
                {
                    this->flush.set(RelayEnums::State::OFF);
                    this->state = RoSystemEnum::State::FILL;
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
            break;
        default:
            error = "ROSystem was requested to enter an invalid state!";
    }

    if (isAlreadyInState)
    {
        error = "System attempted to change into state it was already in.";
    }
    RoSystemMessage::StateChange message(newState, !isAlreadyInState && this->state == newState, requestReason.c_str(), error.c_str());
    this->Notify(MessageType::ROSYSTEM_STATE_MSG, &message);
}

bool ROSystem::activatePump()
{
    unsigned long curMillis = millis();
    static unsigned long lastWarning = curMillis;
    
    if(this->pump.GetState() == RelayEnums::State::OFF)
    {
        if (!this->isPumpReady()) {
            if (curMillis - lastWarning >= WARNING_DELAY)
            {
                this->logger.warning("Attempted to activate the pump during cooldown period.");
                lastWarning = curMillis;
            }
            return false;
        }
        SINGLE_THREADED_BLOCK()
        {
            this->inlet.set(RelayEnums::State::ON);
            delay(PUMP_INLET_DELAY_MS);

            curMillis = millis();
            this->pumpRunTime = curMillis;

            this->pump.set(RelayEnums::State::ON);
        }
        ++this->totalPumpRuns;
        return true;
    }
    else if(/*Relay::State::ON == this->pump.GetState() && */RelayEnums::State::ON == this->inlet.GetState())
    {
        return true;
    }
    else
    {
        this->deactivatePump();
        this->logger.error("Pump was on, but inlet was closed. Shutting down pump.");
        return false;
    }
}

bool ROSystem::deactivatePump()
{
    unsigned long curMillis = millis();

    if(RelayEnums::State::ON == pump.GetState())
    {
        this->totalPumpTime += curMillis - this->pumpRunTime;
        this->lastPumpTime = curMillis;

        SINGLE_THREADED_BLOCK()
        {
            this->pump.set(RelayEnums::State::OFF);
            delay(PUMP_INLET_DELAY_MS);
            this->inlet.set(RelayEnums::State::OFF);
        }
    }
    return true;
}

bool ROSystem::isPumpReady() const
{
    return millis() - this->lastPumpTime >= this->pumpCooldown;
}

void ROSystem::Shutdown()
{
    this->requestState(RoSystemEnum::State::IDLE, "Shutdown requested.");
}
