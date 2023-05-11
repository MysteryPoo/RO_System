/*
  IConfiguration - Interface class for configurations. Configurations are things that announce their options and can be set externally
  through the mqtt channels.
*/

#ifndef _ICONFIGURATION_
#define _ICONFIGURATION_

class IConfiguration
{
public:
  virtual const char* GetName() const = 0;
};

#endif
