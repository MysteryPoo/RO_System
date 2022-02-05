
#include "mqtt-client.h"

#define MQTTClient_DEFAULT_PERIOD 10000 // One-second

MQTTClient::MQTTClient(SystemLog& logger):
  logger(logger)
{
  this->client.Initialize(NULL, this->ipAddress, this->port, MQTT_DEFAULT_KEEPALIVE, MQTT_MAX_PACKET_SIZE, NULL, true);
  this->client.RegisterCallbackListener(this);
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
    this->client.publish("outTopic/message", "Still connected");
    this->client.loop();
  }
  else
  {
    this->client.connect("sparkclient_" + String(Time.now()), "user", "password");
    if (this->client.isConnected())
    {
        this->client.publish("outTopic/message", "Connected");
        this->client.subscribe("inTopic/message", MQTT::EMQTT_QOS::QOS1);
    }
  }
}

void MQTTClient::Callback(char* topic, uint8_t* buffer, unsigned int bufferLength)
{
  char p[bufferLength + 1];
    memcpy(p, buffer, bufferLength);
    p[bufferLength] = '\0';

    logger.trace(String(topic) + ": " + String(p));
}
