#include <stdio.h>
#include <stdbool.h>
//#include <SDL2/SDL.h>
#include "Libs/MiscLib.h"

#undef main

int main() {
    unsigned char a = 0b10101001, b, c, d;
    
    a = convertValueToByte(2, 2, 0, a);
    printf("%u ",a);

    /*SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window * window = SDL_CreateWindow("Hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 1024, SDL_WINDOW_SHOWN);
    SDL_Surface * surface = SDL_GetWindowSurface(window);

    SDL_Surface * oldSurface = SDL_LoadBMP("Sprites/Test2.bmp");
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
    printf("The pixel format is: %s\n", SDL_GetPixelFormatName((surface->format->format)));


    for (int i = 0; i < pitch; i++) {
        for(int i1 = 0; i1 < width; i1++) {
            printf("%08X ", pixels[i * width + i1]);
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < olPitch; i++) {
        for(int i1 = 0; i1 < olWidth; i1++) {
            printf("%08X ", olPixels[i * olWidth + i1]);
        }
        printf("\n");
    }

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