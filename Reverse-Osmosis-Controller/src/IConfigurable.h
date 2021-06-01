/*
    Interface for a component that is configurable.
*/

#ifndef _ICONFIGURABLE_
#define _ICONFIGURABLE_
class IConfigurable
{
    virtual void fireConfigurationMessage() const = 0;
};
#endif
