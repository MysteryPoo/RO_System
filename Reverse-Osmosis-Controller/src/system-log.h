
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
#include "IComponent.h"
#include <queue>

#define SYSTEMLOG_MESSAGEBUFFERSIZE 32

class SystemLog : public IComponent
{
public:
    SystemLog();
    void trace(String message);
    void information(String message);
    void warning(String message);
    void error(String message);
    void pushMessage(String component, String data);
    void pushMessage(String component, char* data);
    boolean isEmpty() { return messageQueueSize() == 0; }
    int messageQueueSize() { return messageQueue.size(); }
    static JSONBufferWriter createBuffer(int size);
    virtual void Update() override;

    boolean enabled;
    
private:
    byte currentMessageIndex;
    std::queue<Message*> messageQueue;
    unsigned long lastBurst;
    
    void publishLog();
    boolean isFull() { return messageQueueSize() == SYSTEMLOG_MESSAGEBUFFERSIZE; }
    void simpleMessage(String label, String message);

};

#endif
