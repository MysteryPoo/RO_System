/*
  Interface class for classes that watch members of other classes.
*/
#ifndef _REPORTERBASE_
#define _REPORTERBASE_

#include "ObserverPattern/IObserver.h"

class MqttManager;
class Subject;

class AbstractReporter : public IObserver
{
public:
  AbstractReporter(MqttManager& manager);
  // IObserver
  virtual ~AbstractReporter() override {};
  virtual void Update(Subject* subject) override = 0;
protected:
  MqttManager& mqtt;
  void report(const char* topic, const bool value) const;
  void report(const char* topic, const char* value) const;
  void report(const char* topic, const float value) const;
};

#endif
