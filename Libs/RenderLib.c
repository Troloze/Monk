#include "RenderLib.h"


/**
 * \brief Esta função carrega uma superfice para o programa.
 * 
 * \param path Caminho para a imagem a ser carregada, em BMP.
 * 
 * \return Uma superfice com a imagem em path carregada.
 */
SDL_Surface * loadSurface(char * path) {
    SDL_Surface * loadSurface = SDL_LoadBMP(path);   // Superfice que irá carregar a imagem.
    
    if (loadSurface == NULL) {  // Checando se o carregamento foi bem sucedido.
        printf("Erro carregando superfice: %s\n", IMG_GetError());
        return NULL;
    }
    
    return loadSurface;  // Retornando a superfice
}

/**
 * \brief Função que inicializa os sistemas de imagem e janela do SDL.
 * 
 * \param window Ponteiro para a janela principal.
 * \param renderer Ponteiro para o renderizador.
 * 
 * \return true, caso tudo se inicie corretamente; false, caso contrário.  
 */
bool renderInit(SDL_Window ** window) {
    *window = SDL_CreateWindow(
        "Troloze's Amazing Adventure(TM)",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );  // Criar janela.
    if(*window == NULL) {    // Checar se ela foi criada com sucesso.
        printf("Janela não pôde ser criada! Erro: %s\n", SDL_GetError());
        return false;
    }

    // Colocar todas as flags de imagem nessa variável, a forma como a função IMG_Init retorna possíveis erros
    // faz com que o código seja dessa forma.
    int imageFlags = IMG_INIT_PNG;  // Flags do SDL_Image, para inicialização.
    if( !(IMG_Init(imageFlags) & imageFlags)) { // Inicializando os subsistemas do SDL_Image e checando por erros.
        printf("Não pode inicializar os subsistemas de imagem. Erro: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

/**
 * \brief Finaliza todos os sistemas de imagem e janela do SDL.
 * 
 * \param window Ponteiro para a janela principal.
 * \param renderer Ponteiro para o renderizador principal.
 */
void renderShut(SDL_Window ** window) {
    SDL_DestroyWindow(*window);
    window = NULL;

    IMG_Quit();
}
