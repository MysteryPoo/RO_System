#include "global-defines.h"
#include "spark_wiring_ticks.h"
#include "spark_wiring_wifi.h"
#include "spark_wiring_random.h"
#include "WifiModule.h"
#include "Messages/SignalMessage.h"
#include "Messages/QualityMessage.h"
#include "ObserverPattern/MessageType.h"
#include <cmath>

#define EPSILON 0.01f

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
  float prevSignal = this->_signal;
  float prevQuality = this->_quality;
  WiFiSignal wifi = WiFi.RSSI();
#ifndef TESTING
  this->_signal = wifi.getSignal();
  this->_quality = wifi.getQuality();
#else
  this->_signal = random(20, 100);
  this->_quality = random(20, 100);
#endif
  if (std::fabs(prevSignal - this->_signal) < EPSILON)
  {
    WifiMessage::Signal msg(this->_signal);
    this->Notify(MessageType::WIFI_SIGNAL_MSG, &msg);
  }
  if (std::fabs(prevQuality - this->_quality) < EPSILON)
  {
    WifiMessage::Quality msg(this->_quality);
    this->Notify(MessageType::WIFI_QUALITY_MSG, &msg);
  }
}
