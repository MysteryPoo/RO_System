
#ifndef _HEARTBEAT_MANAGER_
#define _HEARTBEAT_MANAGER_

#include "application.h"
#include "system-log.h"
#include "IComponent.h"
#include "IHeartbeatReporter.h"
#include <vector>


class HeartbeatManager: public IComponent
{
public:
  HeartbeatManager(SystemLog& logger);
  void RegisterReporter(String name, IHeartbeatReporter*);
  void SetPeriod(unsigned long newPeriod) { this->updatePeriod = newPeriod; }

  // IComponent
  virtual void Update() override;

private:
  SystemLog& logger;
  unsigned long updatePeriod;

  std::vector<IHeartbeatReporter*> reporters;
};

#endif
