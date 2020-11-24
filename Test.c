#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <SDL2/SDL.h>
//#include "Libs/MiscLib.h"

#undef main

struct a {
    int inteiro;
    int * vetor;
};

int main() {
    struct a b;
    b.vetor = malloc(sizeof(int));
    printf("%d ", b.vetor);
    free(b.vetor);
    b.vetor = NULL;
    printf("%d ", b.vetor);
     

    /*
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window * window = SDL_CreateWindow("Hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 1024, SDL_WINDOW_SHOWN);
    SDL_Surface * surface = SDL_GetWindowSurface(window);

    SDL_Surface * oldSurface = SDL_LoadBMP("Sprites/Test.bmp");
    SDL_Surface * newSurface = SDL_ConvertSurfaceFormat(oldSurface, surface->format->format, 0);
    SDL_Rect  rect;
    rect.h = 1024;
    rect.w = 1024;
    rect.x = 0;
    rect.y = 0;
    Uint32 *olPixels = (Uint32*) oldSurface->pixels;
    Uint32 *pixels = (Uint32*) newSurface->pixels;
    int pitch = newSurface->pitch;
    int width = newSurface->w;
    int olPitch = oldSurface->pitch;
    int olWidth = oldSurface->w;
    printf("The pixel format is: %s\n", SDL_GetPixelFormatName((surface->format->format)));//*

    rect.x = 2;
    rect.y = 2;
    rect.h = 4;
    rect.w = 4;
    SDL_FillRect(newSurface, &rect, 0xFF58A9F3);

    rect.h = 1024;
    rect.w = 1024;
    rect.x = 0;
    rect.y = 0;

    SDL_BlitScaled(newSurface, NULL, surface, &rect);


    SDL_UpdateWindowSurface(window);

    SDL_FreeSurface(newSurface);

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                running = false;
            } 
        }


    }//
    SDL_DestroyWindow(window);
    SDL_Quit();//*/
    return 0;
}