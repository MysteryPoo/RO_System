#include "MQTT/mqtt-manager.h"
#include "RoSystemReporter.h"
#include "ROSystem.h"
#include "spark_wiring_json.h"
#include "Messages/EnabledMsg.h"
#include "Messages/FlushedMsg.h"
#include "Messages/PumpCooldownMsg.h"
#include "Messages/FlushDurationMsg.h"
#include "Messages/StateChangeMessage.h"
#include "ObserverPattern/Subject.h"
#include "ObserverPattern/MessageType.h"
#include "Utility/JsonBuffer.h"
#include "Utility/TimeHelper.h"

#include "RoSystemEnum.h"

using namespace RoSystemMessage;

RoSystemReporter::RoSystemReporter(ROSystem* system, MqttManager& manager)
  : AbstractReporter(manager)
{
  if (nullptr != system) system->Attach(this);
}

void RoSystemReporter::Update(const Subject* subject, const MessageType type, void* msg) const
{
  const ROSystem* system = static_cast<const ROSystem*>(subject);
  switch(type)
  {
    case MessageType::ROSYSTEM_ENABLED_MSG: {
      const Enabled* message = static_cast<Enabled*>(msg);
      this->report(system->GetName() + "/enabled", message->enabled);
      break;
    }
    case MessageType::ROSYSTEM_FLUSHED_MSG: {
      const Flushed* message = static_cast<Flushed*>(msg);
      this->report(system->GetName() + "/flushed", message->flushed);
      break;
    }
    case MessageType::ROSYSTEM_STATE_MSG: {
      const RoSystemMessage::StateChange* message = static_cast<RoSystemMessage::StateChange*>(msg);
      this->report(system->GetName() + "/state-request", message);
      break;
    }
    case MessageType::ROSYSTEM_PUMP_COOLDOWN_MSG: {
      const RoSystemMessage::PumpCooldown* message = static_cast<RoSystemMessage::PumpCooldown*>(msg);
      this->report(system->GetName() + "/pump-cooldown", message->cooldown);
      break;
    }
    case MessageType::ROSYSTEM_FLUSH_DURATION_MSG: {
      const RoSystemMessage::FlushDuration* message = static_cast<RoSystemMessage::FlushDuration*>(msg);
      this->report(system->GetName() + "/flush-duration", message->duration);
      break;
    }
  }
}

void RoSystemReporter::report(const char* topic, const RoSystemMessage::StateChange* message) const
{
  JSONBufferWriter writer = JsonBuffer::createBuffer(2048);
  writer.beginObject()
  .name("datetime").value(TimeHelper::GetTimeIfAble())
  .name("state").value(RoSystemEnum::ToString(message->state))
  .name("success").value(message->success)
  .name("requestReason").value(message->requestReason)
  .name("failureReason").value(message->failureReason)
  .endObject();
  this->mqtt.PushPayload(topic, writer.buffer());
  delete[] writer.buffer();
}
