/*
  IUpdateOnLoop - Needs to have Update() called every loop
*/

#ifndef _IUPDATEONLOOP_
#define _IUPDATEONLOOP_

class IUpdateOnLoop
{
public:
  virtual void Update() = 0;
};

#endif
