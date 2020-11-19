#include "Libs/Main.h"

#undef main

int main(int argc, char ** argv) {
    // Declarando as variáveis principais aqui.
    bool running = true;        // Bandeira do loop principal

    Uint32 frameID;             // Auxiliar para estabilização de frames.
    Uint32 frameDelta;          // Auxiliar para estabilização de frames.
    int FPS = 60;               // Limite de FPS.
    int fpsMs = 1000/FPS;       // Tempo, em milisegundos, que um frame consome.

    SDL_Window * mWindow;       // Ponteiro para a janela principal.
    SDL_Surface * mSurface;     // Ponteiro para a superfície da janela.
    SDL_Renderer * mRenderer;   // Ponteiro para o renderizador principal.

    SDL_Texture * mTexture;     // Ponteiro para a textura do display.

    init();   // Inicializando todos os sistemas do SDL

    Uint8 * keyStates = malloc(sizeof(Uint8) * SDL_NUM_SCANCODES);  // Vetor responsável por armazenar o estado do teclado.
    inputAxis axisArray[Axis_Ammount];                              // Vetor responsável por armazenar os dados de todos os eixos de entrada.
    inputTrigger triggerArray[Trigger_Ammount];                     // Vetor responsável por armazenar os dados de todos os gatilhos de entrada.
    inputMouse mouse;                                               // Variável responsável por armazenar os dados do mouse.

    createTrigger("Exit", SDL_SCANCODE_ESCAPE);
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

