#ifndef _EFFECTS_H
#define _EFFECTS_H
#include "EffectManager.h"


int SimpleSpectrum    (Canvas *c, EffectManager *em, char mode);
int WarpSpectrum      (Canvas *c, EffectManager *em, char mode);
int PinwheelSpectrum  (Canvas *c, EffectManager *em, char mode);
int ElevatorSpectrum  (Canvas *c, EffectManager *em, char mode);
int SolidColors       (Canvas *c, EffectManager *em, char mode);
int SolidSpectrum     (Canvas *c, EffectManager *em, char mode);
int SpotlightSpectrum (Canvas *c, EffectManager *em, char mode);
                      
int SimpleColumns     (Canvas *c, EffectManager *em, char mode);
int Spotlight         (Canvas *c, EffectManager *em, char mode);
int CheckerBoard      (Canvas *c, EffectManager *em, char mode);
                      
int RingRainbow       (Canvas *c, EffectManager *em, char mode);
int RingRadio         (Canvas *c, EffectManager *em, char mode);
int RingFlash         (Canvas *c, EffectManager *em, char mode);
int BlitzyIdle        (Canvas *c, EffectManager *em, char mode);

// What to do when people have taken too long
int Overtime          (Canvas *c, EffectManager *em, char mode);


// Ideas
// Single Color Pulsing with Single Pixel
// Pulsing Spotlight on Phone


#define EFFECTS_NUM_IDLE 1
#define EFFECTS_NUM_RING 2
#define EFFECTS_NUM_CALL 1
#define EFFECTS_NUM_OVER 1

static Effect effectsIdle[EFFECTS_NUM_IDLE] =
{ 
    {&BlitzyIdle, 0}
};

static Effect effectsRing[EFFECTS_NUM_RING] =
{ 
    {&RingFlash, 0}
,   {&RingRainbow, 0}
};

static Effect effectsCall[EFFECTS_NUM_CALL] =
{
    {&RingRainbow, 0}
//     {&CheckerBoard, 0}
// ,   {&Spotlight, 0}
// ,   {&WarpSpectrum, 0}
// ,   {&ElevatorSpectrum, 0}
// ,   {&PinwheelSpectrum, 0}
// ,   {&SolidColors, 0}
};

static Effect effectsOver[EFFECTS_NUM_OVER] =
{ 
    {&Overtime, 0}
};



#endif
