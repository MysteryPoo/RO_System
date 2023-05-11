#ifndef _FLOATSWITCHSTATUSMESSAGE_
#define _FLOATSWITCHSTATUSMESSAGE_
namespace FloatSwitchMessage
{
  struct Status
  {
    Status(bool status)
      : status(status)
    {}
    
    bool status;
  };
}

#endif
