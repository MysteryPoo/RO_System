/*
  Float Meter

  Reads a voltage level between 0V and 3.3V. The attached hardware should generate these values
  based on the water level (ex. a magnetic float passing read switches).
*/

#ifndef _FLOAT_METER_
#define _FLOAT_METER_

#include "global-defines.h"
#include "application.h"
#include "ICloud.h"
#include "IComponent.h"
#include "IConfigurable.h"
#include "Sensors/ISensor.h"
#include "IHeartbeatReporter.h"

// Foward delcaration
class SystemLog;

class FloatMeter : public IComponent, public ISensor, public IHeartbeatReporter, public IConfigurable {

public:
  FloatMeter(int pin, SystemLog &logger);

  float voltage() const;
  
  // IComponent
  virtual void Update() override;
  // ISensor
  virtual bool isFull() const override;
  // IHeartbeatReporter
  virtual void ReportHeartbeat(JSONBufferWriter&) const override;
  // IConfigurable
  virtual void Configure(JSONValue json) override;

#ifdef TESTING
  void setValue(int newValue);
#endif

private:
  int pin;
  int input;
  int fullValue;

  // IConfigurable
  virtual void fireConfigurationMessage() const override;

#ifdef TESTING
  int simulatedValue;
#endif

protected:
  SystemLog &logger;

};

#endif
