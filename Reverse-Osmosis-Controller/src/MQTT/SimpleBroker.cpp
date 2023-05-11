#include "MQTT/MqttSubscriber.h"
#include "SimpleBroker.h"
#include "spark_wiring_string.h"

SimpleBroker::SimpleBroker()
{

}

void SimpleBroker::Subscribe(MqttSubscriber* subscriber, String topic)
{
  if (_subscribers.find(topic) == _subscribers.end())
  {
    _subscribers[topic] = new SubscriberList;
  }
  _subscribers[topic]->push_back(subscriber);
}

void SimpleBroker::RouteMessage(const char* topic, uint8_t* payloadAsByteArray, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payloadAsByteArray, length);
  p[length] = '\0';

  String payload(p);

  if (_subscribers.find(topic) != _subscribers.end())
  {
    SubscriberList* list = _subscribers[topic];
    for(auto subscriber : *list)
    {
      subscriber->Update(payload.c_str());
    }
  }
}

void SimpleBroker::Unsubscribe(MqttSubscriber* subscriber, String topic)
{
  // Not implemented
}
