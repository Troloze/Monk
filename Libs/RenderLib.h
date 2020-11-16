#ifndef __RenderLib_Tro
#define __RenderLib_Tro
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define width 1024
#define height 768
#define vWidth 256
#define vHeight 192
#define wRatio width/vWidth
#define hRatio height/vHeight

SDL_Texture * loadTexture(SDL_Renderer * renderer, const char * path);

bool renderInit(SDL_Window * window, SDL_Renderer * renderer);
void renderShut(SDL_Window * window, SDL_Renderer * renderer);

#endif