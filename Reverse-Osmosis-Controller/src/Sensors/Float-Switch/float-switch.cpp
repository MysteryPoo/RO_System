
#include "global-defines.h"
#include "float-switch.h"
#include "system-log.h"
#include "mqtt-client.h"
#include "mqtt-queue.h"

#define TIME_CONSIDERED_STABLE_MS 1000
#define TIME_CONSIDERED_UNSTABLE_MS 10000
#define COMPONENT_NAME "float-switch"

FloatSwitch::FloatSwitch(int pin, SystemLog &logger, MqttQueue& mqttQueue) :
    pin(pin),
    stableTimer(millis() + 1000),
    lastStatus(false),
    status(false),
    lastStable(millis()),
    stable(false),
    firedWarning(false),
    isReliable(true),
    logger(logger),
    mqttQueue(mqttQueue)
{
#ifndef TESTING
    pinMode(pin, INPUT_PULLDOWN);
#endif
    this->fireConfigurationMessage();
}

FloatSwitch::FloatSwitch(int pin, SystemLog &logger, MQTTClient* mqtt, MqttQueue& mqttQueue)
: FloatSwitch(pin, logger, mqttQueue)
{
    if (nullptr != mqtt)
    {
        mqtt->RegisterCallbackListener(this);
        this->mqttClient = mqtt;
    }
}

void FloatSwitch::Configure(JSONValue json)
{
    if (!json.isValid())
    {
        return;
    }
    JSONObjectIterator it(json);
    while(it.next())
    {
#ifdef TESTING
        if (it.name() == "float")
        {
            this->setStatus(it.value().toBool());
        }
#endif
        if (it.name() == "Reset Reliable")
        {
            this->isReliable = it.value().toBool();
            logger.trace("Float Switch Reliable flag reset!");
        }
    }
}

void FloatSwitch::Callback(char* topic, uint8_t* payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = '\0';

    if (strcmp(topic, "to/" + System.deviceID() + "/" COMPONENT_NAME "/configuration"))
    {
        return;
    }

    JSONValue configuration = JSONValue::parseCopy(String(p));
    this->Configure(configuration);

}

void FloatSwitch::OnConnect(bool success, MQTTClient* mqtt)
{
    if (nullptr != mqtt)
    {
        mqtt->Subscribe(COMPONENT_NAME "/configuration", MQTT::EMQTT_QOS::QOS1);
        JSONBufferWriter message = SystemLog::createBuffer(512);
        message.beginObject();
        message.name("display").value("Float Switch");
        message.name("description").value("On when the tank is full. Off when not.");
        message.name("options").beginArray()
#ifdef TESTING
        .beginObject()
        .name("name").value("float")
        .name("type").value("boolean")
        .endObject()
#endif
        .beginObject()
        .name("name").value("Reset Reliable")
        .name("type").value("trigger")
        .endObject()
        .endArray();
        message.endObject();
        //mqtt->Publish("configuration/" COMPONENT_NAME, message.buffer());
        mqttQueue.PushPayload("configuration/" COMPONENT_NAME, message.buffer());
        delete[] message.buffer();
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
    if (this->mqttClient == nullptr) return;
#ifndef TESTING
    int pinValue = this->pin;
#else
    String pinValue("Simulated");
#endif
    JSONBufferWriter writer = SystemLog::createBuffer(128);
    writer.beginObject();
    writer.name("pin").value(pinValue);
    writer.endObject();
    this->mqttQueue.PushPayload(COMPONENT_NAME "/pin", writer.buffer());
    delete[] writer.buffer();
}

void FloatSwitch::ReportHeartbeat(JSONBufferWriter& writer) const
{
    writer.name(COMPONENT_NAME).beginObject()
    .name("float").value(this->isFull())
    .name("reliable").value(this->isReliable)
    .endObject();
}

String FloatSwitch::GetName() const
{
    return COMPONENT_NAME;
}

#ifdef TESTING
void FloatSwitch::setStatus(bool status)
{
    this->status = status;

    JSONBufferWriter writer = SystemLog::createBuffer(256);
    writer.beginObject();
    writer.name("status").value(status);
    writer.endObject();
    this->logger.pushMessage(COMPONENT_NAME, writer.buffer());
}
#endif
