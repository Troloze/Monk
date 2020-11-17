#ifndef __Main_Tro
#define __Main_Tro
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_mixer.h>
#include "InputLib.h"
#include "RenderLib.h"
#include "AudioLib.h"

bool init(SDL_Window ** window);
void shut(SDL_Window ** window);

#endif