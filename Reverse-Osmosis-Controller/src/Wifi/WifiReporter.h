/*
  WiFiReporter

  Reports WiFi status changes
*/

#ifndef _WIFIREPORTER_
#define _WIFIREPORTER_

#include "Reporters/AbstractReporter.h"

class MqttManager;
class Subject;
class WifiModule;
enum MessageType : int;

class WifiReporter : public AbstractReporter
{
public:
  WifiReporter(WifiModule* wifi, MqttManager& manager);
  virtual void Update(const Subject* subject, const MessageType type, void* msg) const override;
};

#endif
