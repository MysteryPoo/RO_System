#include "barometer.h"
#include "BarometerConstants.h"
#include "BarometerReporter.h"
#include "Messages/BarometerPressureMessage.h"
#include "MQTT/mqtt-manager.h"
#include "ObserverPattern/MessageType.h"

BarometerReporter::BarometerReporter(Barometer* sensor, MqttManager& manager)
  : AbstractReporter(manager)
{
  if (nullptr != sensor) sensor->Attach(this);
}

void BarometerReporter::Update(const Subject* subject, const MessageType type, void* msg) const
{
  const Barometer* barometer = static_cast<const Barometer*>(subject);
  switch(type)
  {
    case MessageType::BAROMETER_PRESSURE_MSG: {
      const BarometerMessage::Pressure* message = static_cast<BarometerMessage::Pressure*>(msg);
      this->report(barometer->GetName() + "/pressure", message->pressure);
      break;
    }
  }
}
