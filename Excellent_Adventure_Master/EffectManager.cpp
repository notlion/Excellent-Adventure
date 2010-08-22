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
    m_disablePanels = false;
}

void EffectManager :: DisableEffects()
{
    m_disablePanels = true;
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
    static unsigned short effectCount = 0;
    EM_DEBUG(effectCount);
   
    

    if ((time - m_pollDelay) > EFFECT_POLL_DELAY_MS)
    {
        //static int ringer = 0;
        //ringer++;
        
        m_pollDelay = time;
        bool switchEffect = (effectCount == 0);

        // Which index are we in the array of effects for this mode:
        static char *currentEffect; // 0 in case it is not specified

        static Effect * effects;
        switch (m_mode)
        {
        case EM_MODE_IDLE:
        case EM_MODE_RING:
        default:
            if (m_disablePanels)
            {
                m_mode = EM_MODE_DISABLE;
                m_panelsDisabled = false;
            }
            if
            (
                digitalRead(PHONE_PIN_RING_DETECT)
            //||  (ringer & 32)
            )
            {
                m_mode = EM_MODE_RING;
            } else {
                m_mode = EM_MODE_IDLE;
                if (switchEffect)
                {
                    if (++m_currentIdle >= m_sizeIdle)
                    {
                        m_currentIdle = 0;
                    }
                }
            }
            
            if (offHook)
            {
                m_mode = EM_MODE_CALL;
            }
            break;
        case EM_MODE_CALL:
            if (switchEffect)
            {
                if (++m_currentCall >= m_sizeCall)
                {
                    m_currentCall = 0;
                }
                effectCount = 4096 + (GetRandomNumber() >> 4);
            }
            //currentEffect = m_currentCall;
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
            if (switchEffect)
            {
                if (++m_currentOver >= m_sizeOver)
                {
                    m_currentOver = 0;
                }

            }            

            // Whoops, the person was on the line too long:
            //currentEffect = m_currentOver;
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
            if (m_disablePanels == false)
            {
                m_panelsDisabled = false;
                RebootPanels();
                m_mode = EM_MODE_CALLENDED_REBOOTED;
            }
            
            // Do nothing.
            m_pollDelay += EM_FADE_DELAY_MS; // To reduce poll time.

            break;
        }

        if (m_modePrevious != m_mode)
        {
            // All variables in this section must be persistent
            runEffects = false;
            // Triggered once per mode change.
            switch(m_mode)
            {
            case EM_MODE_IDLE:
                EM_DEBUG("POLL, ONCE: IDLE");
                effects = m_effectsIdle;
                currentEffect = &m_currentIdle;
                runEffects = true;
                break;
            case EM_MODE_RING:
                EM_DEBUG("POLL, ONCE: RING");
                effects = m_effectsRing;
                currentEffect = &m_currentRing;
                if (++m_currentRing >= m_sizeRing)
                {
                    m_currentRing = 0;
                }
                runEffects = true;
                break;
            case EM_MODE_CALL:
                EM_DEBUG("POLL, ONCE: CALL");

                // The current time so we can shut down the effects if the
                // person is taking too long
                m_duration = time;
                effectCount = 0;
                // Restore the lights!
                currentEffect = &m_currentCall;
                effects = m_effectsCall;
                runEffects = true;
                break;               
            case EM_MODE_CALL_OVERTIME:
                EM_DEBUG("POLL, ONCE: OVER");
                // The current time so we can shut down the effects if the
                // person is taking too long
                effectCount = 0;
                // Restore the lights!
                effects = m_effectsOver;
                currentEffect = &m_currentOver;
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
            EM_DEBUG((int)(*currentEffect));
            //EM_DEBUG("ringer");
            //EM_DEBUG((int)(ringer));
            m_spectrum.ReadSpectrum();
            effects[*currentEffect].func(&m_canvas, const_cast<EffectManager *>(this), EFFECTMODE_LOOP);
            m_canvas.BlitToPanels();
        }
        if (effectCount > 0)
        {
            effectCount--;
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


