
#include "ROSystem.h"
#include "application.h"
#include "Sensors/ISensor.h"
#include "relay.h"
#include "system-log.h"
#include "mqtt-client.h"
#include "mqtt-queue.h"

#define FLUSH_TIMER_MS 300000
#define PUMP_INLET_DELAY_MS 5000
#define PUMP_FREQUENCY_MS 500000 // Default
#define FILL_START_DISTANCE_CM 160
#define FILL_STOP_DISTANCE_CM 40
#define PUMP_RUN_MIN_TO_FLUSH 2
#define WARNING_DELAY 60000
#define COMPONENT_NAME "ro-system"

ROSystem::ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger, MqttQueue& mqttQueue) :
    state(ROSystem::BOOT),
    pump(pump),
    inlet(inlet),
    flush(flush),
    logger(logger),
    mqttQueue(mqttQueue),
    flushedToday(false),
    totalPumpTime(0),
    totalPumpRuns(0),
    lastPumpTime(0),
    flushStartedTime(0),
    flushDuration(FLUSH_TIMER_MS),
    pumpRunTime(0),
    firstPump(true),
    enabled(true),
    fillStartDistance(FILL_START_DISTANCE_CM),
    fillStopDistance(FILL_STOP_DISTANCE_CM),
    pumpCooldown(PUMP_FREQUENCY_MS)
{
    
}

ROSystem::ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger, MQTTClient* mqtt, MqttQueue& mqttQueue) :
    ROSystem(pump, inlet, flush, logger, mqttQueue)
{
    if (nullptr != mqtt)
    {
        mqtt->RegisterCallbackListener(this);
        this->mqttClient = mqtt;
    }
}

void ROSystem::AddSensor(ISensor* sensor)
{
    sensors.push_back(sensor);
}

void ROSystem::Update()
{
    bool tankIsFull = false;
    for (ISensor* sensor : sensors)
    {
        tankIsFull |= sensor->isFull();
    }
    this->update(tankIsFull);
}

void ROSystem::ReportHeartbeat(JSONBufferWriter& writer) const
{
    writer.name(COMPONENT_NAME).beginObject();
    writer.name("enabled").value(getEnabled());
    writer.name("Total Pump Time").value(this->totalPumpTime);
    writer.endObject();
}

void ROSystem::Configure(JSONValue json)
{
    if (!json.isValid())
    {
        return;
    }
    JSONObjectIterator jsonIt(json);
    while(jsonIt.next())
    {
        if(jsonIt.name() == "enabled")
        {
            this->enabled = jsonIt.value().toBool();
        }
        if(jsonIt.name() == "fillDistances")
        {
            JSONValue distances = jsonIt.value();
            this->configureFillDistances(distances);
        }
        if(jsonIt.name() == "Pump Cooldown")
        {
            this->configurePumpCooldown(jsonIt.value().toInt());
        }
        if(jsonIt.name() == "Flush Duration")
        {
            this->configureFlushDuration(jsonIt.value().toInt());
        }
    }
}

void ROSystem::Callback(char* topic, uint8_t* payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = '\0';

    if (strcmp(topic, "to/" + System.deviceID() + "/ro-system/configuration"))
    {
        return;
    }

    JSONValue configuration = JSONValue::parseCopy(String(p));
    this->Configure(configuration);
}

void ROSystem::OnConnect(bool connectSuccess, MQTTClient* mqtt)
{
    if (nullptr != mqtt)
    {
        mqtt->Subscribe("ro-system/configuration", MQTT::EMQTT_QOS::QOS1);
        JSONBufferWriter message = SystemLog::createBuffer(512);
        message.beginObject();
        message.name("display").value("RO System");
        message.name("description").value("The component responsible for maintaining water level.");
        message.name("options").beginArray()
        .beginObject()
        .name("name").value("enabled")
        .name("type").value("boolean")
        .name("default").value(true)
        .endObject()
        .beginObject()
        .name("name").value("Pump Cooldown")
        .name("type").value("number")
        .name("units").value("millisecond")
        .name("default").value(PUMP_FREQUENCY_MS)
        .endObject()
        .beginObject()
        .name("name").value("Flush Duration")
        .name("type").value("number")
        .name("units").value("millisecond")
        .name("default").value(FLUSH_TIMER_MS)
        .endObject()
        .endArray();
        message.endObject();
        //mqtt->Publish("configuration/ro-system", message.buffer());
        mqttQueue.PushPayload("configuration/" COMPONENT_NAME, message.buffer());
        delete[] message.buffer();
    }
}

void ROSystem::update(bool tankFull)
{
    unsigned long curMillis = millis();
    static unsigned long lastAttempt = curMillis;

    switch(this->state)
    {
        case ROSystem::BOOT:
            requestState(ROSystem::State::IDLE, "Initial boot.");
            break;
        case ROSystem::IDLE:
            if(curMillis < lastAttempt + WARNING_DELAY)
            {
                return; // Bail out early if we're checking inside Idle too often.
            }
            // Check if the tank is considered not-full
            if(!tankFull)
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
            if(!this->enabled || tankFull || curMillis > this->flushStartedTime + this->flushDuration)
            {
                this->flushedToday = true;
                if (!tankFull)
                {
                    this->requestState(ROSystem::State::FILL, this->enabled ? "Flush complete. Completing fill." : "System has been disabled.");
                }
                else
                {
                    this->requestState(ROSystem::State::IDLE, this->enabled ? "Flush complete." : "System has been disabled.");
                }
                lastAttempt = curMillis;
            }
            break;
        case ROSystem::FILL:
            // Cutoff fill routine once full
            if(!this->enabled || tankFull)
            {
                this->requestState(ROSystem::State::IDLE, this->enabled ? "Tank is full." : "System has been disabled.");
                lastAttempt = curMillis;
            }
            break;
        default:
            logger.error("ROSystem is in an invalid state!");
    }
}

void ROSystem::requestState(ROSystem::State newState, const char* requestReason)
{
    String reason = String(requestReason);
    this->requestState(newState, reason);
}

void ROSystem::requestState(ROSystem::State newState, String requestReason)
{
    String error;

    bool isAlreadyInState = newState == this->state;

    JSONBufferWriter writer = SystemLog::createBuffer(2048);
    writer.beginObject()
    .name("datetime").value(Particle.connected() ? Time.now() : 0);
    switch(newState)
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
            writer.name("state").value("IDLE");
            break;
        case ROSystem::FLUSH:
            error = "";
            if(this->enabled)
            {
                if(activatePump())
                {
                    flush.set(Relay::State::ON);
                    this->flushStartedTime = millis();
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
            writer.name("state").value("FLUSH");
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
            writer.name("state").value("FILL");
            break;
        default:
            logger.error("ROSystem was requested to enter an invalid state!");
            writer.name("state").value(this->getStateString());
            writer.name("success").value(false);
    }

    if (isAlreadyInState)
    {
        error = "System attempted to change into state it was already in.";
    }
    writer.name("success").value(!isAlreadyInState && this->state == newState);
    writer.name("requestReason").value(requestReason);
    writer.name("failureReason").value(error);
    writer.endObject();
    this->mqttQueue.PushPayload(COMPONENT_NAME "/state-request", writer.buffer());
    delete[] writer.buffer();
}

bool ROSystem::activatePump()
{
    unsigned long curMillis = millis();
    static unsigned long lastWarning = curMillis;
    
    if(this->pump.get() == Relay::State::OFF || this->firstPump || this->lastPumpTime + this->pumpCooldown < curMillis)
    {
        SINGLE_THREADED_BLOCK()
        {
            this->inlet.set(Relay::State::ON);
            delay(PUMP_INLET_DELAY_MS);

            curMillis = millis();
            this->pumpRunTime = curMillis;

            this->pump.set(Relay::State::ON);
        }
        ++this->totalPumpRuns;
        this->firstPump = false;
        return true;
    }
    else if(Relay::State::ON == this->pump.get() && Relay::State::ON == this->inlet.get())
    {
        return true;
    }
    else
    {
        if(curMillis > lastWarning + WARNING_DELAY)
        {
            this->logger.warning("Attempted to activate the pump too frequently.");
            lastWarning = curMillis;
        }
        return false;
    }
}

bool ROSystem::deactivatePump()
{
    unsigned long curMillis = millis();

    if(Relay::State::ON == pump.get())
    {
        this->totalPumpTime += curMillis - this->pumpRunTime;
        this->lastPumpTime = curMillis;

        SINGLE_THREADED_BLOCK()
        {
            this->pump.set(Relay::State::OFF);
            delay(PUMP_INLET_DELAY_MS);
            this->inlet.set(Relay::State::OFF);
        }
    }
    return true;
}

void ROSystem::shutdown()
{
    this->requestState(ROSystem::State::IDLE, "Shutdown requested.");
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

#ifdef TESTING
int ROSystem::configureState(String newState)
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
#endif

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

int ROSystem::configureFillDistances(spark::JSONValue& distances)
{
    int error = 0;
    if(distances.isObject())
    {
        JSONObjectIterator fillDistances(distances);
        int start = -1, stop = -1;
        while(fillDistances.next())
        {
            if(fillDistances.name() == "start")
            {
                start = fillDistances.value().toInt();
            }
            if(fillDistances.name() == "stop")
            {
                stop = fillDistances.value().toInt();
            }
        }
        if(start != -1 && stop != -1)
        {
            this->setFillDistances(String(start) + String(',') + String(stop));
        }
        else
        {
            error = 1;
        }
    }
    else
    {
        error = 1;
    }

    return error;
}

void ROSystem::configurePumpCooldown(int newPumpCooldown)
{
    if(newPumpCooldown > 0)
    {
        this->pumpCooldown = (unsigned int) newPumpCooldown;
    }
}

void ROSystem::configureFlushDuration(int duration)
{
    if(duration > 0)
    {
        this->flushDuration = (unsigned int) duration;
    }
}

String ROSystem::GetName() const
{
    return COMPONENT_NAME;
}
