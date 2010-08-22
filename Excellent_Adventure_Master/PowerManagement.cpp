#include "PowerManagement.h"
#include "Globals.h"
#include <WProgram.h>

PowerManagement :: PowerManagement()
{
    m_time = millis();
    m_ready = true;
    m_lowPowerStatus = PM_LOW_POWER_MODE_OFF;
    m_lowPowerStatusOld = PM_LOW_POWER_MODE_OFF;
    m_powerStatus = PM_POWER_OFF;
}

PowerManagement :: ~PowerManagement()
{
}

void PowerManagement :: InitPins()
{
    pinMode(BOOTH_PIN_LIGHT_SENSOR, INPUT);
    pinMode(BOOTH_PIN_LOW_POWER_EN, OUTPUT);
    digitalWrite(BOOTH_PIN_LOW_POWER_EN, LOW);
}

bool PowerManagement :: Ready()
{
    return m_ready;
}

bool PowerManagement :: GetPowerStatus()
{
    return m_powerStatus;
}

bool PowerManagement :: GetLowPowerStatus()
{
    return m_lowPowerStatus;
}

void PowerManagement :: PowerUp()
{
    digitalWrite(BOOTH_PIN_LOW_POWER_EN, HIGH);
    m_powerStatus = PM_POWER_ON;
    m_ready = false;
}

void PowerManagement :: PowerDown()
{
    digitalWrite(BOOTH_PIN_LOW_POWER_EN, LOW);
    m_powerStatus = PM_POWER_OFF;
    m_ready = false;
}

bool PowerManagement :: Poll
(
    unsigned long                                   time
)
{
    if ((time - m_time) > LIGHT_SENSOR_POLLING_MS)
    {
        m_time = time;

        // Shift the bitfield down one, and append the last value read.
        m_debounce = 
            (m_debounce << 1) 
            |   (   (analogRead(BOOTH_PIN_LIGHT_SENSOR) > LIGHT_SENSOR_THRESHOLD)
                    ?   0x1 
                    :   0
                );
        m_ready = true;
    }
    if 
    (   (m_debounce == DEBOUNCE_ALL_ZEROS) 
    ||  (m_debounce == DEBOUNCE_ALL_ONES)
    ) 
    {
        m_lowPowerStatus = 
            (m_debounce == DEBOUNCE_ALL_ONES) 
            ?   PM_LOW_POWER_MODE_ON
            :   PM_LOW_POWER_MODE_OFF;
        if (m_lowPowerStatusOld != m_lowPowerStatus)
        {
            m_lowPowerStatusOld = m_lowPowerStatus;
            return true;
        }
    }

    return false;
}

