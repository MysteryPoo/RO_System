
#include "global-defines.h"
#include "heartbeat-manager.h"

#define THIRTY_SECONDS_MS 30000

HeartbeatManager::HeartbeatManager(SystemLog& logger) :
  logger(logger) {}


void HeartbeatManager::RegisterReporter(String name, IHeartbeatReporter* reporter)
{
  this->reporters.push_back(reporter);
}

void HeartbeatManager::Update()
{
  unsigned long curMillis = millis();
  static unsigned long timer = curMillis;
  if (curMillis < timer + THIRTY_SECONDS_MS)
  {
    return;
  }
  timer = curMillis;
  
  JSONBufferWriter message = SystemLog::createBuffer(2048);
  message.beginObject();
  message.name("event").value("heartbeat");
  message.name("messageQueueSize").value(logger.messageQueueSize());
  message.name("version").value(VERSION_STRING);
  for (IHeartbeatReporter* reporter : this->reporters)
  {
    reporter->reportHeartbeat(message);
  }
  message.endObject();

  logger.pushMessage("system/tick", message.buffer());
  logger.pushMessage("system/heartbeat", message.buffer());
}
