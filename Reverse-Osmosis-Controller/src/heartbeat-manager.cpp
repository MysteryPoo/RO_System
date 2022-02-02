
#include "global-defines.h"
#include "heartbeat-manager.h"

#define THIRTY_SECONDS_MS 30000

HeartbeatManager::HeartbeatManager(SystemLog& logger) :
  logger(logger),
  updatePeriod(THIRTY_SECONDS_MS)
{}


void HeartbeatManager::RegisterReporter(String name, IHeartbeatReporter* reporter)
{
  this->reporters.push_back(reporter);
}

void HeartbeatManager::Update()
{
  unsigned long curMillis = millis();
  static unsigned long timer = curMillis;
  if (curMillis < timer + updatePeriod)
  {
    return;
  }
  timer = curMillis;
  
  const int bufferSize = 2048;
  JSONBufferWriter message = SystemLog::createBuffer(bufferSize);
  message.beginObject();
  message.name("event").value("heartbeat");
  message.name("messageQueueSize").value(logger.messageQueueSize());
  message.name("version").value(VERSION_STRING);
  for (IHeartbeatReporter* reporter : this->reporters)
  {
    reporter->reportHeartbeat(message);
  }
  message.endObject();

  const int dataSize = message.dataSize();
  if (bufferSize >= dataSize)
  {
    logger.pushMessage("system/tick", message.buffer());
    logger.pushMessage("system/heartbeat", message.buffer());
  }
  else
  {
    JSONBufferWriter errorMessage = SystemLog::createBuffer(1024);
    message.beginObject();
    message.name("event").value("error");
    message.name("messageQueueSize").value(logger.messageQueueSize());
    message.name("version").value(VERSION_STRING);
    message.name("error").value("Data size is greater than buffer size!");
    message.name("buffer-size").value(bufferSize);
    message.name("data-size").value(dataSize);
    message.endObject();

    logger.pushMessage("system/tick", errorMessage.buffer());
    logger.pushMessage("system/heartbeat", errorMessage.buffer());
  }
}
