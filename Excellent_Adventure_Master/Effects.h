#ifndef _EFFECTS_H
#define _EFFECTS_H
#include "EffectManager.h"

#define NEWEFFECT(NAME) int NAME (Canvas *c, EffectManager *em, char mode)

#define EFFECT(NAME)    {&(NAME), 0}

// Idle effects

NEWEFFECT( BlitzyIdle           );


NEWEFFECT( RingRainbow       );
NEWEFFECT( RingRadio         ); 
NEWEFFECT( RingFlash         );  
NEWEFFECT( SimpleSpectrum    );
NEWEFFECT( WarpSpectrum      );
NEWEFFECT( PinwheelSpectrum  );
NEWEFFECT( ElevatorSpectrum  );
NEWEFFECT( SolidColors       );
NEWEFFECT( SolidSpectrum     );
NEWEFFECT( SpotlightSpectrum );
NEWEFFECT( SimpleColumns     );
NEWEFFECT( Spotlight         );
NEWEFFECT( CheckerBoard      );
NEWEFFECT( Glitz             );
NEWEFFECT( Barrel            );
NEWEFFECT( LightTornado      );
NEWEFFECT( SoothingRiser     );
NEWEFFECT( Barrel            );
                             
NEWEFFECT( Overtime          );




/*
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
*/    
    /*
int BlitzyIdle        (Canvas *c, EffectManager *em, char mode);

// Ring Effects
int RingRainbow       (Canvas *c, EffectManager *em, char mode);
int RingRadio         (Canvas *c, EffectManager *em, char mode);
int RingFlash         (Canvas *c, EffectManager *em, char mode);

// More effects!
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
int Glitz             (Canvas *c, EffectManager *em, char mode);
int Barrel            (Canvas *c, EffectManager *em, char mode);
int LightTornado      (Canvas *c, EffectManager *em, char mode);
int LightTornado      (Canvas *c, EffectManager *em, char mode);
*/
// Overtime effects
// What to do when people have taken too long
//int Overtime          (Canvas *c, EffectManager *em, char mode);


// Ideas
// Single Color Pulsing with Single Pixel
// Pulsing Spotlight on Phone


#define EFFECTS_NUM_IDLE 1
#define EFFECTS_NUM_RING 2
#define EFFECTS_NUM_CALL 11
#define EFFECTS_NUM_OVER 1

static Effect effectsIdle[EFFECTS_NUM_IDLE] =
{ 
    EFFECT( BlitzyIdle )
//    {&Barrel,0}
};

static Effect effectsRing[EFFECTS_NUM_RING] =
{ 
    EFFECT( RingFlash )
,   EFFECT( RingRainbow )
};

static Effect effectsCall[EFFECTS_NUM_CALL] =
{
    EFFECT( SpotlightSpectrum )
,   EFFECT( Barrel )
,   EFFECT( LightTornado )
,   EFFECT( CheckerBoard )
,   EFFECT( Spotlight )
,   EFFECT( SoothingRiser )
,   EFFECT( WarpSpectrum )
,   EFFECT( ElevatorSpectrum )
,   EFFECT( PinwheelSpectrum )
,   EFFECT( Glitz )
,   EFFECT( SolidColors )
};

static Effect effectsOver[EFFECTS_NUM_OVER] =
{ 
    EFFECT( Overtime )
};



#endif
