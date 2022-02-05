
#ifndef _HEARTBEAT_MANAGER_
#define _HEARTBEAT_MANAGER_

#include "application.h"
#include "system-log.h"
#include "IComponent.h"
#include "IHeartbeatReporter.h"
#include "IConfigurable.h"
#include <vector>


class HeartbeatManager: public IComponent, public IConfigurable
{
public:
  HeartbeatManager(SystemLog& logger);
  void RegisterReporter(String name, IHeartbeatReporter*);
  void SetPeriod(unsigned long newPeriod) { this->updatePeriod = newPeriod; }
  void ForceHeartbeat(); // TODO : Make this const

  // IComponent
  virtual void Update() override;
  // IConfigurable
  virtual void Configure(JSONValue json) override;

private:
  SystemLog& logger;
  unsigned long updatePeriod;

  std::vector<IHeartbeatReporter*> reporters;

  void sendHeartbeat();

  // IConfigurable
  virtual void fireConfigurationMessage() const override {};
};

#endif
