#include "ObserverPattern/Subject.h"
#include "MQTT/mqtt-manager.h"
#include "WifiModule.h"
#include "WifiReporter.h"
#include "Messages/SignalMessage.h"
#include "Messages/QualityMessage.h"
#include "ObserverPattern/MessageType.h"

#define MODULE_NAME "wifi"

using namespace WifiMessage;

WifiReporter::WifiReporter(WifiModule* wifi, MqttManager& manager)
  : AbstractReporter(manager)
{
  if (nullptr != wifi) { wifi->Attach(this); }
}

void WifiReporter::Update(const Subject* subject, const MessageType type, void* messagePtr) const
{
  //WifiModule* wifi = static_cast<WifiModule*>(subject);
  switch(type)
  {
    case MessageType::WIFI_SIGNAL_MSG: {
      const WifiMessage::Signal* message = static_cast<WifiMessage::Signal*>(messagePtr);
      this->report(MODULE_NAME "/signal", message->signal);
      break;
    }
    case MessageType::WIFI_QUALITY_MSG: {
      const Quality* message = static_cast<Quality*>(messagePtr);
      this->report(MODULE_NAME "/quality", message->quality);
      break;
    }
  }
}
