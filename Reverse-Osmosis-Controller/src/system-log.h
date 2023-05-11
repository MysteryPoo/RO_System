
/*
    SystemLog
    
    A logging API to wrap the Particle.publish() cloud event messaging system for remote logging.
    
    To use, simply include the header, use the trace, information, warning, or error methods to tag
    a message with severity. publishLog must be called on some interval, preferably no more frequently than
    once per second.
*/

#ifndef _SYSTEM_LOG_
#define _SYSTEM_LOG_

class MqttManager;
class MqttQueue;
class String;

class SystemLog
{
public:
    SystemLog(MqttManager& manager);
    void trace(String message);
    void information(String message);
    void warning(String message);
    void error(String message);
    
private:
    MqttManager& mqtt;
    
    void simpleMessage(String label, String message);

};

#endif
