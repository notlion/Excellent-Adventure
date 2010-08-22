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
    return m_status;
}

bool PowerManagement :: GetLowPowerStatus()
{
    return PM_LOW_POWER_MODE_OFF;
}

void PowerManagement :: PowerUp()
{
    m_status = PM_POWER_ON;
}

void PowerManagement :: PowerDown()
{
    m_status = PM_POWER_OFF;
}

bool PowerManagement :: Poll
(
    unsigned long                                   time
)
{
    
    return false;
}

