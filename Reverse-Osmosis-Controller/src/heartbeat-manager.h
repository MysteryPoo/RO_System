
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

  // IComponent
  virtual void Update() override;

private:
  SystemLog& logger;

  std::vector<IHeartbeatReporter*> reporters;
};

#endif
