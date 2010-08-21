#ifndef _EFFECT_MANAGER_H
#define _EFFECT_MANAGER_H
#include "Globals.h"
#include "Spectrum.h"
#include "PowerManagement.h"
#include "Canvas.h"

#define EM_MODE_IDLE                0x00
#define EM_MODE_RING                0x01

// 0b 
#define EM_MODE_CALL                0x10
#define EM_MODE_CALLENDED           0x11
#define EM_MODE_CALLENDED_FADE_END  0x12

// 64 + submodes
#define EM_MODE_DISABLE             0x40
#define EM_MODE_DISABLE_FADE_END    0x41
#define EM_MODE_DISABLE_STANDBY     0x42

#define EFFECTMODE_INTRO            0
#define EFFECTMODE_LOOP             1
#define EFFECTMODE_OUTRO            2

#define EM_CALL_DURATION_FRAMES     ((1000 / EFFECT_POLL_DELAY_MS) * MAX_CALL_DURATION_SEC)

// Idle -> Ring -> 


// The effect manager (EM) controls the effect scheduling for each booth.  It 
// is driven by a timer interrupt service routine (ISR), and switches between
// various effects based on different conditions.

class EffectManager;

struct Effect
{
    int                                             (*func)
                                                    (
                                                        Canvas *, 
                                                        EffectManager *,
                                                        char
                                                    );
    int                                             flags;
};




class EffectManager
{
    Effect                                      *   m_effectsIdle;
    Effect                                      *   m_effectsRing;
    Effect                                      *   m_effectsCall;

    char                                            m_sizeIdle;
    char                                            m_currentIdle;
    char                                            m_sizeRing;
    char                                            m_currentRing;
    char                                            m_sizeCall;
    char                                            m_currentCall;
    char                                            m_mode;
    char                                            m_modePrevious;
    bool                                            m_disabled;
    unsigned long                                   m_pollDelay;
    unsigned long                                   m_duration;
    Canvas                                          m_canvas;
    Spectrum                                        m_spectrum;
    PowerManagement                             *   m_pm;
    void InitPanels();
    void InitSpectrum();
    void InitPins();
public:
    EffectManager
    (
        PowerManagement                         *   pm
    );
    ~EffectManager();

    void Destroy();
    // An effect is a static function that is called for each frame of the
    // animation.  The "type" of an effect is its 

    void InitHardware();

    void AddEffectsArrays
    ( 
        Effect                                  *   effectsIdle,
        char                                        sizeIdle,
        Effect                                  *   effectsRing,
        char                                        sizeRing,
        Effect                                  *   effectsCall,
        char                                        sizeCall
    );

    void SetMode
    (
        char                                        mode
    );

    unsigned short GetRandomNumber();

    void InstallAnimator();
    unsigned short * GetSpectrum();
    void EnableEffects();
    void DisableEffects();
    bool EffectsDisabled();
    void Poll
    (
        unsigned long                               time,
        bool                                        offHook
        
    );
    
};

//extern void ISRGlobal();


#endif
