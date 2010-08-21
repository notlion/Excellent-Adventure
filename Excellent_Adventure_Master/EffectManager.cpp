
#ifdef ISR_ANIMATOR
#include <TimerOne.h>
#endif
#include <WProgram.h>
#include "EffectManager.h"

//#include "TimerOne.h"



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
    char                                            sizeCall
)
{
    m_effectsIdle = effectsIdle;
    m_effectsCall = effectsCall;
    m_effectsRing = effectsRing;

    m_sizeIdle = sizeIdle;
    m_sizeRing = sizeRing;
    m_sizeCall = sizeCall;
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

void EffectManager :: Poll
(
    unsigned long                                   time,
    bool                                            offHook
)
{
    //static int ringer = 0;
    //ringer++;
    static unsigned short effectCount = 0;
   
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
        effectCount = 0x8000 | (GetRandomNumber() >> 1);
    }
    effectCount--;


    if ((time - m_pollDelay) > EFFECT_POLL_DELAY_MS)
    {
        m_pollDelay = time;
        bool runEffects = false;

        char currentEffect;

        Effect * e;
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
            break;
        case EM_MODE_CALLENDED_FADE_END:
            // Wait until the power manager is ready, and then
            // reboot the panels:
            
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
            m_mode = EM_MODE_IDLE;
            break;
        case EM_MODE_DISABLE_STANDBY:
            // Do nothing.
            m_pollDelay += EM_FADE_DELAY_MS; // To reduce poll time.
            break;
        }

        if (m_modePrevious != m_mode)
        {
            // Triggered once per mode change.
            switch(m_mode)
            {
            case EM_MODE_IDLE:
                e = m_effectsIdle;
                runEffects = true;
                break;
            case EM_MODE_RING:
                e = m_effectsRing;
                runEffects = true;
                break;
            case EM_MODE_CALL:
                // Restore the lights!
                e = m_effectsCall;
                runEffects = true;
                break;               
            case EM_MODE_CALLENDED:
                // The call ended, fade those panels out.
                m_canvas.FadeToBlack();
                m_pollDelay += EM_FADE_DELAY_MS;
                m_mode = EM_MODE_CALLENDED_FADE_END;
                break;
            case EM_MODE_DISABLE:
                // Fade out
                m_canvas.FadeToBlack();
                m_pollDelay += EM_FADE_DELAY_MS; // 2 seconds delay
                m_mode = EM_MODE_DISABLE_FADE_END;
                break;
            case EM_MODE_DISABLE_FADE_END:
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


