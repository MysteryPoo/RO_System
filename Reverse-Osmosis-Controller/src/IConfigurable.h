/*
    Interface for a component that is configurable.
*/

#ifndef _ICONFIGURABLE_
#define _ICONFIGURABLE_
class IConfigurable
{
public:
    virtual void Configure(JSONValue json) = 0;
private:
    virtual void fireConfigurationMessage() const = 0;
};
#endif
