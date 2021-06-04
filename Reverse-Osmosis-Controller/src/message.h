#ifndef _MESSAGE_
#define _MESSAGE_

#include "application.h"

class Message
{
public:
    String component;
    int datetime;
    String data;

    Message();
    String toJSON();
    
};

#endif
