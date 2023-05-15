#ifndef _BAROMETERREPORTER_
#define _BAROMETERREPORTER_

#include "Reporters/AbstractReporter.h"

class Barometer;
class MqttManager;
class Subject;
enum MessageType : int;

class BarometerReporter : public AbstractReporter
{
public:
  BarometerReporter(Barometer* sensor, MqttManager& manager);
  // AbstractReporter
  virtual ~BarometerReporter() override {};
  virtual void Update(const Subject* subject, const MessageType type, void* msg) const override;
};

#endif
