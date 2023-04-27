#include "spark_wiring_ticks.h"
#include "spark_wiring_wifi.h"
#include "spark_wiring_random.h"
#include "WifiModule.h"

using namespace spark;

#define CHECK_PERIOD 10000

WifiModule::WifiModule()
  : _signal(0.f)
  , _quality(0.f)
  , _lastCheck(-CHECK_PERIOD)
{
}

void WifiModule::Update()
{
  if (!WiFi.ready())
  {
    return;
  }
  if (millis() - this->_lastCheck < CHECK_PERIOD)
  {
    return;
  }
  this->_lastCheck = millis();
  WiFiSignal wifi = WiFi.RSSI();
#ifdef TESTING
  this->_signal = wifi.getSignal();
  this->_quality = wifi.getQuality();
#else
  this->_signal = random(20, 100);
  this->_quality = random(20, 100);
#endif
  this->Notify();
}
