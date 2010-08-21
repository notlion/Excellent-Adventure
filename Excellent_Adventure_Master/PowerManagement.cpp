#include "PowerManagement.h"

PowerManagement :: PowerManagement()
{
}

PowerManagement :: ~PowerManagement()
{
}

void PowerManagement :: InitPins()
{
}

bool PowerManagement :: Ready()
{
    return true;
}

bool PowerManagement :: GetPowerStatus()
{
}

void PowerManagement :: PowerUp()
{
}

void PowerManagement :: PowerDown()
{
}

bool PowerManagement :: Poll
(
    unsigned long                                   time
)
{
    return PM_LOW_POWER_MODE_OFF;
}

