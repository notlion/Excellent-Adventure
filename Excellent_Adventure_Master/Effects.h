#ifndef _EFFECTS_H
#define _EFFECTS_H
#include "EffectManager.h"



int RingFlash(Canvas *c, EffectManager *em, char mode);


int SimpleSpectrum(Canvas *c, EffectManager *em, char mode);
int SimpleColumns(Canvas *c, EffectManager *em, char mode);
int Spotlight(Canvas *c, EffectManager *em, char mode);
int CheckerBoard(Canvas *c, EffectManager *em, char mode);

int BlitzyIdle(Canvas *c, EffectManager *em, char mode);

// What to do when people have taken too long
int Overtime(Canvas *c, EffectManager *em, char mode);



#endif
