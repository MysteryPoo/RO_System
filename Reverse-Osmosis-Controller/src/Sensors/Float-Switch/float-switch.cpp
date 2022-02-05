
#include "float-switch.h"
#include "system-log.h"
#include "mqtt-client.h"

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
    this->fireConfigurationMessage();
}

FloatSwitch::FloatSwitch(int pin, SystemLog &logger, MQTTClient* mqtt) : FloatSwitch(pin, logger)
{
    if (nullptr != mqtt)
    {
        mqtt->RegisterCallbackListener(this);
    }
}

int FloatSwitch::ResetReliableFlag(String reset)
{
    this->isReliable = true;
    return 0;
}

void FloatSwitch::Configure(JSONValue json)
{
    JSONObjectIterator jsonIt(json);
    while(jsonIt.next())
    {
#ifdef TESTING
        if(jsonIt.name() == "float")
        {
            this->setStatus(jsonIt.value().toBool());
        }
#endif
    }
}

void FloatSwitch::Callback(char* topic, uint8_t* payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = '\0';

    if (strcmp(topic, "to/" + System.deviceID() + "/float-switch"))
    {
        return;
    }

    JSONValue configuration = JSONValue::parseCopy(String(p));
    if (!configuration.isValid())
    {
        return;
    }
    JSONObjectIterator it(configuration);
    while(it.next())
    {
#ifdef TESTING
        if(it.name() == "float")
        {
            this->setStatus(it.value().toBool());
        }
#endif
    }

}

void FloatSwitch::OnConnect(bool success, MQTTClient* mqtt)
{
    if (nullptr != mqtt)
    {
        mqtt->Subscribe("to/" + System.deviceID() + "/float-switch/#", MQTT::EMQTT_QOS::QOS1);
    }
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
        if(curMillis > this->lastStable + TIME_CONSIDERED_UNSTABLE_MS)
        {
            this->isReliable = false;
            this->logger.error("Float switch is highly unstable. System shutdown.");
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
}

void FloatSwitch::fireConfigurationMessage() const
{
#ifndef TESTING
    int pinValue = this->pin;
#else
    String pinValue("Simulated");
#endif
    JSONBufferWriter writer = SystemLog::createBuffer(256);
    writer.beginObject();
    writer.name("event").value("configuration");
    writer.name("pin").value(pinValue);
    writer.endObject();
    this->logger.pushMessage(COMPONENT_NAME, writer.buffer());
}

void FloatSwitch::reportHeartbeat(JSONBufferWriter& writer) const
{
    writer.name(COMPONENT_NAME).value(this->isFull());
}

#ifdef TESTING
void FloatSwitch::setStatus(bool status)
{
    this->status = status;

    JSONBufferWriter writer = SystemLog::createBuffer(256);
    writer.beginObject();
    writer.name("status").value(status);
    writer.endObject();
    this->logger.pushMessage("float-switch", writer.buffer());
}
#endif
