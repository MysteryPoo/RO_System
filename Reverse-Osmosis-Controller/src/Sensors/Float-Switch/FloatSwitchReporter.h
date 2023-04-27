#include "Reporters/AbstractReporter.h"

class FloatSwitch;
class MqttManager;
class Subject;

class FloatSwitchReporter : public AbstractReporter
{
public:
  FloatSwitchReporter(FloatSwitch* sensor, MqttManager& manager);
  // AbstractReporter
  virtual ~FloatSwitchReporter() override {};
  virtual void Update(Subject* subject) override;
private:
  bool _lastStatus;
  bool _lastReliable;
};
