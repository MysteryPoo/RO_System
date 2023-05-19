#include "global-defines.h"
#ifdef FEATURE_BAROMETER
#include "barometer.h"
#include "BarometerConstants.h"
#include "system-log.h"
#include "spark_wiring_string.h"
#include "Messages/BarometerPressureMessage.h"
#include "ObserverPattern/MessageType.h"

Barometer::Barometer(SystemLog& logger)
  : AbstractSensor(logger)
  , isSetup(false)
  , pressure(0L)
  , fullPressure(FULL_PRESSURE_DEFAULT)
  , lastUpdate(-UPDATE_PERIOD)
{}

void Barometer::Setup()
{
  if (this->isSetup) return;
#ifndef TESTING
  if(!this->begin())
  {
    this->logger.error("Failed to find BMP085 sensor.");
  }
#endif
  this->logger.information("Barometer has been initialized.");
  this->isSetup = true;
}

const bool Barometer::isFull() const
{
  return this->pressure >= this->fullPressure;
}

void Barometer::Update()
{
  if (!this->isSetup) return;
  system_tick_t curMillis = millis();
  if (curMillis - this->lastUpdate >= UPDATE_PERIOD)
  {
    this->sample();
    this->lastUpdate = curMillis;
  }
}

const String Barometer::GetName() const
{
  return COMPONENT_NAME;
}

void Barometer::sample()
{
  int32_t prevPressure = this->pressure;
#ifndef TESTING
  this->pressure = this->readPressure();
#endif
  if (prevPressure != this->pressure)
  {
    BarometerMessage::Pressure msg(this->pressure);
    this->Notify(MessageType::BAROMETER_PRESSURE_MSG, &msg);
  }
}

#endif
