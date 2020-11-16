#include "RenderLib.h"

/**
 * \brief Esta função carrega uma textura para o programa.
 * 
 * \param renderer Ponteiro ao renderizador.
 * \param path Caminho para a imagem a ser renderizada, em PNG.
 * 
 * \return Uma textura com a imagem carregada.
 */
SDL_Texture * loadTexture(SDL_Renderer * renderer, const char * path) {
    SDL_Texture * rTexture = NULL; // Textura a ser retornada.
    SDL_Surface * rSurface = IMG_Load(path); // Superfície com a imagem a ser carregada.
    if (rSurface == NULL) {
        printf("Não foi possível carregar a imagem \"%s\". Erro: %s\n", path, IMG_GetError());
        return rTexture;    // Em caso de erro retornar textura nula.
    }
    rTexture = SDL_CreateTextureFromSurface(renderer, rSurface);    // Criando a textura a partir da superfície.
    if (rTexture == NULL) printf("Não foi possível criar textura de \"%s\". Erro: %s\n", path, IMG_GetError());

    SDL_FreeSurface(rSurface);  // Limpando a superfície carregada já que não precisamos mais dela.

    return rTexture;
}

/**
 * \brief Função que inicializa os sistemas de imagem e janela do SDL.
 * 
 * \param window Ponteiro para a janela principal.
 * \param renderer Ponteiro para o renderizador.
 * 
 * \return true, caso tudo se inicie corretamente; false, caso contrário.  
 */
bool renderInit(SDL_Window * window, SDL_Renderer * renderer) {
    window = SDL_CreateWindow(
        "Troloze's Amazing Adventure(TM)",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );  // Criar janela.
    if(window == NULL) {    // Checar se ela foi criada com sucesso.
        printf("Janela não pôde ser criada! Erro: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);  // Criar renderizador.
    if(renderer = NULL) {   // Checar se foi criado com sucesso.
        printf("Renderizador não pôde ser criado! Erro %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);   // Colocando a cor de desenho para branco.

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
void renderShut(SDL_Window * window, SDL_Renderer * renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;

    IMG_Quit();
}
