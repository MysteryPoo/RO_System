
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

#define SYSTEMLOG_MESSAGEBUFFERSIZE 32

class Message
{
public:
    String component;
    String datetime;
    String data;

    Message()
    : component("undefined")
    , datetime("undefined")
    , data("\"undefined\"")
    {}

    String toJSON() {
        String json;
        json += "{";
        json += "\"component\":";
        json += "\"" + this->component + "\"";
        json += ",\"datetime\":";
        json += "\"" + this->datetime + "\"";
        json += ",\"data\":";
        json += this->data;
        json += "}";

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
