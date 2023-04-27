
#include "application.h"
#include "mqtt-queue.h"
#include "mqtt-payload.h"

#define MQTTQUEUE_QUEUESIZE 50

MqttQueue::MqttQueue()
{
}

void MqttQueue::PushPayload(MqttPayload* payload)
{
  this->queue.push(payload);
  // TODO: Refactor to minimize loss, not worried at the moment, as we never get close to this.
  if (this->queue.size() >= MQTTQUEUE_QUEUESIZE)
  {
    this->FlushQueue();
  }
}

void MqttQueue::PushPayload(String topic, String payload)
{
  MqttPayload* mqttPayload = new MqttPayload(topic, payload);
  this->PushPayload(mqttPayload);
}

MqttPayload* MqttQueue::FrontPayload() const
{
  if (this->queue.empty()) return nullptr;
  return this->queue.front();
}

void MqttQueue::PopPayload()
{
  if (this->queue.empty()) return;
  MqttPayload* front = this->queue.front();
  this->queue.pop();
  delete front;
}

void MqttQueue::FlushQueue()
{
  while(!this->queue.empty())
  {
    MqttPayload* payload = this->queue.front();
    this->queue.pop();
    delete payload;
  }
  this->PushPayload("mqtt/error", "{message: \"Queue was flushed.\"}");
}
