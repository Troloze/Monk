#include "Libs/Main.h"

#undef main

bool running = true;        // Bandeira do loop principal

int main(int argc, char ** argv) {
    // Declarando as variáveis principais aqui.
    const bool printFRate = SHOW_FPS;     // Printar quantos ms um frame demora.
    Uint32 a = 0;;
    Uint32 frameID;             // Auxiliar para estabilização de frames.
    Uint32 frameC = 0;              // Valor do frame atual.
    Uint32 frameDelta;          // Auxiliar para estabilização de frames.
    const int FPS = 60;               // Limite de FPS.
    const int fpsMs = 1000/FPS;       // Tempo, em milisegundos, que um frame consome.
    renderMetasprite * logo;

    if (!init()) return -1;     // Inicializando todos os sistemas do SDL
    
    SDL_Event e;    // Variável que vai receber todos os eventos do SDL.

    if (SHOW_LOGO) {
        logo = createMetaFromSheet("Sprites/Logo.bmp",-1, 32, 32, -1 ,-1, -1, -1, 0, 0, NULL, 20);
        logo->state = SPRITE_STATE_SHOWN;
        addMetaToRender(logo);
    }

    while (running) {
        frameID = SDL_GetTicks();
        frameC++;
        
        while (SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT) {
                running = false;
            } 
        }
        if (SHOW_LOGO && frameC < 180) {
            frameID = SDL_GetTicks();
            monkOnLogoUpdate();
            frameDelta = SDL_GetTicks() - frameID + 1;
            if (printFRate) {
                printf("FPS: ");
                if ((frameDelta - 1) <= fpsMs) printf("%d.", FPS);
                else printf("%d.", 1000/frameDelta);
                printf(" Virtual FPS: %d. Delta: %d\n", 1000/frameDelta, frameDelta - 1);
            }
            if (frameDelta - 1 < fpsMs) SDL_Delay(fpsMs - frameDelta - 1);
        }
        if (SHOW_LOGO && frameC == 180) {
           logo->state = 0;
           destroyObject(logo->obj);
           if (!monkAfterLogo()) {
               running = 0;
           }
        }

        inputUpdate();          // Update da Entrada.
        monkUpdate(frameC);           // Update do Usuário.
        coreUpdate();           // Update do Sistema central.  
        gameUpdate();           // Update das áreas.
        audioUpdate();          // Update do áudio.
        renderUpdate();         // Update do render.
        if (!errorUpdate()) running = false;    // Checando se não há erros.

        frameDelta = SDL_GetTicks() - frameID + 1;
        if (printFRate) {
            printf("FPS: ");
            if ((frameDelta - 1) <= fpsMs) printf("%d.", FPS);
            else printf("%d.", 1000/frameDelta);
            printf(" Virtual FPS: %d. Delta: %d\n", 1000/frameDelta, frameDelta);
        }
        if (frameDelta - 1 < fpsMs) SDL_Delay(fpsMs - frameDelta - 1);
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

    
    // SDL_ShowCursor(0);

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
    
    if (!gameInit()) {
        printf("%s\n", errorGet());
        fileShut();
        audioShut();
        renderShut();
        coreShut();
        SDL_Quit();
        errorShut();
        return false;
    }
    
    if (!inputInit()) {
        printf("%s\n", errorGet());
        fileShut();
        audioShut();
        renderShut();
        coreShut();
        SDL_Quit();
        errorShut();
        gameShut();
        return false;
    }
    
    
    if (!monkStart(&running)) {
        fileShut();
        audioShut();
        renderShut();
        coreShut();
        SDL_Quit();
        errorShut();
        gameShut();
        fileShut();
        return false;
    }
    
    if (!SHOW_LOGO) monkAfterLogo();

    return true;
}

void shut() {
    monkExit();
    shutInit();
    inputShut();
    fileShut();
    audioShut();
    coreShut();
    gameShut();
    renderShut();
    SDL_Quit();
    errorShut();
}