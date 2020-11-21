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

    Uint8 * keyStates = malloc(sizeof(Uint8) * SDL_NUM_SCANCODES);  // Vetor responsável por armazenar o estado do teclado.
    inputAxis axisArray[Axis_Ammount];                              // Vetor responsável por armazenar os dados de todos os eixos de entrada.
    inputTrigger triggerArray[Trigger_Ammount];                     // Vetor responsável por armazenar os dados de todos os gatilhos de entrada.
    inputMouse mouse;                                               // Variável responsável por armazenar os dados do mouse.

    renderSprite dRS;

    createTrigger("Exit", SDL_SCANCODE_ESCAPE);
    createTrigger("Enter", SDL_SCANCODE_RETURN);
    createAxis("Horizontal", 64, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT);
   

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

        if (getTrigger("Enter").value == 1) {
            clearLayer(1);
        }

        dRS = getDefaultSprite();
        void * palette = dRS.palette;
        if (getMouse().leftButtonState == 0) dRS = getCoreSprites()[CORE_SPRITE_CURSOR_1];
        else dRS = getCoreSprites()[CORE_SPRITE_CURSOR_2];

        dRS.globalX = getMouse().x/4;
        dRS.palette = palette;
        dRS.globalY = getMouse().y/4;
        dRS.state = SPRITE_STATE_SHOWN;
        

        setDefaultSprite(dRS);    

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

    if (!renderInit()) return false; 
    if (!audioInit()) return false;

    inputInit();

    return true;
}

void shut() {
    inputShut();
    renderShut();
    audioShut();
    SDL_Quit();
}

