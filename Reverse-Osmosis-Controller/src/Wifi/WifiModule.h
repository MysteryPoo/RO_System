/*
  WifiModule

  Module for Wifi stats
*/

#ifndef _WIFIMODULE_
#define _WIFIMODULE_

#include "IUpdateOnLoop.h"
#include "ObserverPattern/Subject.h"

class WifiModule : public Subject, public IUpdateOnLoop
{
public:
  WifiModule();
  const float GetSignal() const { return this->_signal; }
  const float GetQuality() const { return this->_quality; }
  // IUpdateOnLoop
  virtual void Update() override;
private:
  float _signal;
  float _quality;
  unsigned long _lastCheck;
};

#endif
