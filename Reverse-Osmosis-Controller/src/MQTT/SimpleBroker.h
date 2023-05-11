/*
  SimpleBroker
*/
#ifndef _SIMPLEBROKER_
#define _SIMPLEBROKER_

#include <map>
#include <vector>
#include <stdint.h>

class MqttSubscriber;
class String;

typedef std::vector<MqttSubscriber*> SubscriberList;
typedef std::map<String, SubscriberList*> SubscriberTopicMap;

class SimpleBroker
{
public:
  SimpleBroker();
  void Subscribe(MqttSubscriber* subscriber, String topic);
  void RouteMessage(const char*, uint8_t*, unsigned int);
  void Unsubscribe(MqttSubscriber* subscriber, String topic);
private:
  SubscriberTopicMap _subscribers;
};

#endif
