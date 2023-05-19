#ifndef _BAROMETER_
#define _BAROMETER_

#include "Adafruit_BMP085_v1_0_3/Adafruit_BMP085.h"
#include "Sensors/AbstractSensor.h"

class BarometerConfiguration;
class String;
class SystemLog;

class Barometer : public Adafruit_BMP085, public AbstractSensor
{
  friend BarometerConfiguration;
public:
  Barometer(SystemLog& logger);
  void Setup();

  // AbstractSensor
  virtual const bool isFull() const override;
  virtual void Update() override;
  virtual const String GetName() const override;
private:
  bool isSetup;
  void sample();
  int32_t pressure;
  int32_t fullPressure;
  system_tick_t lastUpdate;
};

#endif
