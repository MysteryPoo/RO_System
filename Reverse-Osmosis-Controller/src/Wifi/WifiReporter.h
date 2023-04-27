/*
  WiFiReporter

  Reports WiFi status changes
*/

#ifndef _WIFIREPORTER_
#define _WIFIREPORTER_

#include "Reporters/AbstractReporter.h"

class MqttManager;
class Subject;
class WifiModule;

class WifiReporter : public AbstractReporter
{
public:
  WifiReporter(WifiModule* wifi, MqttManager& manager);
  virtual void Update(Subject* subject) override;
private:
  float _lastSignal;
  float _lastQuality;
};

#endif
