
#ifndef _TEST_ULTRA_SONIC
#define _TEST_ULTRA_SONIC

#include "ultra-sonic.h"

class TestUltraSonic : public UltraSonic
{
public:
    TestUltraSonic(int trig, int echo, SystemLog &logger);
    ~TestUltraSonic() {};

    virtual void sample() override;
    virtual int getDistance() override;

    virtual void cloudSetup() override;

    void setDistance(int distance);

private:
    int testDistance;
};

#endif