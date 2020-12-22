#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include "../Libs/InputLib.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define square(v) (v * v)
#define cube(v) (v * v * v)
#define pot4(v) (v * v * v * v)
#define pot5(v) (v * v * v * v * v)

#define fps 30
#define fpsms (1000/fps)

#undef main

int main() {
    SDL_Window * test;
    Mix_Chunk * chunk;
    int tester;
    bool r = true, w = false;
    long long int fpsid;
    int delta;
    Sint8 volume = 127;
    int x = 10, y = 10;
    int angle, dist, vdist;
    double auxDist, auxVolum, auxVolumMin, auxVolum2, ddist;
    Sint8 panning;

    SDL_Init(SDL_INIT_AUDIO);
    test = SDL_CreateWindow("Bom", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 100, 100, 0);

    SDL_Event e;

    inputInit();
    createAxis("Volume",255, SDL_SCANCODE_W, SDL_SCANCODE_S);
    createAxis("Panning", 4, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT);
    createAxis("Horizontal",255, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
    createAxis("Vertical", 255, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT);
    createTrigger("Exit", SDL_SCANCODE_ESCAPE);
    createTrigger("Toggle", SDL_SCANCODE_SPACE);

    Mix_Init(0);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(1);

    chunk = Mix_LoadWAV("../Audio/Test.wav");
    tester = Mix_PlayChannel(-1, chunk, -1);
    printf("%d\n", tester);

    while(r) {
        fpsid = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                r = false;
            } 
        }

        inputUpdate();
        if (getTrigger("Exit").value == 1) {
            r = false;
        }
        if (getTrigger("Toggle").value == 1) {
            w = !w;
        }
        if (!((volume == 127 && getAxis("Volume").value > 0) || (volume == 0 && getAxis("Volume").value < 0))) volume += getAxis("Volume").value;
        if (w) {
            x += getAxis("Horizontal").value;
            y += getAxis("Vertical").value;
            angle = (atan2(y, x)) * 180 / M_PI;
            if (angle < 0) angle += 360;
            dist = min(sqrt((x * x) + (y * y)), 255);
            
            Mix_Volume(tester, volume);
            Mix_SetPosition(tester, angle, vdist);
        }
        if (!w) {
            if (!((volume == 127 && getAxis("Volume").value > 0) || (volume == 0 && getAxis("Volume").value < 0))) volume += getAxis("Volume").value;
            panning = 127 * getAxis("Panning").value;


            Mix_Volume(tester, volume);    
            Mix_SetPanning(tester, 127 - panning, 127 + panning);
        }

        delta = SDL_GetTicks() - fpsid;
        if (delta < fpsms) SDL_Delay(fpsms - delta);
    }
    
    Mix_ExpireChannel(tester, 0);
    Mix_FreeChunk(chunk);
    Mix_AllocateChannels(0);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyWindow(test);
    inputShut();
    SDL_Quit();

    return 0;
}