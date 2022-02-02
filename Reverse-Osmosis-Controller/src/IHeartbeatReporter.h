#ifndef _IHEARTBEAT_REPORTER_
#define _IHEARTBEAT_REPORTER_

class IHeartbeatReporter
{
public:
  virtual void reportHeartbeat(JSONBufferWriter& writer) const = 0;
};

#endif
