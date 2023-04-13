
#include "mqtt-client.h"
#include "mqtt-queue.h"
#include "mqtt-payload.h"

#define MQTTQUEUE_QUEUESIZE 50
#define UPDATE_PERIOD 1000

MqttQueue::MqttQueue(MQTTClient& mqttClient)
: mqttClient(mqttClient)
{

}

void MqttQueue::PushPayload(MqttPayload* payload)
{
  this->queue.push(payload);
}

void MqttQueue::PushPayload(String topic, String payload)
{
  MqttPayload* mqttPayload = new MqttPayload(topic, payload);
  this->PushPayload(mqttPayload);
}

void MqttQueue::Update()
{
  unsigned long curMillis = millis();
  static unsigned long timer = curMillis;
  if (curMillis < timer + UPDATE_PERIOD || !WiFi.ready() || !this->mqttClient.isConnected())
  {
    return;
  }
  timer = curMillis;

  this->PublishQueue();
}

void MqttQueue::PublishQueue()
{
  if (!this->mqttClient.isConnected())
  {
    if (this->queue.size() >= MQTTQUEUE_QUEUESIZE)
    {
      this->FlushQueue();
    }
  }
  else
  {
    while(!this->queue.empty() && this->mqttClient.isConnected())
    {
      MqttPayload* payload = this->queue.front();
      this->mqttClient.Publish(payload->topic, payload->payload);
      delete payload;
      this->queue.pop();
    }
  }
}

void MqttQueue::FlushQueue()
{
  while(!this->queue.empty())
  {
    MqttPayload* payload = this->queue.front();
    delete payload;
    this->queue.pop();
  }
  this->PushPayload("mqtt/error", "{message: \"Queue was flushed.\"}");
}
