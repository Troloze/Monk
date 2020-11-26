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

    dRS = getDefaultSprite();

    createTrigger("Exit", SDL_SCANCODE_ESCAPE);
    createTrigger("Enter", SDL_SCANCODE_RETURN);
    createTrigger("Up", SDL_SCANCODE_UP);
    createTrigger("Down", SDL_SCANCODE_DOWN);
    createTrigger("Left", SDL_SCANCODE_LEFT);
    createTrigger("Right", SDL_SCANCODE_RIGHT);

    createAxis("Horizontal", 255, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT);
    createAxis("Vertical", 255, SDL_SCANCODE_DOWN, SDL_SCANCODE_UP);

    renderCamera * camera = getDefaultCamera();

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

        
       
        camera->object->localX += getAxis("Horizontal").value;
        camera->object->localY += getAxis("Vertical").value;
        
        

        if (getMouse().leftButtonState == 1) {

        }
        
        dRS = getDefaultSprite();
        void * palette = dRS->palette;
        if (getMouse().leftButtonState == 0) for (int i = 0; i < 16; i++) dRS->pixels[i] = getCoreSprites()[CORE_SPRITE_CURSOR_1].pixels[i];
        else for (int i = 0; i < 16; i++) dRS->pixels[i] = getCoreSprites()[CORE_SPRITE_CURSOR_2].pixels[i];
        dRS->object->localX = getMouse().x/4;
        dRS->object->localY = getMouse().y/4;

        coreUpdate();
        renderUpdate();
        
        frameDelta = SDL_GetTicks() - frameID;
        printf("%d\n", frameDelta);
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

    

    inputInit();

    return true;
}

void shut() {
    inputShut();
    renderShut();
    audioShut();
    coreShut();
    SDL_Quit();
}

