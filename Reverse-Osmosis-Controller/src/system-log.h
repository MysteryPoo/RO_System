
/*
    SystemLog
    
    A logging API to wrap the Particle.publish() cloud event messaging system for remote logging.
    
    To use, simply include the header, use the trace, information, warning, or error methods to tag
    a message with severity. publishLog must be called on some interval, preferably no more frequently than
    once per second.
*/

#ifndef _SYSTEM_LOG_
#define _SYSTEM_LOG_

#include "application.h"
#include <vector>
#include "json.h"

#define SYSTEMLOG_MESSAGEBUFFERSIZE 32

class Message
{
public:
    String component;
    long datetime;
    String data;

    Message()
    : component("undefined")
    , datetime(0)
    , data("\"undefined\"")
    {}

    String toJSON() {
        String json = JHelp::begin();
        json += JHelp::field("component", this->component);
        json += JHelp::next();
        if(this->datetime != 0l)
        {
            json += JHelp::field("datetime", this->datetime);
        }
        else
        {
            json += JHelp::field("datetime", "undefined");
        }
        json += JHelp::next();
        json += JHelp::field("data", this->data, true);
        json += JHelp::end();

        return json;
    }
};

class SystemLog
{
public:
    SystemLog();
    void publishLog();
    void trace(String message);
    void information(String message);
    void warning(String message);
    void error(String message);
    void pushMessage(String component, String data);
    boolean isEmpty() { return currentMessageIndex == 0; }
    int messageQueueSize() { return currentMessageIndex; }

    boolean enabled;
    
private:
    byte currentMessageIndex;
    Message messages[SYSTEMLOG_MESSAGEBUFFERSIZE];
    unsigned long lastBurst;
    
    boolean isFull() { return currentMessageIndex == SYSTEMLOG_MESSAGEBUFFERSIZE; }

};

#endif
