
/*
    Main logic controller for the overall system
    
    Requires a pump relay, inlet valve relay, and flush valve relay.
    
    Simple state machine to determine, based on water level inputs, whether to refill the resovoir or not.
*/

#ifndef _ROSYSTEM_
#define _ROSYSTEM_

#include "ICloud.h"

class Relay;
class SystemLog;
class String;

class ROSystem : public ICloud {
public:
    enum State {
        IDLE,
        FLUSH,
        FILL
    };
    
    ROSystem(Relay &pump, Relay &inlet, Relay &flush, SystemLog &logger);
    void update(bool tankFull, unsigned short distance);

    virtual void cloudSetup() override;

    void shutdown();

    String getStateString();

    int cloudRequestState(String newState);
    
    
private:
    State state;
    Relay &pump;
    Relay &inlet;
    Relay &flush;
    SystemLog &logger;
    bool flushedToday;
    unsigned long totalPumpTime;
    unsigned long totalPumpRuns;
    unsigned long nextPumpTime;
    unsigned long flushDelay;
    unsigned long pumpRunTime;
    
    void requestState(ROSystem::State state);
    bool activatePump();
    bool deactivatePump();
    
};

#endif
