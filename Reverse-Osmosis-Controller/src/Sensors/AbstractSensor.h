/*
  Interface for a sensor. The sensor itself can be analog or digital,
  however, what we care is that the sensor needs to be read as digital
  in terms of whether to engage/disengage the FILL routine.
*/

#ifndef _ISENSOR_
#define _ISENSOR_

#include "ObserverPattern/Subject.h"
#include "IComponent.h"

class SystemLog;

class AbstractSensor : public Subject, public IComponent
{
public:
  AbstractSensor(SystemLog& logger) : logger(logger) {}
  virtual const bool isFull() const = 0;
  // IComponent
  virtual void Update() override = 0;
  virtual const String GetName() const override = 0;
protected:
  SystemLog& logger;
};

#endif
