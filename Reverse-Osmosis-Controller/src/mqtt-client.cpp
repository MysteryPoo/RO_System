
#include "mqtt-client.h"

#define MQTTClient_DEFAULT_PERIOD 10000

MQTTClient::MQTTClient()
{
  this->client.Initialize(NULL, this->ipAddress, this->port, MQTT_DEFAULT_KEEPALIVE, MQTT_MAX_PACKET_SIZE, NULL, true);
  this->client.RegisterCallbackListener(this);
}

void MQTTClient::Publish(const char* topic, const String payload)
{
  this->client.publish("from/" + System.deviceID() + "/" + topic, payload);
}

void MQTTClient::RegisterCallbackListener(ISubCallback* listener)
{
  this->client.RegisterCallbackListener(listener);
  this->listeners.push_back(listener);
}

void MQTTClient::Subscribe(const char* topic, MQTT::EMQTT_QOS qos)
{
  this->client.subscribe(topic, qos);
}

void MQTTClient::Update()
{
  unsigned long curMillis = millis();
  static unsigned long timer = curMillis;
  if (curMillis < timer + MQTTClient_DEFAULT_PERIOD)
  {
    return;
  }
  timer = curMillis;

  if (this->client.isConnected())
  {
    this->client.loop();
  }
  else
  {
    bool connectionSuccess = this->client.connect("sparkclient_" + String(Time.now()), USERNAME, PASSWORD);
    if (this->client.isConnected())
    {
        this->client.publish("from/" + System.deviceID() + "/status", "online");
        for(ISubCallback* listener : this->listeners)
        {
          listener->OnConnect(connectionSuccess, this);
        }
    }
  }
}

void MQTTClient::Callback(char* topic, uint8_t* buffer, unsigned int bufferLength)
{
  char p[bufferLength + 1];
  memcpy(p, buffer, bufferLength);
  p[bufferLength] = '\0';
}
