
#ifndef _TEST_FLOAT_SWITCH_
#define _TEST_FLOAT_SWITCH_

#include "float-switch.h" // Foward declares SystemLog
#include "system-log.h"

class TestFloatSwitch : public FloatSwitch
{
    public:
        TestFloatSwitch(int, SystemLog&);
        ~TestFloatSwitch() {};

        virtual void cloudSetup() override;

        virtual void sample() override;
        virtual bool isStable() override;
        virtual bool isActive() override;

        void setStatus(bool status);

    private:
        bool testValue;

};

#endif