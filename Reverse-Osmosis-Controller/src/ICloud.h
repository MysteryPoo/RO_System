
/*
    Interface for cloud enabled objects.

    Contains a public method to call cloud functions some time after initialization.
    
*/

#ifndef _ICLOUD_
#define _ICLOUD_

class ICloud {

public:
    virtual void cloudSetup() = 0;
};

#endif
