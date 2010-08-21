#include "Globals.h"

#ifdef ISR_ANIMATOR
#include <TimerOne.h>
#endif
#include <WProgram.h>
#include "EffectManager.h"
//#include "TimerOne.h"
//#define EM_DEBUG_ENABLE

#ifdef EM_DEBUG_ENABLE
#define EM_DEBUG(MSG)  PRINTLN(MSG)
#else
#define EM_DEBUG(MSG)
#endif

EffectManager :: EffectManager
(
    PowerManagement                             *   pm
)
{
    m_pm = pm;
    m_modePrevious = -1;
    m_currentIdle = 0;
    m_currentRing = 0;
    m_currentCall = 0;
    SetMode(EM_MODE_IDLE);
}


EffectManager :: ~EffectManager()
{
    Destroy();
}
void EffectManager :: Destroy ()
{
    m_canvas.Destroy();
}

void EffectManager :: InitPanels()
{
    m_canvas.InitPanels();
}

void EffectManager :: InitPins()
{
    pinMode(PHONE_PIN_RING_DETECT, INPUT);
    pinMode(BOOTH_PIN_LASER, OUTPUT);


    digitalWrite(BOOTH_PIN_LASER, LOW);
}


void EffectManager :: InitHardware()
{
    InitPanels();
    InitSpectrum();
    InitPins();
}

void EffectManager :: InitSpectrum()
{
    m_spectrum.InitSpectrumPins();
}



void EffectManager :: AddEffectsArrays
( 
    Effect                                      *   effectsIdle,
    char                                            sizeIdle,
    Effect                                      *   effectsRing,
    char                                            sizeRing,
    Effect                                      *   effectsCall,
    char                                            sizeCall,
    Effect                                      *   effectsOver,
    char                                            sizeOver
)
{
    m_effectsIdle = effectsIdle;
    m_effectsCall = effectsCall;
    m_effectsRing = effectsRing;
    m_effectsOver = effectsOver;

    m_sizeIdle = sizeIdle;
    m_sizeRing = sizeRing;
    m_sizeCall = sizeCall;
    m_sizeOver = sizeOver;
}

void EffectManager :: SetMode
(
    char                                            mode
)
{
    m_mode = mode;
}


void EffectManager :: EnableEffects()
{
    if (m_pm->GetPowerStatus() == PM_POWER_OFF)
    {
        m_pm->PowerUp();
        // Idea: wait another bit before switching mode?
        m_mode = EM_MODE_IDLE;
        m_pollDelay += EM_FADE_DELAY_MS;
    }

}

void EffectManager :: DisableEffects()
{
    if (m_mode & EM_MODE_DISABLE) // Covers all the disable modes
    {
        m_mode = EM_MODE_DISABLE;
    }
}

bool EffectManager :: EffectsDisabled()
{
    // We are only actually disabled 
    return (m_mode == EM_MODE_DISABLE_STANDBY);
}

unsigned short * EffectManager :: GetSpectrum()
{
    return m_spectrum.GetSpectrum();
}

unsigned short EffectManager :: GetRandomNumber()
{
    return random(0xFFFF);
}

// This just sets up the reboot mode and an initial reboot.  You must then
// repeatedly check the status of the reboot.
void EffectManager :: RebootPanels()
{
    m_rebooting = true;
    RebootComplete();
}

bool EffectManager :: RebootComplete()
{
    if (m_rebooting)
    {
        if (m_pm->Ready())
        {
            if (m_pm->GetPowerStatus() == PM_POWER_ON)
            {
                m_pm->PowerDown();
            } else {
                m_pm->PowerUp();
                // Idea: wait another bit before switching mode?
                m_rebooting = false;
            }
        }
        return false;
    } 
    return true;
}


void EffectManager :: Poll
(
    unsigned long                                   time,
    bool                                            offHook
)
{
    EM_DEBUG("BEGIN POLL");
    static bool runEffects = false;
    //static int ringer = 0;
    //ringer++;
    static unsigned short effectCount = 0;
    EM_DEBUG(effectCount);
   
    if (effectCount == 0)
    {
        if (++m_currentIdle >= m_sizeIdle)
        {
            m_currentIdle = 0;
        }
        if (++m_currentRing >= m_sizeRing)
        {
            m_currentRing = 0;
        }
        if (++m_currentCall >= m_sizeCall)
        {
            m_currentCall = 0;
        }
        if (++m_currentOver >= m_sizeOver)
        {
            m_currentOver = 0;
        }
        effectCount = 4096 + (GetRandomNumber() >> 4);
    }
    effectCount--;
    

    if ((time - m_pollDelay) > EFFECT_POLL_DELAY_MS)
    {
        m_pollDelay = time;
        // Which index are we in the array of effects for this mode:
        char currentEffect = 0; // 0 in case it is not specified

        static Effect * e;
        switch (m_mode)
        {
        case EM_MODE_IDLE:
        case EM_MODE_RING:
        default:
            if
            (
                digitalRead(PHONE_PIN_RING_DETECT)
            //||  (ringer & 4096)
            )
            {
                m_mode = EM_MODE_RING;
                currentEffect = m_currentRing;
            } else {
                m_mode = EM_MODE_IDLE;
                currentEffect = m_currentIdle;
            }

            if (offHook)
            {
                m_mode = EM_MODE_CALL;
            }

            break;
        case EM_MODE_CALL:
            currentEffect = m_currentCall;
            if (!offHook)
            {
                m_mode = EM_MODE_CALLENDED;
            }
            if ((time - m_duration) > MAX_CALL_DURATION_MS)
            {
                m_mode = EM_MODE_CALL_OVERTIME;
            }
            break;
        case EM_MODE_CALL_OVERTIME:
            // Whoops, the person was on the line too long:
            currentEffect = m_currentOver;
            if (!offHook)
            {
                m_mode = EM_MODE_CALLENDED;
            }
            break;
        case EM_MODE_CALLENDED_REBOOTED:
            if (RebootComplete())
            {
                m_mode = EM_MODE_IDLE;
            }
            break;
        case EM_MODE_DISABLE_STANDBY:
            // Do nothing.
            m_pollDelay += EM_FADE_DELAY_MS; // To reduce poll time.
            break;
        }

        if (m_modePrevious != m_mode)
        {
            runEffects = false;
            // Triggered once per mode change.
            switch(m_mode)
            {
            case EM_MODE_IDLE:
                EM_DEBUG("POLL, ONCE: IDLE");
                e = m_effectsIdle;
                currentEffect = m_currentIdle;
                runEffects = true;
                break;
            case EM_MODE_RING:
                EM_DEBUG("POLL, ONCE: RING");
                e = m_effectsRing;
                currentEffect = m_currentRing;
                runEffects = true;
                break;
            case EM_MODE_CALL:
                EM_DEBUG("POLL, ONCE: CALL");

                // The current time so we can shut down the effects if the
                // person is taking too long
                m_duration = time;
                effectCount = 0;
                // Restore the lights!
                currentEffect = m_currentCall;
                e = m_effectsCall;
                runEffects = true;
                break;               
            case EM_MODE_CALL_OVERTIME:
                EM_DEBUG("POLL, ONCE: OVER");
                // The current time so we can shut down the effects if the
                // person is taking too long
                effectCount = 0;
                // Restore the lights!
                e = m_effectsOver;
                currentEffect = m_currentOver;
                runEffects = true;
                break;               
            case EM_MODE_CALLENDED:
                EM_DEBUG("POLL, ONCE: CALLENDED");
                // The call ended, fade those panels out.
                m_canvas.FadeToBlack();
                m_pollDelay += EM_FADE_DELAY_MS;
                m_mode = EM_MODE_CALLENDED_FADE_END;
                break;
            case EM_MODE_CALLENDED_FADE_END:
                EM_DEBUG("POLL, ONCE: FADEEND");
                // Wait until the power manager is ready, and then
                // reboot the panels:
                RebootPanels();
                m_mode = EM_MODE_CALLENDED_REBOOTED;
                /*if (m_pm->Ready())
                {
                    if (m_pm->GetPowerStatus() == PM_POWER_ON)
                    {
                        m_pm->PowerDown();
                    } else {
                        m_pm->PowerUp();
                        // Idea: wait another bit before switching mode?
                        m_mode = EM_MODE_IDLE;
                    }
                }*/
                break;
            case EM_MODE_DISABLE:
                EM_DEBUG("POLL, ONCE: DISABLE");
                // Fade out
                m_canvas.FadeToBlack();
                m_pollDelay += EM_FADE_DELAY_MS; // 2 seconds delay
                m_mode = EM_MODE_DISABLE_FADE_END;
                break;
            case EM_MODE_DISABLE_FADE_END:
                EM_DEBUG("POLL, ONCE: DISABLE_FADEEND");
                // We're all faded out, terminate power.  The only way out of 
                // this mode is a call to EnableEffects().
                m_pm->PowerDown();
                m_mode = EM_MODE_DISABLE_STANDBY;
                break;
            default:
                break;
            };
            m_modePrevious = m_mode;
        }
        if (runEffects)
        {
            EM_DEBUG("runEffects");
            EM_DEBUG((int)currentEffect);
            m_spectrum.ReadSpectrum();
            e[currentEffect].func(&m_canvas, const_cast<EffectManager *>(this), EFFECTMODE_LOOP);
            m_canvas.BlitToPanels();
        }
    }
}




#ifdef ISR_ANIMATOR


EffectManager *emGlobal;

void ISRGlobal()
{
    emGlobal->Poll();
}

void EffectManager :: InstallAnimator ()
{
    // microseconds
    Timer1.initialize(EFFECT_POLL_DELAY_MS * 1000);
    //typedef void (EffectManager ::* EMISR)();
    //EMISR p = &EffectManager::ISR;
    emGlobal = this;
    Timer1.attachInterrupt(ISRGlobal);
}

#endif


