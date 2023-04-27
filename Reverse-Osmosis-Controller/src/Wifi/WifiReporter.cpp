#include "ObserverPattern/Subject.h"
#include "MQTT/mqtt-manager.h"
#include "WifiModule.h"
#include "WifiReporter.h"

#define MODULE_NAME "wifi"

WifiReporter::WifiReporter(WifiModule* wifi, MqttManager& manager)
  : AbstractReporter(manager)
  , _lastSignal(0.f)
  , _lastQuality(0.f)
{
  if (nullptr != wifi) { wifi->Attach(this); }
}

void WifiReporter::Update(Subject* subject)
{
  WifiModule* wifi = static_cast<WifiModule*>(subject);
  const float signal = wifi->GetSignal();
  const float quality = wifi->GetQuality();
  if (signal != this->_lastSignal)
  {
    this->report(MODULE_NAME "/signal", signal);
    this->_lastSignal = signal;
  }
  if (quality != this->_lastQuality)
  {
    this->report(MODULE_NAME "/quality", quality);
    this->_lastQuality = quality;
  }
}
