
#include "ultra-sonic.h"
#include "application.h"
#include "system-log.h"
#include "json.h"

#define US_TRIGGER_TIME 11
#define US_INTERVAL_TIME 51
#define CM_PER_MICROSECOND 0.0343

UltraSonic::UltraSonic(int trig, int echo, SystemLog &logger) :
    triggerPin(trig),
    echoPin(echo),
    distance(0),
    cooldownPeriod(0),
    logger(logger)
{
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT_PULLDOWN);
    
    digitalWrite(trig, LOW); // Start low

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
}

int UltraSonic::getDistance()
{
    return this->distance;
}

void UltraSonic::fireConfigurationMessage() const
{
    String configurationMessage = JHelp::begin();
    configurationMessage += JHelp::field("event", "configuration");
    configurationMessage += JHelp::next();
    configurationMessage += JHelp::field("trigger", this->triggerPin);
    configurationMessage += JHelp::next();
    configurationMessage += JHelp::field("echo", this->echoPin);
    configurationMessage += JHelp::end();

    this->logger.pushMessage("ultra-sonic", configurationMessage);
}
