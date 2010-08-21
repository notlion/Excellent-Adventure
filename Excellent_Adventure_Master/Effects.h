#ifndef _EFFECTS_H
#define _EFFECTS_H
#include "EffectManager.h"


int SimpleSpectrum   (Canvas *c, EffectManager *em, char mode);
int WarpSpectrum     (Canvas *c, EffectManager *em, char mode);
int PinwheelSpectrum (Canvas *c, EffectManager *em, char mode);
int ElevatorSpectrum (Canvas *c, EffectManager *em, char mode);

int SimpleColumns    (Canvas *c, EffectManager *em, char mode);
int Spotlight        (Canvas *c, EffectManager *em, char mode);
int CheckerBoard     (Canvas *c, EffectManager *em, char mode);


#endif
