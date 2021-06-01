/*
    Interface for a hardware component that gets an update call every loop.
*/

#ifndef _ICOMPONENT_
#define _ICOMPONENT_

class IComponent
{
public:
    virtual void Update() = 0;
};

#endif
