
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
#include "message.h"

#define SYSTEMLOG_MESSAGEBUFFERSIZE 32

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
    void pushMessage(String component, char* data);
    boolean isEmpty() { return currentMessageIndex == 0; }
    int messageQueueSize() { return currentMessageIndex; }
    static JSONBufferWriter createBuffer(int size);

    boolean enabled;
    
private:
    byte currentMessageIndex;
    Message messages[SYSTEMLOG_MESSAGEBUFFERSIZE];
    unsigned long lastBurst;
    
    boolean isFull() { return currentMessageIndex == SYSTEMLOG_MESSAGEBUFFERSIZE; }
    void simpleMessage(String label, String message);

};

#endif
