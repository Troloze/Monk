#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#undef main

int main() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
        printf("SDL não pôde ser inicializado! Erro: %s\n", SDL_GetError());
        return false;
    }

    SDL_Window * mWindow = NULL;
    SDL_Renderer * mRenderer = NULL;
    SDL_Surface * nSurface = NULL;
    SDL_Texture * nTexture = NULL;

    bool running = true;

    mWindow = SDL_CreateWindow("Troloze's Amazing Adventure(TM)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN);
    if(mWindow == NULL) {
        printf("Janela não pôde ser criada! Erro: %s\n", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_SOFTWARE);
    if(mRenderer = NULL) {
        printf("Renderizador não pôde ser criado! Erro %s\n", SDL_GetError());
        return false;
    }

    int imageFlags = IMG_INIT_PNG;  
    if( !(IMG_Init(imageFlags) & imageFlags)) {
        printf("Couldn't initialize image subsistems! Error: %s\n", IMG_GetError());
        return false;
    }    
 
    SDL_Event e;
    nSurface = IMG_Load("Test.png");
    if (nSurface == NULL) {
        printf("error %s\n", IMG_GetError());
    }
    nTexture = SDL_CreateTextureFromSurface(mRenderer, nSurface);
    if (nTexture == NULL) {
        printf("Error: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(nSurface);

    
    while(running) {
        while (SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                running = false;
            } 
        }
        SDL_RenderClear(mRenderer);
        SDL_RenderCopy(mRenderer, nTexture, NULL, NULL);
        SDL_RenderPresent(mRenderer);//
    }
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    mRenderer = NULL;
    mWindow = NULL;
    IMG_Quit();
    SDL_Quit();

    return 0;
}