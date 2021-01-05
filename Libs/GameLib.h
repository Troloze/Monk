#ifndef __GameLib_Tro
#define __GameLib_Tro

#include "SDL2/SDL.h"
#include "Core.h"
#include "../GameImport.h"

#define OBJ_MASK_GAME 3
#define GAME_TYPE_AREA 0

#define AREA_NO_MASK 0
#define AREA_L_MASK(x) (1 << (x))

#define AREA_NO_CONTACT 0
#define AREA_ON_ENTER 1
#define AREA_OVERLAP 2
#define AREA_ON_EXIT 3

typedef struct gameArea {
    object * obj;
    char * tag;
    
    Uint8 width, height;
    
    Uint8 layer;        // Max 64;
    Uint64 layerMask;   // To check collision with.
    
    Uint32 pos;
    
    bool active;
} gameArea;

typedef struct gameOverlapFunc {
    bool (*onEnter)(void * self, void * overlap);
    bool (*onOverlap)(void * self, void * overlap);
    bool (*onExit)(void * self, void * overlap);
} gameOverlapFunc;

gameArea * createArea(Uint16 w, Uint16 h, Uint8 layer, Uint64 layerMask, object * parent, char * instance);

gameArea * addAreaToDump(gameArea * toDump);

bool freeArea(gameArea * area);

object * cloneArea(object * source, char * inst);

bool updateArea();

void printOverlap();

bool addFuncToInstOverlap(char * inst1, char * inst2, bool (*onEnter)(void * self, void * overlap), bool (*onOverlap)(void * self, void * overlap), bool (*onExit)(void * self, void * overlap));

bool gameUpdate();

bool gameInit();

bool gameShut();

#endif