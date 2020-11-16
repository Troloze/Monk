#include "Libs/Main.h"

#undef main

int main() {
    // Declarando as variáveis principais aqui.
    bool running = true;        // Bandeira do loop principal

    Uint32 frameID;             // Auxiliar para estabilização de frames.
    Uint32 frameDelta;          // Auxiliar para estabilização de frames.
    int FPS = 100;               // Limite de FPS.
    int fpsMs = 1000/FPS;       // Tempo, em milisegundos, que um frame consome.

    SDL_Window * mWindow;       // Ponteiro para a janela principal.
    SDL_Surface * mSurface;     // Ponteiro para a superfície da janela.
    SDL_Renderer * mRenderer;   // Ponteiro para o renderizador principal.

    SDL_Texture * mTexture;     // Ponteiro para a textura do display.

    init(mWindow, mRenderer);   // Inicializando todos os sistemas do SDL
    
    
    Uint8 * keyStates = malloc(sizeof(Uint8) * SDL_NUM_SCANCODES);  // Vetor responsável por armazenar o estado do teclado.
    inputAxis axisArray[Axis_Ammount];                              // Vetor responsável por armazenar os dados de todos os eixos de entrada.
    inputTrigger triggerArray[Trigger_Ammount];                     // Vetor responsável por armazenar os dados de todos os gatilhos de entrada.
    inputMouse mouse;                                               // Variável responsável por armazenar os dados do mouse.

    triggerArray[0] = createTrigger("Exit", SDL_SCANCODE_ESCAPE);

    SDL_Event e;    // Variável que vai receber todos os eventos do SDL.
    mTexture = loadTexture(mRenderer,"Test.png");
    while (running) {
        frameID = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                running = false;
            } 
        }
        // Update do sistema de entrada.
        inputUpdate(keyStates, triggerArray, axisArray, &mouse);
        if (getTrigger(triggerArray, "Exit").value == 1) {
            running = false;
        }
        
        // Atualizando o renderizador.
        SDL_RenderClear(mRenderer);                         // Limpando a tela.
        SDL_RenderCopy(mRenderer, mTexture, NULL, NULL);    // Atualizando o renderizador.
        SDL_RenderPresent(mRenderer);                       // Atualizando a tela.// */

        frameDelta = SDL_GetTicks() - frameID;
        printf("%d\n", frameDelta);
        if (frameDelta < fpsMs) SDL_Delay(fpsMs - frameDelta);
    }// */
    free(keyStates); 
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    mRenderer = NULL;
    mWindow = NULL;

    IMG_Quit();
    shut(mWindow, mRenderer);

    return 0;
}

bool init(SDL_Window * window, SDL_Renderer * renderer) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
        printf("SDL não pôde ser inicializado! Erro: %s\n", SDL_GetError());
        return false;
    }

    if (!renderInit(window, renderer)) return false; 
    if (!audioInit()) return false;

    return true;
}

void shut(SDL_Window * window, SDL_Renderer * renderer) {
    //renderShut(window, renderer);
    audioShut();
    SDL_Quit();
}

