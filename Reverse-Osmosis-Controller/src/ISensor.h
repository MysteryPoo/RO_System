/*
  Interface for a sensor. The sensor itself can be analog or digital,
  however, what we care is that the sensor needs to be read as digital
  in terms of whether to engage/disengage the FILL routine.
*/

#ifndef _ISENSOR_
#define _ISENSOR_

class ISensor
{
public:
  virtual bool isFull() = 0;
};

#endif
