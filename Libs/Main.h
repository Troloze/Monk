#ifndef __Main_Tro
#define __Main_Tro

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "Core.h"
#include "InputLib.h"
#include "RenderLib.h"
#include "AudioLib.h"
#include "ColorLib.h"
#include "FileLib.h"
#include "ErrorLib.h"
#include "GameLib.h"



#define Splash false

bool init();
void shut();

#endif