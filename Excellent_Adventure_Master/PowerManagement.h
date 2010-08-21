#ifndef _POWER_MANAGEMENT_H
#define _POWER_MANAGEMENT_H
#include "Globals.h"

class PowerManagement
{
public:
    PowerManagement();
    ~PowerManagement();
    void InitPins();
    bool Ready();
    bool GetPowerStatus();

    void PowerUp();
    void PowerDown();

    bool Poll
    (
        unsigned long                               time
    );

};


#endif
