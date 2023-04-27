/*
#include "global-defines.h"
#include "ultra-sonic.h"
#include "application.h"
#include "system-log.h"

#define US_TRIGGER_TIME 11
#define US_INTERVAL_TIME 120000
#define US_SAMPLE_COUNT 3
#define CM_PER_MICROSECOND 0.0343f

UltraSonic::UltraSonic(int trig, int echo, SystemLog &logger) :
    triggerPin(trig),
    echoPin(echo),
    distance(0),
    cooldownPeriod(0),
    logger(logger)
{
#ifndef TESTING
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT_PULLDOWN);
    
    digitalWrite(trig, LOW); // Start low
#endif
    samplingUltraSonicStatus = new LEDStatus(RGB_COLOR_ORANGE);
    this->fireConfigurationMessage();
}

UltraSonic::~UltraSonic()
{
    delete samplingUltraSonicStatus;
}

void UltraSonic::Update()
{
    this->sample();
}

void UltraSonic::Configure(JSONValue json)
{
    JSONObjectIterator jsonIt(json);
    while(jsonIt.next())
    {
#ifdef TESTING
        if(jsonIt.name() == "distance")
        {
            this->setDistance(jsonIt.value().toInt());
        }
#endif
    }
}

void UltraSonic::sample()
{
#ifndef TESTING
	unsigned long curMillis = millis();
    if(curMillis < this->cooldownPeriod) return; // Early bail out, we're still on cooldown
    
    unsigned long sampleTotal = 0ul;
    unsigned long validReadings = 0ul;
    for (int sample = 0; sample < US_SAMPLE_COUNT; ++sample)
    {
        ApplicationWatchdog::checkin();
        
        SINGLE_THREADED_BLOCK()
        {
            samplingUltraSonicStatus->setActive();
            digitalWrite(this->triggerPin, HIGH); // Start trigger
            delayMicroseconds(US_TRIGGER_TIME);
            digitalWrite(this->triggerPin, LOW); // End trigger
            
            unsigned long thisReading = pulseIn(this->echoPin, HIGH);

            if(thisReading != 0)
            {
                sampleTotal += thisReading;
                validReadings += 1ul;
            }
            samplingUltraSonicStatus->setActive(false);
        }
    }

    if(validReadings > 0)
    {
        unsigned long averageSample = sampleTotal / validReadings;
        unsigned long averageSampleOneWay = averageSample / 2ul;
        unsigned long convertedToCM = (unsigned long)((float)averageSampleOneWay * CM_PER_MICROSECOND);
        this->distance = convertedToCM;
    }
    else
    {
        // TODO: If this is a normal occurrence, remove this log message to reduce spam
        this->logger.warning("Ultra Sonic sensor failed to read.");
    }
    
    this->cooldownPeriod = millis() + US_INTERVAL_TIME;
#endif
}

int UltraSonic::getDistance()
{
    return this->distance;
}

void UltraSonic::fireConfigurationMessage() const
{
#ifdef TESTING
    String triggerValue("Simulated");
    String echoValue("Simulated");
#else
    int triggerValue = this->triggerPin;
    int echoValue = this->echoPin;
#endif
    JSONBufferWriter writer = SystemLog::createBuffer(256);

    writer.beginObject();
    writer.name("event").value("configuration");
    writer.name("trigger").value(triggerValue);
    writer.name("echo").value(echoValue);
    writer.endObject();

    this->logger.pushMessage("ultra-sonic", writer.buffer());
}

void UltraSonic::ReportHeartbeat(JSONBufferWriter& writer) const
{
    writer.name("ultra-sonic").value(this->distance);
}

#ifdef TESTING
void UltraSonic::setDistance(int distance)
{
    this->distance = distance;

    JSONBufferWriter writer = SystemLog::createBuffer(64);
    writer.beginObject();
    writer.name("distance").value(distance);
    writer.endObject();
    this->logger.pushMessage("ultra-sonic", writer.buffer());
}
#endif
*/