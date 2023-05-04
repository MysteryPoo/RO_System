#include "global-defines.h"
#include "IComponent.h"
#include "Configuration/IConfiguration.h"
#include "IOnConnect.h"
#include "Utility/JsonBuffer.h"
#include "mqtt-manager.h"
#include "mqtt-payload.h"
#include "MqttSubscriber.h"
#include "mqtt-queue.h"
#include "SimpleBroker.h"
#include "system-log.h"

#define STATUS_REPORT_PERIOD 10000
#define DISCOVERY_PERIOD 5000
#define MQTT_PACKET_SIZE 2048
#define MQTT_PORT 1883
#define MQTT_UDP_DISCOVER_PORT 1882
#define MQTT_UDP_LISTEN_PORT 11882

MqttManager::MqttManager(SimpleBroker& broker, MqttQueue& queue)
: broker(broker)
, queue(queue)
, connected(false)
, discovery(true)
, lastUpdate(-STATUS_REPORT_PERIOD)
, lastDiscovery(-DISCOVERY_PERIOD)
{
  this->client.Initialize(NULL, this->ipAddress, MQTT_PORT, MQTT_DEFAULT_KEEPALIVE, MQTT_PACKET_SIZE, NULL, true);
  this->client.AttachMqttManager(this);
}

void MqttManager::Subscribe(MqttSubscriber* subscriber, const char* topic, MQTT::EMQTT_QOS qos)
{
  this->client.subscribe("to/" + System.deviceID() + "/" + topic, qos);
  this->broker.Subscribe(subscriber, topic);
}

void MqttManager::AttachConfiguration(IConfiguration* configuration)
{
  this->configurations.push_back(configuration);
}

void MqttManager::AttachOnConnect(IOnConnect* listener)
{
  this->onConnectListeners.push_back(listener);
}

void MqttManager::Update()
{
  unsigned long curMillis = millis();
  if (!WiFi.ready())
  {
    return;
  }

  if (this->connected && this->client.isConnected())
  {
    this->client.loop();
    MqttPayload* front = this->queue.FrontPayload();
    if (nullptr != front)
    {
      this->publish(front->topic, front->payload);
      this->queue.PopPayload();
    }
    if (curMillis - this->lastUpdate >= STATUS_REPORT_PERIOD)
    {
      this->connected = this->client.publish("from/" + System.deviceID() + "/status", "online");
      this->lastUpdate = curMillis;
    }
  }
  else
  {
    if (this->discovery && curMillis - this->lastDiscovery >= DISCOVERY_PERIOD)
    {
      discoverMQTT();
      char message[1028];
      int length = udp.receivePacket((byte*)message, 1028, 500);
      if (length > 0)
      {
        JSONValue json = JSONValue::parseCopy(message);
        if (!json.isValid())
        {
          return;
        }
        String username;
        String password;
        String override;
        JSONObjectIterator it(json);
        while(it.next())
        {
          if (it.name() == "username")
          {
            username = it.value().toString().data();
          }
          if (it.name() == "password")
          {
            password = it.value().toString().data();
          }
          if (it.name() == "override")
          {
            override = it.value().toString().data();
            this->parseIpFromString(override.c_str());
          }
        }

        if (override == "")
        {
          IPAddress remoteIp = udp.remoteIP();
          for (int i = 0; i < 4; ++i)
          {
            this->ipAddress[i] = remoteIp[i];
          }
        }

        this->client.setBroker(this->ipAddress, MQTT_PORT);
        bool connectionSuccess = this->client.connect("sparkclient_" + String(Time.now()), username, password);
        if (this->client.isConnected())
        {
          this->queue.PushPayload("status", "connected");
          this->queue.PushPayload("version", VERSION_STRING);
          this->announceConfigurations();
          for(IOnConnect* listener : this->onConnectListeners)
          {
            listener->OnConnect(connectionSuccess);
          }
          this->connected = true;
          this->discovery = false;
          udp.stop();
        }
        else
        {
          discoverMQTT();
        }
      }
    }
    else
    {
      discoverMQTT();
    }
  }
}

void MqttManager::OnMqttMessage(char* topic, uint8_t* buffer, unsigned int bufferLength)
{
  this->broker.RouteMessage(topic, buffer, bufferLength);
}

void MqttManager::PushPayload(String topic, String payload)
{
  this->queue.PushPayload(topic, payload);
}

void MqttManager::announceConfigurations()
{
    JSONBufferWriter message = JsonBuffer::createBuffer(512);
    message.beginArray();
    for(IConfiguration* configuration : this->configurations)
    {
      message.value(configuration->GetName());
    }
    message.endArray();

    this->publish("feature-list", message.buffer());
    delete[] message.buffer();
}

void MqttManager::discoverMQTT()
{
  udp.begin(MQTT_UDP_LISTEN_PORT);
  this->discovery = true;
  //IPAddress broadcast(224, 0, 0, 116);
  IPAddress broadcast(255,255,255,255);
  udp.beginPacket(broadcast, MQTT_UDP_DISCOVER_PORT);
  udp.write("Looking for MQTT server");
  udp.endPacket();
  this->lastDiscovery = millis();
}

// Code from https://forum.arduino.cc/t/get-ip-address-from-string/478308/20
void MqttManager::parseIpFromString(const char* cstringToParse)
{
  uint8_t idx = 0;
  byte part = 0;
  if (cstringToParse[idx] != '\0') {
    while (true)
    {
      if (cstringToParse[idx] == '.' || cstringToParse[idx] == '\0') { // part separator / end of string
        part++ ;  // skip to next byte
        if (cstringToParse[idx] == 0 || part == 4)  // done if 4 parts parsed or end of string
          break ;
        else  this->ipAddress[part] = 0 ;
      } else if (cstringToParse[idx] >= '0' && cstringToParse[idx] <= '9') {
        this->ipAddress[part] = this->ipAddress[part] * 10 + (cstringToParse[idx] - '0') ; // build up value from decimal digits.
      } else
        break ;
      idx++ ;  // step to next char
    }
  }
}

void MqttManager::publish(const char* topic, const String payload)
{
  this->connected = this->client.publish("from/" + System.deviceID() + "/" + topic, payload);
}
