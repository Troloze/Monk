#include "Libs/Main.h"

#undef main

int main(int argc, char ** argv) {
    // Declarando as variáveis principais aqui.
    bool running = true;        // Bandeira do loop principal

    Uint32 frameID;             // Auxiliar para estabilização de frames.
    Uint32 frameDelta;          // Auxiliar para estabilização de frames.
    int FPS = 60;               // Limite de FPS.
    int fpsMs = 1000/FPS;       // Tempo, em milisegundos, que um frame consome.

    init();   // Inicializando todos os sistemas do SDL

    renderSprite * dRS;
    renderPalette * newP = createPalette(0x00000000,0xFFD9B76A,0xFF3E3012,0xFFFFFFFF);
    renderPalette * metaP = createPalette(0x00000000,0xFFD9B76A,0xFF3E3012,0xFFFFFFFF);
    renderSprite a = createSprite(getCoreSprites()[CORE_SPRITE_UNDEFINED].pixels, NULL, NULL, 0, 0, SPRITE_STATE_SHOWN);

    renderMetasprite newMeta = createMetaspriteFromSheet("Sprites/Logo.bmp", 0, 0, 32, 32, metaP, NULL, 0, 0);

    dRS = getDefaultSprite();

    createTrigger("Exit", SDL_SCANCODE_ESCAPE);
  
    SDL_Event e;    // Variável que vai receber todos os eventos do SDL.

    while (running) {
        frameID = SDL_GetTicks();

        
        while (SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                running = false;
            } 
        }

        // Update do sistema de entrada.
        inputUpdate();
        if (getTrigger("Exit").value == 1) {
            running = false;
        }


        if (frameID/fpsMs < 64 && Splash) {
            metaP->color1 = colorMultiplyByValue(newP->color1, (frameID/fpsMs * 4));
            metaP->color2 = colorMultiplyByValue(newP->color2, (frameID/fpsMs * 4));
            metaP->color3 = colorMultiplyByValue(newP->color3, (frameID/fpsMs * 4));
            metaP->color4 = colorMultiplyByValue(newP->color4, (frameID/fpsMs * 4));
        } else if (frameID/fpsMs <= 64*2 && Splash) {
            metaP->color1 = newP->color1;
            metaP->color2 = newP->color2;
            metaP->color3 = newP->color3;
            metaP->color4 = newP->color4;
        } else if (frameID/fpsMs <= 64*3 && Splash) {
            metaP->color1 = colorMultiplyByValue(newP->color1, 4 * (64 - (frameID/fpsMs % 64)));
            metaP->color2 = colorMultiplyByValue(newP->color2, 4 * (64 - (frameID/fpsMs % 64)));
            metaP->color3 = colorMultiplyByValue(newP->color3, 4 * (64 - (frameID/fpsMs % 64)));
            metaP->color4 = colorMultiplyByValue(newP->color4, 4 * (64 - (frameID/fpsMs % 64)));
        } else {
            metaP->color1 = 0xFF;
            metaP->color2 = 0;
            metaP->color3 = 0;
            metaP->color4 = 0;
        }

        dRS = getDefaultSprite();
        void * palette = dRS->palette;
        if (getMouse().leftButtonState == 0) for (int i = 0; i < 16; i++) dRS->pixels[i] = getCoreSprites()[CORE_SPRITE_CURSOR_1].pixels[i];
        else for (int i = 0; i < 16; i++) dRS->pixels[i] = getCoreSprites()[CORE_SPRITE_CURSOR_2].pixels[i];
        dRS->object->localX = (int) (getMouse().x * getWindowRatioX());
        dRS->object->localY = (int) (getMouse().y * getWindowRatioY());
        

        coreUpdate();
        renderUpdate();
        
        frameDelta = SDL_GetTicks() - frameID;
        if (frameDelta < fpsMs) SDL_Delay(fpsMs - frameDelta);
    }
    shut();

    return 0;
}

bool init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
        printf("SDL não pôde ser inicializado! Erro: %s\n", SDL_GetError());
        return false;
    }

    SDL_ShowCursor(0);

    coreInit();

    if (!renderInit()) return false; 
    if (!audioInit()) return false;

    
    fileInit();
    inputInit();

    return true;
}

void shut() {
    inputShut();
    fileShut();
    audioShut();
    renderShut();
    coreShut();
    SDL_Quit();
}

