#ifndef _POWER_MANAGEMENT_H
#define _POWER_MANAGEMENT_H
#include "Globals.h"

class PowerManagement
{
    bool                                            m_status;
public:
    PowerManagement();
    ~PowerManagement();
    void InitPins();
    bool Ready();
    bool GetPowerStatus();
    bool GetLowPowerStatus();
    void PowerUp();
    void PowerDown();
    // Returns whether the status has changed:
    bool Poll
    (
        unsigned long                               time
    );

};


#endif
