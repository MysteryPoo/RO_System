/*
    Interface for a hardware component that gets an update call every loop.
*/

#ifndef _ICOMPONENT_
#define _ICOMPONENT_

#include "IUpdateOnLoop.h"

class String;

class IComponent : public IUpdateOnLoop
{
public:
    virtual const String GetName() const = 0;
    // IUpdateOnLoop
    virtual void Update() override = 0;
};

#endif
