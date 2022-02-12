
#include "mqtt-client.h"
#include "system-log.h"

#define MQTTClient_DEFAULT_PERIOD 10000
#define MQTT_PACKET_SIZE 2048
#define MQTT_PORT 1883
#define MQTT_UDP_DISCOVER_PORT 1882
#define MQTT_UDP_LISTEN_PORT 11882

MQTTClient::MQTTClient()
: connected(false)
, discovery(false)
{
  this->client.Initialize(NULL, this->ipAddress, MQTT_PORT, MQTT_DEFAULT_KEEPALIVE, MQTT_PACKET_SIZE, NULL, true);
  this->client.RegisterCallbackListener(this);
}

bool MQTTClient::Publish(const char* topic, const String payload)
{
  bool success = this->client.publish("from/" + System.deviceID() + "/" + topic, payload);
  if (!success) {
    this->connected = false;
  }
  return success;
}

void MQTTClient::RegisterCallbackListener(ISubCallback* listener)
{
  this->client.RegisterCallbackListener(listener);
  this->listeners.push_back(listener);
}

void MQTTClient::Subscribe(const char* topic, MQTT::EMQTT_QOS qos)
{
  this->client.subscribe("to/" + System.deviceID() + "/" + topic, qos);
}

void MQTTClient::Update()
{
  unsigned long curMillis = millis();
  static unsigned long timer = curMillis;
  if (curMillis < timer + MQTTClient_DEFAULT_PERIOD || !WiFi.ready())
  {
    return;
  }
  timer = curMillis;

  if (this->connected && this->client.isConnected())
  {
    this->client.loop();
    this->connected = this->client.publish("from/" + System.deviceID() + "/status", "online");
  }
  else
  {
    if (this->discovery)
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
          for(ISubCallback* listener : this->listeners)
          {
            listener->OnConnect(connectionSuccess, this);
          }
          this->connected = true;
          this->discovery = false;
          udp.stop();
          this->client.publish("from/" + System.deviceID() + "/status", "connected");
          this->announceFeatures();
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

void MQTTClient::Callback(char* topic, uint8_t* buffer, unsigned int bufferLength)
{
  char p[bufferLength + 1];
  memcpy(p, buffer, bufferLength);
  p[bufferLength] = '\0';
}

void MQTTClient::discoverMQTT()
{
  udp.begin(MQTT_UDP_LISTEN_PORT);
  this->discovery = true;
  IPAddress broadcast(255, 255, 255, 255);
  udp.beginPacket(broadcast, MQTT_UDP_DISCOVER_PORT);
  udp.write("Looking for MQTT server");
  udp.endPacket();
}

// Code from https://forum.arduino.cc/t/get-ip-address-from-string/478308/20
void MQTTClient::parseIpFromString(const char* cstringToParse)
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

void MQTTClient::announceFeatures()
{
    JSONBufferWriter message = SystemLog::createBuffer(512);
    message.beginObject();
    message.name("features").beginArray()
    .value("ro-system")
    .value("heartbeat")
#ifdef FEATURE_FLOATSWITCH
    .value("float-switch")
#endif
#ifdef FEATURE_ULTRASONIC
    .value("ultra-sonic")
#endif
#ifdef FEATURE_FLOATMETER
    .value("float-meter")
#endif
    .endArray();
    message.endObject();

    this->Publish("feature-list", message.buffer());
    delete[] message.buffer();
}
