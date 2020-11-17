#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#undef main

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window * window = SDL_CreateWindow("Hello", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 1024, SDL_WINDOW_SHOWN);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface * surface = SDL_GetWindowSurface(window);

    SDL_Surface * newSurface = SDL_LoadBMP("Sem título.bmp");

    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, newSurface);

    SDL_FreeSurface(newSurface);

    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                running = false;
            } 
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        //SDL_BlitSurface(newSurface, NULL, surface, NULL);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        
        SDL_RenderPresent(renderer);
    }//*/
    SDL_Delay(1000);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}