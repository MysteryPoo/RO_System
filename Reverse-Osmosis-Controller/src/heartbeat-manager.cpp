
#include "global-defines.h"
#include "heartbeat-manager.h"
#include "system-log.h"
#include "IHeartbeatReporter.h"
#include "mqtt-client.h"
#include "mqtt-queue.h"

#define THIRTY_SECONDS_MS 30000
#define ONE_MINUTE_MS 60000
#define COMPONENT_NAME "heartbeat"

HeartbeatManager::HeartbeatManager(SystemLog& logger, MQTTClient* mqtt, MqttQueue& mqttQueue)
: logger(logger)
, mqttQueue(mqttQueue)
#ifndef TESTING
, updatePeriod(ONE_MINUTE_MS * 10)
#else
, updatePeriod(THIRTY_SECONDS_MS)
#endif
{
  if (nullptr != mqtt)
  {
    mqtt->RegisterCallbackListener(this);
  }
}

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
  
  sendHeartbeat();
}

void HeartbeatManager::Configure(JSONValue json)
{
  if (!json.isValid())
  {
    return;
  }
  JSONObjectIterator jsonIt(json);
  while(jsonIt.next())
  {
    if (jsonIt.name() == "Rate")
    {
      int rawRate = jsonIt.value().toInt();
      if (rawRate > 0)
      {
        this->SetPeriod((unsigned long)rawRate);
      }
    }
  }
}

bool HeartbeatManager::ForceHeartbeat() const
{
  return this->sendHeartbeat();
}

void HeartbeatManager::Callback(char* topic, uint8_t* payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = '\0';

  if (strcmp(topic, "to/" + System.deviceID() + "/heartbeat/configuration"))
  {
      return;
  }

  JSONValue configuration = JSONValue::parseCopy(String(p));
  this->Configure(configuration);
}

void HeartbeatManager::OnConnect(bool connectSuccess, MQTTClient* mqtt)
{
  if (nullptr != mqtt)
  {
    mqtt->Subscribe("heartbeat/configuration", MQTT::EMQTT_QOS::QOS1);
    JSONBufferWriter message = SystemLog::createBuffer(512);
    message.beginObject();
    message.name("display").value("Heartbeat Manager");
    message.name("description").value("Periodically sends current state data.");
    message.name("options").beginArray()
    .beginObject()
    .name("name").value("Rate")
    .name("type").value("number")
    .name("units").value("millisecond")
    .name("default").value(this->updatePeriod)
    .endObject()
    .endArray();
    message.endObject();
    mqttQueue.PushPayload("configuration/heartbeat", message.buffer());
    delete[] message.buffer();
  }
}

bool HeartbeatManager::sendHeartbeat() const
{
  if (!WiFi.ready())
  {
    return false;
  }

  WiFiSignal wifi = WiFi.RSSI();

  const int bufferSize = 2048;
  JSONBufferWriter message = SystemLog::createBuffer(bufferSize);
  message.beginObject();
  message.name("event").value("heartbeat");
  message.name("messageQueueSize").value(logger.messageQueueSize());
  message.name("version").value(VERSION_STRING);
  message.name("heartbeat-rate").value(this->updatePeriod);
  message.name("WiFi").beginObject()
#ifdef TESTING
  .name("signal").value(random(20, 100))
  .name("quality").value(random(20,100))
#else
  .name("signal").value(wifi.getStrength())
  .name("quality").value(wifi.getQuality())
#endif
  .endObject();
  for (IHeartbeatReporter* reporter : this->reporters)
  {
    reporter->ReportHeartbeat(message);
  }
  message.endObject();

  const int dataSize = message.dataSize();
  if (bufferSize >= dataSize)
  {
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

    logger.pushMessage("system/heartbeat", errorMessage.buffer());
  }
  return true;
}

String HeartbeatManager::GetName() const
{
  return COMPONENT_NAME;
}
