#ifndef _FLOATSWITCHREPORTER_
#define _FLOATSWITCHREPORTER_

#include "Reporters/AbstractReporter.h"

class FloatSwitch;
class MqttManager;
class Subject;
enum MessageType : int;

class FloatSwitchReporter : public AbstractReporter
{
public:
  FloatSwitchReporter(FloatSwitch* sensor, MqttManager& manager);
  // AbstractReporter
  virtual ~FloatSwitchReporter() override {};
  // IObserver
  virtual void Update(const Subject* subject, const MessageType type, void* msg) const override;
};

#endif
