#ifndef __GameLib_Tro
#define __GameLib_Tro

#include "SDL2/SDL.h"
#include "Core.h"
#include "../GameImport.h"

typedef struct gameActor {
    object * obj;
    bool (*onSpawn)(void * self);
    bool (*update)(void * self);
    bool (*onDestroy)(void * self);
} gameActor;

typedef struct gameArea {
    object * obj;
    Uint8 width, height;
    Uint8 layer;    // Max 64;
    Uint64 layerMask;
    bool (*onTrigger)(void * self, void ** overlap);
} gameArea;

#endif