#include "Libs/Main.h"

#undef main

int main(int argc, char ** argv) {
    // Declarando as variáveis principais aqui.
    bool running = true;        // Bandeira do loop principal

    Uint32 frameID;             // Auxiliar para estabilização de frames.
    Uint32 frameDelta;          // Auxiliar para estabilização de frames.
    int FPS = 60;               // Limite de FPS.
    int fpsMs = 1000/FPS;       // Tempo, em milisegundos, que um frame consome.

    if (!init()) return -1;     // Inicializando todos os sistemas do SDL

    renderSprite * a;
    renderPalette * p = createPalette(0xFFF33570, 0, 0, 0);
    object * obj, * mouseObj = createObject(0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, mouseObj);

    for (int i = 0; i < 40000; i++) {
        a = getCoreSprites()->layer[i%CORE_SPRITE_LIMIT];
        obj = a->object;
        parentObject(mouseObj, obj, false);
        addSpriteToRender(a);
    }

    a = createSprite(getCoreSprites()->layer[CORE_SPRITE_BLANK]->pixels, p, obj, 0, 0, SPRITE_STATE_SHOWN, 0);
    addSpriteToRender(a);

    a = NULL;

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

        mouseObj->localX = getMouse().x * getWindowRatioX();
        mouseObj->localY = getMouse().y * getWindowRatioY();
        
        coreUpdate();
        audioUpdate();
        renderUpdate();
        if (!errorUpdate()) running = false;

        frameDelta = SDL_GetTicks() - frameID;
        printf("%d\n", frameDelta);
        if (frameDelta < fpsMs) SDL_Delay(fpsMs - frameDelta);
    }
    
    shut();

    return 0;
}

bool init() {
    errorInit();

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
        printf("SDL não pôde ser inicializado! Erro: %s\n", SDL_GetError());
        return false;
    }

    SDL_ShowCursor(1);

    if (!coreInit()) {
        printf("%s\n", errorGet());
        SDL_Quit();
        errorShut();
        return false;
    }

    if (!renderInit()) {
        printf("%s\n", errorGet());
        coreShut();
        SDL_Quit();
        errorShut();
        return false;
    }
    
    if (!audioInit()) {
        printf("%s\n", errorGet());
        renderShut();
        coreShut();
        SDL_Quit();
        errorShut();
        return false;
    }

    fileInit();

    if (!inputInit()) {
        printf("%s\n", errorGet());
        fileShut();
        audioShut();
        renderShut();
        coreShut();
        SDL_Quit();
        errorShut();
        return false;
    }
    
    return true;
}

void shut() {
    inputShut();
    fileShut();
    audioShut();
    renderShut();
    coreShut();
    SDL_Quit();
    errorShut();
}

