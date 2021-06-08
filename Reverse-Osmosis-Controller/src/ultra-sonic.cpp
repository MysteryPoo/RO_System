
#include "global-defines.h"
#include "ultra-sonic.h"
#include "application.h"
#include "system-log.h"

#define US_TRIGGER_TIME 11
#define US_INTERVAL_TIME 51
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
    this->fireConfigurationMessage();
}

void UltraSonic::cloudSetup()
{
    Particle.variable("Ultra-Sonic-Distance", this->distance);
}

void UltraSonic::Update()
{
    this->sample();
}

void UltraSonic::sample()
{
#ifndef TESTING
	unsigned long curMillis = millis();
    if(curMillis < this->cooldownPeriod) return; // Early bail out, we're still on cooldown
    
    digitalWrite(this->triggerPin, HIGH); // Start trigger
    delayMicroseconds(US_TRIGGER_TIME);
    digitalWrite(this->triggerPin, LOW); // End trigger
    
    unsigned long pulse = pulseIn(this->echoPin, HIGH) / 2ul;
    unsigned long sample = (unsigned long)(((float)pulse * CM_PER_MICROSECOND) / 2.f);
    if(sample != 0)
    {
        this->distance = ((unsigned long)this->distance + sample) / 2ul;
    }
    
    this->cooldownPeriod = curMillis + US_INTERVAL_TIME;
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
